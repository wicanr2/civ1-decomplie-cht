#!/bin/bash
cd /mnt/d/03_game_tmp/civ1-decomplie-cht
for f in HAM1000B HAM2000B HAM3000B; do
    echo "=== $f first 256 bytes ==="
    xxd -c 16 -l 256 team-a/dumps/sav/${f}.bin
done
