#!/bin/bash
# 從 4 個 .RSC 抽全部 CvPc 出來
set -e
OUT=/mnt/d/03_game_tmp/civ1-decomplie-cht/assets-extracted/tiles
SCRIPT=/mnt/d/03_game_tmp/civ1-decomplie-cht/team-a/tools/extract_tiles.py
GAME=/mnt/d/03_game_tmp/_sfx_build_civ1/game

# 清掉舊的 evidence 檔
rm -rf "$OUT"
mkdir -p "$OUT"

for rsc in Civdata0.RSC CIVDATA2.RSC Civdata3.rsc CIVDATA4.RSC; do
    echo "=== $rsc ==="
    python3 "$SCRIPT" "$GAME/$rsc" --out-dir "$OUT" 2>&1 | tail -3
done

echo "==="
echo "extracted CvPc files:"
ls "$OUT"/*.png 2>/dev/null | wc -l
echo "total bytes:"
du -sh "$OUT"
