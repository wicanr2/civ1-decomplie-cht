#!/bin/bash
# Game test: run SDL2 rewrite snapshots for all UI screens
set -e
cd /mnt/d/03_game_tmp/civ1-decomplie-cht/team-b/build
export SDL_VIDEODRIVER=dummy
export CIV1_DATA_DIR=/mnt/d/03_game_tmp/civ1/civ1win/civ1win
OUT=/tmp/civ1_diff_us
mkdir -p "$OUT"

for mode in main city tech splash diplomat-elizabeth diplomat-frederick diplomat-mao diplomat-gandhi birth-1 birth-2 birth-3 birth-4 birth-5 birth-6 birth-7 birth-8; do
    echo "--- $mode ---"
    if ./tests/test_world_snapshot "$OUT/${mode}.ppm" "$mode" 2>&1 | tail -3; then
        convert "$OUT/${mode}.ppm" "$OUT/${mode}.png" 2>&1 || echo "  convert fail $mode"
    else
        echo "  RUN FAIL: $mode"
    fi
done

echo "=== outputs ==="
ls -la "$OUT"/*.png 2>&1 | head -30
