#!/usr/bin/env python3
"""掃 SAV dump + 5 個 RSC 內找 manual ground-truth unit stats array"""

import os
import glob

ATTACK_28  = bytes([0,1,1,3,2,3,2,4,6,8,4,10,6,12,3,12,1,1,2,4,6,18,8,1,0,99,0,0])
DEFENSE_28 = bytes([1,1,2,1,3,5,1,2,1,1,1,5,6,2,3,1,0,1,2,4,6,12,2,12,3,0,0,1])
MOVE_28    = bytes([1,1,1,1,1,1,2,2,1,1,2,3,3,2,10,8,3,3,3,4,6,4,3,5,4,16,2,1])
COST_28    = bytes([40,10,20,20,30,30,20,40,40,40,40,80,50,60,60,120,40,40,40,60,80,160,50,160,50,160,30,50])

PATTERNS = {
    'attack': ATTACK_28,
    'defense': DEFENSE_28,
    'move':   MOVE_28,
    'cost':   COST_28,
}


def scan_file(path, name):
    with open(path, 'rb') as f:
        buf = f.read()
    hits_any = False
    for label, pat in PATTERNS.items():
        for N in [28, 20, 14, 10, 8, 6]:
            p = pat[:N]
            found = []
            start = 0
            while True:
                idx = buf.find(p, start)
                if idx < 0: break
                found.append(idx)
                start = idx + 1
                if len(found) > 20: break
            if found:
                print(f"  {name} {label} N={N}: {len(found)} hit @ {[hex(h) for h in found[:5]]}")
                hits_any = True
                if N >= 14: break  # 找到大 prefix, 跳過更短
    if not hits_any:
        print(f"  {name}: no hit")


def main():
    base = '/mnt/d/03_game_tmp/civ1-decomplie-cht/team-a/dumps/sav'
    for f in sorted(glob.glob(f'{base}/HAM*.bin')):
        scan_file(f, os.path.basename(f))

    base = '/mnt/d/03_game_tmp/win31/C/MPS/CIVWIN'
    for f in sorted(glob.glob(f'{base}/Civdata*.RSC')) + [f'{base}/CIV.EXE']:
        scan_file(f, os.path.basename(f))


if __name__ == '__main__':
    main()
