#!/usr/bin/env python3
"""
從 1993 Win16 NE CIV.EXE 解出 24 個 RT_DIALOG resource 結構。

Win16 DLGTEMPLATE 格式 (in-resource，null-terminated 字串):
    u32 style                       Windows style flags (DS_*, WS_*)
    u8  n_items                     control 數
    u16 x, y, cx, cy                位置 / 大小 (dialog 單位)
    [menu name]      null-term 字串；首 byte 0xFF -> 後接 WORD ordinal；0 -> 無
    [class name]     同上格式；0 -> 預設 DIALOG class
    [caption]        null-term 字串
    if style & DS_SETFONT:
        u16 font_size
        [face name] null-term

對每個 control，DLGITEMTEMPLATE:
    u16 x, y, cx, cy, id
    u32 style
    [class]   0xFF + u8 ordinal (1=Button 2=Edit 3=Static 4=ListBox
              5=ScrollBar 6=ComboBox) 或 null-term class name
    [text]    null-term 字串
    u16 cb_data; cb_data bytes
    (16-bit align to next item)

用法:
    python3 ne_dialog_dump.py <CIV.EXE> [--id ID] [--json]
"""
import argparse
import json
import struct
import sys
from pathlib import Path


WIN16_CONTROL_CLASS = {
    0x80: "Button",
    0x81: "Edit",
    0x82: "Static",
    0x83: "ListBox",
    0x84: "ScrollBar",
    0x85: "ComboBox",
}

DS_SETFONT  = 0x0040


def u8(b, o):  return b[o]
def u16(b, o): return struct.unpack_from("<H", b, o)[0]
def u32(b, o): return struct.unpack_from("<I", b, o)[0]


def read_zterm(data, off):
    """null-term ASCII/Latin1 字串。回 (str, next_off)"""
    if off >= len(data) or data[off] == 0:
        return "", off + 1
    end = off
    while end < len(data) and data[end] != 0:
        end += 1
    return data[off:end].decode("latin-1"), end + 1


def read_dlg_sz_or_ord(data, off):
    """dialog header 內 menu / class / caption 用：0x00 empty, 0xFF+WORD
    ordinal, 否則 null-term 字串。回 (value, next_off)。"""
    if off >= len(data):
        return None, off
    if data[off] == 0x00:
        return "", off + 1
    if data[off] == 0xFF and off + 3 <= len(data):
        return u16(data, off + 1), off + 3
    return read_zterm(data, off)


def read_item_class(data, off):
    """item.class 用：0x80-0x85 單 byte ordinal，否則 null-term 字串。
    回 (descriptor, next_off)。"""
    if off >= len(data):
        return "?", off
    b = data[off]
    if 0x80 <= b <= 0x85:
        return WIN16_CONTROL_CLASS.get(b, f"0x{b:02X}"), off + 1
    return read_zterm(data, off)


def read_item_text(data, off):
    """item.text 用：0x00 empty, 0xFF+WORD ordinal, 否則 null-term 字串。"""
    return read_dlg_sz_or_ord(data, off)


# ---------------------------------------------------------------------------
# NE resource table walker — 只關注 RT_DIALOG (type id 0x8005)
# ---------------------------------------------------------------------------

def find_dialog_resources(data):
    """回傳 list of (rid, file_offset, length)"""
    if data[:2] != b"MZ":
        raise ValueError("not MZ")
    ne_off = u32(data, 0x3C)
    if data[ne_off : ne_off + 2] != b"NE":
        raise ValueError("not NE")
    res_off_rel = u16(data, ne_off + 0x24)
    rt_base     = ne_off + res_off_rel
    align_shift = u16(data, rt_base)

    p = rt_base + 2
    results = []
    while p < len(data):
        tid = u16(data, p)
        if tid == 0:
            break
        n_entries = u16(data, p + 2)
        p += 8
        for _ in range(n_entries):
            r_off_sectors = u16(data, p + 0)
            r_len_sectors = u16(data, p + 2)
            r_id          = u16(data, p + 6)
            file_off = r_off_sectors << align_shift
            file_len = r_len_sectors << align_shift
            if tid == 0x8005:    # RT_DIALOG
                rid = r_id & 0x7FFF
                results.append((rid, file_off, file_len))
            p += 12
    return results


# ---------------------------------------------------------------------------
# DLGTEMPLATE 解析
# ---------------------------------------------------------------------------

def parse_dialog(data, off, length):
    """解一個 RT_DIALOG resource。回 dict。"""
    if length < 13:
        return None
    base = off
    style = u32(data, base + 0)
    n_items = u8(data, base + 4)
    x = u16(data, base + 5)
    y = u16(data, base + 7)
    cx = u16(data, base + 9)
    cy = u16(data, base + 11)
    p = base + 13

    menu, p    = read_dlg_sz_or_ord(data, p)
    cls,  p    = read_dlg_sz_or_ord(data, p)
    caption, p = read_dlg_sz_or_ord(data, p)

    font = None
    if style & DS_SETFONT:
        if p + 2 <= len(data):
            font_size = u16(data, p)
            p += 2
            face, p = read_zterm(data, p)
            font = (font_size, face)

    items = []
    for _ in range(n_items):
        # item 沒有對齊要求（Track A spec 確認）
        if p + 14 > len(data):
            break
        ix     = u16(data, p)
        iy     = u16(data, p + 2)
        icx    = u16(data, p + 4)
        icy    = u16(data, p + 6)
        iid    = u16(data, p + 8)
        istyle = u32(data, p + 10)
        p += 14
        iclass, p = read_item_class(data, p)
        itext,  p = read_item_text(data, p)
        if p + 1 > len(data):
            break
        cb_data = data[p]
        p += 1
        if cb_data > 0:
            p += cb_data
        items.append({
            "x": ix, "y": iy, "cx": icx, "cy": icy,
            "id": iid,
            "style": istyle,
            "class": iclass,
            "text": itext,
        })

    return {
        "style": style,
        "n_items_claimed": n_items,
        "n_items_parsed": len(items),
        "x": x, "y": y, "cx": cx, "cy": cy,
        "menu": menu,
        "class": cls,
        "caption": caption,
        "font": font,
        "items": items,
    }


# ---------------------------------------------------------------------------
# 輸出
# ---------------------------------------------------------------------------

def format_text(captured):
    """把可能含 Big5 raw 的字串顯示成可讀（嘗試 cp950 decode）"""
    if not captured:
        return repr("")
    try:
        # 試 cp950 (Big5)
        b = captured.encode("latin-1")
        s = b.decode("cp950")
        if any(ord(c) > 0x7F for c in s):
            return f"{s!r}  (Big5)"
        return repr(s)
    except UnicodeDecodeError:
        return repr(captured)


def print_dialog(rid, d):
    print(f"\n=== Dialog ID {rid} ({d['style']:#010x}, {d['cx']}×{d['cy']} dlg units)")
    print(f"  caption: {format_text(d['caption'])}")
    if d['menu']:    print(f"  menu:    {d['menu']!r}")
    if d['class']:   print(f"  class:   {d['class']!r}")
    if d['font']:    print(f"  font:    size={d['font'][0]} face={format_text(d['font'][1])}")
    print(f"  controls: {d['n_items_parsed']} / {d['n_items_claimed']}")
    for i, it in enumerate(d['items']):
        cls = it['class'] if it['class'] else "?"
        print(f"    [{i:>2}] {cls:<10} id={it['id']:5}  "
              f"@ ({it['x']:>3},{it['y']:>3}) {it['cx']:>3}×{it['cy']:>3}  "
              f"text={format_text(it['text'])}")


def main():
    p = argparse.ArgumentParser()
    p.add_argument("exe", help="CIV.EXE 路徑")
    p.add_argument("--id", type=int, help="只顯示某 ID")
    p.add_argument("--json", action="store_true", help="輸出 JSON")
    args = p.parse_args()

    data = Path(args.exe).read_bytes()
    rs = find_dialog_resources(data)
    print(f"找到 {len(rs)} 個 RT_DIALOG resource")

    all_dialogs = {}
    for rid, off, length in rs:
        if args.id is not None and rid != args.id:
            continue
        d = parse_dialog(data, off, length)
        if d is None:
            print(f"  ID {rid}: parse failed")
            continue
        all_dialogs[rid] = d
        if not args.json:
            print_dialog(rid, d)

    if args.json:
        json.dump(all_dialogs, sys.stdout, ensure_ascii=False, indent=2)
    return 0


if __name__ == "__main__":
    sys.exit(main())
