#!/usr/bin/env python3
"""
extract_tiles.py — Civ1 (1993 MicroProse Civilization for Windows) asset
research + extraction tool.

Phase 1 — asset format research; outputs are evidence, not deliverables.

WHAT THIS TOOL DOES
-------------------
1. Parse a Mac-style Resource Fork .RSC (Civdata0..4) and print an
   index of every resource inside.
2. For each CvPc (Civ Picture) resource: parse its 5-byte image
   header, slice off the LZW-bearing tail, and:
     a. Try a pure GIF89a LZW decode using a stand-alone decoder
        (fails today — see "Known LZW unknowns" below; documented in
        team-a/specs/03_asset_formats_and_tiles.md).
     b. Reconstruct a real GIF89a file by wrapping the CvPc payload in
        a fabricated GIF header + a grayscale palette and feed it to
        Pillow. If Pillow groks it we save the PNG; if not the .gif
        stays on disk as evidence + future oracle input.
3. Optionally slice any successfully decoded image into fixed-size
   tiles and dump each tile to a PNG.

SCOPE / NON-GOALS
-----------------
* No EDILZSS2 here — the .RSC files are *not* EDILZSS2 wrapped.
  EDILZSS2 lives one layer up around CIV.EX$, CIVFONTS.FO$, etc.
  (See Track A docs ../docs/TRACK_A_README.md §EDILZSS2.)
* No decompiling CIV.EXE — palette discovery (the missing piece) is
  on a sibling work item.

ORIGIN OF THE SPEC
------------------
* Mac Resource Fork: Apple "Inside Macintosh Vol I" (public). The
  source name `resmgr.c` listed in
  team-a/specs/01_compiler_and_api_surface.md §1.2 confirms Civ1 Win
  is a Mac port that retained the fork layout.
* CvPc 5-byte image header: empirically reverse-engineered in this
  tool (Track A docs explicitly say .RSC internals were never
  decoded).
* GIF89a LZW: CompuServe/W3C GIF89a spec (public), Appendix F. The
  reference C source `gr_pic.c LoadGifPicture` (named in spec §1.2)
  is a direct hint that the original `.gif` extension on CvPc res
  names is no accident — under the 5-byte CvPc framing the body
  really is GIF Image Data.

NOT USED FROM TRACK A
---------------------
Track A's `tools/edilzss2_decode.py` is *not* imported or referenced
in this implementation — that decoder targets an entirely different
container (the .EX$/.FO$ outer LZSS wrapper). CvPc uses GIF-derived
LZW, which is encoded differently (variable-width codes + KwKwK
dictionary build-up vs. EDILZSS2's 12-bit offset+length back-refs).

USAGE
-----
  # Index a .RSC:
  python extract_tiles.py Civdata3.rsc --list

  # Try to extract all CvPcs to PNG (whatever decodes):
  python extract_tiles.py Civdata3.rsc --all --output-dir out/

  # Slice an extracted PNG into 32x32 tiles afterward:
  python extract_tiles.py CIVDATA4.RSC --resource-id 200 \
      --tile-size 32 --output-dir out/

  # Single resource, also dump the raw LZW stream + reconstructed GIF
  # for further offline analysis:
  python extract_tiles.py CIVDATA4.RSC --resource-id 200 \
      --output-dir out/ --dump-evidence
"""
from __future__ import annotations

import argparse
import io
import os
import struct
import sys
from pathlib import Path
from typing import NamedTuple

try:
    from PIL import Image
except ImportError:
    print("error: Pillow not installed. pip install pillow", file=sys.stderr)
    sys.exit(2)


# ---------------------------------------------------------------------------
# Mac Resource Fork parser
# ---------------------------------------------------------------------------
#
# Layout (all multi-byte fields are big-endian):
#
#   0x00  u32  dataOffset
#   0x04  u32  mapOffset
#   0x08  u32  dataLength
#   0x0C  u32  mapLength
#   0x10  240B reserved
#
#   At mapOffset:
#     0x18  u16  typeListOffset (relative to mapOffset)
#     0x1A  u16  nameListOffset (relative to mapOffset)
#     0x1C  u16  numTypes - 1
#     0x1E  type list, 8 bytes per entry:
#                 4B type tag
#                 2B numResources - 1
#                 2B refListOffset (relative to typeListOffset)
#     ...   reference lists, 12 bytes per entry:
#                 2B s16 resource id
#                 2B u16 nameOffset (0xFFFF = no name)
#                 1B attributes
#                 3B u24 dataOffset (relative to file dataOffset; points
#                                    at a u32 length prefix)
#                 4B reserved
#
# (Cross-checked byte-for-byte against the actual Civdata0.RSC bytes.)


class ResourceEntry(NamedTuple):
    type_tag: str
    res_id: int
    name: str
    data_offset: int
    payload: bytes


def _u32_be(buf: bytes, off: int) -> int:
    return struct.unpack_from(">I", buf, off)[0]


def _u16_be(buf: bytes, off: int) -> int:
    return struct.unpack_from(">H", buf, off)[0]


def _s16_be(buf: bytes, off: int) -> int:
    return struct.unpack_from(">h", buf, off)[0]


def _u24_be(buf: bytes, off: int) -> int:
    return (buf[off] << 16) | (buf[off + 1] << 8) | buf[off + 2]


def parse_resfork(buf: bytes) -> list[ResourceEntry]:
    data_off = _u32_be(buf, 0x00)
    map_off = _u32_be(buf, 0x04)
    type_list_rel = _u16_be(buf, map_off + 0x18)
    name_list_rel = _u16_be(buf, map_off + 0x1A)
    num_types_m1 = _u16_be(buf, map_off + 0x1C)
    if num_types_m1 == 0xFFFF:
        return []
    num_types = num_types_m1 + 1
    type_list_abs = map_off + type_list_rel
    name_list_abs = map_off + name_list_rel
    out: list[ResourceEntry] = []
    for t in range(num_types):
        entry = type_list_abs + 2 + t * 8
        tag = buf[entry : entry + 4].decode("latin-1", errors="replace")
        n_res = _u16_be(buf, entry + 4) + 1
        ref_list_rel = _u16_be(buf, entry + 6)
        ref_list_abs = type_list_abs + ref_list_rel
        for i in range(n_res):
            ref = ref_list_abs + i * 12
            rid = _s16_be(buf, ref)
            name_off = _u16_be(buf, ref + 2)
            d_rel = _u24_be(buf, ref + 5)
            d_abs = data_off + d_rel
            d_len = _u32_be(buf, d_abs)
            payload = bytes(buf[d_abs + 4 : d_abs + 4 + d_len])
            name = ""
            if name_off != 0xFFFF:
                n_abs = name_list_abs + name_off
                n_len = buf[n_abs]
                if n_len:
                    name = buf[n_abs + 1 : n_abs + 1 + n_len].decode(
                        "latin-1", errors="replace"
                    )
            out.append(
                ResourceEntry(
                    type_tag=tag,
                    res_id=rid,
                    name=name,
                    data_offset=d_abs,
                    payload=payload,
                )
            )
    return out


# ---------------------------------------------------------------------------
# CvPc image header parser
# ---------------------------------------------------------------------------
#
# Empirically derived. Spec text lives in
# team-a/specs/03_asset_formats_and_tiles.md §3.
#
#   off  size field
#   0x00  u16 width  (BE)
#   0x02  u16 height (BE)
#   0x04  u8  lzwMinCodeSize / pixel-bits (observed: 6, 7, 8)
#   0x05  ... GIF-style Image Data tail:
#              [u8 sub-block size][N bytes]...[u8 0 = terminator]
#              feeding a variable-width LZW stream whose semantics
#              EXACTLY MATCH GIF89a Appendix F at the bit-stream level.
#
# Observed but unexplained:
#   * On every CvPc, the first 8-bit LZW code is `CLEAR` (= 1<<root),
#     as expected. The next ~15 codes are literal pixel indices that
#     match a gray-ramp pattern (0,0,0,32,32,32,64,...). Then the
#     stream produces a code value that exceeds the current
#     dictionary size by ~30 entries. We have not yet figured out
#     whether this is (a) a non-standard early code-size bump, (b) a
#     custom dictionary pre-seed, or (c) a private RLE+LZW hybrid as
#     used by the 1991 DOS Civ `.pic` codec (PROJECT_MEMORY notes
#     "RLE+LZW+18bit palette"). All three are listed as
#     待解問題 in the spec doc.


class CvPcHeader(NamedTuple):
    width: int
    height: int
    lzw_min_code_size: int


def parse_cvpc_header(payload: bytes) -> CvPcHeader:
    if len(payload) < 6:
        raise ValueError("CvPc payload too short")
    w = _u16_be(payload, 0)
    h = _u16_be(payload, 2)
    lzw_root = payload[4]
    return CvPcHeader(w, h, lzw_root)


# ---------------------------------------------------------------------------
# Path A — pure GIF89a LZW decoder (stand-alone, public spec)
# ---------------------------------------------------------------------------
#
# Re-derived from the GIF89a Appendix F text, NOT copied from any other
# source. As of writing this decoder cannot make it past ~16 codes on
# real CvPc data (see "Known LZW unknowns" above). We keep it in the
# tool because (a) it is correct for *real* GIF89a files and so any
# future fix to the format wedge can be plugged in by tweaking the
# pre-decode setup, and (b) it documents the algorithm we believe the
# original `LoadGifPicture` ran.


def _collect_lzw_subblocks(payload: bytes, start: int) -> bytes:
    out = bytearray()
    p = start
    n = len(payload)
    while p < n:
        size = payload[p]
        p += 1
        if size == 0:
            break
        if p + size > n:
            out += payload[p:n]
            break
        out += payload[p : p + size]
        p += size
    return bytes(out)


def gif_lzw_decode(compressed: bytes, root_size: int) -> bytes:
    CLEAR = 1 << root_size
    EOI = CLEAR + 1
    out = bytearray()
    buf = 0
    nbits = 0
    bp = 0
    n = len(compressed)
    code_size = root_size + 1
    dictionary: list[bytes] = [bytes([i]) for i in range(CLEAR)] + [b"", b""]
    prev_code: int | None = None
    while True:
        while nbits < code_size:
            if bp >= n:
                return bytes(out)
            buf |= compressed[bp] << nbits
            nbits += 8
            bp += 1
        code = buf & ((1 << code_size) - 1)
        buf >>= code_size
        nbits -= code_size
        if code == CLEAR:
            code_size = root_size + 1
            dictionary = [bytes([i]) for i in range(CLEAR)] + [b"", b""]
            prev_code = None
            continue
        if code == EOI:
            return bytes(out)
        if code < len(dictionary):
            entry = dictionary[code]
            if not entry and code >= CLEAR:
                return bytes(out)
            if prev_code is not None and len(dictionary) < 4096:
                dictionary.append(dictionary[prev_code] + entry[:1])
        elif code == len(dictionary):
            if prev_code is None:
                return bytes(out)
            entry = dictionary[prev_code] + dictionary[prev_code][:1]
            if len(dictionary) < 4096:
                dictionary.append(entry)
        else:
            return bytes(out)
        out.extend(entry)
        prev_code = code
        if len(dictionary) == (1 << code_size) and code_size < 12:
            code_size += 1


# ---------------------------------------------------------------------------
# Path B — wrap CvPc payload as a real GIF89a and hand to Pillow
# ---------------------------------------------------------------------------
#
# If Path A ever fails (which it always does today), we fall back to
# letting Pillow's battle-tested giflib do the decode. We synthesise a
# minimal GIF89a around the CvPc bytes. Today this *also* fails (PIL
# says "broken data stream"), which is itself evidence: it confirms the
# CvPc body is not literally a vanilla GIF89a Image Data section. The
# .gif files this path writes are kept on disk as a tarball of evidence
# for the next iteration (e.g., feeding them to other GIF decoders, or
# to a decompiled gr_pic.c::LoadGifPicture in Ghidra).


def _grayscale_lct(n: int) -> bytes:
    out = bytearray()
    for i in range(n):
        v = (i * 255) // max(1, n - 1)
        out += bytes([v, v, v])
    return bytes(out)


def wrap_as_gif89a(hdr: CvPcHeader, payload_tail: bytes) -> bytes:
    """Build a GIF89a file whose Image Data is `payload_tail` (already
    sub-block framed). Local color table is a grayscale ramp sized to
    2**hdr.lzw_min_code_size entries."""
    w, h, bpp = hdr.width, hdr.height, hdr.lzw_min_code_size
    lct = _grayscale_lct(1 << bpp)
    out = bytearray(b"GIF89a")
    out += struct.pack("<HH", w, h)
    # LSD packed: no global color table.
    out += bytes([0x00, 0x00, 0x00])
    # Image Descriptor.
    out += b"\x2C"
    out += struct.pack("<HHHH", 0, 0, w, h)
    # Packed: local CT (0x80), size bits = bpp - 1.
    out += bytes([0x80 | ((bpp - 1) & 0x07)])
    out += lct
    # Image Data.
    out += bytes([bpp])
    out += payload_tail
    if not payload_tail or payload_tail[-1] != 0x00:
        out += b"\x00"
    out += b"\x3B"
    return bytes(out)


def try_pillow_decode(hdr: CvPcHeader, payload_tail: bytes) -> Image.Image | None:
    gif_bytes = wrap_as_gif89a(hdr, payload_tail)
    try:
        img = Image.open(io.BytesIO(gif_bytes))
        img.load()
        return img.convert("P")
    except Exception:
        return None


# ---------------------------------------------------------------------------
# Palette + image rendering
# ---------------------------------------------------------------------------


def _gradient_palette() -> list[tuple[int, int, int]]:
    pal = []
    for i in range(256):
        r = i
        g = (i * 5 + 32) & 0xFF
        b = (i * 11 + 64) & 0xFF
        pal.append((r, g, b))
    return pal


def _load_external_palette(path: Path) -> list[tuple[int, int, int]]:
    raw = path.read_bytes()
    if len(raw) == 256 * 3:
        return [tuple(raw[i * 3 : i * 3 + 3]) for i in range(256)]
    if len(raw) == 128 * 3:
        pal = [tuple(raw[i * 3 : i * 3 + 3]) for i in range(128)]
        pal += [(0, 0, 0)] * 128
        return pal
    raise ValueError(
        f"palette file size {len(raw)} not 768 (256x3) or 384 (128x3)"
    )


def render_image(
    width: int,
    height: int,
    pixels: bytes,
    palette: list[tuple[int, int, int]],
) -> Image.Image:
    expected = width * height
    if len(pixels) < expected:
        pixels = pixels + bytes(expected - len(pixels))
    img = Image.new("P", (width, height))
    flat_pal: list[int] = []
    for r, g, b in palette:
        flat_pal.extend([r, g, b])
    while len(flat_pal) < 256 * 3:
        flat_pal.extend([0, 0, 0])
    img.putpalette(flat_pal[: 256 * 3])
    img.frombytes(pixels[:expected])
    return img


def slice_tiles(img: Image.Image, tile_size: int) -> list[tuple[int, int, Image.Image]]:
    out = []
    w, h = img.size
    for ry in range(h // tile_size):
        for cx in range(w // tile_size):
            box = (cx * tile_size, ry * tile_size, (cx + 1) * tile_size, (ry + 1) * tile_size)
            out.append((cx, ry, img.crop(box)))
    return out


# ---------------------------------------------------------------------------
# Driver
# ---------------------------------------------------------------------------


def safe_name(s: str) -> str:
    keep = "".join(c if c.isalnum() or c in "._-" else "_" for c in s)
    return keep or "noname"


def process_resource(
    res: ResourceEntry,
    palette: list[tuple[int, int, int]],
    tile_size: int,
    out_dir: Path,
    dump_evidence: bool,
) -> dict:
    hdr = parse_cvpc_header(res.payload)
    # NOTE: GIF sub-block defragmentation walks `size,data...` until it
    # hits a 0 byte. CvPc payloads contain stray 0 bytes well inside
    # the real LZW data, so the walk would stop ~300 bytes in and we
    # would feed Path A almost nothing. As a temporary workaround we
    # also keep the raw tail (payload[5:]) and try Path A on both.
    lzw_stream = _collect_lzw_subblocks(res.payload, 5)
    raw_tail = res.payload[5:]

    info = {
        "id": res.res_id,
        "name": res.name,
        "width": hdr.width,
        "height": hdr.height,
        "lzw_root": hdr.lzw_min_code_size,
        "compressed_bytes": len(lzw_stream),
        "expected_pixels": hdr.width * hdr.height,
        "path_a_decoded": None,
        "path_b_pil": False,
        "image_path": None,
        "tiles_dumped": 0,
    }
    base = f"{safe_name(res.name) or 'cvpc'}_id{res.res_id}"

    # Path A — stand-alone decoder. Try both the sub-block defragmented
    # version and the raw tail; keep whichever decoded more pixels.
    px_subblock = gif_lzw_decode(lzw_stream, hdr.lzw_min_code_size)
    px_raw = gif_lzw_decode(raw_tail, hdr.lzw_min_code_size)
    px = px_subblock if len(px_subblock) >= len(px_raw) else px_raw
    info["path_a_decoded"] = len(px)

    # Path B — Pillow on a reconstructed GIF89a (uses payload[5:] raw,
    # which still includes the sub-block framing).
    img = try_pillow_decode(hdr, res.payload[5:])
    if img is not None:
        info["path_b_pil"] = True

    # Pick the best image source.
    final_img: Image.Image | None = None
    if img is not None:
        final_img = img
    elif len(px) >= hdr.width * hdr.height // 8:
        # Path A produced at least an eighth of the image — render it
        # as a partial debug image.
        final_img = render_image(hdr.width, hdr.height, px, palette)

    if final_img is not None:
        img_path = out_dir / f"{base}_W{hdr.width}xH{hdr.height}.png"
        final_img.save(img_path)
        info["image_path"] = str(img_path)
        if tile_size > 0 and hdr.width >= tile_size and hdr.height >= tile_size:
            tiles = slice_tiles(final_img, tile_size)
            for cx, ry, sub in tiles:
                sub.save(out_dir / f"{base}_tile_r{ry:02d}_c{cx:02d}.png")
            info["tiles_dumped"] = len(tiles)

    if dump_evidence:
        ev_dir = out_dir / "_evidence"
        ev_dir.mkdir(parents=True, exist_ok=True)
        # Raw LZW stream after sub-block defragmentation.
        (ev_dir / f"{base}.lzwstream.bin").write_bytes(lzw_stream)
        # Reconstructed GIF89a (whatever PIL did with it).
        (ev_dir / f"{base}.reconstructed.gif").write_bytes(
            wrap_as_gif89a(hdr, res.payload[5:])
        )
        # Whatever Path A managed to decode (partial palette indices).
        (ev_dir / f"{base}.pathA_partial.bin").write_bytes(px)

    return info


def main() -> int:
    ap = argparse.ArgumentParser(description="Civ1 CvPc / tile research extractor")
    ap.add_argument("input", help="path to .RSC archive")
    ap.add_argument("--resource-id", type=int, default=None)
    ap.add_argument("--all", action="store_true", help="process every CvPc")
    ap.add_argument("--list", action="store_true", help="just list resources")
    ap.add_argument("--palette", type=Path, default=None)
    ap.add_argument("--tile-size", type=int, default=0)
    ap.add_argument("--output-dir", type=Path, default=Path("./out"))
    ap.add_argument("--dump-evidence", action="store_true",
                    help="dump raw lzw stream + reconstructed GIF for offline RE")
    args = ap.parse_args()

    in_path = Path(args.input)
    if not in_path.is_file():
        print(f"error: not a file: {in_path}", file=sys.stderr)
        return 2

    buf = in_path.read_bytes()
    entries = parse_resfork(buf)

    if args.list:
        by_type: dict[str, int] = {}
        for e in entries:
            by_type[e.type_tag] = by_type.get(e.type_tag, 0) + 1
        print(f"{in_path.name}: {len(entries)} resources")
        for t, n in by_type.items():
            print(f"  type='{t}' count={n}")
        for e in entries:
            print(f"  id={e.res_id:5d} type='{e.type_tag}' "
                  f"size={len(e.payload):7d} name='{e.name}'")
        return 0

    palette = (
        _load_external_palette(args.palette)
        if args.palette
        else _gradient_palette()
    )
    args.output_dir.mkdir(parents=True, exist_ok=True)

    cvpcs = [e for e in entries if e.type_tag == "CvPc"]
    print(f"[info] {in_path.name}: {len(entries)} total resources, {len(cvpcs)} CvPc")

    if args.all:
        targets = cvpcs
    elif args.resource_id is not None:
        targets = [e for e in cvpcs if e.res_id == args.resource_id]
        if not targets:
            print(f"error: CvPc id={args.resource_id} not found", file=sys.stderr)
            return 1
    else:
        targets = cvpcs[:1]

    a_ok = 0
    b_ok = 0
    for r in targets:
        try:
            info = process_resource(
                r, palette, args.tile_size, args.output_dir, args.dump_evidence,
            )
        except Exception as e:
            print(f"[fail] id={r.res_id} '{r.name}': {e}", file=sys.stderr)
            continue
        path_a_ok = info["path_a_decoded"] >= info["expected_pixels"] // 8
        if path_a_ok: a_ok += 1
        if info["path_b_pil"]: b_ok += 1
        print(
            f"[res]  id={info['id']:5d} '{info['name']:24s}' "
            f"{info['width']}x{info['height']} "
            f"lzw_root={info['lzw_root']} "
            f"comp={info['compressed_bytes']} "
            f"pathA_px={info['path_a_decoded']:6d}/{info['expected_pixels']:6d} "
            f"pathB_pil={info['path_b_pil']} "
            f"tiles={info['tiles_dumped']}"
        )
    print(f"[done] Path A >=1/8 decoded: {a_ok}/{len(targets)}, "
          f"Path B Pillow OK: {b_ok}/{len(targets)}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
