#!/bin/bash
set -e
DEST=/mnt/d/03_game_tmp/civ1-decomplie-cht/docs/screenshots/gametest_2026-06-07
mkdir -p "$DEST/us" "$DEST/orig"
cp /tmp/civ1_diff_us/*.png "$DEST/us/"
cp /tmp/civ1_diff_orig/civ1_win_main_menu.jpg "$DEST/orig/" 2>/dev/null || true
cp /tmp/civ1_diff_orig/civ1_win_civilopedia_dropdown.png "$DEST/orig/"
cp /tmp/civ1_diff_orig/civ1_win_tech_discovery.png "$DEST/orig/"
cp /tmp/civ1_diff_orig/civ1_win_city_screen.png "$DEST/orig/"
cp /tmp/civ1_diff_orig/civ1_win_city_screen_rome.png "$DEST/orig/" 2>/dev/null || true
for f in /tmp/civ1_diff_orig/*.webp; do
    n=$(basename "$f" .webp)
    convert "$f" "$DEST/orig/${n}.png"
done
ls -la "$DEST/us/" "$DEST/orig/" | head -40
echo "---total---"
ls "$DEST/us/" "$DEST/orig/" | wc -l
