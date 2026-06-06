#!/usr/bin/env python3
"""掃 CIV.EXE / 5 RSC / 3 SAV 找 OpenCivOne 揭穿的 34-byte UnitDefinition struct.

Settlers first 34-byte packed:
  Name:    "Settlers" + 4 null = 12 byte
  Cancel:  127 (NewFutureTech) = 7F 00
  MoveType: 0 (Land)           = 00 00
  Move:    1                   = 01 00
  Outside: 0                   = 00 00
  Attack:  0                   = 00 00
  Defense: 1                   = 01 00
  Cost:    4                   = 04 00
  Sight:   0                   = 00 00
  Cargo:   0                   = 00 00
  AIRole:  0 (Settler)         = 00 00
  Required: -1 (None)          = FF FF
"""

import glob


def make_settlers_34():
    name = b"Settlers" + b"\x00" * 4         # 12
    fields = bytes([
        127, 0,   # cancel
        0, 0,     # movetype
        1, 0,     # move
        0, 0,     # outside
        0, 0,     # attack
        1, 0,     # defense
        4, 0,     # cost
        0, 0,     # sight
        0, 0,     # cargo
        0, 0,     # ai_role
        0xFF, 0xFF, # required
    ])
    return name + fields


def scan_file(path, settlers):
    with open(path, 'rb') as f:
        buf = f.read()
    name = b"Settlers"
    hits = []
    start = 0
    while True:
        idx = buf.find(name, start)
        if idx < 0:
            break
        hits.append(idx)
        start = idx + 1
    print(f"{path}: 'Settlers' string {len(hits)} hits @ {[hex(h) for h in hits[:20]]}")

    # check if any has the 34-byte UnitDefinition struct
    found_struct = []
    start = 0
    while True:
        idx = buf.find(settlers, start)
        if idx < 0:
            break
        found_struct.append(idx)
        start = idx + 1
    print(f"  full 34-byte Settlers struct: {len(found_struct)} hits @ {[hex(h) for h in found_struct]}")

    # try smaller prefix (name + a few fields)
    for prefix_len in [16, 20, 24, 28]:
        partial_hits = []
        start = 0
        while True:
            idx = buf.find(settlers[:prefix_len], start)
            if idx < 0: break
            partial_hits.append(idx)
            start = idx + 1
        if partial_hits:
            print(f"  prefix {prefix_len} byte: {len(partial_hits)} hits @ {[hex(h) for h in partial_hits[:5]]}")
            break


def main():
    settlers = make_settlers_34()
    print(f"Settlers 34-byte target: {settlers.hex(' ')}")
    print()

    for path in (['/mnt/d/03_game_tmp/win31/C/MPS/CIVWIN/CIV.EXE']
                 + sorted(glob.glob('/mnt/d/03_game_tmp/win31/C/MPS/CIVWIN/Civdata*.RSC'))
                 + sorted(glob.glob('/mnt/d/03_game_tmp/civ1-decomplie-cht/team-a/dumps/sav/HAM*.bin'))):
        scan_file(path, settlers)
        print()


main()
