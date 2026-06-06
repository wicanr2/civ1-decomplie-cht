# Phase log

## 2026-06-06 — Phase 0 kickoff

- Repo created: https://github.com/wicanr2/civ1-decomplie-cht
- Target binary identified: `CIV.EXE` (Win16 NE), 832,512 bytes, MD5 `336FF64650F6391C65A8B804ADFC31C9`, NE signature confirmed at offset `0x60`.
- WSL environment: Ubuntu 22.04.5 LTS on WSL2 kernel 6.6.114.1, JDK 21.0.11 pre-installed (sufficient for Ghidra 11.x).
- Decision: drop prior Track B (`openciv1pp/`, derived from OpenCiv1) entirely; this repo is a clean-room rebuild from disassembly.
- Decision: no DOSBox / wine / reference-binary oracle. Validation comes from spec-derived fixtures, asset round-trip, and layout-only screenshot comparison against Track A's frozen Big5 build.
- Pending: Ghidra 11.x installation in WSL; first Team A session to dump NE structure (segments, exports, imports, resources).
