# 1993 Civ Windows 原版視覺參考

> 來源: 使用者 2026-06-06 提供 (`user-provided-data/`),
> 用於跟 Track C SDL 重寫版做 layout / palette 對位 ground-truth.

## 圖檔

### `civ1_win_civilopedia_dropdown.png` — main game screen + 下拉選單

原版 1993 Civ Windows 主畫面, Civilopedia menu 展開中:

**Win16 layout 證實 (對照我們 M6-minimap)**:

| 區塊 | 原版 | 我們 (m6_minimap_real.png) |
|---|---|---|
| Title bar | "CIVILIZATION" Win16 標題列 + min/max | 我們做窄 menu bar 取代 |
| Menu bar | **8 個 item**: File / Edit / Orders / Advisors / World / Civilopedia / City / Help | 我們做 5 個: Game / Orders / Advisors / World / Civilopedia (少 File/Edit/City/Help) |
| 左欄上 | **獨立 Win16 子視窗** "World Map" + 自己 title bar | minimap 我們是嵌入主 widget, 沒 title bar |
| 左欄下 | **獨立 Win16 子視窗** "Status" + 自己 title bar, 內部分: 年代(顯眼大字) / tax-lux-sci bar / 選中單位 panel ("Indian / Militia / Moves:1 / Delhi / (Grassland)") | status 嵌入主 widget, 顯示順序對 (AD/Gold/Rate/Gov/Unit) 但缺 title bar 跟 sub-window chrome |
| 主地圖 | 大面積, 含 city sprite + 城市名 label + unit sprite | ✓ 我們有 unit, 缺 city sprite + city name label |
| Cursor / cursor 在 main map 顯示 | 中央亮黃格 | ✓ 對 |

**Civilopedia dropdown** (右上):
- "Complete" (currently selected)
- Civilization Advances
- City Improvements
- Military Units
- Terrain Types
- Miscellaneous

對應 STR# 147 (我們 civ_dict.c 已翻 "文明進展 / 城市建築 / 世界奇蹟 / 軍事單位 / 地形種類 / 遊戲概念").

**Status panel 細節** (放大):
- 大字 "40,000+" → 應該是 turn count 或某 game stat (不確定具體)
- "3,420 BC" — 西元前 3420 年 (西元年 = -3420)
- "25@ 0.5.5" — tax(25%) / lux(0?) / sci(5+?) — 25@ 暗示 25 個 trade arrow? Civ1 trade unit
- Unit panel: "Indian / Militia / Moves: 1 / Delhi / (Grassland)" — 國名/單位類型/移動點/母城/所在地形

我們 status panel 順序 (AD/Gold/Rate/Gov/Unit) **基本對**, 但細節缺 (e.g. Unit panel 應顯示 母城 + 所在地形).

**Terrain ground-truth 校驗** (原版色 vs 我們 m5c):
- OCEAN: 中藍, 帶白波紋 dot — ✓ 我們 sample 出來色雖然偏綠藍但大致對
- GRASS: 純綠 — ✓
- FOREST: 深綠 + 樹冠 — ✓
- MOUNTAIN: 灰岩 + 白頂 — ✓
- 城市 sprite: 小白塔 + 城市名 label "Delhi" / "Bombay" — **我們還沒做城市**

### `civ1_win_city_screen.png` — 城市畫面 (City Screen)

點選城市進入的細節畫面 (M7 範圍, 我們還沒做). 重要 layout:
- 標題 "PARIS (POP:10,000)"
- 右上 "PALACE" wonder 顯示框
- 左上 "CITY RESOURCES" 框 (顯示城市產出: 食物/工生產/科技/錢)
- 中央 縮小的城市周圍地圖 (city tile + 8 個 worked tile)
- 左下 "FOOD STORAGE" 食物儲存框
- 中下 INFO / HAPPY / MAP / VIEW 4 個 tab
- 右下 "CHANGE" / "BUY" 按鈕 (改建造項目 / 加速生產)
- 右下角 "EXIT" 按鈕

→ **存進 M7 spec 的 reference**. 目前我們 D-M6-full-lite 還沒接 city 系統.

### `civ1_win_main_menu.jpg` — 主選單

原版 *Sid Meier's CIVILIZATION* 大金字字 + 6 個選項:
- Start a New Game
- Load a Saved Game
- Play on EARTH
- Customize World
- View Hall of Fame
- Quit

→ 對應我們 M4-full 的「新局精靈」(已 ship), 但精靈是後續對話, 主選單本身我們還沒做.
   STR# 136 "Civ Preferences" 對應 Customize World 那條? 待確認.

## 對接 Track C 後續 milestone

| 缺口 | 來源圖 | milestone |
|---|---|---|
| 主選單 (6 個 entry) | main_menu.jpg | M4-menu (新) — 加在 M4-full 前 |
| Win16 子視窗 chrome | civilopedia | M5-D layout polish |
| City sprite + name label | civilopedia | M6-full (city 系統) |
| City screen 完整 UI | city_screen | M7 (4 tab + change/buy) |
| 8 個 menu item (File/Edit/City/Help) | civilopedia | M5-D |

## License

原圖 © 1993 MicroProse / 現屬 Take-Two / Firaxis. 本 repo 為 clean-room 反組譯研究, **不商用、不重打包**.
參考圖來源 (使用者標註):
- civilopedia screenshot: oldgam.es watermark
- city screen: 用途辨識
- main menu: squakenet.com watermark (社群 review 站)
