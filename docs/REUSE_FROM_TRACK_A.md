# What can be reused from Track A — and what cannot

The parent project `D:\03_game_tmp\civ1_cht\` ran a separate effort called **Track A**: a Win16 NE inline binary patch of `CIV.EXE` that produced a 5.21 MB Big5-patched portable build via otvdm. That work is frozen and is **not** part of this project. However, some of its outputs are pure data and can be cleanly used here.

## Reusable (data, not implementation)

| Item | Where in `civ1_cht` | Notes |
|------|---------------------|-------|
| EDILZSS2 **format specification** | `tools/edilzss2_decode.py` (read for format only) | Team A re-derives the spec from disassembly + this format description. Team B re-implements the decoder from Team A's spec — Team B does not see `edilzss2_decode.py`. |
| `RT_DIALOG` resource ID inventory | `tools/ne_dialog_patch.py` (read for resource layout only) | The list of which dialog IDs exist and their slot structure is fact, not implementation. |
| Chinese string catalog | `data/inline_translations.json`, `data/dialog_translations.json` | CC BY-SA 4.0. Used directly as the seed `assets/zh_TW/` catalog. |
| Manual playthrough screenshots of Track A's Big5 build | `_sfx_build_civ1/` | Used only for *visual layout* comparison, not behavioral oracle. |

## NOT reusable (implementation tainted by other RE)

| Item | Why not |
|------|---------|
| `tools/edilzss2_decode.py` (the actual Python code) | Implementation. Team B re-writes from spec. |
| `tools/ne_dialog_patch.py` | Track A patcher implementation. |
| `tools/inline_string_patch.py` | Track A patcher implementation. |
| `tools/ne_font_patch_charset.py` | Track A patcher implementation. |
| `openciv1pp/src/**` | **Track B** — derived from OpenCiv1 (MIT C# port). This is the primary thing we are doing a clean-room *against*. Team B never opens this directory. |

## Procedure

1. Team A reads format-spec items from the reusable list and writes a fresh, structured spec in `team-a/specs/`.
2. The spec PR explicitly cites which Track A files were read (for audit).
3. Team B implements from the spec without reading the cited Track A files.
