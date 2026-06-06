# Reference Screenshots — 1993 Civ Windows 原版視覺對位

> 從原版 1991 DOS / 1993 Windows 截圖萃取的視覺 ground-truth, 用於 clean-room SDL2 重寫的對位驗證.

## 既有檔案

| 檔名 | 來源 | 內容 | 對應 spec |
|---|---|---|---|
| `civ1_win_civilopedia_dropdown.png` | 使用者 2026-06-06 提供 | 主畫面 + Civilopedia menu 下拉 | spec 04 + R4 layout |
| `civ1_win_city_screen.png` | 使用者 2026-06-06 提供 | 城市畫面 (PARIS POP:10,000) | R6 city_screen |
| `civ1_win_main_menu.jpg` | 使用者 2026-06-06 提供 | 主選單 (CIVILIZATION logo + 6 entries) | M4 (新局精靈, 已 ship) |

## 2026-06-07 使用者新提供 (待手動存檔)

使用者透過 chat 提供 2 張新 reference. 因 chat inline 圖檔無法自動存盤, 請使用者本機:

### 1. Tech Discovery Screen — `civ1_win_tech_discovery.png`

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

### 2. City Screen w/ Embedded Tile Background — `civ1_win_city_screen_rome.png`

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

## 如何手動存檔

從 chat 截圖到 `docs/screenshots/reference/` (跟既有 3 張同位置):

```bash
# Windows: 右鍵 chat 內圖 → 另存新檔
# 存到: D:\03_game_tmp\civ1-decomplie-cht\docs\screenshots\reference\
# 檔名:
#   civ1_win_tech_discovery.png        (BRONZE WORKING 截圖)
#   civ1_win_city_screen_rome.png      (ROME 城市截圖)
```

存完後可加進 git:
```bash
git add docs/screenshots/reference/civ1_win_tech_discovery.png
git add docs/screenshots/reference/civ1_win_city_screen_rome.png
git commit -m "docs: 2 張新 reference (tech discovery + ROME city screen)"
```

## License

原版截圖 © 1993 MicroProse / 現屬 Take-Two / Firaxis. 本 repo 為 clean-room 反組譯研究, 純技術考古 + 翻譯範圍規劃, **不商用、不重打包**.
