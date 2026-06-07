#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$SCRIPT_DIR/../build"
OUT_DIR="$SCRIPT_DIR/../../docs/screenshots"
DATA_DIR=/mnt/d/03_game_tmp/civ1/civ1win/civ1win

# 4 diplomat scenes
for who in elizabeth frederick mao gandhi; do
    SDL_VIDEODRIVER=dummy CIV1_DATA_DIR="$DATA_DIR" \
        ./tests/test_world_snapshot /tmp/r27color_${who}.ppm diplomat-${who} 2>&1 | tail -1
    convert /tmp/r27color_${who}.ppm "$OUT_DIR/m11_diplomat_${who}_zh.png"
done

# Main map with status panel KING icon + score
SDL_VIDEODRIVER=dummy CIV1_DATA_DIR="$DATA_DIR" \
    ./tests/test_world_snapshot /tmp/r27color_main.ppm 2>&1 | tail -1
convert /tmp/r27color_main.ppm "$OUT_DIR/m10_main_zhtw.png"

# CIV title splash
SDL_VIDEODRIVER=dummy CIV1_DATA_DIR="$DATA_DIR" \
    ./tests/test_world_snapshot /tmp/r27color_splash.ppm splash 2>&1 | tail -1
convert /tmp/r27color_splash.ppm "$OUT_DIR/m13_civ_title_splash.png"

echo DONE
