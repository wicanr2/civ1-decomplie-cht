# civ1-decomplie-cht

> Clean-room reverse engineering of **1993 MicroProse *Civilization for Windows*** (Win16 NE `CIV.EXE`) into portable **C99 + SDL2**, with built-in 繁體中文 localization.

## What this is

A from-scratch decompile-and-rewrite of the 1993 Windows version of Sid Meier's *Civilization*. No upstream open-source codebase is referenced (in particular, this project does **not** look at [OpenCiv1](https://codeberg.org/rhorvat/OpenCiv1) or any of its forks). No binary patching of the original `CIV.EXE`. No reliance on Win16 APIs (GDI / USER / MMSYSTEM) — SDL2 replaces all of them.

Output target: a single C99 codebase that compiles cross-platform via SDL2, with an internal palette framebuffer (320×200 / 640×480 native), bicubic upscaling to any window size, and a CJK glyph cache (16×16 inline / 24×24 headings) overlaid in the palette layer.

## Clean-room two-team protocol

| Team | Sees | Writes | Forbidden |
|------|------|--------|-----------|
| **Team A** | Ghidra disassembly + decompile of `CIV.EXE` | `team-a/specs/*.md` (prose, pseudo-code, constant tables, data structure descriptions) | Writing C code; reading `team-b/` |
| **Team B** | `team-a/specs/*.md` only | `team-b/src/**/*.{c,h}` (C99 + SDL2 implementation) | Opening Ghidra; reading the original binary; reading OpenCiv1 / civ-clones |

Each spec is signed off twice before implementation begins:
- Team A signs: "this spec describes the original behavior accurately"
- Team B signs: "this spec is implementable without further disassembly access"

The same human/agent may serve on both teams at different times, but never simultaneously, and never carries Ghidra-derived knowledge into a Team B session except via the signed spec.

## Repo layout

```
team-a/                       Team A workspace — disassembly side
  specs/                       Markdown specs (the clean-room interface)
  dumps/                       Ghidra decompile text exports (informational)
  notes/                       Internal scratch (not part of the spec)
  binary/                      Original CIV.EXE (gitignored, user-supplied)

team-b/                       Team B workspace — implementation side
  src/                         C99 + SDL2 source tree
  build/                       CMake build output (gitignored)
  tests/                       Unit tests against spec-derived fixtures
  assets/zh_TW/                Translation catalogs (CC BY-SA 4.0)

docs/
  CLEAN_ROOM.md                Protocol detail, sign-off workflow
  PHASE_LOG.md                 Phase-by-phase progress log
  REUSE_FROM_TRACK_A.md        What we cleanly pull from the prior Track A work

assets-extracted/             Extracted .pic / .pal / .txt from CIV.EXE (gitignored)
tools/                         Asset extraction scripts (EDILZSS2 etc., MIT)
```

## Status

| Phase | Content | Status |
|-------|---------|--------|
| 0 | WSL toolchain (Ghidra 11 / rizin / retdec), CIV.EXE intake, clean-room repo skeleton | 🟡 in progress |
| 1 | Asset extraction (reuse EDILZSS2 spec from Track A) | ⏳ |
| 2 | WinMain / message pump / API boundary inventory | ⏳ |
| 3 | Per-subsystem RE → C (loaders → renderer → input → audio → game state → AI → save) | ⏳ |
| 4 | SDL2 scaffolding + CJK 16/24px glyph cache | ⏳ |
| 5 | Ghidra-only validation oracle | ⏳ |

## Provenance

`CIV.EXE` reference (user-supplied binary, never committed):
- Size: 832,512 bytes (Track A Big5-patched 1993 *Civilization for Windows*; code segments unchanged from the pristine MicroProse release)
- MD5: `336FF64650F6391C65A8B804ADFC31C9`
- SHA-256: `720C5EA4EFD47FC7069A89C9A366A3A18A561A88E39F963865C6084EEC4DB022`
- NE signature confirmed at offset `0x60`; module name `WINCIV`; description `'文明帝國 視窗版'` (Big5)

The pristine 1993 English binary (`MODULE_DESC = 'CIVILIZATION for Windows'`, 833,024 bytes, MD5 `DCC4399E…`) exists at `D:\03_game_tmp\win31\C\MPS\CIVWIN\CIV.EXE` and is structurally identical (same 133 segments, same 6 imports, same 11 exports). Track A's patches modify only inline string slots and `RT_DIALOG` labels; no code byte differs between the two binaries. The Big5 patched version is the work-of-record so that spec validation against running screenshots uses the same artifact the user's portable build is shipped from.

## What this project does **not** do

- It does not distribute any part of the original game binary or assets.
- It does not embed disassembled bytes, function listings, or decompiler output in the implementation source tree.
- It does not modify, patch, or repackage `CIV.EXE` (that path is the parent project's Track A, frozen).

## License

- Code (`team-b/src/`, `tools/`): **MIT**
- Translations (`team-b/assets/zh_TW/`): **CC BY-SA 4.0**
- Specifications (`team-a/specs/`): **CC BY 4.0**
- *Sid Meier's Civilization for Windows* © 1993 MicroProse Software, Inc. — users must supply their own legal copy of the original binary.
