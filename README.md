# civ1-decomplie-cht

> 對 **1993 MicroProse《文明帝國 視窗版》**（Win16 NE `CIV.EXE`）做 clean-room 反組譯，重寫成可攜的 **C99 + SDL2**，並內建繁體中文化。

## 專案目的

從 1993 Windows 版 *Sid Meier's Civilization* 的二進位檔從頭反組譯並重寫。不參考任何 upstream 的 open source 程式庫（特別是不看 [OpenCiv1](https://codeberg.org/rhorvat/OpenCiv1) 或其衍生分支）。不 patch 原始 `CIV.EXE`。也不依賴任何 Win16 API（GDI / USER / MMSYSTEM 全部由 SDL2 取代）。

輸出目標是一份單一的 C99 程式碼，透過 SDL2 跨平台編譯；內部維持 palette framebuffer（640×480 原生）；中文字模（內文 16×16、標題 24×24）由 FreeType MONO 在 palette 層合成。

## 目前 ship 狀態 (2026-06-06)

最後 ship: spec 06 v0.2 — 28 unit (修 manual Fighter 3-3 → 4-2) + 25 building + 22 wonder + 47 tech + 24 terrain (OpenCivOne MIT ground-truth)

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
| **M6-full-lite** | unit 系統 (5 類型) + 8 player slot + placeholder 戰鬥 + selection panel + 方向鍵移動 | ✅ | `m6_full_lite_units.png` |
| **M6-minimap** | minimap 真實 60×30 → 160×120 縮圖 + per-terrain palette-nearest RGB + view rect overlay + unit owner dots | ✅ | `m6_minimap_real.png` |
| **M6-full** | 真實戰鬥公式 (spec 07) + 城市生產 + RLL 存讀檔 | 🟡 阻 spec 06+07 | — |
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
| [`06` v0.2](team-a/specs/06_game_data_tables.md) | **28 unit + 25 building + 22 wonder + 47 tech + 24 terrain** ground-truth | 主要源自 [OpenCivOne](https://codeberg.org/rhorvat/OpenCivOne) (MIT, FOSS preservation of 1991 DOS Civ v475.05). 揭穿 34-byte unit struct + 連 manual P41 Fighter 3-3 都是錯的 (真值 4-2). Binary offset 仍 TBD — 1993 Win port struct 可能拆 name 進 STR# 132, 純數值欄位 ~22 byte. v0.3 補 14 civ AI personality + SETI/Cure for Cancer 兩個 wonder + STR# 130 剩 20 entry. |
| [`07`](team-a/specs/07_save_format_and_rle.md) | **SAV file RLE 壓縮** (`load.c::RLLEncode/Decode`) §7.1 完整 + §7.2 v0.1 SAV layout map | §7.1 取自 [Honza Havlicek 2008](team-a/external/). §7.2 R2 cross-compare 3 HAM*.SAV (107 KB fixed-size) 對位 header / king names / civ names / player table / city table / 256 city pool. |

### 待寫 spec ❌

| spec | 範圍 | 為什麼 deferred |
|---|---|---|
| **`06` v0.2** | 72 科技 tree (prereq DAG) + 21 wonders 完整 effect + 14 civ AI personality / GDAT 結構 | v0.1 已 ship 28 unit + 21 building + 6 gov 數值; v0.2 補完餘部 |
| **`06` §6.9 binary offset** | 28 unit stats 在 CIV.EXE / RSC 內的實際位址 | R3 scan 證實非 contiguous array, 推測 hardcoded 在 code segment MOV immediate, 待 Ghidra 全 function MOV pattern walk |
| **`07` §7.2 v0.2** | byte→struct field 對位細部 (treasury / known_techs bitmap / world map terrain) | §7.2 v0.1 layout map 已 ship (R2); v0.2 待 Ghidra walk `CivLoadGame` 看 `fread()` 順序 |
| **`07` combat + AI** | combat 公式 + AI 決策 + 外交 | 數值表 ground-truth 已 ship (spec 06 §6.1.1); 公式骨架 manual P35 明示, 可直接實作 Team B `civ_combat_resolve()` |
| **`08`** | 音效 — MMSYSTEM 4 個 API call site + 23 個 WAV 資源未分類 | 不擋遊戲邏輯；可獨立後做 |
| **`09`** | 勝利條件 / 結局 / scoring | STR# 155-157 (Space 1/2/Archeologist) 文字側已 RE，演算法未碰 |

### 子系統 RE 狀態

| 子系統 | 對應 source 推測 | 已 RE | 缺口 |
|---|---|---|---|
| Renderer | `godpal/gr/gr_pic/gr_port/shape` | palette 模型、CvPc LZW、blit | tile blending 邊界規則；14/199 CvPc 仍 LZW variant fail |
| Loader (Mac shim) | `mac/resmgr` | RSC parser + 5 個 archive | 436 個 `GLOBALLOCK` call site 個別語義 |
| 存讀檔 | `load.c` | 70% — RLE 完整 + §7.2 v0.1 layout map: header 12 byte / king names 256 / civ plural+singular 512 / player stats table 0x314 area / 32-byte city records 0x8A0 area / STR# 135 256 city pool @ 0x07D4A | 細部 byte→struct field 對位 (treasury / known_techs bitmap / world map terrain) 待 R3 Ghidra walk |
| Dialog system | `dialogs.c` | 100% (spec 04) | — |
| 視窗 proc | `wdwmap/wdwsmmap/wdwstat/windows` | dispatch table 骨架 (7/22 + 7/9 + 7/9) | 個別 message handler 待補 |
| AI / 外交 | location 不確定 | 5% | 只確認 `FUN_10e8_2d46` 是 "AI 策略表 init"，其他演算法未抽 |
| 音效 | MMSYSTEM 4 call | 0% | API call 統計過但語義未分析 |
| Game state machine | 推測 `wdwmap` + WinMain D 段 | turn 推進骨架 (M6-lite) + unit 系統 (M6-full-lite) | city growth / tech research 演算法未抽 |
| Combat | location 不確定 | 50% — manual P35 公式 ground-truth (`A/(A+D)` 機率) + spec 06 §6.1.1 修飾 (veteran ×1.5 / walls ×3 / terrain) | 真正 RNG seed + 攻擊推進規則 + 命中分配 |
| 城市生產 | per-turn handler | 30% — spec 06 §6.2 21 building cost + maint + prereq 完整 | shield/food/trade 配率算法 + 7 Wonder 全 effect |
| 數值表 | hardcoded in CIV.EXE | 60% — spec 06 v0.1 ground-truth from manual (28 unit / 21 building / 6 gov) | 72 tech tree + 21 wonders + 14 civ AI personality (v0.2) |
| 太空船 | `Dock` STR# 150 | 文字側 RE 完成 | 組裝/啟動演算法未碰 |

### 資產解碼狀態

| 資產 | 數量 | 狀態 | 備註 |
|---|---|---|---|
| CvPc sprite | 199 | 185 解通 ✅ + 14 ❌ | 14 個有 LZW variant 差異 |
| SPR32X32 terrain (col,row) 對位 | 12 種 | 11 個 ground-truth ✅ + HILLS 暫共用 mountain | 真 HILLS tile + base+overlay 雙 layer renderer 是後續工作 |
| STR# 文字 list | 33 | 解通 ✅ | 翻譯進度 376/731 條 (22/33 表覆蓋,52%) |
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
- [`docs/screenshots/m6_minimap_real.png`](docs/screenshots/m6_minimap_real.png) — 最新 ship 截圖 (M6-minimap: 真實縮圖 + view rect + unit dots)
- [`docs/screenshots/reference/`](docs/screenshots/reference/) — **1993 Civ Windows 原版視覺 reference** (使用者 2026-06-06 提供): 主畫面 + 城市畫面 + 主選單 + layout gap notes
- [`team-a/external/`](team-a/external/) — **外部 RE 研究資料 (Team A only)**: Honza Havlicek 2008 *CivWin File Format demonstrator* (RSC parser + Civ1 LZW + SAV RLE) + **OpenCivOne (MIT, 2023-)** (28 unit/25 imp/22 wonder/47 tech ground-truth) — Team B 不可直接讀, 經 spec 03/06/07 萃取後才接觸. 詳見 [`docs/CLEAN_ROOM.md`](docs/CLEAN_ROOM.md)
- **1991 官方 Manual** (使用者提供, 126 頁): spec 06 v0.1 的 ground-truth 來源 (28 unit + 21 building + 6 government). 不入 repo (版權), 使用者本機保留
- [`docs/screenshots/m6_full_lite_units.png`](docs/screenshots/m6_full_lite_units.png) — M6-full-lite: unit 系統 + 多 player 場景
- [`docs/screenshots/m5c_terrain_groundtruth.png`](docs/screenshots/m5c_terrain_groundtruth.png) — M5-C terrain 真實 SPR32X32 對位
- [`docs/screenshots/m5b_layout_v2.png`](docs/screenshots/m5b_layout_v2.png) — M5-B layout 對齊原版
- [`docs/screenshots/terrain_rows/`](docs/screenshots/terrain_rows/) — SPR32X32 row 0..11 強化縮放 strip + col 22 zoom 4x ground-truth 對位證據
- 完整版權聲明：[`NOTICE.md`](NOTICE.md)

> 所有 sprite © 1993 MicroProse Software / 現屬 Take-Two Interactive / Firaxis Games。**本 repo 為 clean-room 反組譯研究**，純技術考古 + 翻譯範圍規劃，不商用、不重打包、不替代原版商品。

## License

- 程式碼（`team-b/src/`、`tools/`）：**MIT**
- 翻譯文字（`team-b/assets/zh_TW/`）：**CC BY-SA 4.0**
- 規格文件（`team-a/specs/`）：**CC BY 4.0**
- 原版 *Sid Meier's Civilization for Windows* © 1993 MicroProse Software, Inc. — 使用者必須自備合法拷貝。

## Credits

- **Honza Havlicek** (havlicek.honza@gmail.com), *CivWin File Format demonstrator*, 2008 — RSC parser / Civ1 GIF (LZW 變體) decoder / SAV RLE 格式 RE. Spec 03 §3.5.1 SPR32X32 palette 結構 + Spec 07 §7.1 SAV RLE 算法直接引用. 公開研究 free redistribute license + 須 credit. 詳見 [`team-a/external/README.md`](team-a/external/README.md).
- **Rajko Horvat**, *OpenCivOne* (https://codeberg.org/rhorvat/OpenCivOne), MIT license, 2023- — FOSS preservation project based on 1991 DOS Civilization v475.05. Spec 06 §6.1-6.6 (28 unit + 25 improvement + 22 wonder + 47 tech + 24 terrain) ground-truth 直接萃取. 揭穿 manual P41 Fighter 3-3 是印錯 (binary 為 4-2).
