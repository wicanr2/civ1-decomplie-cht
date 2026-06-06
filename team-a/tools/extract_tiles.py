#!/usr/bin/env python3
"""
CvPc 抽取器 (production)

從 Civdata*.RSC (Mac Resource Fork) 解 CvPc 影像，輸出 PNG 或 PPM。

CvPc 格式（從 LoadGifPicture + PicDecompress @ 10b8:158c 反推）:
    offset 0-1 : width  (BE16)
    offset 2-3 : height (BE16)
    offset 4   : min_code_size for LZW (例：7 → CLEAR=128 → 128-color)
    offset 5   : palette_count - 1
    offset 6 .. 6 + palette_count*3 : RGB triplets (8-bit per channel)
    剩餘        : GIF89a 風格 LZW (sub-block 串流, LSB-first bit packing,
                  CLEAR = 1 << min_code, END = CLEAR + 1)

CLI:
    python3 extract_tiles.py <input.rsc> [--out-dir DIR] [--id ID] [--list]
"""
import argparse
import struct
import sys
from pathlib import Path


# ---------------------------------------------------------------------------
# Mac Resource Fork (Apple ResEdit 標準格式) minimal parser
# ---------------------------------------------------------------------------

def parse_rsrc_fork(data):
    """回傳 dict[fourCC] = list of (id, name, payload)"""
    data_off, map_off, _data_len, _map_len = struct.unpack(">IIII", data[:16])
    mp = map_off

    type_list_off = struct.unpack(">H", data[mp + 0x18 : mp + 0x1A])[0] + mp
    name_list_off = struct.unpack(">H", data[mp + 0x1A : mp + 0x1C])[0] + mp
    n_types = struct.unpack(">H", data[type_list_off : type_list_off + 2])[0] + 1

    out = {}
    for i in range(n_types):
        e = type_list_off + 2 + i * 8
        fourcc = data[e : e + 4]
        n_ref = struct.unpack(">H", data[e + 4 : e + 6])[0] + 1
        ref_off = struct.unpack(">H", data[e + 6 : e + 8])[0] + type_list_off

        entries = []
        for j in range(n_ref):
            r = ref_off + j * 12
            rid = struct.unpack(">H", data[r : r + 2])[0]
            name_off_off = struct.unpack(">h", data[r + 2 : r + 4])[0]
            name = None
            if name_off_off != -1:
                nl = name_list_off + name_off_off
                nlen = data[nl]
                name = data[nl + 1 : nl + 1 + nlen].decode("ascii", "replace")
            doff_packed = struct.unpack(">I", data[r + 4 : r + 8])[0]
            doff = doff_packed & 0x00FFFFFF
            d_start = data_off + doff
            d_size = struct.unpack(">I", data[d_start : d_start + 4])[0]
            payload = data[d_start + 4 : d_start + 4 + d_size]
            entries.append((rid, name, payload))
        out[fourcc] = entries
    return out


# ---------------------------------------------------------------------------
# CvPc decoder
# ---------------------------------------------------------------------------

def decode_cvpc(payload):
    """解一個 CvPc payload。回傳 (w, h, palette[bytes RGB*N], pixels[bytes w*h])"""
    if len(payload) < 6:
        raise ValueError(f"payload 太短 ({len(payload)} bytes)")
    w        = struct.unpack(">H", payload[0:2])[0]
    h        = struct.unpack(">H", payload[2:4])[0]
    min_code = payload[4]
    pal_n    = payload[5] + 1
    pal_off  = 6
    pal_end  = pal_off + pal_n * 3
    palette  = payload[pal_off:pal_end]
    if pal_end > len(payload):
        raise ValueError(f"palette 超出 payload")

    # Defrag GIF89a sub-blocks
    stream = bytearray()
    i = pal_end
    while i < len(payload):
        block_len = payload[i]
        i += 1
        if block_len == 0:
            break
        stream.extend(payload[i : i + block_len])
        i += block_len

    pixels = lzw_decode(stream, min_code, w * h)
    return w, h, palette, pixels


def lzw_decode(stream, min_code_size, target_len):
    """標準 GIF89a LZW decoder (LSB-first bit packing)。"""
    CLEAR     = 1 << min_code_size
    END       = CLEAR + 1
    code_size = min_code_size + 1
    mask      = (1 << code_size) - 1
    next_code = END + 1

    dictionary = [bytes([i]) for i in range(CLEAR)] + [b"", b""]
    while len(dictionary) < 4096:
        dictionary.append(b"")

    prev_code  = -1
    output     = bytearray()
    bit_buf    = 0
    bit_count  = 0
    pos        = 0
    stream_len = len(stream)

    while True:
        while bit_count < code_size and pos < stream_len:
            bit_buf  |= stream[pos] << bit_count
            bit_count += 8
            pos += 1
        if bit_count < code_size:
            return bytes(output)

        code = bit_buf & mask
        bit_buf  >>= code_size
        bit_count -= code_size

        if code == CLEAR:
            code_size = min_code_size + 1
            mask      = (1 << code_size) - 1
            next_code = END + 1
            prev_code = -1
            continue
        if code == END:
            return bytes(output)

        if code < next_code:
            entry = dictionary[code]
        elif code == next_code and prev_code != -1:
            prev_entry = dictionary[prev_code]
            entry = prev_entry + prev_entry[:1]
        else:
            return bytes(output)

        output.extend(entry)
        if len(output) >= target_len:
            return bytes(output[:target_len])

        if prev_code != -1 and next_code < 4096:
            prev_entry = dictionary[prev_code]
            dictionary[next_code] = prev_entry + entry[:1]
            next_code += 1
            if next_code == (1 << code_size) and code_size < 12:
                code_size += 1
                mask = (1 << code_size) - 1

        prev_code = code


# ---------------------------------------------------------------------------
# 輸出
# ---------------------------------------------------------------------------

def write_indexed_ppm(path, w, h, palette, pixels):
    pal_n = len(palette) // 3
    with open(path, "wb") as f:
        f.write(f"P6\n{w} {h}\n255\n".encode())
        for i in range(w * h):
            if i >= len(pixels):
                f.write(b"\xff\x00\xff")
                continue
            idx = pixels[i]
            if idx >= pal_n:
                f.write(b"\x00\xff\xff")
                continue
            f.write(palette[idx * 3 : idx * 3 + 3])


def try_write_png(path_png, w, h, palette, pixels):
    """嘗試用 Pillow 寫 PNG；失敗回 False"""
    try:
        from PIL import Image
    except ImportError:
        return False
    pal_n = len(palette) // 3
    img = Image.new("P", (w, h))
    flat_pal = bytearray()
    for i in range(256):
        if i < pal_n:
            flat_pal.extend(palette[i * 3 : i * 3 + 3])
        else:
            flat_pal.extend(b"\xff\x00\xff")
    img.putpalette(bytes(flat_pal))
    actual = min(len(pixels), w * h)
    if actual < w * h:
        pixels = pixels + bytes([0] * (w * h - actual))
    img.frombytes(bytes(pixels[: w * h]))
    img.save(path_png)
    return True


# ---------------------------------------------------------------------------
# CLI
# ---------------------------------------------------------------------------

def cmd_list(rsc_path):
    data = Path(rsc_path).read_bytes()
    fork = parse_rsrc_fork(data)
    for fourcc, entries in sorted(fork.items()):
        print(f"{fourcc.decode('ascii', 'replace')} × {len(entries)}")
        for rid, name, payload in entries:
            print(f"    id={rid:5d}  name={name!r}  size={len(payload)}")


def cmd_extract(rsc_path, out_dir, target_id):
    data = Path(rsc_path).read_bytes()
    fork = parse_rsrc_fork(data)
    cvpcs = fork.get(b"CvPc", [])
    if not cvpcs:
        print(f"沒有 CvPc 資源在 {rsc_path}")
        return 1

    out_dir = Path(out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    ok, fail = 0, 0
    for rid, name, payload in cvpcs:
        if target_id is not None and rid != target_id:
            continue
        slug = (name or f"id{rid}").replace("/", "_").replace(" ", "_")
        base = out_dir / f"{Path(rsc_path).stem}_{rid:03d}_{slug}"
        try:
            w, h, palette, pixels = decode_cvpc(payload)
            png_path = base.with_suffix(".png")
            wrote_png = try_write_png(str(png_path), w, h, palette, pixels)
            if not wrote_png:
                ppm_path = base.with_suffix(".ppm")
                write_indexed_ppm(str(ppm_path), w, h, palette, pixels)
                print(f"  OK  {ppm_path.name}  {w}×{h}")
            else:
                print(f"  OK  {png_path.name}  {w}×{h}")
            ok += 1
        except Exception as e:
            print(f"  FAIL id={rid} ({slug}): {e}")
            fail += 1
    print(f"done: {ok} ok, {fail} fail")
    return 0


def main():
    p = argparse.ArgumentParser()
    p.add_argument("rsc", help="Civdata*.RSC 路徑")
    p.add_argument("--list", action="store_true", help="只列出資源不抽出")
    p.add_argument("--id", type=int, help="只抽出特定 id")
    p.add_argument("--out-dir", default="extracted", help="輸出目錄")
    args = p.parse_args()

    if args.list:
        cmd_list(args.rsc)
        return 0
    return cmd_extract(args.rsc, args.out_dir, args.id)


if __name__ == "__main__":
    sys.exit(main())
