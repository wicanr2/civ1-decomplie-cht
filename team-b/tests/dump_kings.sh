#!/bin/bash
# R20 dev tool — dump 14 KING sprites for visual identification.
# 用後即丟; 識別出真實 sprite→leader mapping 後可刪.
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$SCRIPT_DIR/../build"
OUT_DIR="$SCRIPT_DIR/_king_dump"
mkdir -p "$OUT_DIR"
for i in 0 1 2 3 4 5 6 7 8 9 10 11 12 13; do
    SDL_VIDEODRIVER=dummy CIV1_DATA_DIR=/mnt/d/03_game_tmp/civ1/civ1win/civ1win \
        ./tests/test_world_snapshot /tmp/king_${i}.ppm diplomat-id-${i} 2>&1 | tail -1
    convert /tmp/king_${i}.ppm "$OUT_DIR/king_${i}.png"
done
ls "$OUT_DIR"/king_*.png | wc -l

# R21 dev tool — dump 3 GOVT*M backdrops (id 404/405/406, 939×320 each).
# 確認是否為 diplomat throne room scene (背景 + advisor + parchment + spear).
DUMP_TOOL="$SCRIPT_DIR/dump_cvpc.sh"
if [ -x "$DUMP_TOOL" ]; then
    for gid in 404 405 406; do
        SDL_VIDEODRIVER=dummy CIV1_DATA_DIR=/mnt/d/03_game_tmp/civ1/civ1win/civ1win \
            "$DUMP_TOOL" "$gid" > "$OUT_DIR/govt_$gid.ppm"
        convert "$OUT_DIR/govt_$gid.ppm" "$OUT_DIR/govt_$gid.png" 2>/dev/null
    done
fi
