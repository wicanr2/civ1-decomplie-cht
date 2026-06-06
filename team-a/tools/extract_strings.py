#!/usr/bin/env python3
"""
從 Mac Resource Fork (Civdata*.RSC) 抽 STR# (string list) + TEXT 資源。

STR# 格式（Apple Mac Resource Manager 標準）:
    BE16 count
    count × Pascal string (BYTE len + len bytes)

TEXT 格式: 純文字 byte stream，可能含 Mac 換行 CR (0x0D)。

CIV.EXE 已被 Track A patched 為 Big5；STR# / TEXT 中字串可能：
- 英文 ASCII（沒被 patch 的條目）
- Big5（Track A 已翻譯的條目）
- 混合（半譯狀態）

CLI:
    python3 extract_strings.py <file.RSC> [--type STR# | TEXT | all] [--encoding utf8 | big5 | auto]
"""
import argparse
import json
import struct
import sys
from pathlib import Path


def parse_rsrc_fork(data):
    """從 spec 03 §3.3 抽 (fourCC, id, name, payload) 清單"""
    data_off, map_off, _data_len, _map_len = struct.unpack(">IIII", data[:16])
    mp = map_off
    tl = struct.unpack(">H", data[mp + 0x18 : mp + 0x1A])[0] + mp
    nl = struct.unpack(">H", data[mp + 0x1A : mp + 0x1C])[0] + mp
    n_types = struct.unpack(">H", data[tl : tl + 2])[0] + 1

    out = []
    for i in range(n_types):
        e = tl + 2 + i * 8
        fourcc = data[e : e + 4]
        n_refs = struct.unpack(">H", data[e + 4 : e + 6])[0] + 1
        ref_off = struct.unpack(">H", data[e + 6 : e + 8])[0] + tl
        for j in range(n_refs):
            r = ref_off + j * 12
            rid = struct.unpack(">H", data[r : r + 2])[0]
            name_off_rel = struct.unpack(">h", data[r + 2 : r + 4])[0]
            name = None
            if name_off_rel >= 0:
                np = nl + name_off_rel
                if np < len(data):
                    nlen = data[np]
                    if np + 1 + nlen <= len(data):
                        name = data[np + 1 : np + 1 + nlen].decode("ascii", "replace")
            doff = struct.unpack(">I", data[r + 4 : r + 8])[0] & 0xFFFFFF
            d_start = data_off + doff
            d_size = struct.unpack(">I", data[d_start : d_start + 4])[0]
            payload = data[d_start + 4 : d_start + 4 + d_size]
            out.append((fourcc, rid, name, payload))
    return out


def parse_str_hash(payload):
    """STR# = BE16 count + Pascal strings (BYTE len + bytes)"""
    if len(payload) < 2:
        return []
    n = struct.unpack(">H", payload[:2])[0]
    p = 2
    strings = []
    for _ in range(n):
        if p >= len(payload):
            break
        L = payload[p]
        p += 1
        s = payload[p : p + L]
        p += L
        strings.append(s)
    return strings


def decode_smart(raw):
    """嘗試解 UTF-8 / Big5；fallback 顯示原 byte"""
    # 第一步：純 ASCII?
    if all(b < 0x80 for b in raw):
        try:
            return raw.decode("ascii"), "ascii"
        except UnicodeDecodeError:
            pass
    # 第二步：UTF-8?
    try:
        s = raw.decode("utf-8", errors="strict")
        return s, "utf-8"
    except UnicodeDecodeError:
        pass
    # 第三步：Big5 (cp950)?
    try:
        s = raw.decode("cp950", errors="strict")
        return s, "big5"
    except UnicodeDecodeError:
        pass
    # 最後：latin-1
    return raw.decode("latin-1"), "latin1"


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("rsc")
    ap.add_argument("--type", choices=["STR#", "TEXT", "all"], default="all")
    ap.add_argument("--json", action="store_true")
    args = ap.parse_args()

    data = Path(args.rsc).read_bytes()
    items = parse_rsrc_fork(data)

    str_items  = [(rid, name, p) for fc, rid, name, p in items if fc == b"STR#"]
    text_items = [(rid, name, p) for fc, rid, name, p in items if fc == b"TEXT"]

    out_struct = {"file": args.rsc, "str_hash": {}, "text": {}}

    if args.type in ("STR#", "all") and str_items:
        if not args.json:
            print(f"=== STR# resources: {len(str_items)} ===\n")
        for rid, name, payload in str_items:
            strings = parse_str_hash(payload)
            decoded = []
            for raw in strings:
                s, enc = decode_smart(raw)
                decoded.append({"raw_bytes": raw.hex(), "text": s, "enc": enc})
            out_struct["str_hash"][rid] = {
                "name": name, "count": len(strings), "items": decoded,
            }
            if not args.json:
                print(f"STR# id={rid:5}  name={name!r}  count={len(strings)}")
                for i, d in enumerate(decoded[:6]):
                    tag = f"[{d['enc']}]" if d['enc'] != "ascii" else ""
                    print(f"    [{i:2}] {tag} {d['text']!r}")
                if len(decoded) > 6:
                    print(f"    ... {len(decoded) - 6} more")
                print()

    if args.type in ("TEXT", "all") and text_items:
        if not args.json:
            print(f"\n=== TEXT resources: {len(text_items)} ===\n")
        for rid, name, payload in text_items[:30]:
            # Mac TEXT 用 CR (0x0D) 換行；改為 LF 給現代顯示
            t, enc = decode_smart(payload.replace(b"\r", b"\n"))
            out_struct["text"][rid] = {"name": name, "text": t, "enc": enc, "len": len(payload)}
            if not args.json:
                preview = t[:80].replace("\n", " | ")
                print(f"TEXT id={rid:5}  name={name!r}  enc={enc}  len={len(payload)}")
                print(f"    {preview!r}")
        if not args.json and len(text_items) > 30:
            print(f"\n... {len(text_items) - 30} more TEXT entries (use --json for all)\n")
        # 全部塞進 JSON
        if args.json:
            for rid, name, payload in text_items[30:]:
                t, enc = decode_smart(payload.replace(b"\r", b"\n"))
                out_struct["text"][rid] = {"name": name, "text": t, "enc": enc, "len": len(payload)}

    if args.json:
        json.dump(out_struct, sys.stdout, ensure_ascii=False, indent=2)
    return 0


if __name__ == "__main__":
    sys.exit(main())
