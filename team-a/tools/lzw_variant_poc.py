#!/usr/bin/env python3
"""
CvPc LZW variant PoC：驗證從 PicDecompress 反推的「header byte = min_code_size - 1」假設。

從 SPR32X32.GIF (CvPc id 200 in CIVDATA4.RSC) 抽 payload，用調整後的
LZW decoder 解，輸出 PGM (灰階 PPM-like) 給肉眼驗證。

執行：
    python3 lzw_variant_poc.py <CIVDATA4.RSC> [<output.pgm>]
"""
import struct
import sys
from pathlib import Path


# ---------------------------------------------------------------------------
# Mac Resource Fork minimal parser
# ---------------------------------------------------------------------------

def parse_rsrc_fork(data):
    """回傳 { fourCC: { id: bytes } }"""
    data_off, map_off, data_len, map_len = struct.unpack(">IIII", data[:16])
    mp = map_off

    type_list_off = struct.unpack(">H", data[mp + 0x18 : mp + 0x1A])[0] + mp
    n_types = struct.unpack(">H", data[type_list_off : type_list_off + 2])[0] + 1
    out = {}

    for i in range(n_types):
        e = type_list_off + 2 + i * 8
        fourcc = data[e : e + 4]
        n_ref = struct.unpack(">H", data[e + 4 : e + 6])[0] + 1
        ref_off = struct.unpack(">H", data[e + 6 : e + 8])[0] + type_list_off

        out[fourcc] = {}
        for j in range(n_ref):
            r = ref_off + j * 12
            rid = struct.unpack(">H", data[r : r + 2])[0]
            doff_packed = struct.unpack(">I", data[r + 4 : r + 8])[0]
            doff = doff_packed & 0x00FFFFFF
            d_start = data_off + doff
            d_size = struct.unpack(">I", data[d_start : d_start + 4])[0]
            out[fourcc][rid] = data[d_start + 4 : d_start + 4 + d_size]
    return out


# ---------------------------------------------------------------------------
# CvPc LZW decoder（從 PicDecompress 反推的變體）
# ---------------------------------------------------------------------------

def decode_cvpc(payload, expected_pixels):
    """
    Header (從 LoadGifPicture + PicDecompress 反推):
        offset 0-1 : width  (BE16)
        offset 2-3 : height (BE16)
        offset 4   : byte 之後傳給 PicDecompress 作 LZW 最小碼長
                     PicDecompress 線 118: CLEAR = 2 << (byte4 & 0x1F)
                     → actual_min_code_size = byte4 + 1
        offset 5   : palette_count - 1
        offset 6 .. 6+(palette_count)*3 : RGB triplets
        之後                              : LZW payload (sub-block 串流)
                                            **沒有 header byte**；直接是壓縮 bytes
    """
    w = struct.unpack(">H", payload[0:2])[0]
    h = struct.unpack(">H", payload[2:4])[0]
    byte4 = payload[4]
    pal_count = payload[5] + 1
    pal_bytes = pal_count * 3
    palette = payload[6 : 6 + pal_bytes]

    lzw_start = 6 + pal_bytes
    if lzw_start >= len(payload):
        raise ValueError("LZW start beyond payload")

    # PoC #2: byte4 直接是 min_code_size (不是 +1 變體)
    actual_min_code = byte4
    print(f"  W×H = {w}×{h}, byte4=0x{byte4:02X} → min_code_size = {actual_min_code}")
    print(f"  palette={pal_count} entries ({pal_bytes} bytes)")

    # Defrag GIF89a sub-blocks (length-prefixed)
    stream_bytes = bytearray()
    i = lzw_start
    while i < len(payload):
        block_len = payload[i]
        i += 1
        if block_len == 0:
            break
        stream_bytes.extend(payload[i : i + block_len])
        i += block_len
    print(f"  sub-block stream length = {len(stream_bytes)} bytes")

    return lzw_decode_gif_style(stream_bytes, actual_min_code, expected_pixels), \
           (w, h, palette, byte4)


def lzw_decode_gif_style(stream, min_code_size, expected_pixels):
    """標準 GIF89a LZW decoder（least-significant-bit-first bit packing）。
       min_code_size 是「actual」值，已含 header byte+1 修正。"""
    CLEAR = 1 << min_code_size
    END   = CLEAR + 1
    code_size = min_code_size + 1
    mask = (1 << code_size) - 1

    # Dictionary
    # entry = (prefix_code, byte)  或 直接 (output_bytes,)
    # 為了簡單存全字串：dict[code] = bytes
    dictionary = {}
    for i in range(CLEAR):
        dictionary[i] = bytes([i])
    dictionary[CLEAR] = None   # CLEAR sentinel
    dictionary[END]   = None   # END sentinel

    next_code = END + 1
    prev_code = None
    output = bytearray()

    bit_buf = 0
    bit_count = 0
    stream_pos = 0

    while True:
        # 從 stream 餵 bits 直到 bit_count >= code_size
        while bit_count < code_size:
            if stream_pos >= len(stream):
                # 沒料了
                print(f"  stream exhausted at output pos {len(output)}, code_size={code_size}, "
                      f"next_code={next_code}, dict size={next_code}")
                return bytes(output)
            bit_buf |= stream[stream_pos] << bit_count
            bit_count += 8
            stream_pos += 1

        code = bit_buf & mask
        bit_buf >>= code_size
        bit_count -= code_size

        if code == CLEAR:
            code_size = min_code_size + 1
            mask = (1 << code_size) - 1
            next_code = END + 1
            prev_code = None
            continue
        if code == END:
            return bytes(output)

        if code < next_code:
            entry = dictionary[code]
        elif code == next_code and prev_code is not None:
            # KwKwK
            prev_entry = dictionary[prev_code]
            entry = prev_entry + prev_entry[:1]
        else:
            print(f"  ERROR: code {code} > next_code {next_code} (prev={prev_code}, pos={len(output)})")
            return bytes(output)

        output.extend(entry)
        if len(output) >= expected_pixels:
            return bytes(output)

        # 加新字典條目
        if prev_code is not None and next_code < (1 << 12):
            prev_entry = dictionary[prev_code]
            dictionary[next_code] = prev_entry + entry[:1]
            next_code += 1
            # 字典快滿就升 code_size
            if next_code == (1 << code_size) and code_size < 12:
                code_size += 1
                mask = (1 << code_size) - 1

        prev_code = code


# ---------------------------------------------------------------------------
# Output
# ---------------------------------------------------------------------------

def write_indexed_ppm(path, w, h, palette, pixels):
    """Palette: pal_count * 3 bytes (RGB)
       Pixels:  w * h bytes (palette indices)"""
    pal_count = len(palette) // 3
    actual = min(len(pixels), w * h)
    with open(path, "wb") as f:
        f.write(f"P6\n{w} {h}\n255\n".encode())
        for i in range(w * h):
            if i >= actual:
                f.write(b"\xff\x00\xff")     # magenta fill 表示資料不夠
                continue
            idx = pixels[i]
            if idx >= pal_count:
                f.write(b"\x00\xff\xff")     # cyan = palette out of range
                continue
            f.write(palette[idx * 3 : idx * 3 + 3])


def main():
    if len(sys.argv) < 2:
        print("usage: lzw_variant_poc.py <CIVDATA4.RSC> [<output.ppm>]")
        return 1
    rsc_path = sys.argv[1]
    out_path = sys.argv[2] if len(sys.argv) > 2 else "cvpc_lzw_test.ppm"

    data = Path(rsc_path).read_bytes()
    fork = parse_rsrc_fork(data)
    cvpc_map = fork.get(b"CvPc", {})
    if not cvpc_map:
        print(f"no CvPc in {rsc_path}; available: {[c.decode('ascii', 'replace') for c in fork]}")
        return 1

    # 抓 id 200 (SPR32X32 in CIVDATA4)；如果不存在抓第一個
    rid = 200 if 200 in cvpc_map else sorted(cvpc_map.keys())[0]
    payload = cvpc_map[rid]
    print(f"Decoding CvPc id {rid}, payload {len(payload)} bytes")

    w_pre = struct.unpack(">H", payload[0:2])[0]
    h_pre = struct.unpack(">H", payload[2:4])[0]
    pixels, (w, h, palette, byte4) = decode_cvpc(payload, w_pre * h_pre)
    print(f"  decoded {len(pixels)} bytes, expected {w * h}")

    write_indexed_ppm(out_path, w, h, palette, pixels)
    print(f"  → {out_path}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
