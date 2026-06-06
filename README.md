# civ1-decomplie-cht

> 對 **1993 MicroProse《文明帝國 視窗版》**（Win16 NE `CIV.EXE`）做 clean-room 反組譯，重寫成可攜的 **C99 + SDL2**，並內建繁體中文化。

## 專案目的

從 1993 Windows 版 *Sid Meier's Civilization* 的二進位檔從頭反組譯並重寫。不參考任何 upstream 的 open source 程式庫（特別是不看 [OpenCiv1](https://codeberg.org/rhorvat/OpenCiv1) 或其衍生分支）。不 patch 原始 `CIV.EXE`。也不依賴任何 Win16 API（GDI / USER / MMSYSTEM 全部由 SDL2 取代）。

輸出目標是一份單一的 C99 程式碼，透過 SDL2 跨平台編譯；內部維持 palette framebuffer（640×480 原生）；中文字模（內文 16×16、標題 24×24）由 FreeType MONO 在 palette 層合成。

## 目前 ship 狀態 (2026-06-06)

最後 ship: D-spec06 terrain ground-truth 對位

| Milestone | 範圍 | 狀態 | 證據 |
|---|---|---|---|
| **M0** | SDL 視窗 + 主迴圈 + ESC 退出 | ✅ | `ctest test_window_lifecycle` |
| **M1** | palette framebuffer 8bpp 640×480 + Bresenham/fill/frame + FreeType MONO + UTF-8 walker + CJK glyph cache | ✅ | `m1_demo.png` |
| **M2** | 3 widget skeleton (map/minimap/status) + dispatch table 模型 + modal_lock | ✅ | `m2_demo.png` |
| **M3** | Mac Resource Fork parser + CvPc decoder + EARTH 端到端 blit | ✅ | `m3_demo.png` |
| **M4-lite** | STR# 載入 + 14 文明 master + 14 領袖肖像 + zh-TW catalog 241 條 | ✅ | `m4_demo.png` |
| **M4-full** | Dialog framework + 3 頁新局精靈 (Difficulty → Civ → Name) | ✅ | `m4full_p1/p2/p3.png` |
| **M5-prep** | palette stomp RGB-nearest LUT remap (修 spec 03 §12 #3) | ✅ | — |
| **M5** | world map grid + sprite sheet + 端到端 blit (atlas debug 模式) | ✅ | `m5_demo.png` |
| **M5 真** | terrain enum → SPR32X32 (col, row) lookup 取代 atlas 平鋪 | ✅ | `m5_terrain_render.png` |
| **M5-B** | layout 對齊原版 — minimap 左上 / menu bar / status panel game state | ✅ | `m5b_layout_v2.png` |
| **M5-C** | terrain (col, row) ground-truth 對位 — 12 種 terrain → SPR32X32 真實座標 | ✅ | `m5c_terrain_groundtruth.png` |
| **M6-lite** | turn loop + 西元年 + AI mock counter | ✅ | `m6_status.png` |
| **M6-full** | unit move + 戰鬥 + 城市生產 + 存讀檔 | 🟡 阻 spec 06+07 | — |
| **M7** | 奇蹟 + 外交 + 勝利條件 | ❌ | — |

最後 ctest: 14/14 PASS，build 33/33 zero warning。

## Clean-room 雙隊制度

| 隊伍 | 可看 | 可寫 | 禁止 |
|---|---|---|---|
| **Team A** | `CIV.EXE` 的 Ghidra 反組譯與 decompile | `team-a/specs/*.md`（規格文件） | 寫 C 程式碼；讀 `team-b/` |
| **Team B** | 只看 `team-a/specs/*.md` | `team-b/src/**/*.{c,h}`（C99 + SDL2 實作） | 開 Ghidra；讀原始 binary；讀 OpenCiv1 / 其他 Civ clone |

詳細見 [`docs/CLEAN_ROOM.md`](docs/CLEAN_ROOM.md)。

## 反組譯 (RE) 覆蓋盤點

### 已 spec ✅

| spec | 範圍 | 關鍵發現 |
|---|---|---|
| [`00`](team-a/specs/00_ne_structure.md) | NE container / binary 身分 / 11 callback 列表 | module `WINCIV` / 133 segments / 6 imports |
| [`01`](team-a/specs/01_compiler_and_api_surface.md) | Borland C++ 1991 ID / 12 個原始 `.c` source 還原 / API call site 統計 (USER 970 / GDI 552 / KERNEL 667) | **1993 Win 版實質是 Mac port**（`mac.c` + `resmgr.c` Memory/Resource Manager shim）— 影響整個 loader 與資源模型 |
| [`02`](team-a/specs/02_startup_and_game_loop.md) | WinMain A-G 7 段 init / `while (DAT_12d8_24ee == 0) FUN_1088_0000()` 主迴圈 / 11 callback dispatch table 模板 | TIMERPROC 只是 counter decrement (不 post message); WDWMAP/SM/STATUS 同 9-entry table 同模板; modal_lock 短路 |
| [`03`](team-a/specs/03_asset_formats_and_tiles.md) | 5 個 `.RSC` 完整 breakdown / CvPc 格式 / LZW 解通 (185/199 sprite) | **沒有 .PIC 檔** (Track A 誤判)。所有資產在 5 個 Mac Resource Fork: CIVDATA0..4 + CIVHELP |
| [`04`](team-a/specs/04_dialogs_and_controls.md) | 24 個 RT_DIALOG 完整 parse + 控制項家族 | 全 dialog 用 `CIVDIALOG` 自製類別；caption 空字串自繪 |
| [`05`](team-a/specs/05_game_data_and_strings.md) | 33 STR# + 399 TEXT master tables 定位 | **master tables 不在 CIV.EXE** 而在 `Civdata0.RSC`；72 科技 / 46 建築 / 28 單位 / 24 地形 / 14 文明 / 14 領袖 / 6 政府 全名清單已抽出 |

### 待寫 spec ❌

| spec | 範圍 | 為什麼 deferred |
|---|---|---|
| **`06`** | 14 文明 / 14 領袖 / 72 科技 / 28 單位 / 46 建築/奇蹟 **數值表** (attack / defense / cost / movement / production / 科技 prereq DAG) | spec 05 line 104 明點：推測 hardcoded 在 code segment `const` 陣列；M6-full 戰鬥/生產直接阻擋於此 |
| **`07`** | combat 公式 + AI 決策 + 外交 + **存讀檔格式 (`load.c::RLLEncode/Decode`)** | spec 01 已點出 `load.c` 含 RLL encoder/decoder，**完全未抽** — M6-full save 必須做 |
| **`08`** | 音效 — MMSYSTEM 4 個 API call site + 23 個 WAV 資源未分類 | 不擋遊戲邏輯；可獨立後做 |
| **`09`** | 勝利條件 / 結局 / scoring | STR# 155-157 (Space 1/2/Archeologist) 文字側已 RE，演算法未碰 |

### 子系統 RE 狀態

| 子系統 | 對應 source 推測 | 已 RE | 缺口 |
|---|---|---|---|
| Renderer | `godpal/gr/gr_pic/gr_port/shape` | palette 模型、CvPc LZW、blit | tile blending 邊界規則；14/199 CvPc 仍 LZW variant fail |
| Loader (Mac shim) | `mac/resmgr` | RSC parser + 5 個 archive | 436 個 `GLOBALLOCK` call site 個別語義 |
| 存讀檔 | `load.c` | 0% | **完全未碰** — `RLLEncode`/`RLLDecode` 名稱已知，格式未抽 |
| Dialog system | `dialogs.c` | 100% (spec 04) | — |
| 視窗 proc | `wdwmap/wdwsmmap/wdwstat/windows` | dispatch table 骨架 (7/22 + 7/9 + 7/9) | 個別 message handler 待補 |
| AI / 外交 | location 不確定 | 5% | 只確認 `FUN_10e8_2d46` 是 "AI 策略表 init"，其他演算法未抽 |
| 音效 | MMSYSTEM 4 call | 0% | API call 統計過但語義未分析 |
| Game state machine | 推測 `wdwmap` + WinMain D 段 | turn 推進骨架 (M6-lite) | city growth / tech research 演算法未抽 |
| Combat | location 不確定 | 0% | 攻擊/防禦公式 + RNG |
| 城市生產 | per-turn handler | 0% | shield / food / trade 配率 |
| 太空船 | `Dock` STR# 150 | 文字側 RE 完成 | 組裝/啟動演算法未碰 |

### 資產解碼狀態

| 資產 | 數量 | 狀態 | 備註 |
|---|---|---|---|
| CvPc sprite | 199 | 185 解通 ✅ + 14 ❌ | 14 個有 LZW variant 差異 |
| SPR32X32 terrain (col,row) 對位 | 12 種 | 11 個 ground-truth ✅ + HILLS 暫共用 mountain | 真 HILLS tile + base+overlay 雙 layer renderer 是後續工作 |
| STR# 文字 list | 33 | 解通 ✅ | 翻譯進度 241/433 條 |
| TEXT (Civilopedia 條目) | 399 | 識別 ✅ 文字未全 dump | M7 i18n 範圍 |
| GDAT | 7 | ❌ 格式未碰 | 推測動畫 |
| KDAT | 14 | ❌ 格式未碰 | 推測 keymap 或動畫 |
| WAV | 23 | ❌ 未載入 | spec 08 範圍 |
| CIVFONTS.FON | 1 | 略過 (clean-room 自畫字模) | — |

## Roadmap

| 階段 | 內容 | 狀態 |
|---|---|---|
| Phase 0–2 | spec 00/01/02 + Ghidra 工具棧 + clean-room 雙隊制度 | ✅ |
| Phase 3 (子系統 RE) | spec 03 (assets) + spec 04 (dialogs) + spec 05 (master tables) | ✅ |
| Phase 4 (M0-M6-lite SDL 落地) | 13 個 milestone ship + 14/14 ctest PASS + layout 對齊原版 | ✅ |
| Phase 5 (RE 第二輪) | spec 06 數值表 / spec 07 combat+AI+save / spec 08 sound / spec 09 victory | 🟡 spec 06 進行中 |
| Phase 6 (M6-full) | unit move / 戰鬥 / 城市生產 / 存讀檔 | ⏳ 阻 spec 06/07 |
| Phase 7 (M7) | 奇蹟 / 外交 / 勝利條件 | ⏳ |
| Phase 8 (Polish) | terrain (col,row) ground-truth / minimap render / hotkey marker 預剝 / i18n catalog JSON 化 | ⏳ |

## 工作對象（work-of-record）

`CIV.EXE` 參考資訊（使用者自備，不入 repo）：
- 大小：832,512 bytes（Track A Big5-patched 版；code segments 與原版完全相同）
- MD5：`336FF64650F6391C65A8B804ADFC31C9`
- SHA-256：`720C5EA4EFD47FC7069A89C9A366A3A18A561A88E39F963865C6084EEC4DB022`
- NE 簽名位於 offset `0x60`；module name `WINCIV`；description `'文明帝國 視窗版'`（Big5）

原版 1993 英文 binary 位於 `D:\03_game_tmp\win31\C\MPS\CIVWIN\CIV.EXE`，結構完全相同；選 Big5-patched 版為 work-of-record 是因為 SFX portable 是同一份 binary build 的。

## 目錄結構

```
team-a/                       Team A 工作區 — disassembly 側
  specs/                       Markdown 規格（00..05 完成）
  dumps/                       Ghidra decompile 文字輸出
  notes/                       Team A 私人筆記 + commit message 草稿
  tools/                       Ghidra Jython post-script + parser
  binary/                      原始 CIV.EXE（gitignore）

team-b/                       Team B 工作區 — 實作側
  src/                         C99 + SDL2 source
    gfx/                       palette / surface / primitive / cvpc / present
    res/                       rsrcfork / loader (Mac shim 壓平)
    text/                      big5 / utf8 / glyph_cache / text_out
    widgets/                   map / minimap / status / widget
    world/                     world / sprite_sheet / turn
    dialog/                    wizard / dialog
    data/                      civs / strings / civ_dict
  tests/                       14 個 ctest (含 4 個 snapshot)
  build/                       CMake/Ninja 輸出（gitignore）

docs/
  ASSETS_INDEX.md              185 個 sprite 完整分類索引
  CIV1_REFERENCE.md            Fandom 公開資料對照（非 spec）
  CLEAN_ROOM.md                雙隊制度與簽核流程細節
  PHASE_LOG.md                  逐階段進度紀錄
  REUSE_FROM_TRACK_A.md        從前置 Track A 可乾淨取用什麼
  screenshots/                 M0..M5-B 共 12 張 ship snapshot

assets-extracted/             從 .RSC 抽出的 sprite / palette (gitignore raw)
tools/                         共用資產抽取工具（MIT）
```

## 本專案**不**做的事

- 不散布原版 binary 或資產的任何部分。
- 不把反組譯結果（function listing、disassembly bytes、decompiler 輸出）嵌入到實作 source tree。
- 不修改、patch、或重打包 `CIV.EXE`（那是母專案 Track A 的路線，已封存）。

## 反組譯資產（研究用途）

- [`docs/ASSETS_INDEX.md`](docs/ASSETS_INDEX.md) — **185 個 sprite 完整分類索引**（領袖 14 / 科技 ~65 / 單位 ~28 / 奇蹟 / 政府 / 太空 / 動畫 / UI），含縮圖
- [`docs/screenshots/cvpc_spr32x32_decoded.png`](docs/screenshots/cvpc_spr32x32_decoded.png) — 主 sprite sheet 1472×400
- [`docs/screenshots/cvpc_king00_elizabeth.png`](docs/screenshots/cvpc_king00_elizabeth.png) — Queen Elizabeth I 領袖肖像示意
- [`docs/screenshots/m5c_terrain_groundtruth.png`](docs/screenshots/m5c_terrain_groundtruth.png) — 最新 ship 截圖 (M5-C terrain 真實 SPR32X32 對位)
- [`docs/screenshots/m5b_layout_v2.png`](docs/screenshots/m5b_layout_v2.png) — M5-B layout 對齊原版 (前一張)
- [`docs/screenshots/terrain_rows/`](docs/screenshots/terrain_rows/) — SPR32X32 row 0..11 強化縮放 strip + col 22 zoom 4x ground-truth 對位證據
- 完整版權聲明：[`NOTICE.md`](NOTICE.md)

> 所有 sprite © 1993 MicroProse Software / 現屬 Take-Two Interactive / Firaxis Games。**本 repo 為 clean-room 反組譯研究**，純技術考古 + 翻譯範圍規劃，不商用、不重打包、不替代原版商品。

## License

- 程式碼（`team-b/src/`、`tools/`）：**MIT**
- 翻譯文字（`team-b/assets/zh_TW/`）：**CC BY-SA 4.0**
- 規格文件（`team-a/specs/`）：**CC BY 4.0**
- 原版 *Sid Meier's Civilization for Windows* © 1993 MicroProse Software, Inc. — 使用者必須自備合法拷貝。
