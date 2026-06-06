# Spec 05 — 遊戲 master 資料表 + STR# / TEXT 翻譯來源

> **Team A 規格。** 涵蓋兩個原本分開的任務：(1) 遊戲 master 資料表（14 文明 / 72 科技 / 28 單位 / 46 建築+奇蹟 / 24 地形 / 6 政府 / 256 城市名）的所在位置 (2) 433 條翻譯來源字串對應翻譯規劃。
>
> 配套：[`team-a/dumps/05a_*.c`](../dumps/)（WinMain E 段 7 個 `FUN_*()` decompile），[`06a_civdata0_strings.txt`](../dumps/06a_civdata0_strings.txt) 人讀，[`06b_civdata0_strings.json`](../dumps/06b_civdata0_strings.json) Team B 用，[`team-a/tools/extract_strings.py`](../tools/extract_strings.py) 萃取工具。

## 5.1  Spec 02 §2.1.2 E 段 7 個函式的 CORRECTED 角色

[spec 02 §2.1.2 E 段](02_startup_and_game_loop.md#212--winmain-內部-init--fun_1008_0000--10080000) 原本標 7 個 `FUN_*()` 是「載入遊戲資料」。實際 decompile 後**全部不是資料表載入**：

| spec 02 假設 | 函式 | 實際角色（spec 05 確認） |
|---|---|---|
| 載入文明資料表 | `FUN_1008_059a` (149B) | **顯示模式探測** — `GETDEVICECAPS` 查 BITSPIXEL / PLANES / RASTERCAPS / SIZEPALETTE，設 `DAT_1420_17a6` (RC_PALETTE 旗標) 與 `DAT_1420_17a8` (16-color mode 旗標) |
| 載入科技樹 | `FUN_10b8_0000` (1244B) | **16-color 圖檔轉換對話框** — 若 `DAT_1420_17a8 != 0` (16-color mode)，`_LOPEN` 0x1f54 路徑，不存在則 MessageBox `"CONVERT CIVILIZATION ARTWORK"` 詢問是否轉換 256→16 色（對應 dialog #666 progress） |
| 載入政府型態 | `FUN_1008_0b4a` (117B) | **讀 26 byte 偏好設定** — `_LOPEN` + `_LREAD(0x1A bytes)` 讀進 `DAT_1420_17b2`（顯示/音效/介面選項） |
| 載入建築 | `FUN_1090_0488` (270B) | TBD（待後續 decompile pass） |
| 載入單位 | `FUN_11e8_0337` (461B) | **8 player slot 初始化 + 320×200 game-map framebuffer 配置** — 設 `DAT_12d8_45be..45c5 = {0..7}` (7 玩家 + 野蠻人 0xF8..0xFF)，`FUN_1100_05b8(64000)` 配 64000 byte (320×200) framebuffer，預載 8/13/256 個 sprite tag |
| 地圖 / 城市 init | `FUN_11e0_0000` (545B) | TBD |
| 載入 advisor 文本 | `FUN_10e8_2d46` (1124B) | **AI 策略表 init** — 含 `s_FIRSTMOVE_*` / `s_PILLAGE_*` / `s_Climate_*` 字串參考，跑兩個 nested loop 處理 64×N matrix 找最小 cost path — 不是 advisor 文字 |

**結論**：WinMain E 段不載入任何 master table 內容。資料表來自別處 → §5.3。

## 5.2  「為什麼 master table 不在 CIV.EXE 字串裡」

對 work-of-record CIV.EXE grep 標準 Civ1 名稱（[`team-a/tools/grep_civ_names.py`](../tools/grep_civ_names.py)）：

| 類別 | 在 CIV.EXE 找到 |
|---|---|
| 14 文明（Roman / Babylonian / German / ...） | **0 個** |
| 14 領袖（Caesar / Hammurabi / ...） | 僅 `Cleopatra`（1 hit，message context） |
| 12 地形（Plains / Forest / ...） | 0 個 |
| 18 科技候選 | 0 個 |
| 6 政府 | 4 個 — `Despotism`/`Republic`/`Democracy`/`Anarchy`（皆 message context，非表 entry） |
| 8 奇蹟候選 | 0 個 |
| 24 單位候選 | 4 個 — `Settlers`/`Cannon`/`Nuclear`/`Diplomat`（皆 message context） |

**結論**：master tables 不以英文字串形式存在於 CIV.EXE。

## 5.3  Master tables 真正位置：`Civdata0.RSC` 內 33 個 STR# resource

[Spec 03 §3.1](03_asset_formats_and_tiles.md) 揭穿 `Civdata0.RSC` 有 33 STR# + 399 TEXT，當時 Track A `PROJECT_MEMORY` 誤判「裡面沒有遊戲文字」。**完整 STR# 內容已抽出**，是 1993 Civ1 所有 master tables 的 single source of truth：

| STR# ID | name | count | 內容 |
|---:|---|---:|---|
| 128 | Tax Rates | 11 | `'  0% Tax, (100% Science)' .. '100% Tax, (  0% Science)'` |
| 129 | Lux Rates | 6  | `'0% Luxuries..' .. '50% Luxuries..'` |
| **130** | **Civ Advances** | **72** | **完整科技樹** — Alphabet / Code of Laws / Currency / Atomic Theory / The Democracy / The Monarchy / ... |
| **131** | **Improvements & Wonders** | **46** | NONE / Palace / Barracks / Granary / Temple / MarketPlace / ... + 7 wonders |
| **132** | **People** | **28** | Settlers / Militia / Phalanx / Legion / Musketeers / Riflemen / ... + Cavalry / Tank / Battleship / Submarine ... |
| **133** | **Terrains** | **24** | Desert / Plains / Grassland / Forest / Hills / Mountains / + special tile types |
| 134 | Miscellaneous | 21 | Veteran Units / Fortify / Irrigation / Mining / Roads / RailRoads / ... |
| **135** | **Cities** | **256** | 14 文明 × ~18 城市 — Rome, Caesarea, Carthage, Nicopolis, Byzantium, Brundisium ... |
| 136 | Misc Prompts | 14 | Civ Preferences / Please name this game / Saved Game 1 / Show / Hide / Map ... |
| 137 | Credits | 10 | **`A MicroProse Presentation` / `Designed by SID MEIER with BRUCE SHELLEY` / `Windows Version by` / `PAUL L. ROWAN` / `WILLIAM F. DENMAN, JR.`** — **1993 Windows port 開發團隊** |
| 138 | Report Titles | 7 | CITY STATUS / MILITARY STATUS / INTELLIGENCE REPORT / ATTITUDE SURVEY / TRADE REPORT / SCIENCE REPORT / + 1 |
| **139** | **Story** | **44** | **完整開場文字** — 「In the beginning, the Earth was without form, and void. ...」匹配 [Fandom CIV1_REFERENCE §5](../../docs/CIV1_REFERENCE.md) |
| **140** | **Leaders** | **16** | (empty) + Caesar / Hammurabi / Frederick / **Ramesses**（非 Cleopatra）/ Abe Lincoln + 10 more — **15 領袖 + 1 NONE** |
| **141** | **Governments** | **6** | Anarchy / Despotism / Monarchy / Communist / Republic / Democratic |
| 142 | Caravan Goods | 8  | Silk / Silver / Wine / Copper / Gems / Dye / + 2 |
| **143** | **Army Singular** | **8** | Barbarian / Roman / Babylonian / German / Egyptian / American + 2 — **8 player slots 文明形容詞**（對應 `FUN_11e8_0337` 設的 8 slot） |
| **144** | **Armies Plural** | **8** | Barbarians / Romans / Babylonians / Germans / Egyptians / Americans + 2 |
| 145 | King Text | 8  | Attila / Caesar / Shaka / Genghis Khan / Montezuma / Xerxes + 2 |
| 146 | King Title | 13 | Mr. / Emperor / King / Comrade / President / President + 7 |
| 147 | CivPedia Subtitle | 6 | Civilization Advance / City Improvement / Wonder of the World / Military Unit / Terrain Type / Game Concept |
| 148 | Leaders Civ Singular | 16 | NONE / Roman / Babylonian / ... |
| 149 | Leaders Civ Plural | 16 | NONE / Romans / Babylonians / ... |
| 150 | Dock | 10 | Structure1 / Structure2 / Structure3 / Propulsion / Fuel / NONE / + 4 — **太空船零件 (Apollo Program)** |
| 151 | Input Titles | 3  | City Name... / Your Name... / + 1 |
| 155 | Space 1 | 19 | 太空競賽勝利文字 — `'In the year $RPLC1' .. 'voyagers from the planet Earth' ..` |
| 156 | Space 2 | 19 | 同上替代版本 |
| 157 | Archeologist | 11 | 失敗結局：`'Centuries later, archeologists discover the remains of your ancient civilization. ...'` |
| 158 | Alert Text | 10 | 錯誤訊息 — `'Civilization™ only supports 8-bit color (256 colors) and Black/White.'` 等 |
| 159 | Civ Disk Names | 7 | `'Civilization™ Disk 1' .. 'Civilization™ Disk 7'` — 古早多磁片 |
| 160 | Civ File Names | 8 | `'Civ Data 1' .. 'Civ Data 8'` |

**Sprintf-style placeholders**：`$RPLC1` / `$BUCKS0` / `$THEM` 等 token 在 runtime 被替換 — Team B 的 i18n 必須處理。

### Big5 patch 狀態

`Civdata0.RSC` **全部為英文 ASCII** — Track A 沒翻譯這些字串。需要全新翻譯 433 條（33 個 STR# 各 N 條 + 399 個 TEXT），這是 Batch B–E 完整翻譯工作的 ground truth。

## 5.4  399 個 TEXT resource = Civilopedia 完整文字描述

從 id 3174 (`SPACE`) 開始的 399 個 `TEXT` resource 是 Civilopedia 條目的完整文字內容。每條 ~200-500 byte 散文，覆蓋：
- 72 科技 × Civilopedia entry
- 46 建築 / 奇蹟 × entry
- 28 單位 × entry
- 24 地形 × entry
- 6 政府 × entry
- 14 文明 × entry
- 6 大類概念說明
- 太空競賽 / 結局文字變體

需要進一步分類（用 resource name 對齊 §5.3 的 STR# 名稱）。

## 5.5  與其他 spec 的關係

| 來源 | 提供什麼 |
|---|---|
| [spec 03 §3.1](03_asset_formats_and_tiles.md) | Civdata0 結構（33 STR# + 399 TEXT 計數） |
| [spec 04 §4.3](04_dialogs_and_controls.md#43--dialog-999--外交對話框) | 外交對話框使用 STR# 字串作 button label / 提示 |
| [spec 04 §4.4](04_dialogs_and_controls.md#44--dialog-2000--ui-widget-展示測試) | UI widget showcase 用部分 STR# 作示意 |
| **本 spec 05** | master tables 的 single source of truth + 翻譯來源 |
| spec 06（待寫） | 14 文明 / 14 領袖 / 72 科技 / 28 單位等 stats 數值表（attack / defense / cost / movement / production）的位置 — 推測 hardcoded 在 code segment `const` 陣列 |

## 5.6  Team B 整合介面契約

```c
/* team-b/src/data/civ_strings.h */
typedef enum {
    CIV_STR_TAX_RATES         = 128,
    CIV_STR_LUX_RATES         = 129,
    CIV_STR_TECH_ADVANCES     = 130,
    CIV_STR_IMPROVEMENTS      = 131,
    CIV_STR_PEOPLE_UNITS      = 132,
    CIV_STR_TERRAINS          = 133,
    CIV_STR_MISC              = 134,
    CIV_STR_CITIES            = 135,
    CIV_STR_MISC_PROMPTS      = 136,
    CIV_STR_CREDITS           = 137,
    CIV_STR_REPORT_TITLES     = 138,
    CIV_STR_STORY             = 139,
    CIV_STR_LEADERS           = 140,
    CIV_STR_GOVERNMENTS       = 141,
    CIV_STR_CARAVAN_GOODS     = 142,
    CIV_STR_ARMY_SING         = 143,
    CIV_STR_ARMY_PLUR         = 144,
    CIV_STR_KING_TEXT         = 145,
    CIV_STR_KING_TITLE        = 146,
    CIV_STR_CIVPEDIA_SUBTITLE = 147,
    CIV_STR_LEADERS_CIV_SING  = 148,
    CIV_STR_LEADERS_CIV_PLUR  = 149,
    CIV_STR_DOCK              = 150,
    CIV_STR_INPUT_TITLES      = 151,
    CIV_STR_SPACE_1           = 155,
    CIV_STR_SPACE_2           = 156,
    CIV_STR_ARCHEOLOGIST      = 157,
    CIV_STR_ALERT_TEXT        = 158,
    CIV_STR_DISK_NAMES        = 159,
    CIV_STR_FILE_NAMES        = 160,
} civ_str_id_t;

/* 載入所有 STR# / TEXT 資源（M4 init 時呼叫一次） */
int civ_strings_load(struct civ_rsrc *r);   /* 使用 spec 03 §3.10 的 civ_rsrc API */

/* 取一條字串（i18n catalog lookup 後）。
 *   strid: STR# 資源 ID
 *   index: 在該 STR# 內的索引（0-based） */
const char *civ_strings_get(civ_str_id_t strid, int index);

/* 取一條 Civilopedia TEXT 條目 */
const char *civ_strings_get_text(int16_t text_id);
```

### 翻譯 catalog 結構建議

```jsonc
// team-b/assets/zh_TW/strings.json
{
  "STR#/130": {                                   // Civ Advances
    "0":  { "en": "Alphabet",      "zh-TW": "字母" },
    "1":  { "en": "Code of Laws",  "zh-TW": "法典" },
    ...
  },
  "STR#/132": {                                   // People
    "0":  { "en": "Settlers",      "zh-TW": "墾荒者" },
    ...
  },
  "TEXT/3174": {
    "en":    "In the year $RPLC1 voyagers from the planet Earth...",
    "zh-TW": "西元 $RPLC1 年，地球的旅人..."
  },
  ...
}
```

## 5.7  待解

- §5.1 中 `FUN_1090_0488` 與 `FUN_11e0_0000` 角色尚未 decompile 完
- spec 06 待寫：stats 數值表（attack/defense/cost/movement）位置確認 — 推測 hardcoded `const` 陣列在 code segment，需找 caller 反推
- 14 文明 → 8 player slot 對應的 selection 邏輯：應該在 startup wizard dialog 0xAB/B1/BE/CB/D8 內（spec 04 §4.5 待解 5 dialog 的對應）
- 256 個 city names 與 14 文明的對應規則（是固定 18×14 還是動態）

## Sign-off

- [x] Team A：spec 05 把 master tables 真正位置揭穿（不在 CIV.EXE，在 Civdata0.RSC STR#），E 段 7 函式的角色 CORRECTED。
- [ ] Team B：本 spec + JSON dump 可作為 M4+ master table 載入與翻譯 catalog 建立的依據。

**使用者代簽 2026-06-06**：整體認可。
