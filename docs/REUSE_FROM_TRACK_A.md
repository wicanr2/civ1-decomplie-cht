# Track A 可重用 / 不可重用清單

母專案 `D:\03_game_tmp\civ1_cht\` 跑了一條另外路線 — **Track A**：對 `CIV.EXE` 做 Win16 NE inline 二進位 patch，產出 5.21 MB Big5-patched portable build（用 otvdm）。該工作已凍結，**不**屬於本專案。但其中部分產出是純資料，可以乾淨取用。

## 可重用（資料，非實作）

| 項目 | `civ1_cht` 內位置 | 備註 |
|---|---|---|
| EDILZSS2 **格式規範** | `tools/edilzss2_decode.py`（只讀格式描述） | Team A 從 disassembly + 此格式描述重新推 spec；Team B 從 Team A 的 spec 重新實作 decoder — 不看 `edilzss2_decode.py` |
| `RT_DIALOG` resource ID 清單 | `tools/ne_dialog_patch.py`（只讀 resource layout） | 哪些 dialog ID 存在、各別 slot 結構是事實，不是實作 |
| 中文翻譯字串 catalog | `data/inline_translations.json`、`data/dialog_translations.json` | CC BY-SA 4.0；直接作為 `assets/zh_TW/` 種子 catalog |
| Track A Big5-build 的手動 playthrough 截圖 | `_sfx_build_civ1/` | 僅用於 **視覺 layout** 比對，不作為行為 oracle |

## **不**可重用（實作被其他 RE 污染）

| 項目 | 原因 |
|---|---|
| `tools/edilzss2_decode.py`（Python source 本身） | 實作。Team B 從 spec 重新寫 |
| `tools/ne_dialog_patch.py` | Track A patcher 實作 |
| `tools/inline_string_patch.py` | Track A patcher 實作 |
| `tools/ne_font_patch_charset.py` | Track A patcher 實作 |
| `openciv1pp/src/**` | **Track B** — 衍生自 OpenCiv1（MIT C# port）。本專案 clean-room 的主要對象就是這個。Team B 永不開該目錄 |

## 流程

1. Team A 從上表「可重用」項目讀格式 spec，在 `team-a/specs/` 寫一份全新、結構化的 spec。
2. spec PR 明確 cite 讀了哪些 Track A 檔（留 audit trail）。
3. Team B 從 spec 實作，不讀那些 cited Track A 檔。
