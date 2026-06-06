#!/bin/bash
set -e
cd /mnt/d/03_game_tmp/civ1-decomplie-cht
gcc -I team-b/src team-a/tools/civ_sav_extract.c team-b/build/libciv_core.a -lSDL2 -lfreetype -lm -o /tmp/civ_sav_extract
mkdir -p team-a/dumps/sav
for f in HAM1000B HAM2000B HAM3000B; do
    /tmp/civ_sav_extract /mnt/d/03_game_tmp/win31/C/MPS/CIVWIN/${f}.SAV team-a/dumps/sav/${f}.bin
done
ls -la team-a/dumps/sav/
