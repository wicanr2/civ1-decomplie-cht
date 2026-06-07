# Game Test Gap List (1993 原版 vs SDL2 重寫)

## 日期: 2026-06-07
## 環境: WSL Ubuntu-22.04 / SDL2 dummy driver / reference-only (wine prefix init 超時, 改用 docs/screenshots/reference 1993 ground-truth)

## 截圖位置

- 我們 SDL2 (16 張): `docs/screenshots/gametest_2026-06-07/us/`
- 原版 reference (7 張): `docs/screenshots/gametest_2026-06-07/orig/`
- 也保留在 `/tmp/civ1_diff_{us,orig}/` (WSL local)

對比配對表:

| Screen | 我們 (us/) | 原版 (orig/) |
|--------|------------|--------------|
| Main + minimap + status | main.png | civ1_win_civilopedia_dropdown.png (主畫面+下拉) |
| Splash CIV title | splash.png | civ1_win_main_menu.jpg |
| Tech discovery | tech.png | civ1_win_tech_discovery.png |
| City screen | city.png | civ1_win_city_screen.png / civ1_win_city_screen_rome.png |
| Diplomat Elizabeth | diplomat-elizabeth.png | civ1_diplomat_elizabeth_pc29.png |
| Diplomat Frederick | diplomat-frederick.png | civ1_diplomat_frederick_pc29.png |
| Diplomat Mao / Gandhi | diplomat-mao/gandhi.png | (無 reference, 用兩個既有外交 reference 共享構圖判斷) |
| Birth sequence 1..8 | birth-1..8.png | (無, 用 spec 04 reference) |

---

## Critical (玩家立即看出差異)

### C1. 主畫面 Civilopedia 下拉選單未實作

- **我們的 (us/main.png)**: 只有 menu bar 標題 (檔案/編輯/命令/顧問/世界/百科/城市/說明), 點下去無 dropdown 行為 (snapshot mode 看不到, 但 source code 無 dropdown widget)
- **原版 (orig/civ1_win_civilopedia_dropdown.png)**: Civilopedia 點下後展開 6 項子選單 (Complete / Civilization Advances / City Improvements / Military Units / Terrain Types / Miscellaneous), Help 同類, 黃色 highlight 行
- **修法建議**: 新增 `widgets/menu_dropdown.c` (R4 Win16 chrome 已有 menu bar). 參考 spec 05 §menu_chain. dropdown 用 `widgets/popup.c` reuse. 文字繁中化 keys 加進 `civ_dict` (Complete=完整 / Civilization Advances=文明進展 / ...)

### C2. Main 畫面缺 unit 數字/旗幟標示 (vs reference)

- **我們的**: unit 圖塊 (墾荒者/騎兵等) 沒有畫上方 nationality flag 條 (旗幟色 1px line), 也沒有 stack indicator (數字)
- **原版**: Delhi 旁的 settler/militia unit 上方有「印度民兵」橘色 nation flag + 數字 1 (Moves: 1)
- **修法建議**: spec 05 §unit_overlay. SPR32X32 上方 padding 區 2-3px draw_rect 用 nation primary color (參考 `world/nation.c::primary_color`). 主面板 status box 已正確顯示「印第安人/民兵/Moves:1/Delhi」, 等同 logic 已存在 — 只缺 sprite overlay

### C3. Tech discovery modal — 缺「科技官員」立繪

- **我們的 (us/tech.png)**: 用 `Civdata3.rsc` 內某個 advisor sprite 對的 (有顯示女性祭司站在鏡台旁邊). **重新檢視: 其實畫了, 不算 critical** — 降為 Major C3
- **原版 (orig/civ1_win_tech_discovery.png)**: BRONZE WORKING 配煮鍋大鼎插圖, 文字 "Civilization Advance / (Taken From French)"
- **降級為 Major M3** (見下)

### C4. City screen — 食物存量 / 生產進度條缺視覺

- **我們的 (us/city.png)**: FOOD STORAGE 框是空的, 只寫 "0 / 40" 數字
- **原版 (orig/civ1_win_city_screen.png)**: FOOD STORAGE 區應有 wheat icon row 顯示已存量 (圖示, 不是數字)
- **修法建議**: spec 07 §city_food_grid. `widgets/city_screen.c::draw_food_storage()` 用 SPR16x16 #wheat 重複畫 N 個 icon (N = current_food). 同理 trade arrow icon

### C5. City screen — CITY RESOURCES 欄位排版不同

- **我們的**: 左上「食物:6 / 生產:5 盾 / 貿易:3 / 人民: 快樂3/不滿0」純文字
- **原版**: 用 emoji-like 小圖示 (wheat / shield / lucky face) row, 一排一排
- **修法建議**: spec 07 §city_resource_panel. 同 C4 路線 — 文字後接 icon row. 字串保留繁中當 label

### C6. Diplomat screen — 左右 portrait column 對齊錯位

- **我們的 (us/diplomat-elizabeth/frederick/mao/gandhi)**: 左側有「軍官 portrait 直疊 4 個」, 右側單 portrait + 西裝紳士
- **原版 (orig/civ1_diplomat_elizabeth_pc29)**: 左右各**一個** advisor (印度王后 / 中亞老者), 中央領袖大像, 下方對話框
- **修法建議**: spec 03 §diplomat_layout 重新對位:
  - 左邊應該只有 **一個** advisor portrait (印度貴族色 — 對應對手 nation), 不該 stack 4 個
  - 右邊 advisor 顯示**我方** civ ally 顧問 (中亞智者), 不該是西裝現代人 (現在用了錯的 sprite slot, 可能是 CVPC ADVI sheet 切錯)
  - 中央大像 OK (Elizabeth/Frederick/Mao/Gandhi 都對得上)
  - 上方天空 (us 是淡藍純色 / orig 是漸層 sky + 遠山 mountain silhouette + 小騎兵剪影). 需載 `Civdata3.rsc` 的 DIPL backdrop (id 待確認, 可能在 #diplomat-bg)

### C7. Diplomat 下方裝飾 — 矛/盾欄杆 vs 黃色窗簾

- **我們的**: 下方對話 box 周圍是黃色 banana-pattern 窗簾, 左右各有暗紅矛盾 small column
- **原版**: 下方是綠色刺繡 banner + 黑底紅交叉劍紋 column (左右), 中間 banner 寫 "Greetings from Elizabeth I, ruler and Empress of the English..."
- **修法建議**: spec 03 §diplomat_chrome. 換用對的 banner sprite. 目前用了錯的 sheet (黃色 是 throne backdrop, 不是 dialog frame). 看 `Civdata3.rsc` index, 找 DIPLOFR (frame) 或 BNNR slot

---

## Major (明顯 polish, 不影響可玩性)

### M1. Splash — 沒有「Start a New Game / Load / Earth / Customize / Hall of Fame / Quit」對話框

- **我們的 (us/splash.png)**: 只有 "Sid Meier's CIVILIZATION" 金色 title, 黑色背景, 完全沒有選項框
- **原版 (orig/civ1_win_main_menu.jpg)**: title 下方有灰色 raised panel 6 個 radio button:
  - ● Start a New Game
  - ○ Load a Saved Game
  - ○ Play on EARTH
  - ○ Customize World
  - ○ View Hall of Fame
  - ○ Quit
  - [OK] button 右下角
- **修法建議**: spec 05 §splash_menu. 新增 `widgets/start_menu.c`. 文字繁中化 (見 civ_dict 新 key)

### M2. Splash — 缺「OldGames.sk」浮水印小 logo

- 原版右下角有 oldgames.sk 浮水印 — **我們不需要做**, 那是 reference 來源的水印 ✓

### M3. Tech screen — 大鼎 icon (左上)

- **我們的 (us/tech.png)**: 棕色方框 + 「銅」中文字 — 文字版 placeholder
- **原版**: 黑色大鼎 + 火焰 + 鍋裡黃色液體 — 全 pixel-art illustration
- **修法建議**: 載入 `Civdata3.rsc` 內 tech icon sheet. 對位 BRONZE WORKING → caldron icon. 完整 99 個科技各對一張 icon. spec 06 §tech_icon_table

### M4. Tech screen — "Civilization Advance / (Taken From X)" subtitle 顏色

- **我們的**: "文明進展 / (取自埃及)" 灰黑色
- **原版**: "Civilization Advance / (Taken From French)" 銀灰色 + 不同 font face (handwriting feel)
- **修法建議**: 純美術細節, R5 polish round. 加 `text_style_emphasis` shade

### M5. Tech screen — Allows 區用 unit/wonder icon 是對的, 但 Wonder icon 取錯

- **我們的**: 「巨像 奇蹟」icon 顯示一個藍色船帆 (錯了, 那是 ship)
- **原版**: 「Colossus Wonder」icon 顯示 colossus 雕像 (站立的人形雕像)
- **修法建議**: spec 06 §wonder_icon_table. 修 `Civdata3.rsc` wonder sheet 對位

### M6. Diplomat screen — 領袖大像周圍光照不對

- **我們的**: 領袖 portrait 直接貼在背景上, 沒陰影沒邊光
- **原版**: 領袖周圍有微弱光暈 + 下方衣服延伸到 banner 上方 (overlap layering)
- **修法建議**: spec 03 §portrait_layering. 在 portrait blit 前畫 soft shadow, layer order 調整

### M7. Diplomat — 文字配色

- **我們的**: 「英格蘭女皇伊莉莎白一世向您致意……」深紅色, 對話框背景黃米色 — **很 OK**
- **原版**: "Greetings from Elizabeth I, ruler and Empress of the English…" 深紅色 + 同樣黃米背景 — 一致 ✓
- **無修, 算對齊好**

### M8. Main — minimap 邊框

- **我們的 (us/main.png)**: 左上 minimap 直接深藍底 + 小綠陸塊 — 簡單
- **原版**: minimap 有 "World Map" title bar + 白色 viewport rectangle (顯示主畫面 viewport)
- **修法建議**: 主 title bar 中文「世界地圖」已有 ✓. 但缺 viewport rect 框 (白色 1px outline). spec 05 §minimap_viewport

### M9. Main — status panel "Status" title bar 對位

- **我們的**: 左下 status 框上有「狀態」中文 title bar ✓
- **原版**: 同位置 "Status" 英文 title bar
- 中文化已對齊, OK

---

## Minor (細節 / 純美術)

### Mi1. Splash — Birth sequence (intro 動畫) 雖各 frame render 完整, 但無動畫播放邏輯

- 我們有 8 frame (大爆炸→星雲→星球→生命→文明) 每 frame 漂亮 ✓
- 但 R5 應該串成 sequence 自動播 (timer-based frame advance), 目前 snapshot mode 才看得到單張
- spec 04 §intro_sequence 已寫. 等 R5 動畫 stub

### Mi2. Main — 終端 unit 移動 indicator (selected unit hex)

- 我們的: 選中 unit 周圍有黑底 sprite frame ✓
- 原版: 選中 unit 周圍有 white pulsing 1px outline (animated)
- 純動畫差, R5+ polish

### Mi3. Splash 在 birth-2 (星雲 / 銀河) 後沒有 title 跨入過場

- 我們的 birth-2.png 是 512×320 圖直接 letter-box, 上下黑邊厚
- 原版 sequence 是 320 高 fullscreen
- 純 layout 差

### Mi4. 字型 — 西元 0 年 / 金庫 / 稅 樂 科 — uming.ttc 字寬

- 我們的: uming.ttc 12px 渲染 OK, 但「金庫: $0」遇 $ 符號的中文化邊界處理略嫌寬鬆
- spec 不嚴, 可不改

### Mi5. City screen — INFO/HAPPY/MAP/VIEW tab 與 CHANGE/BUY 按鈕

- 我們的: 已有 5 個 button ✓ (INFO 高亮, CHANGE/BUY 灰邊)
- 原版: 同 ✓
- 對齊 OK

### Mi6. Birth sequence 配置

- birth-1: 黑底純星空 (大爆炸瞬間 — 對, 我們 ✓)
- birth-2: 銀河系誕生 ✓
- birth-3: 地球形成 (我們有岩漿+岩石 ✓ 但氣氛少)
- birth-4: 早期生命 ✓
- birth-5: 大爆發 (我們有花花綠綠 — 對)
- birth-6: 動植物多樣 ✓
- birth-7: 古文明萌芽 ✓
- birth-8: 現代文明 + 火箭 ✓
- **整體 PASS** — 8 張都對得上 spec 04 reference

---

## 環境問題 / 跑不起來的部分

### E1. Wine prefix 初始化超時 (本 round 阻塞)

- 嘗試: `WINEPREFIX=/tmp/wineprefix_civ wineboot --init` 跑超過 90 秒未完成 (卡在 `setupapi InstallHinfSection wine.inf` 階段)
- 原因: 第一次跑 wine 必須 setup 整個 prefix (registry / system32 stub / fonts), WSL2 第一次大約 5-10 分鐘
- 影響: 無法直接跑原版 `CIV.EX$` 抓真實 1993 截圖, 改用 `docs/screenshots/reference/` 已存的 ground-truth (足夠覆蓋 main / splash / tech / city / 2 個 diplomat)
- **下次跑前**: 先 `nohup /opt/wine-stable/bin/wineboot --init` 跑到底 (預期 5-10 min), prefix 建好後續測試就快
- **完全 reproducible 替代**: 用 `otvdm` (Win16 dedicated emulator, 比 wine 對 NE 支援更好), 但需另外 build/apt install
- DOS 路線: `/mnt/d/03_game_tmp/civ1/civilization.zip` 解壓是 raw disk image (.img, 3MB), 需要先掛載提取, 不能直接餵 dosbox

### E2. 4 個領袖 (Mao / Gandhi) 沒有 reference

- `docs/screenshots/reference/` 只有 Elizabeth + Frederick 的 1993 reference
- Mao / Gandhi 部分用「兩個 reference 共享構圖」推論 (左右 advisor / 中央領袖 / 下方 banner). 構圖差異與 C6/C7 同
- 補救: 下次跑 wine 成功時, 用 in-game diplomat trigger 跑全 14 個領袖 portrait 確認

### E3. Snapshot mode 是靜態, 沒有動畫/互動測試

- 鍵盤 mapping / modal lock / 鼠標 hover / 動畫 frame timing 全沒測
- 需要 X11 driver + `xdotool keydown` 整套 (本 round 沒做, R 次再來)

---

## 優先級總結

| 優先級 | Count | 主要修法集中點 |
|--------|-------|----------------|
| Critical | 7 (C1-C7) | 主畫面 dropdown / Unit overlay / City resource icon / Diplomat 構圖+chrome |
| Major | 8 (M1-M8) | Splash menu / Tech icon table / Wonder icon / Minimap viewport |
| Minor | 6 (Mi1-Mi6) | 動畫播放 / 純美術細節 / Birth sequence (整體已 PASS) |

## 下一輪 (R5) 建議優先順序

1. **C1** (Splash menu + dropdown) — UX 缺角, 沒做完玩家連 New Game 都按不下去
2. **C6 + C7** (Diplomat 構圖修正 + chrome) — 美術一致性最差
3. **C4 + C5** (City resource icon) — 對 spec 07 §city_food_grid 直接 ship
4. **C2** (Unit overlay nation flag + count) — 1 px rect 工作量小但體感大
5. **M1** + **C1** 合併實作 (都是 start menu 線)

預估 R5 工時 (依 [estimate_2026_tools](../../C:/Users/原來是個胖仔/.claude/projects/.../feedback_estimate_2026_tools.md) baseline): 4-6 hr (with AI agent assist).
