#!/usr/bin/env python3
"""
NE (New Executable) header / segment / import / resource dumper.

Reads only the publicly documented NE format (Microsoft Win16 NE spec, also
described in the Win16 SDK and the OSDev wiki). Does not derive anything from
the input binary beyond what the format itself prescribes. Output is a
plain-text dump suitable for a Team A audit trail.

Usage:
    python3 ne_dump.py <path-to-ne-exe>
"""

import struct
import sys
from pathlib import Path


def u8(b, o):  return b[o]
def u16(b, o): return struct.unpack_from("<H", b, o)[0]
def u32(b, o): return struct.unpack_from("<I", b, o)[0]


def read_pstr(b, o):
    """Pascal-style length-prefixed string (1 byte length, then bytes)."""
    n = b[o]
    return b[o + 1 : o + 1 + n].decode("ascii", errors="replace"), n + 1


TARGET_OS = {
    0x00: "Unknown",
    0x01: "OS/2",
    0x02: "Windows",
    0x03: "DOS 4.x",
    0x04: "Windows 386",
    0x05: "BOSS",
}

NE_FLAGS = [
    (0x0001, "SINGLEDATA"),
    (0x0002, "MULTIPLEDATA"),
    (0x0004, "GLOBAL_INIT"),
    (0x0008, "PROTECTED_MODE_ONLY"),
    (0x0010, "8086"),
    (0x0020, "80286"),
    (0x0040, "80386"),
    (0x0080, "80x87"),
    (0x0700, "APP_TYPE_MASK"),  # 0x100=fullscreen, 0x200=winaware, 0x300=winpm
    (0x0800, "OS2_FAMILY"),
    (0x2000, "ERROR_IN_IMAGE"),
    (0x8000, "DLL_OR_DRIVER"),
]

SEG_FLAGS = [
    (0x0001, "DATA"),
    (0x0002, "ALLOC"),
    (0x0004, "LOADED"),
    (0x0010, "MOVABLE"),
    (0x0020, "SHARED"),
    (0x0040, "PRELOAD"),
    (0x0080, "READONLY"),
    (0x0100, "HAS_RELOC"),
    (0x1000, "DISCARD"),
]


def decode_flags(value, table):
    bits = [name for mask, name in table if (value & mask) == mask and mask not in (0x0700,)]
    app_type = {0x100: "FULLSCREEN", 0x200: "WIN_AWARE", 0x300: "WIN_PM"}.get(value & 0x0700)
    if app_type:
        bits.append(app_type)
    return bits


def dump(path):
    data = Path(path).read_bytes()
    print(f"# NE structure dump for {path}")
    print(f"# file size:    {len(data):,} bytes")
    print()

    # --- MZ header ---
    if data[:2] != b"MZ":
        sys.exit("not an MZ binary")
    e_lfanew = u32(data, 0x3C)
    print(f"## MZ stub")
    print(f"  e_lfanew = 0x{e_lfanew:X}")
    print()

    # --- NE header ---
    ne = e_lfanew
    sig = data[ne : ne + 2]
    if sig != b"NE":
        sys.exit(f"signature at 0x{ne:X} is {sig!r}, not 'NE'")

    linker_major = u8(data, ne + 0x02)
    linker_minor = u8(data, ne + 0x03)
    entry_tbl_off = u16(data, ne + 0x04)
    entry_tbl_len = u16(data, ne + 0x06)
    crc = u32(data, ne + 0x08)
    flags = u16(data, ne + 0x0C)
    autodata_seg = u16(data, ne + 0x0E)
    init_heap = u16(data, ne + 0x10)
    init_stack = u16(data, ne + 0x12)
    csip = u32(data, ne + 0x14)  # entry point CS:IP
    sssp = u32(data, ne + 0x18)
    seg_count = u16(data, ne + 0x1C)
    modref_count = u16(data, ne + 0x1E)
    nonres_name_len = u16(data, ne + 0x20)
    seg_tbl_off = u16(data, ne + 0x22)
    res_tbl_off = u16(data, ne + 0x24)
    resnam_tbl_off = u16(data, ne + 0x26)
    modref_tbl_off = u16(data, ne + 0x28)
    impname_tbl_off = u16(data, ne + 0x2A)
    nonres_tbl_off = u32(data, ne + 0x2C)
    movable_entry_count = u16(data, ne + 0x30)
    align_shift = u16(data, ne + 0x32)
    res_count = u16(data, ne + 0x34)
    exetype = u8(data, ne + 0x36)
    addflags = u8(data, ne + 0x37)
    fastload_off = u16(data, ne + 0x38)
    fastload_len = u16(data, ne + 0x3A)
    swap_min = u16(data, ne + 0x3C)
    wversion = u16(data, ne + 0x3E)

    alignment = 1 << align_shift

    print(f"## NE header @ 0x{ne:X}")
    print(f"  linker version:        {linker_major}.{linker_minor}")
    print(f"  entry-table off/len:   0x{entry_tbl_off:X} / {entry_tbl_len}")
    print(f"  CRC:                   0x{crc:08X}")
    print(f"  flags:                 0x{flags:04X} = {' | '.join(decode_flags(flags, NE_FLAGS)) or '0'}")
    print(f"  autodata segment:      {autodata_seg}")
    print(f"  init heap / stack:     {init_heap} / {init_stack} bytes")
    print(f"  entry point CS:IP:     {csip >> 16}:0x{csip & 0xFFFF:04X}")
    print(f"  initial SS:SP:         {sssp >> 16}:0x{sssp & 0xFFFF:04X}")
    print(f"  segment count:         {seg_count}")
    print(f"  module-ref count:      {modref_count}")
    print(f"  segment table off:     0x{seg_tbl_off:X}")
    print(f"  resource table off:    0x{res_tbl_off:X}")
    print(f"  resident-name off:     0x{resnam_tbl_off:X}")
    print(f"  module-ref table off:  0x{modref_tbl_off:X}")
    print(f"  imp-name table off:    0x{impname_tbl_off:X}")
    print(f"  nonres-name off:       0x{nonres_tbl_off:X} (len={nonres_name_len})")
    print(f"  movable entry count:   {movable_entry_count}")
    print(f"  sector alignment:      2^{align_shift} = {alignment} bytes")
    print(f"  resource count:        {res_count}")
    print(f"  exe type:              0x{exetype:02X} ({TARGET_OS.get(exetype, '?')})")
    print(f"  fastload off/len:      0x{fastload_off:X} / {fastload_len}")
    print(f"  win version expected:  0x{wversion:04X} ({wversion >> 8}.{wversion & 0xFF:02X})")
    print()

    # --- Segment table ---
    print(f"## Segment table ({seg_count} segments)")
    print(f"  {'#':>3}  {'sector':>8}  {'len':>8}  {'flags':>6}  {'minalloc':>9}  decoded")
    seg_tbl_abs = ne + seg_tbl_off
    for i in range(seg_count):
        e = seg_tbl_abs + i * 8
        sector = u16(data, e + 0)
        seglen = u16(data, e + 2)
        sflags = u16(data, e + 4)
        minalloc = u16(data, e + 6)
        # sector 0 = NULL, otherwise file offset = sector << align_shift
        file_off = sector << align_shift if sector else 0
        # seglen 0 means 65536
        actual_len = seglen if seglen else (65536 if sector else 0)
        decoded = " | ".join(decode_flags(sflags, SEG_FLAGS)) or "0"
        print(f"  {i+1:>3}  0x{file_off:06X}  {actual_len:>8}  0x{sflags:04X}  {minalloc:>9}  {decoded}")
    print()

    # --- Module reference / imported names ---
    print(f"## Imported modules ({modref_count})")
    modref_abs = ne + modref_tbl_off
    impname_abs = ne + impname_tbl_off
    imports = []
    for i in range(modref_count):
        name_off = u16(data, modref_abs + i * 2)
        name, _ = read_pstr(data, impname_abs + name_off)
        imports.append(name)
        print(f"  [{i+1}] {name}")
    print()

    # --- Resident name table (exports by name) ---
    print(f"## Resident-name table (module name + exports)")
    p = ne + resnam_tbl_off
    first = True
    while p < len(data):
        n = data[p]
        if n == 0:
            break
        name = data[p + 1 : p + 1 + n].decode("ascii", errors="replace")
        ordinal = u16(data, p + 1 + n)
        tag = "MODULE_NAME" if first else f"ord={ordinal}"
        print(f"  {tag:>14}  {name!r}")
        first = False
        p += 1 + n + 2
    print()

    # --- Non-resident name table (more exports by name, esp. module description) ---
    print(f"## Non-resident-name table")
    if nonres_tbl_off and nonres_name_len:
        p = nonres_tbl_off  # NB: this offset is FILE-RELATIVE per spec
        end = nonres_tbl_off + nonres_name_len
        first = True
        while p < end:
            n = data[p]
            if n == 0:
                break
            name = data[p + 1 : p + 1 + n].decode("ascii", errors="replace")
            ordinal = u16(data, p + 1 + n)
            tag = "MODULE_DESC" if first else f"ord={ordinal}"
            print(f"  {tag:>14}  {name!r}")
            first = False
            p += 1 + n + 2
    print()

    # --- Resource table (RT_*) ---
    print(f"## Resource table @ NE+0x{res_tbl_off:X}")
    rt = ne + res_tbl_off
    rt_align_shift = u16(data, rt)
    rt_align = 1 << rt_align_shift
    print(f"  resource alignment: 2^{rt_align_shift} = {rt_align} bytes")
    p = rt + 2
    rt_types = {
        0x8001: "RT_CURSOR", 0x8002: "RT_BITMAP", 0x8003: "RT_ICON",
        0x8004: "RT_MENU",   0x8005: "RT_DIALOG", 0x8006: "RT_STRING",
        0x8007: "RT_FONTDIR",0x8008: "RT_FONT",   0x8009: "RT_ACCELERATOR",
        0x800A: "RT_RCDATA", 0x800B: "RT_MESSAGETABLE",
        0x800C: "RT_GROUP_CURSOR", 0x800E: "RT_GROUP_ICON", 0x8010: "RT_VERSION",
    }
    while p < len(data):
        type_id = u16(data, p)
        if type_id == 0:
            break
        count = u16(data, p + 2)
        # reserved dword at p+4
        type_name = rt_types.get(type_id, f"0x{type_id:04X}")
        print(f"  type {type_name:18}  entries={count}")
        p += 8
        for _ in range(count):
            r_off_sectors = u16(data, p + 0)
            r_len_sectors = u16(data, p + 2)
            r_flags = u16(data, p + 4)
            r_id = u16(data, p + 6)
            # reserved dword at p+8
            file_off = r_off_sectors * rt_align
            file_len = r_len_sectors * rt_align
            id_str = (f"#{r_id & 0x7FFF}" if (r_id & 0x8000) else
                      f"name@NE+0x{res_tbl_off + r_id:X}")
            print(f"    id={id_str:<10}  file=0x{file_off:06X} len={file_len}  flags=0x{r_flags:04X}")
            p += 12
    print()

    # --- Per-segment imported procedure ordinals (count of calls into each module) ---
    # We can't decode reloc records without the segment payload, but we can at
    # least count reloc records per segment to gauge how API-heavy each segment is.
    print(f"## Per-segment relocation-record counts (proxy for API call density)")
    print(f"  {'seg':>3}  {'reloc_records':>13}  notes")
    for i in range(seg_count):
        e = seg_tbl_abs + i * 8
        sector = u16(data, e + 0)
        seglen = u16(data, e + 2)
        sflags = u16(data, e + 4)
        if not (sflags & 0x0100):  # HAS_RELOC
            print(f"  {i+1:>3}  {'-':>13}  no reloc table")
            continue
        file_off = sector << align_shift
        actual_len = seglen if seglen else 65536
        reloc_off = file_off + actual_len
        if reloc_off + 2 > len(data):
            print(f"  {i+1:>3}  {'??':>13}  reloc table truncated")
            continue
        reloc_count = u16(data, reloc_off)
        print(f"  {i+1:>3}  {reloc_count:>13}  reloc table @ 0x{reloc_off:X}")
    print()

    print("# end of dump")


if __name__ == "__main__":
    if len(sys.argv) != 2:
        sys.exit("usage: ne_dump.py <ne-binary>")
    dump(sys.argv[1])
