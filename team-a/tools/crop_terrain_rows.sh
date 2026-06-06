#!/bin/bash
# Crop SPR32X32 cols 14-29 (x=448..960, w=512) into 12 row strips
set -e
cd /mnt/d/03_game_tmp/civ1-decomplie-cht
mkdir -p docs/screenshots/terrain_rows
for r in 0 1 2 3 4 5 6 7 8 9 10 11; do
    y=$((r * 32))
    convert docs/screenshots/cvpc_spr32x32_decoded.png \
        -crop "512x32+448+${y}" +repage \
        "docs/screenshots/terrain_rows/row${r}_cols14-29.png"
    # Also extract a single representative tile at col 22 (x=448+8*32=704)
    convert docs/screenshots/cvpc_spr32x32_decoded.png \
        -crop "32x32+704+${y}" +repage -filter point -resize 128x128 \
        "docs/screenshots/terrain_rows/row${r}_col22_zoom4x.png"
done
ls docs/screenshots/terrain_rows/
