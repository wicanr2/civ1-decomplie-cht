#!/bin/bash
cd /mnt/d/03_game_tmp/civ1-decomplie-cht
echo "=== HAM1 0x0112..0x021F (post-king block) ==="
xxd -c 16 -s 0x0112 -l 0x110 team-a/dumps/sav/HAM1000B.bin
echo
echo "=== HAM1 0x0300..0x03FF (suspect player stats area) ==="
xxd -c 16 -s 0x0300 -l 256 team-a/dumps/sav/HAM1000B.bin
echo
echo "=== HAM1 0x06A0..0x06FF (more candidates) ==="
xxd -c 16 -s 0x06A0 -l 96 team-a/dumps/sav/HAM1000B.bin
echo
echo "=== HAM1 0x089A..0x09CF (32-byte stride records) ==="
xxd -c 16 -s 0x089A -l 0x140 team-a/dumps/sav/HAM1000B.bin
echo
echo "=== HAM1 0x09E0..0x0AAF area ==="
xxd -c 16 -s 0x09E0 -l 0xD0 team-a/dumps/sav/HAM1000B.bin
echo
echo "=== HAM1 0x0E000..0x0E0C0 (transition to long zeros) ==="
xxd -c 16 -s 0xE000 -l 0xC0 team-a/dumps/sav/HAM1000B.bin
echo
echo "=== HAM1 last 0x100 bytes ==="
xxd -c 16 -s -256 team-a/dumps/sav/HAM1000B.bin
