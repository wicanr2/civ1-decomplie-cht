#!/bin/bash
# Crop SPR32X32 entire rows (all 46 cols x 32 px tall) for full visual inspection
set -e
cd /mnt/d/03_game_tmp/civ1-decomplie-cht
mkdir -p docs/screenshots/terrain_rows
for r in 0 1 2 3 4 5 6 7 8 9 10 11 12; do
    y=$((r * 32))
    if [ $y -ge 400 ]; then continue; fi
    convert docs/screenshots/cvpc_spr32x32_decoded.png \
        -crop "1472x32+0+${y}" +repage \
        "docs/screenshots/terrain_rows/full_row${r}.png"
done
ls docs/screenshots/terrain_rows/full_row*.png
