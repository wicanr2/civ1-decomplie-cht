#!/usr/bin/env python3
"""
scan_unit_stats.py — 在 CIV.EXE 內掃 28 個 unit stats 表的位置

Civ1 wiki 公開的 28 unit stats (前 5 個):
  Settlers  attack=0 def=1 move=1 cost=40
  Militia   attack=1 def=1 move=1 cost=10
  Phalanx   attack=1 def=2 move=1 cost=20
  Legion    attack=4 def=2 move=1 cost=40
  Musketeers attack=3 def=3 move=1 cost=60

若 unit struct 是 { u8 attack, u8 defense, ... }, 連續 5 unit 的 (a,d)
pair 是: (0,1) (1,1) (1,2) (4,2) (3,3)

掃 CIV.EXE 找此序列, 中間插值容忍 0..30 byte stride.
"""

import sys

# Civ1 28 unit (attack, defense, move, cost) - 1991 官方 manual (P40-42) ground-truth
# Wiki 對 Legion / Musketeers / Riflemen / Fighter 是錯的, 已用 manual 修正.
UNITS = [
    ("Settlers",   0, 1, 1,  40),
    ("Militia",    1, 1, 1,  10),
    ("Phalanx",    1, 2, 1,  20),
    ("Legion",     3, 1, 1,  20),   # manual fix: 不是 4-2
    ("Musketeers", 2, 3, 1,  30),   # manual fix: 不是 3-3
    ("Riflemen",   3, 5, 1,  30),   # manual fix: 不是 5-4
    ("Cavalry",    2, 1, 2,  20),
    ("Knights",    4, 2, 2,  40),
    ("Catapult",   6, 1, 1,  40),
    ("Cannon",     8, 1, 1,  40),
    ("Chariot",    4, 1, 2,  40),
    ("Armor",     10, 5, 3,  80),
    ("MechInf",    6, 6, 3,  50),
    ("Artillery", 12, 2, 2,  60),
    ("Fighter",    3, 3,10,  60),   # manual fix: 不是 4-2
    ("Bomber",    12, 1, 8, 120),
    ("Trireme",    1, 0, 3,  40),
    ("Sail",       1, 1, 3,  40),
    ("Frigate",    2, 2, 3,  40),   # manual fix: move=3 不是 4
    ("Ironclad",   4, 4, 4,  60),
    ("Cruiser",    6, 6, 6,  80),
    ("Battleship",18,12, 4, 160),
    ("Submarine",  8, 2, 3,  50),
    ("Carrier",    1,12, 5, 160),
    ("Transport",  0, 3, 4,  50),
    ("Nuclear",   99, 0,16, 160),
    ("Diplomat",   0, 0, 2,  30),
    ("Caravan",    0, 1, 1,  50),
]
# 簡化提取 (a,d) tuple for old code compat
UNITS_AD = [(name, a, d) for (name, a, d, m, c) in UNITS]
UNITS = UNITS_AD  # for downward compat with rest of script


def main(exe_path):
    with open(exe_path, 'rb') as f:
        buf = f.read()
    print(f"# CIV.EXE size {len(buf)} byte")
    print(f"# 28 unit (attack, defense) target sequence:")
    for u in UNITS:
        print(f"#   {u[0]:>11s}: a={u[1]:>2d} d={u[2]:>2d}")
    print()

    # Try stride 0..16 (各 unit 佔 2+stride byte)
    for stride in range(0, 16):
        record_size = 2 + stride
        # Pattern: 在 fixed offset 找連續 28 個 (a,d) 對, 間距 = record_size
        for start in range(len(buf) - record_size * 28):
            ok = True
            for i, (_, a, d) in enumerate(UNITS):
                off = start + i * record_size
                if buf[off] != a or buf[off + 1] != d:
                    ok = False
                    break
            if ok:
                print(f"** 命中 ** stride={stride} record_size={record_size} offset=0x{start:05X}")
                print(f"   首 record 後 0x10 byte dump:")
                for i in range(min(5, len(UNITS))):
                    off = start + i * record_size
                    print(f"   {UNITS[i][0]:>11s} @ 0x{off:05X}: {buf[off:off+record_size].hex(' ')}")
                return

    # 第二輪: 分離陣列 — attack 28 byte / defense 28 byte 連續
    print("28-unit struct pattern 沒命中, 試「分離陣列」模式")
    print()
    attack_arr  = bytes(u[1] for u in UNITS)
    defense_arr = bytes(u[2] for u in UNITS)
    print(f"  attack 28-byte: {attack_arr.hex(' ')}")
    print(f"  defense 28-byte: {defense_arr.hex(' ')}")
    print()

    a_hits = []
    d_hits = []
    for off in range(len(buf) - 28):
        if buf[off:off+28] == attack_arr:
            a_hits.append(off)
        if buf[off:off+28] == defense_arr:
            d_hits.append(off)
    print(f"  attack array 命中: {len(a_hits)} @ {[hex(h) for h in a_hits]}")
    print(f"  defense array 命中: {len(d_hits)} @ {[hex(h) for h in d_hits]}")
    print()

    # 第三輪: 寬鬆 (前 N 個 byte)
    for N in [10, 14, 20]:
        a_short = attack_arr[:N]
        d_short = defense_arr[:N]
        a_hits = [off for off in range(len(buf) - N) if buf[off:off+N] == a_short]
        d_hits = [off for off in range(len(buf) - N) if buf[off:off+N] == d_short]
        print(f"  N={N} attack 前綴: {len(a_hits)} hits @ {[hex(h) for h in a_hits[:5]]}")
        print(f"  N={N} defense 前綴: {len(d_hits)} hits @ {[hex(h) for h in d_hits[:5]]}")
    print()

    # 第四輪: cost array (1991 manual ground-truth)
    cost_arr = bytes([40,10,20,20,30,30,20,40,40,40,40,80,50,60,60,120,40,40,40,60,80,160,50,160,50,160,30,50])
    print(f"  cost 28-byte: {cost_arr.hex(' ')}")
    c_hits = [off for off in range(len(buf) - 28) if buf[off:off+28] == cost_arr]
    print(f"  cost array 命中: {len(c_hits)} @ {[hex(h) for h in c_hits]}")
    for N in [10, 14, 20]:
        c_short = cost_arr[:N]
        hits = [off for off in range(len(buf) - N) if buf[off:off+N] == c_short]
        print(f"  N={N} cost 前綴: {len(hits)} hits @ {[hex(h) for h in hits[:5]]}")
    print()

    # 第五輪: LE16 attack (萬一是 u16)
    attack_le16 = b''.join(a.to_bytes(2, 'little') for a in (u[1] for u in UNITS))
    print(f"  attack LE16 56-byte: 命中 {sum(1 for off in range(len(buf) - 56) if buf[off:off+56] == attack_le16)} 處")

    # 第六輪: cost / 10 (shield 單位)
    cost10 = bytes([c // 10 for c in [40,10,20,20,30,30,20,40,40,40,40,80,50,60,60,120,40,40,40,60,80,160,50,160,50,160,30,50]])
    print(f"\n  cost/10 28-byte: {cost10.hex(' ')}")
    hits = [off for off in range(len(buf) - 28) if buf[off:off+28] == cost10]
    print(f"  hit: {len(hits)} @ {[hex(h) for h in hits]}")
    for N in [10, 14, 20]:
        cs = cost10[:N]
        hits = [off for off in range(len(buf) - N) if buf[off:off+N] == cs]
        print(f"  N={N} cost/10 前綴: {len(hits)} hits @ {[hex(h) for h in hits[:5]]}")

    # 第七輪: 也在 .RSC 內掃 (Civdata0 / 1 / 2 / 3 / 4)
    print()
    print("# Civdata*.RSC 也掃一遍 (萬一 stats 在 RSC 不在 EXE)")
    import glob
    for rsc in sorted(glob.glob('/mnt/d/03_game_tmp/win31/C/MPS/CIVWIN/Civdata*.RSC')):
        with open(rsc, 'rb') as f:
            rb = f.read()
        for label, arr in [('attack', attack_arr), ('defense', defense_arr),
                            ('cost', cost_arr), ('cost/10', cost10)]:
            hits = [off for off in range(len(rb) - 28) if rb[off:off+28] == arr]
            if hits:
                print(f"  {rsc.split('/')[-1]} {label}: {len(hits)} hit @ {[hex(h) for h in hits[:3]]}")
        for N in [14]:
            for label, arr in [('attack', attack_arr), ('defense', defense_arr),
                                ('cost', cost_arr), ('cost/10', cost10)]:
                hits = [off for off in range(len(rb) - N) if rb[off:off+N] == arr[:N]]
                if hits:
                    print(f"  {rsc.split('/')[-1]} {label} N={N}: {len(hits)} hit @ {[hex(h) for h in hits[:3]]}")


if __name__ == '__main__':
    main(sys.argv[1] if len(sys.argv) > 1 else '/mnt/d/03_game_tmp/win31/C/MPS/CIVWIN/CIV.EXE')
