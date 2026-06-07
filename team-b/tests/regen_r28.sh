#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$SCRIPT_DIR/../build"
OUT_DIR="$SCRIPT_DIR/../../docs/screenshots"
DATA_DIR=/mnt/d/03_game_tmp/civ1/civ1win/civ1win

for who in elizabeth frederick mao gandhi; do
    SDL_VIDEODRIVER=dummy CIV1_DATA_DIR="$DATA_DIR" \
        ./tests/test_world_snapshot /tmp/r28_${who}.ppm diplomat-${who} 2>&1 | tail -1
    convert /tmp/r28_${who}.ppm "$OUT_DIR/m11_diplomat_${who}_zh.png"
done

SDL_VIDEODRIVER=dummy CIV1_DATA_DIR="$DATA_DIR" \
    ./tests/test_world_snapshot /tmp/r28_main.ppm 2>&1 | tail -1
convert /tmp/r28_main.ppm "$OUT_DIR/m10_main_zhtw.png"

SDL_VIDEODRIVER=dummy CIV1_DATA_DIR="$DATA_DIR" \
    ./tests/test_world_snapshot /tmp/r28_tech.ppm tech 2>&1 | tail -1
convert /tmp/r28_tech.ppm "$OUT_DIR/m10_tech_screen_showcase.png"

echo DONE
