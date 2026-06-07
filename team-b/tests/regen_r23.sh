#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$SCRIPT_DIR/../build"
OUT_DIR="$SCRIPT_DIR/../../docs/screenshots"
ENV="SDL_VIDEODRIVER=dummy CIV1_DATA_DIR=/mnt/d/03_game_tmp/civ1/civ1win/civ1win"

# R23: regen splash + 4 m11 with R23 advisor cropping
for who in elizabeth frederick mao gandhi; do
    SDL_VIDEODRIVER=dummy CIV1_DATA_DIR=/mnt/d/03_game_tmp/civ1/civ1win/civ1win \
        ./tests/test_world_snapshot /tmp/r23_${who}.ppm diplomat-${who} 2>&1 | tail -1
    convert /tmp/r23_${who}.ppm "$OUT_DIR/m11_diplomat_${who}_zh.png"
done

# R23: CIV.GIF splash
SDL_VIDEODRIVER=dummy CIV1_DATA_DIR=/mnt/d/03_game_tmp/civ1/civ1win/civ1win \
    ./tests/test_world_snapshot /tmp/r23_splash.ppm splash 2>&1 | tail -2
convert /tmp/r23_splash.ppm "$OUT_DIR/m13_civ_title_splash.png"

# Main with R23 status panel (player_civ_slot hooked)
SDL_VIDEODRIVER=dummy CIV1_DATA_DIR=/mnt/d/03_game_tmp/civ1/civ1win/civ1win \
    ./tests/test_world_snapshot /tmp/r23_main.ppm 2>&1 | tail -1
convert /tmp/r23_main.ppm "$OUT_DIR/m10_main_zhtw.png"

echo DONE
