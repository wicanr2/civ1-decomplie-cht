# Phase log

## 2026-06-06 — Phase 0 kickoff

- Repo created: https://github.com/wicanr2/civ1-decomplie-cht
- WSL environment: Ubuntu 22.04.5 LTS on WSL2 kernel 6.6.114.1, JDK 21.0.11 pre-installed.
- Decision: drop prior Track B (`openciv1pp/`, derived from OpenCiv1) entirely; this repo is a clean-room rebuild from disassembly.
- Decision: no DOSBox / wine / reference-binary oracle. Validation comes from spec-derived fixtures, asset round-trip, and layout-only screenshot comparison against Track A's frozen Big5 build.

## 2026-06-06 — Toolchain and binary intake

- Ghidra 12.1.2 installed to `/opt/ghidra_12.1.2_PUBLIC` (symlinked `/opt/ghidra`); headless analyzer verified against JDK 21.
- Auxiliary tools installed via apt: `p7zip-full`, `binwalk`, `xxd`, `file`. (`radare2` / `rizin` not in Ubuntu 22.04 default repos — deferred; Ghidra alone is sufficient for the first pass.)
- Discovered the `CIV.EXE` initially supplied at `_sfx_build_civ1/game/CIV.EXE` is the Track A Big5-patched build (832,512 bytes), not a pristine 1993 original. Its non-resident-name table `MODULE_DESC` decodes as Big5 `'文明帝國 視窗版'` rather than `'CIVILIZATION for Windows'`.
- Pristine 1993 original located at `D:\03_game_tmp\win31\C\MPS\CIVWIN\CIV.EXE` (833,024 bytes; MD5 `DCC4399E57AEFE490914AC2F5A95CD92`; SHA-256 `F606568096376F8ED8989D74D17F36AB3B18ADFA7A2D56D89A4F29BEE5D94672`). This is the work-of-record; copied into `team-a/binary/CIV.EXE` (gitignored).

## 2026-06-06 — Spec 00: NE structure

- Wrote `team-a/tools/ne_dump.py`, a public-format NE-header / segment / import / resource dumper. Output captured in `team-a/dumps/00_ne_structure.txt`.
- Wrote first signed spec `team-a/specs/00_ne_structure.md` covering binary identity, memory model, 133 segments (~69 code + ~63 data + 1 autodata at segment 133), the six Win16 imports (KERNEL / USER / GDI / WIN87EM / MMSYSTEM / COMMDLG), the eleven exported callbacks (`WDWMAPPROC`, `TIMERPROC`, `CIVDIALOGPROC`, …), and the resource directory (24 `RT_DIALOG`, 1 `RT_MENU`, 16 `RT_CURSOR`, 1 `RT_ICON`, no `RT_STRING`, no `RT_RCDATA`, no `RT_VERSION`).
- Spec 00 sign-off pending Team A and Team B review.
- Ghidra headless auto-analysis on the pristine `CIV.EXE` queued for background execution to feed spec 01 (compiler identification + per-API call inventory).
