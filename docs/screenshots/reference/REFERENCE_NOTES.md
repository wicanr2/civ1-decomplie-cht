# Reference Screenshots — 1993 Civ Windows 原版視覺對位

> 從原版 1991 DOS / 1993 Windows 截圖萃取的視覺 ground-truth, 用於 clean-room SDL2 重寫的對位驗證.

## 既有檔案

| 檔名 | 來源 | 內容 | 對應 spec |
|---|---|---|---|
| `civ1_win_civilopedia_dropdown.png` | 使用者 2026-06-06 | 主畫面 + Civilopedia menu 下拉 | spec 04 + R4 layout |
| `civ1_win_city_screen.png` | 使用者 2026-06-06 | 城市畫面 (PARIS POP:10,000) | R6 city_screen |
| `civ1_win_main_menu.jpg` | 使用者 2026-06-06 | 主選單 (CIVILIZATION logo + 6 entries) | M4 (新局精靈, 已 ship) |
| `civ1_win_tech_discovery.png` | 使用者 2026-06-07 | tech discovery modal (BRONZE WORKING) | M-techscreen (R16+, design 已寫) |
| `civ1_win_city_screen_rome.png` | 使用者 2026-06-07 | ROME 城市 (POP:100,000) w/ Pyramids + Great Library | R15 city_screen ocean tile bg |
| `civ1_poster_collage.jpg` | 使用者 2026-06-07 | 多 era 大幅 marketing collage (logo + maps + Civilopedia) | 整體美術風格參考 |
| `civ1_diplomat_elizabeth_pc29.webp` | 使用者 2026-06-07 | 伊莉莎白一世 GREETING (英格蘭領袖 PC #29) | R18 M-diplomat |
| `civ1_diplomat_frederick_pc29.webp` | 使用者 2026-06-07 | 腓特烈大帝 GREETING (日耳曼領袖 PC #29) | R18 M-diplomat |

## 2026-06-07 使用者新提供 ✅ 已存

使用者 2026-06-07 提供 3 張新 reference, 已存進 `docs/screenshots/reference/`:

### 1. Tech Discovery Screen — `civ1_win_tech_discovery.png` ✅

科技研發完成 modal 截圖. 內容:
- **大鍋火** (cauldron) 圖示 @ 左上
- **"BRONZE WORKING"** 黑色大 serif 標題置中
- **"Civilization Advance"** 灰色 subtitle
- **"(Taken from French)"** 灰色 italic — 對齊 manual P38 Diplomat Steal Technology
- **綠/紅花邊雙層 frame** (Win16 decorative wallpaper border)
- **"Allows:"** 紫字 + 後續解鎖:
  - 藍字: Currency / Iron Working (後續可研發 tech)
  - 紅字 + sprite: Phalanx unit (解鎖 unit)
  - 綠字 + sprite: Colossus Wonder (解鎖 wonder)
- 劍形 cursor @ 右下

**設計文件**: 完整 layout + zh-TW 對齊 + R16+ milestone 拆解見 [`../../TECH_DISCOVERY_SCREEN.md`](../../TECH_DISCOVERY_SCREEN.md).

### 2. City Screen w/ Embedded Tile Background — `civ1_win_city_screen_rome.png` ✅

ROME 城市畫面 (有 Pyramids + Great Library). 比 `civ1_win_city_screen.png` (PARIS) 更完整:
- **"ROME (POP:100,000)"** 標題列
- **"PYRAMIDS PALACE"** 右上 wonder 顯示框
- **CITY RESOURCES** 含 citizen 小人圖 (排列方式對應 happy/content/unhappy)
- **center map** 顯示城市周圍 8 格 + 中心 (跟我們 R6 city_screen 5x5 design 同位)
- **FOOD STORAGE** 含 grain icon 圖
- **INFO / HAPPY / MAP / VIEW** 4 tabs
- **"GREAT LIBRARY"** 右側 wonder 展示框 (含科技小 icon 列表)
- **CHANGE / BUY** 按鈕
- **EXIT** 紅按鈕

**關鍵觀察** (使用者 2026-06-07 指出):
- **背景不是單色 stipple** — 是**真實 SPR32X32 ocean/coast tile 重複貼**形成的紋路
- 對應修正: R15 改 `paint_tile_repeat()` 用 OCEAN sprite (col 22 row 9) repeat
- 之前 R10 的 2x2 stipple checker 不夠像

### 4. Diplomat Visit — `civ1_diplomat_elizabeth_pc29.webp` / `civ1_diplomat_frederick_pc29.webp` ✅ (2026-06-07 第二批)

外交訪問畫面 (PC #29 — Personality Constant 29 對應 first GREETING):

**Layout 觀察** (對應 R18 M-diplomat 實作):
- y 0..360 上半: 山地 horizon + sky gradient
- 中央領袖大頭像 (Elizabeth 紅華服 / Frederick 藍軍服 + 銀白假髮)
- 兩側 advisor 占位 (兩種裝束 — 灰袍 / 棕袍)
- y 360..480 下半: 對話區
  - 左右 spear (黑底 + 紅斜紋 + 銀色矛頭)
  - 中央棕色 parchment + 暗紅 serif 對話文字
  - parchment 底部劍紋 ornament

**對應實作**:
- 文字輸出: 對齊原英文 "Greetings from..." → "XX 向您致意……"
- 領袖代表色: spec 06 §6.7 16 nation personality 配色
- icon 字: 「英」/「德」(對齊 R17 tech_icon_char_zh 設計)

### 3. Civ1 Poster Collage — `civ1_poster_collage.jpg` ✅

3.9 MB 大幅 marketing collage, 含多個原版截圖:
- Sid Meier's CIVILIZATION 主 logo
- 鐵路 (Railroad) map 範例
- ROMAN EMPIRE Democracy 1990 A.D. map
- PRE-GAME OPTIONS 設定
- Civilopedia screenshots
- Terrain charts
- 各種 era / unit / city 樣本

用作多重 era / 文明擴張視覺對比參考, 不對齊單一畫面 但提供整體美術風格 ground-truth.

## License

原版截圖 © 1993 MicroProse / 現屬 Take-Two / Firaxis. 本 repo 為 clean-room 反組譯研究, 純技術考古 + 翻譯範圍規劃, **不商用、不重打包**.
