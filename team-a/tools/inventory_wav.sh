#!/bin/bash
DIR=/mnt/d/03_game_tmp/win31/C/MPS/CIVWIN
echo "# total $(ls $DIR/*.WAV | wc -l) WAV files"
for f in $DIR/*.WAV; do
    name=$(basename $f)
    size=$(stat -c%s $f)
    # WAV header bytes 20-21 = format code, 22-23 = num channels, 24-27 = sample rate
    fmt=$(xxd -s 20 -l 4 -c 4 $f | awk '{print $2}')
    rate=$(xxd -s 24 -l 4 -c 4 $f | awk '{print $2}')
    printf "  %-15s  %7d byte  fmt=%s  rate=%s\n" "$name" "$size" "$fmt" "$rate"
done
