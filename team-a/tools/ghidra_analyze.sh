#!/usr/bin/env bash
# Run Ghidra headless analyzeHeadless on the work-of-record CIV.EXE.
# Idempotent: re-running overwrites the existing project.
#
# Project lives at /root/ghidra-projects/civ1/ (outside the repo so the
# .gpr / .rep working files do not bloat git or hit Windows filesystem
# performance penalties via /mnt/d).
set -euo pipefail

PROJDIR=/root/ghidra-projects
PROJNAME=civ1
BINARY=/mnt/d/03_game_tmp/civ1-decomplie-cht/team-a/binary/CIV.EXE
LOG=/tmp/ghidra_civ1.log

mkdir -p "$PROJDIR"
cd "$PROJDIR"

/opt/ghidra/support/analyzeHeadless \
    "$PROJDIR" "$PROJNAME" \
    -import "$BINARY" \
    -overwrite \
    -log "$LOG"

echo "Done. Log at $LOG"
ls -l "$PROJDIR/$PROJNAME.rep" || true
