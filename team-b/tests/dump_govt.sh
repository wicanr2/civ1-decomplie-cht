#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$SCRIPT_DIR/../build"
OUT_DIR="$SCRIPT_DIR/_king_dump"
mkdir -p "$OUT_DIR"
RSC="/mnt/d/03_game_tmp/civ1/civ1win/civ1win/CIVDATA2.RSC"
for gid in 404 405 406; do
    ./tests/dump_cvpc "$RSC" "$gid" "/tmp/govt_${gid}.ppm" 2>&1
    convert "/tmp/govt_${gid}.ppm" "$OUT_DIR/govt_${gid}.png"
done
ls "$OUT_DIR"/govt_*.png
