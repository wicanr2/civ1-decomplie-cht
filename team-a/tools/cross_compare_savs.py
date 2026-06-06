#!/usr/bin/env python3
"""
cross_compare_savs.py — Civ I 1993 Win SAV 解壓後 cross-compare 工具

3 個 HAM*.bin 都是 107194 byte (fixed-size). 找 byte 差異模式:
  - 3 個都相同的 byte = 格式 marker / constant (signature)
  - 3 個都不同 = 純 state (turn-specific)
  - 2 個相同 1 個不同 = 漸進變化 (likely turn-dependent)

Output 分區報告 + 連續同類 byte stretch 視為候選 record.
"""

import sys
import os


def load_dumps(paths):
    dumps = []
    for p in paths:
        with open(p, 'rb') as f:
            dumps.append(f.read())
    n = len(dumps[0])
    for i, d in enumerate(dumps[1:], 1):
        assert len(d) == n, f"size mismatch: {paths[0]}={n} vs {paths[i]}={len(d)}"
    return dumps, n


def categorize(dumps, offset):
    """Categorize a byte position across 3 dumps.

    Returns 'C' (constant: all 3 same), 'V' (all different), 'M2' (2 of 3 same)
    """
    vals = [d[offset] for d in dumps]
    unique = len(set(vals))
    if unique == 1:
        return 'C', vals
    elif unique == 3:
        return 'V', vals
    else:
        return 'M', vals


def runs(categories, mode):
    """Find contiguous stretches matching mode. Return list of (start, length)."""
    rs = []
    i = 0
    n = len(categories)
    while i < n:
        if categories[i] == mode:
            start = i
            while i < n and categories[i] == mode:
                i += 1
            rs.append((start, i - start))
        else:
            i += 1
    return rs


def hex_dump(b, off, length):
    chunk = b[off:off + length]
    return ' '.join(f'{c:02X}' for c in chunk)


def main():
    if len(sys.argv) < 4:
        print("usage: cross_compare_savs.py file1.bin file2.bin file3.bin")
        sys.exit(2)
    dumps, n = load_dumps(sys.argv[1:4])
    print(f"# Cross-compare {n} byte dumps from {len(dumps)} SAVs")
    print(f"# Sources: {', '.join(os.path.basename(p) for p in sys.argv[1:4])}\n")

    cats = []
    for off in range(n):
        cat, _ = categorize(dumps, off)
        cats.append(cat)

    # Stats
    n_const   = cats.count('C')
    n_var     = cats.count('V')
    n_mid     = cats.count('M')
    print(f"## Byte category stats")
    print(f"  C (constant across 3): {n_const:>7} ({100*n_const/n:.1f}%)")
    print(f"  V (all different):     {n_var:>7} ({100*n_var/n:.1f}%)")
    print(f"  M (mixed 2-of-3):      {n_mid:>7} ({100*n_mid/n:.1f}%)")
    print()

    # Long constant stretches (signature / format markers)
    print("## Long constant stretches (>= 16 byte, top 20 by length)")
    c_runs = sorted(runs(cats, 'C'), key=lambda x: -x[1])[:20]
    for off, ln in c_runs:
        if ln < 16: break
        val_preview = hex_dump(dumps[0], off, min(ln, 24))
        print(f"  0x{off:05X} len={ln:>5}  {val_preview}{'...' if ln > 24 else ''}")
    print()

    # Long variable stretches (state)
    print("## Long all-different stretches (>= 8 byte, top 20)")
    v_runs = sorted(runs(cats, 'V'), key=lambda x: -x[1])[:20]
    for off, ln in v_runs:
        if ln < 8: break
        d1 = hex_dump(dumps[0], off, min(ln, 12))
        d2 = hex_dump(dumps[1], off, min(ln, 12))
        d3 = hex_dump(dumps[2], off, min(ln, 12))
        print(f"  0x{off:05X} len={ln:>5}")
        print(f"    HAM1: {d1}{'...' if ln > 12 else ''}")
        print(f"    HAM2: {d2}{'...' if ln > 12 else ''}")
        print(f"    HAM3: {d3}{'...' if ln > 12 else ''}")
    print()

    # HAM filenames: 1000/2000/3000 likely = BC year, so HAM3 (3000 BC) is EARLIEST
    # and HAM1 (1000 BC) is LATEST. Monotone field would be HAM3 < HAM2 < HAM1.
    print("## Candidate monotone LE16 fields (HAM3<HAM2<HAM1 = earlier→later, "
          "diff > 0 < 1024)")
    for off in range(n - 1):
        v1 = dumps[0][off] | (dumps[0][off+1] << 8)
        v2 = dumps[1][off] | (dumps[1][off+1] << 8)
        v3 = dumps[2][off] | (dumps[2][off+1] << 8)
        if v3 < v2 < v1 and v1 - v3 < 1024:
            print(f"  0x{off:05X} LE16: HAM3={v3} HAM2={v2} HAM1={v1}  Δ={v1-v3}")
    print()

    # Reverse direction (some HAM ordering uncertainty)
    print("## Reverse: HAM1<HAM2<HAM3 (in case filename ordering is forward)")
    for off in range(n - 1):
        v1 = dumps[0][off] | (dumps[0][off+1] << 8)
        v2 = dumps[1][off] | (dumps[1][off+1] << 8)
        v3 = dumps[2][off] | (dumps[2][off+1] << 8)
        if v1 < v2 < v3 and v3 - v1 < 1024 and v1 > 0:
            print(f"  0x{off:05X} LE16: HAM1={v1} HAM2={v2} HAM3={v3}  Δ={v3-v1}")
    print()

    # LE32
    print("## Candidate monotone LE32 fields (any direction, diff > 0 < 65536)")
    for off in range(n - 3):
        v1 = int.from_bytes(dumps[0][off:off+4], 'little')
        v2 = int.from_bytes(dumps[1][off:off+4], 'little')
        v3 = int.from_bytes(dumps[2][off:off+4], 'little')
        if v1 != v2 and v2 != v3 and v1 != v3:
            if 0 < v1 < v2 < v3 < (1 << 24) and v3 - v1 < 65536:
                print(f"  0x{off:05X} LE32 HAM1<2<3: {v1} → {v2} → {v3}")
            elif 0 < v3 < v2 < v1 < (1 << 24) and v1 - v3 < 65536:
                print(f"  0x{off:05X} LE32 HAM3<2<1: {v3} → {v2} → {v1}")
    print()

    # All variable byte positions with values
    print("## All variable (V-category) byte positions and values")
    for off in range(n):
        if cats[off] == 'V':
            v1 = dumps[0][off]
            v2 = dumps[1][off]
            v3 = dumps[2][off]
            print(f"  0x{off:05X}: HAM1=0x{v1:02X} HAM2=0x{v2:02X} HAM3=0x{v3:02X}")
    print()

    # ASCII / printable string regions in dump 0 (HAM1)
    print("## ASCII string-like regions in HAM1 (>= 4 printable ASCII chars)")
    in_str = False
    str_start = 0
    for off in range(n):
        b = dumps[0][off]
        printable = 0x20 <= b < 0x7F
        if printable:
            if not in_str:
                in_str = True
                str_start = off
        else:
            if in_str:
                ln = off - str_start
                if ln >= 4:
                    s = dumps[0][str_start:off].decode('ascii', errors='replace')
                    print(f"  0x{str_start:05X} len={ln}: {s!r}")
                in_str = False
    print()


if __name__ == '__main__':
    main()
