# Civ1 公開資料對照（Fandom 維基）

> 來源：[Sid Meier's Civilization @ civilization.fandom.com](https://civilization.fandom.com/wiki/Sid_Meier%27s_Civilization) （以下簡稱「Fandom」），抓取於 2026-06-06。
>
> 本文件把 Fandom 主頁的公開資料整理成中文摘要，作為 Team A / Team B 的「外部交叉參考」。**不屬於 clean-room spec 的一部分** — 不被 Team B 在實作時直接讀，但可在 Team A 寫 spec 時引用做行為對照（公開遊戲資訊不污染 clean-room）。

## 1. 遊戲身分

| 欄位 | 值 |
|---|---|
| 標題 | *Sid Meier's Civilization* |
| 開發 | MicroProse |
| 發行 | MicroProse（多平台）/ Koei（SNES 版） |
| 製作人 | Sid Meier |
| 設計 | Sid Meier、Bruce Shelley |
| 程式 | Sid Meier |
| 音樂 | Jeffery L. Briggs |
| 文案 | B. C. Milligan、Jeffery L. Briggs、Bruce Campbell Shelley |
| 首發 | 1991 年 9 月（MS-DOS） |
| 類型 | 回合制策略、4X |
| 模式 | 單人 |
| 平台 | MS-DOS、Microsoft Windows、Amiga、Macintosh、Atari ST、Sega Saturn、Super NES、PlayStation、N-Gage |
| 媒體 | Floppy disk、CD-ROM |

**對本專案的意義**：work-of-record 是 1993 Windows 版（spec 00 §0.1），但設計目標是達到 1991 DOS 原版的可玩等價。Fandom 主要描述 DOS 版的行為，視作「目標規範」的公開參考。

## 2. 平台與版本演進

- **MS-DOS（1991）** — 主版本；Sid Meier 親寫，鍵盤 + 滑鼠或純鍵盤皆可。
- **Windows（1993）** — 後續移植，使用 OS 原生選單。**這就是本專案 RE 的對象**（見 spec 00 §0.1 binary 識別）。
- **CivNet（1995）** — Windows 95 重製版，加上 multiplayer（LAN / 早期網路 / hotseat / modem / serial）。玩法幾乎與 1991 原版相同。
- **其他平台**：Amiga、Mac、Atari ST、SNES（Koei 發行）、PlayStation、Sega Saturn、N-Gage — 配樂與介面細節各異。

> Fandom 註：Amiga 版開場動畫不能跳過，被 *Amiga Power* 雜誌的 Gary Penn 批評過慢。

## 3. 介面與操作

> 對 spec 02（[WindowProc 與 message dispatch table](../team-a/specs/02_startup_and_game_loop.md)）的交叉驗證來源。

**雙畫面 + menu** 是各版本共同骨架：
- **map display**（地圖畫面）— 對應本專案的 `WDWMAPPROC`（spec 02 §2.2.6）。
- **city display**（城市畫面）— 推測對應 `CIVDIALOGPROC` 或一個獨立 RT_DIALOG（spec 00 IDs 129–151 範圍）。
- **menu**（DOS 版整合在畫面內；Windows 版用 OS 原生選單 = `RT_MENU #128`）— 4 個必有項目：**Orders**、**Advisors**、**World**、**Civilopedia**。

**滑鼠規約（DOS/Windows）**：
- 左鍵：動作（移動單位、選城市、地圖導航）。
- 右鍵：資訊（點地形開 Civilopedia 該地形條目）。
- 預設 cursor 是火炬精靈（torch sprite）。
- Windows 版 cursor 可變為方向箭頭（單位移動方向預覽）；忙碌時變沙漏。
- Amiga / Atari ST 不用火炬，用一般箭頭。

**對 Team B 的對應**：spec 02 §2.2.6 的 22-entry dispatch table 至少要 cover：左鍵點地圖（單位 / 城市 / tile 互動）、右鍵點地圖（Civilopedia popup）、滑鼠移動（hover / cursor 變形）、鍵盤 hotkey。

## 4. 14 個文明

> 對應 spec 00 §0.7 與 spec 02 §2.1.2 D 段 5 個 startup dialog（其中一個必含 civ 選擇）。

Fandom 列出 14 個可選文明，各自配樂如下：

| # | 文明 | 配樂（DOS） | 備註 |
|---|---|---|---|
| 1 | Roman（羅馬） | *Rise of Rome*（原創） | |
| 2 | Babylonian（巴比倫） | *Hammurabi's Code*（原創） | |
| 3 | German（德意志） | *Variatio 4: Lo stesso movimento* / Bach《Goldberg Variations》 | Amiga 版改用 Mozart *Rondo alla Turca* |
| 4 | Egyptian（埃及） | *Harvest Of the Nile*（原創） | |
| 5 | American（美利堅） | *Battle Hymn of the Republic* | |
| 6 | Greek（希臘） | *Aristotle's Pupil*（原創） | |
| 7 | Mongol（蒙古） | （原創，無名） | |
| 8 | Russian（俄羅斯） | *Song of the Volga Boatmen*（傳統） | |
| 9 | Zulu（祖魯） | （原創，無名） | |
| 10 | French（法蘭西） | *La Marseillaise* | |
| 11 | Aztec（阿茲特克） | *Tenochtitlan Revealed*（原創） | |
| 12 | Chinese（中華） | *The Shining Path*（傳統） | |
| 13 | English（英格蘭） | *Rondeau* / Mouret《Symphonies and Fanfares for the King's Supper》 | |
| 14 | Indian（印度） | *Gautama Ponders*（原創） | |

**Fandom 揭密**：原始 1991 manual 把 **Turks（突厥）** 列為可選文明，開發期間被替換為 **Germans**。本專案 RE 對象（1993 Win 版）顯然是替換後的 14 個。

**對本專案的意義**：在 spec 04（待寫）的「文明資料表」會看到 14 條 record，其中 8 個是原創音樂、6 個是現實旋律 — 可作為翻譯 catalog 對音樂檔名的交叉驗證。

## 5. 開場文字 — **唯一明確提到的資源檔**

> Fandom 原句：「The game pulls the introductory text from a text file located in the game's directory, and it is possible to alter the text file to manipulate the game's introduction.」

開場序列在新局開始時播放，可跳過。**文字本身存於 disk 上的 text file**，可由使用者修改。原文（英文）共 8 段：

> *In the beginning, the Earth was without form, and void.*
>
> *But the Sun shone upon the sleeping Earth and deep inside the brittle crust massive forces waited to be unleashed.*
>
> *The seas parted and great continents were formed. The continents shifted, mountains arose. Earthquakes spawned massive tidal waves. Volcanoes erupted and spewed forth fiery lava and charged the atmosphere with strange gases.*
>
> *Into this swirling maelstrom of Fire and Air and Water the first stirrings of Life appeared: tiny organisms, cells, and amoeba, clinging to tiny sheltered habitats.*
>
> *But the seeds of Life grew, and strengthened, and spread, and diversified, and prospered, and soon every continent and climate teemed with Life.*
>
> *And with Life came instinct, and specialization, natural selection, Reptiles, Dinosaurs, and Mammals and finally there evolved a species known as Man and there appeared the first faint glimmers of Intelligence.*
>
> *The fruits of intelligence were many: fire, tools, and weapons, the hunt, farming, and the sharing of food, the family, the village, and the tribe. Now it required but one more ingredient: a great Leader to unite the quarreling tribes to harness the power of the land to build a legacy that would stand the test of time:*
>
> *a CIVILIZATION!*

Civilization IV 重新用 Leonard Nimoy 旁白朗讀相同文字。

**對本專案的意義**：
- 此 .txt 應該存在於 1993 Windows 版的 game directory，與 `.pic` / `.pal` / `.fnt` 同列。Phase 1（資產抽取，待做）要把它列為待解析的目標檔之一。
- spec 02 §2.1.2 D 段的 5 個 `FUN_1190_0000` 載資源時，其中一個可能就是載入這個開場 .txt — 後續 walk 時可確認。
- 翻譯 catalog 必須有這 8 段中文版（已存在 Track A `data/inline_translations.json`，可整合到 spec 04）。

## 6. 設計血脈

Sid Meier 公開承認科技樹概念**取自 1980 桌遊 *Civilization***（Hartland Trefoil；美國由 Avalon Hill 發行）。早期 1991 版本還附該桌遊的傳單與訂購資料。後來反過來 — 出現以 Sid Meier 電腦版為本的桌遊。

另一條血脈是 1977 年 Walter Bright 設計的 *Empire*（六邊形地圖 + 城市生產回合 + 戰爭單位）。

**未做出的版本**：
- **Danielle Bunten Berry**（《M.U.L.E.》、《Seven Cities of Gold》作者）1983 與 1985 兩度計畫做 Civilization，都沒實現。
- **Don Daglow**（《Utopia》— 第一個 sim 遊戲作者）1987 開始實作 Civilization，但接 Broderbund 高層位置後放棄。

Sid Meier 是「第三個」想做電腦版 Civilization 的設計者，但第一個真正完成。

## 7. 戰鬥與 AI 設計（爭議 / 玩家視角的「平衡」）

**「長矛兵打贏戰艦」現象** — 古代單位在地形 / 防禦工事 / 老兵狀態加成下加上隨機骰，**可以**擊敗現代單位。常見例：山地 + 城牆 + 老兵 phalanx 防禦 vs. 攻擊中的 battleship。Fandom 註：這在數學上可能，但圖像上不直觀。

**AI 隱性加成** — 電腦對手有一組「未文件化」的數值加成（生產、產出、戰鬥），用來補 AI 邏輯的不足。玩家有時遭遇「不知道遊戲規則」的狀況，被視為缺乏對稱性的設計批評之一。

**對本專案的意義**：spec 04（戰鬥模型，待寫）的數值表必須準確還原這些加成 — 包括隱性加成。Track A 的 RE 工作有可能漏抓這些常數；Team A 在 spec 04 時要明示是否有「difficulty-level coupled multiplier」隱藏在資料表內。

## 8. 1996 年 Computer Gaming World 評語

> 「While some games might be equally addictive, none have sustained quite the level of rich, satisfying gameplay quite like Sid Meier's magnum opus. The blend of exploration, economics, conquest and diplomacy is augmented by the quintessential research and development model, as you struggle to erect the Pyramids, discover gunpowder, and launch a colonization spacecraft to Alpha Centauri.」

評為 1996 年史上最佳遊戲第 1 名（150 名單）。1992 年也拿過 Origins Award「年度最佳軍事 / 策略電腦遊戲」。

## 9. 周邊 / 衍生

- **1994 *Sid Meier's Colonization*** — 同 Sid Meier 設計，類似系統。
- **1993 *Master of Magic*** — MicroProse 同年發行，類同系統 + 中世紀奇幻設定（用魔法 spell 取代科技）。**重要**：因為同公司同期，兩個遊戲的程式架構可能相同 — `MM3-CC-Archive` / `EDILZSS2` 等技術知識可雙向流通（與 [memory](../../../../../.claude/projects/C--Users-------/memory/MEMORY.md) 內 `mm3-cc-archive` skill 對接）。
- **1995 *CivNet*** — 多人版。
- **1999 *Civilization: Call to Power*** — Activision 短期擁有名稱權時做。

## 10. IP 歷史

- 2004 年底：Atari 把 *Civilization* IP 賣給 **Take-Two Interactive**。
- 2005-01-26 公開消息。
- 現由 Take-Two 旗下 **2K Games** 發行（Firaxis 開發）。
- 商標歸 Take-Two / Firaxis。

**對本專案的意義**：本專案 **不** 散布原版 binary / 資產（見 [README §License](../README.md#license)）；使用者必須自備合法拷貝。本 clean-room 重寫的程式碼採 MIT；翻譯採 CC BY-SA 4.0；規格採 CC BY 4.0。

## 11. 與本專案 spec 的交叉對照

| Fandom 條目 | 對應到本專案 |
|---|---|
| 4 個 menu（Orders / Advisors / World / Civilopedia） | `RT_MENU #128`（spec 00 §0.7） |
| map display | `WDWMAPPROC` (spec 02 §2.2.6) |
| city display | 推測為 `CIVDIALOGPROC` 之一處理（spec 02 §2.2.5） |
| 左鍵動作、右鍵 Civilopedia | spec 02 §2.4 的 widget dispatch table 應 cover |
| 14 個文明 | spec 04 待寫文明資料表 |
| 開場 .txt 從 disk 讀 | Phase 1 資產抽取的目標檔之一 |
| 配樂 14 首 | `MMSYSTEM` 4 個 call site 中 2 個應為 `mciSendCommand` 播 MIDI（spec 01 §1.4.5） |
| AI 難度隱性加成 | spec 04 戰鬥 / 經濟模型必須抓 |

## 12. Fandom 沒講、需要從其他來源補的事

主頁明顯**沒有**涵蓋：

- 具體檔案副檔名清單與用途（`.PIC` / `.PAL` / `.TXT` / `.FNT` / `.MID` 等 — 我們已從 spec 00 / 01 / Track A 確認的內容）
- 各檔案內部格式（compression、palette layout、tile layout）
- 存檔格式（`.SVE` / 等）
- 科技樹具體結構（雖然有提到「科技樹」但沒列具體科技）
- 單位 / 建築 / 奇蹟 數值

這些**需要從其他公開來源補**：
- **Fandom 子頁面**：Civilopedia (Civ1)、Soundtrack (Civ1)、各文明 / 單位 / 科技子頁面（透過 `{{Civ1}}` navbox 連結）
- **strategywiki.org/wiki/Civilization** — Fandom 列為「lots of detail, under the same publishing license」
- **CivFanatics**（civfanatics.com）— modding 社群有反推格式
- **GameFAQs 上 1990s Civilization FAQ** — vintage 玩家攻略
- 我們的 Track A 反組譯紀錄 — 含 EDILZSS2 格式 spec
- 我們的 Ghidra disassembly + spec 01 §1.2 還原的 `gr_pic.c` / `load.c` / `resmgr.c`

> **建議**：要完整資源格式請 Team A 從 disassembly 推（spec 03 待寫）。Fandom 主頁適合做 **行為層** 對照，不適合做 **格式層** 對照。

## 13. 進一步資料

如果要從 Fandom 補資料，建議優先抓的 sub-page：

| Sub-page | URL | 預期內容 |
|---|---|---|
| Civilopedia (Civ1) | https://civilization.fandom.com/wiki/Civilopedia_(Civ1) | 內建百科內容 |
| Soundtrack (Civ1) | https://civilization.fandom.com/wiki/Soundtrack_(Civ1) | 14 首音樂細節 |
| List of technologies (Civ1) | https://civilization.fandom.com/wiki/Technology_(Civ1) | 科技樹 |
| List of units (Civ1) | https://civilization.fandom.com/wiki/Unit_(Civ1) | 單位數值 |
| List of wonders (Civ1) | https://civilization.fandom.com/wiki/Wonder_(Civ1) | 奇蹟 |
| List of buildings (Civ1) | https://civilization.fandom.com/wiki/Building_(Civ1) | 建築 |

外部：

- https://strategywiki.org/wiki/Civilization — 同 CC 授權，可整合
- https://www.civfanatics.com/civ1/ — modding format 反推

要其中哪個 sub-page 也整合請告訴我。
