#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$SCRIPT_DIR/../build"
OUT_DIR="$SCRIPT_DIR/../../docs/screenshots"
for who in elizabeth frederick mao gandhi; do
    SDL_VIDEODRIVER=dummy CIV1_DATA_DIR=/mnt/d/03_game_tmp/civ1/civ1win/civ1win \
        ./tests/test_world_snapshot /tmp/r20_${who}.ppm diplomat-${who} 2>&1 | tail -1
    convert /tmp/r20_${who}.ppm "$OUT_DIR/m11_diplomat_${who}_zh.png"
done
echo DONE
