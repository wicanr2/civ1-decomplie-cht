# Phase log

## 2026-06-06 — Phase 0 kickoff

- Repo created: https://github.com/wicanr2/civ1-decomplie-cht
- WSL environment: Ubuntu 22.04.5 LTS on WSL2 kernel 6.6.114.1, JDK 21.0.11 pre-installed.
- Decision: drop prior Track B (`openciv1pp/`, derived from OpenCiv1) entirely; this repo is a clean-room rebuild from disassembly.
- Decision: no DOSBox / wine / reference-binary oracle. Validation comes from spec-derived fixtures, asset round-trip, and layout-only screenshot comparison against Track A's frozen Big5 build.

## 2026-06-06 — Toolchain and binary intake

- Ghidra 12.1.2 installed to `/opt/ghidra_12.1.2_PUBLIC` (symlinked `/opt/ghidra`); headless analyzer verified against JDK 21.
- Auxiliary tools installed via apt: `p7zip-full`, `binwalk`, `xxd`, `file`. (`radare2` / `rizin` not in Ubuntu 22.04 default repos — deferred; Ghidra alone is sufficient for the first pass.)
- Discovered the `CIV.EXE` initially supplied at `_sfx_build_civ1/game/CIV.EXE` is the Track A Big5-patched build (832,512 bytes, MD5 `336FF646…`). Its non-resident-name table `MODULE_DESC` decodes as Big5 `'文明帝國 視窗版'` rather than `'CIVILIZATION for Windows'`.
- Pristine 1993 English binary also located at `D:\03_game_tmp\win31\C\MPS\CIVWIN\CIV.EXE` (833,024 bytes, MD5 `DCC4399E…`).
- The two binaries are structurally identical (same 133-segment layout, same 6 imports, same 11 exports); Track A's patches modify only inline string slots and `RT_DIALOG` labels, not code.
- After provisionally pinning the pristine binary as work-of-record and writing spec 00 against it, the user authorized using the Track A Big5-patched binary instead — spec 00, README, and `team-a/binary/CIV.EXE` updated accordingly. The patched binary is the work-of-record going forward because (a) the user's SFX portable build chain targets it, so visual-layout validation uses the same artifact, and (b) Big5 inline-string references in the disassembly directly populate Team B's translation catalog.
- Ghidra was first run against the pristine binary as a sanity check (total analysis time 56 s; Ghidra auto-detected `New Executable (NE)` / `x86:LE:16:Protected Mode` and applied built-in Win16 export symbol tables for KERNEL/USER/GDI/WIN87EM/MMSYSTEM/COMMDLG). That project was deleted and re-imported against the Big5-patched binary.

## 2026-06-06 — Spec 00: NE structure

- Wrote `team-a/tools/ne_dump.py`, a public-format NE-header / segment / import / resource dumper. Output captured in `team-a/dumps/00_ne_structure.txt`.
- Wrote first signed spec `team-a/specs/00_ne_structure.md` covering binary identity, memory model, 133 segments (~69 code + ~63 data + 1 autodata at segment 133), the six Win16 imports (KERNEL / USER / GDI / WIN87EM / MMSYSTEM / COMMDLG), the eleven exported callbacks (`WDWMAPPROC`, `TIMERPROC`, `CIVDIALOGPROC`, …), and the resource directory (24 `RT_DIALOG`, 1 `RT_MENU`, 16 `RT_CURSOR`, 1 `RT_ICON`, no `RT_STRING`, no `RT_RCDATA`, no `RT_VERSION`).
- Spec 00 sign-off pending Team A and Team B review.
- Ghidra headless auto-analysis on the pristine `CIV.EXE` queued for background execution to feed spec 01 (compiler identification + per-API call inventory).
