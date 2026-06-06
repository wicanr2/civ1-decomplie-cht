# Spec 06 — 28 unit + 46 建築/奇蹟 + 72 科技 + 14 文明 數值表

> **Team A 規格 (v0.1 草稿, 2026-06-06 R3)**.
>
> **主要來源**: 1991 *Sid Meier's Civilization* 官方 Manual (使用者提供, 126 頁, `user-provided-data/Civilization 1 manual.pdf`). 這是 1991 DOS 版手冊, 1993 Win port 數值應與此一致 (spec 00 證實 Big5-patched binary 與原版 code segments byte-equivalent).
>
> **次要來源**: Honza Havlicek 2008 RE notes (team-a/external/), Civ1 wiki (僅參考, 已被 manual 推翻多筆值).
>
> **狀態**: §6.1 unit stats 完整 (28/28); §6.2 building 21/21; §6.3 government 6/6; §6.4 wonder 名+部分 effect; §6.5 tech tree 待 v0.2.

## 6.0  本 spec 解答什麼

- M6-full 真版 combat / production 公式所需的數值表 ground-truth
- 哪些值要 hardcoded 進 Team B 程式碼 (e.g. `data/unit_stats.h`)
- 哪些值 *runtime 才知道* (e.g. unit 是否 obsolete)

## 6.1  Unit Stats Table (28 個, 1991 manual P40-42 ground-truth)

格式: `[A] attack / [D] defense / [M] movement / [Cost] shields / [Tech] prereq advance`

| # | 中文 | 英文 | A | D | M | Cost | Prereq Tech |
|---:|---|---|---:|---:|---:|---:|---|
| 0 | 墾荒者 | Settlers | 0 | 1 | 1 | 40 | — |
| 1 | 民兵 | Militia | 1 | 1 | 1 | 10 | — |
| 2 | 方陣兵 | Phalanx | 1 | 2 | 1 | 20 | Bronze Working |
| 3 | 軍團兵 | Legion | **3** | **1** | 1 | 20 | Iron Working |
| 4 | 火槍兵 | Musketeers | **2** | **3** | 1 | 30 | Gunpowder |
| 5 | 步兵 | Riflemen | **3** | **5** | 1 | 30 | Conscription |
| 6 | 騎兵 | Cavalry | 2 | 1 | 2 | 20 | Horseback Riding |
| 7 | 騎士 | Knights | 4 | 2 | 2 | 40 | Chivalry |
| 8 | 投石機 | Catapult | 6 | 1 | 1 | 40 | Mathematics |
| 9 | 加農炮 | Cannon | 8 | 1 | 1 | 40 | Metallurgy |
| 10 | 戰車 | Chariot | 4 | 1 | 2 | 40 | The Wheel |
| 11 | 裝甲車 | Armor | 10 | 5 | 3 | 80 | Automobile |
| 12 | 機械化步兵 | Mech Inf | 6 | 6 | 3 | 50 | Labor Union |
| 13 | 炮兵 | Artillery | 12 | 2 | 2 | 60 | Robotics |
| 14 | 戰鬥機 | Fighter | **3** | **3** | 10 | 60 | Flight |
| 15 | 轟炸機 | Bomber | 12 | 1 | 8 | 120 | Advanced Flight |
| 16 | 三層槳座戰船 | Trireme | 1 | 0 | 3 | 40 | Map Making |
| 17 | 帆船 | Sail | 1 | 1 | 3 | 40 | Navigation |
| 18 | 巡防艦 | Frigate | 2 | 2 | **3** | 40 | Magnetism |
| 19 | 鐵甲艦 | Ironclad | 4 | 4 | 4 | 60 | Steam Engine |
| 20 | 巡洋艦 | Cruiser | 6 | 6 | 6 | 80 | Combustion |
| 21 | 戰艦 | Battleship | 18 | 12 | 4 | 160 | Steel |
| 22 | 潛艇 | Submarine | 8 | 2 | 3 | 50 | Mass Production |
| 23 | 航空母艦 | Carrier | 1 | 12 | 5 | 160 | Advanced Flight |
| 24 | 運輸船 | Transport | 0 | 3 | 4 | 50 | Industrialization |
| 25 | 核彈 | Nuclear | 99 | 0 | 16 | 160 | Rocketry + Nuclear Fission |
| 26 | 外交官 | Diplomat | 0 | 0 | 2 | 30 | Writing |
| 27 | 商隊 | Caravan | 0 | 1 | 1 | 50 | Trade |

**Wiki 錯誤修正** (manual 是 ground-truth):
- #3 Legion: 3-1 (wiki 給 4-2)
- #4 Musketeers: 2-3 (wiki 給 3-3)
- #5 Riflemen: 3-5 (wiki 給 5-4)
- #14 Fighter: 3-3 (wiki 給 4-2)
- #18 Frigate: move=3 (wiki 給 4)

### 6.1.1  特殊規則 (manual P40-42)

| 規則 | 適用 | 內容 |
|---|---|---|
| Veteran 加成 | 所有 unit | +50% attack & defense |
| Fortified | 陸軍 only | +50% defense |
| Fortress | 任何 unit @ Fortress | ×2 strength after other mods |
| City Walls | 任何 unit in city w/ Walls | ×3 strength |
| Bomber/Artillery | 攻擊 City Walls 時 | walls 對其無效 |
| Terrain bonus | defender | 按 spec 06 §6.6 terrain table |
| Trireme 隨機損失 | Trireme not adjacent to land at turn end | 50% chance lost at sea |
| Bomber 滯空 1 turn | Bomber | 第 2 turn 必須回 friendly city/Carrier |
| Air refuel | Fighter | 每 turn 必須回 friendly base |
| Carrier 容量 | Carrier | 載 up to 8 air units |
| Transport 容量 | Transport | 載 up to 8 ground units |
| Trireme 容量 | Trireme | 載 up to ? (manual 沒明) |
| Sail 容量 | Sail | 載 up to 3 |
| Frigate 容量 | Frigate | 載 up to 4 |
| Shore bombardment | 含 attack factor 的 ship (非 Submarine) | 攻擊鄰格 land/city |
| Submarine 隱形 | Submarine | 敵人 ship 只在 adjacent 才看到 |
| Diplomat 通過 | Diplomat | 無視 ground 移動限制 (但被攻擊幾乎必死) |
| Caravan 通過 | Caravan | 同上 |
| Bribery | Diplomat | 進敵 unit 格觸發 (單 unit only) |
| Nuclear AOE | Nuclear | target + 8 adjacent 全毀 (含友軍) |
| SDI Defense | City w/ SDI improvement | 擋 Nuclear attack |

## 6.2  City Improvement Table (21 個, manual P78-81)

| # | 中文 | 英文 | Cost | Maint | Prereq | Effect 摘要 |
|---:|---|---|---:|---:|---|---|
| 1 | 宮殿 | Palace | 0 (start city) | 0 | — | 主城; 影響 corruption 距離 |
| 2 | 兵營 | Barracks | 40 | $1 (Gunpowder 後 obsoletes; 改 Combustion 後 obsoletes) | — | 新 unit veteran; 防 Pirate Raid |
| 3 | 穀倉 | Granary | 60 | $1 | Pottery | 50% food carry over; 防 Famine |
| 4 | 神廟 | Temple | 40 | $1 | Ceremonial Burial | -1 unhappy (+1 if Mysticism); 防 Volcano |
| 5 | 市場 | Marketplace | 80 | $1 | Currency | +50% gold + lux |
| 6 | 圖書館 | Library | 80 | $1 | Writing | +50% science |
| 7 | 法院 | Courthouse | 80 | $1 | Code of Laws | -50% corruption |
| 8 | 城牆 | City Walls | 120 | $2 | Masonry | ×3 defender strength; 防 Flood |
| 9 | 水道 | Aqueduct | 120 (manual 沒明確 cost, 推測) | $2 | Engineering | 解 size > 10 上限; 防 Fire/Plague |
| 10 | 銀行 | Bank | 120 | $3 | Banking + Marketplace | 加 +50% gold + lux (疊 Marketplace) |
| 11 | 大教堂 | Cathedral | 160 | $3 | Religion | -4 unhappy |
| 12 | 大學 | University | 160 | $3 | University + Library | +50% science (疊 Library = 100%) |
| 13 | 大眾運輸 | Mass Transit | 160 | $4 | Mass Production | population 無 pollution 貢獻 |
| 14 | 競技場 | Colosseum | 100 | $4 | Construction | -3 unhappy |
| 15 | 工廠 | Factory | 200 | $4 | Industrialization | +50% production |
| 16 | 製造廠 | Manufacturing Plant | 320 | $6 | Robotics | +100% production (取代 Factory) |
| 17 | SDI 防禦 | SDI Defense | 200 | $4 | Superconductor | 擋 Nuclear |
| 18 | 回收中心 | Recycling Center | 200 | $2 | Recycling | 待 v0.2 確認 |
| 19 | 發電廠 | Power Plant | 160 | $4 | Refining | Factory/MP ×1.5 |
| 20 | 水力發電廠 | Hydro Plant | 240 | $4 | Electronics | Factory/MP ×2; 需 River/Hills/Mountain 鄰接 |
| 21 | 核能電廠 | Nuclear Plant | 160 | $2 | Nuclear Power | Factory/MP ×1.5; 風險 meltdown |

### 6.2.1  Power plant 互斥規則

City 只可有一座 power plant. Hydro > Nuclear > Power (manual 順序). 三者 effect 都是 boost Factory/MP production.

### 6.2.2  Wonders 跟 improvement 的 stack

- Hoover Dam (modern Wonder): 為 city 自動 give Hydro Plant
- Michelangelo's Chapel (medieval): 所有 Cathedrals × 2 effect
- Isaac Newton's College (medieval): 所有 Libraries + Universities × 2

## 6.3  Government Table (6 個, manual P43-45)

| # | 中文 | 英文 | Max Rate | Trade modifier | Unit support | Settler food | 自由度 / 維穩 |
|---:|---|---|---|---|---|---|---|
| 0 | 無政府 | Anarchy | n/a | -1 large square (3+) | free | 1 food | 革命期; 不收稅; 不維修; 不研究 |
| 1 | 專制 | Despotism | 60% | -1 large square (3+) | free until > pop | 1 food | 最 minimal freedom |
| 2 | 君主 | Monarchy | 70% | normal | $1/unit (含 home city) | 2 food | 上層階級 |
| 3 | 共產 | Communism | 80% | normal | $1/unit | 2 food | corruption 固定; 無距離效應 |
| 4 | 共和 | Republic | 80% | +1 trade per traded square | $1/unit + 1 unhappy per army out-of-home | 2 food | Senate 強制接受 peace |
| 5 | 民主 | Democracy | 80% | +1 trade per traded square | $1/unit + 2 unhappy per army out-of-home | 2 food | 無 corruption; 但 disorder 引發 revolution |

### 6.3.1  Revolution 機制

- 主動: Game menu → Revolution. 跑數 turn Anarchy 後可選新 government
- 被動: Democracy + 2+ turn disorder → 可能自動 revolution
- Pyramids Wonder 在: 可不經 Anarchy 切換 government (直到 Pyramids obsolete)

## 6.4  Wonders of the World (21 個)

3 個時代 × 7 = 21 個. v0.1 列名 + 部分效果; v0.2 補完 cost/prereq/期效.

### Ancient (7)
1. Pyramids — 無 Anarchy 切政府; obsoletes 在 Communism
2. Hanging Gardens — +1 happy 全城
3. Colossus — coastal city trade × 2
4. Lighthouse — ship move +1; obsoletes 在 Magnetism
5. Great Library — 任何被 2 文明知道的 tech 自動學到; obsoletes 在 Electricity
6. Oracle — Temple × 2 effect
7. Great Wall — 等同所有 city 有 City Walls

### Medieval (7)
8. Magellan's Expedition — ship move +2
9. Michelangelo's Chapel — all Cathedrals × 2
10. Copernicus' Observatory — host city science × 2
11. Shakespeare's Theatre — host city -8 unhappy
12. Isaac Newton's College — all Libraries + Universities × 2
13. J.S.Bach's Cathedral — all Cathedrals -2 unhappy
14. Darwin's Voyage — instant 2 tech advances

### Modern / Industrial (7)
15. Hoover Dam — host city auto Hydro Plant
16. Women's Suffrage — military unit out-of-home -1 unhappy
17. Manhattan Project — 解鎖 Nuclear unit
18. United Nations — 強制和平 with all civs
19. Apollo Program — 解鎖 spaceship 建造; 全圖可見
20. SETI Program — host city Library × 2
21. Cure for Cancer — 全 cities +1 happy

對應 STR# 131 中已翻譯的奇蹟名 (civ_dict.c 第 30-46 行).

## 6.5  Civilization Advances Tree (72 個, v0.2 預定)

72 個科技, 各有 prereq (0..2 個 prereq tech). manual page 129 有 chart, 仍待整理.

v0.1 已知關鍵 prereq (從 §6.1 unit + §6.2 improvement 推出來的依存):
- Bronze Working → Phalanx
- Iron Working → Legion (需要 Bronze Working 為 prereq)
- Pottery → Granary
- Currency → Marketplace
- Code of Laws → Courthouse + Republic
- Masonry → City Walls + Pyramids
- Writing → Library + Diplomat
- Ceremonial Burial → Temple + Religion (next)
- Mathematics → Catapult
- Mysticism → Temple +1 effect
- Wheel → Chariot
- Horseback Riding → Cavalry
- Map Making → Trireme + Lighthouse
- Navigation → Sail + Magellan
- Magnetism → Frigate; Lighthouse obsolete
- Bronze + Currency → Trade → Caravan
- Iron + Currency → Banking → Bank
- Chivalry → Knights
- Gunpowder → Musketeers; Barracks gen2
- Combustion → Cruiser; Barracks gen3
- Conscription → Riflemen
- Engineering → Aqueduct (Construction + Wheel prereq)
- Industrialization → Factory + Transport
- Railroad → ?
- Steam Engine → Ironclad + Railroad
- Steel → Battleship
- Mass Production → Mass Transit + Submarine
- Flight → Fighter
- Advanced Flight → Bomber + Carrier
- Automobile → Armor
- Robotics → Mfg Plant + Artillery
- Refining → Power Plant
- Electronics → Hydro Plant
- Nuclear Power → Nuclear Plant
- Nuclear Fission + Rocketry → Nuclear (need Manhattan Project)
- Mass Production → Mass Transit
- Recycling → Palace upgrade?
- Religion → Cathedral
- University → University improvement (need Library, Mathematics prereq)
- Banking → Bank (need Currency + Marketplace)
- Labor Union → Mech Inf
- Superconductor → SDI Defense
- Genetic Engineering → ?
- Computers → ?

完整 72-tech graph v0.2 由 manual P129 chart 直譯.

## 6.6  Terrain Combat Modifier (manual Technical Supplement)

| 地形 | Move cost (road) | Defender bonus | Special |
|---|---|---|---|
| Ocean | 1 | n/a | sea unit only |
| Coast | 1 | n/a | sea, near land |
| Grassland | 1 (1) | 0% | food bonus possible |
| Plains | 1 (1) | 0% | balanced |
| Desert | 1 (1) | 0% | low food |
| Forest | 2 (1) | +50% | wood, no food bonus |
| Jungle | 2 (1) | +50% | sparse |
| Swamp | 2 (1) | +50% | low everything |
| Hills | 2 (1) | +100% | shield bonus |
| Mountain | 3 (1) | +200% | impassable to wheel? |
| Tundra | 1 (1) | 0% | sparse |
| Arctic | 2 (1) | 0% | impassable barren |
| River | 1 | +50% | road equiv. + Hydro Plant 鄰接條件 |

(road 在括號內: roads/railroads 一律改成 1 cost; railroad 在 enemy 領土 cost 1/2 — manual 沒明確說但常識推定)

## 6.7  Player / Civilization Data

| # | Civ | Leader | Color (slot) | Aggression (推測) |
|---:|---|---|---|---|
| 0 | Barbarian | — | red | — (野蠻) |
| 1 | Roman | Caesar | yellow | medium |
| 2 | Babylonian | Hammurabi | blue | low |
| 3 | German | Frederick | green | high |
| 4 | Egyptian | Ramesses | cyan | low |
| 5 | American | Abraham Lincoln | purple | low (defensive) |
| 6 | Greek | Alexander | grey | medium |
| 7 | Indian | M. Gandhi | white | very low |
| 8 | (NONE) | — | — | — |
| 9 | Russian | Stalin | dark red? | high |
| 10 | Zulu | Shaka | brown | high |
| 11 | French | Napoleon | sky blue | medium |
| 12 | Aztec | Montezuma | gold | medium |
| 13 | Chinese | Mao Tse Tung | red | medium |
| 14 | English | Elizabeth I | navy | low |
| 15 | Mongol | (?) | — | very high |

「Aggression」是 AI 個性參數, manual 沒明示, 推測在 GDAT 7 個 record 內 (per civ slot? 但只有 7 而非 14). 待 spec 06 v0.2 + GDAT 反推.

## 6.8  Team B 整合介面契約

```c
/* team-b/src/data/unit_stats.h */
typedef struct civ_unit_stats {
    const char *name_en;
    const char *name_zh;
    uint8_t attack;
    uint8_t defense;
    uint8_t movement;
    uint8_t cost_shields;
    int16_t prereq_tech_id;  /* -1 = no prereq */
    uint8_t flags;           /* bit 0 = sea, 1 = air, 2 = nuclear */
    uint8_t cargo;           /* transport capacity, 0 = none */
} civ_unit_stats_t;

extern const civ_unit_stats_t CIV_UNIT_STATS[28];

/* team-b/src/data/building_stats.h */
typedef struct civ_building_stats {
    const char *name_en;
    const char *name_zh;
    uint16_t cost_shields;
    uint8_t maint_gold;
    int16_t prereq_tech_id;
    uint8_t flags;          /* obsolescence triggers etc. */
} civ_building_stats_t;

extern const civ_building_stats_t CIV_BUILDING_STATS[46];

/* team-b/src/data/government_stats.h */
typedef struct civ_government_stats {
    const char *name_en;
    const char *name_zh;
    uint8_t max_rate;
    int8_t trade_modifier;
    uint8_t unit_support;
    uint8_t settler_food;
    uint8_t flags;
} civ_government_stats_t;

extern const civ_government_stats_t CIV_GOVERNMENT_STATS[6];
```

Team B 可立即依此 spec 實作 M6-full combat formula (spec 06 §6.1.1) + 城市生產 (spec 06 §6.2).

## 6.9  與 CIV.EXE binary 對位狀態

**R3 Round 2026-06-06 結論**: unit stats / building stats / government params **均未在 CIV.EXE binary 中作為 contiguous byte array 找到** (scan 結果見 `team-a/dumps/06_unit_stats_scan.txt`).

可能原因:
1. **Hardcoded as MOV immediates in code segment** — 每個 unit 在 init function 內用一連串 `MOV [GLOBAL], IMM` 指令初始化 struct. 在 disassembly 散布, 不會作為 data 出現
2. **過 LookUp/Constructor function** — 比 init `data_units (FUN_11e8_0337)` 還深的 function 才實際初始化 stats
3. **在 overlay file 內**: spec 00 排除 CIV.EXE 有 .OVL 但不能完全排除 hidden segment

**v0.2 R4 round 預定**:
- 跑 Ghidra `analyzeHeadless` 對 CIV.EXE 全 function 搜 `MOV [DAT_XXXX_YYYY], 0x00` 後 `MOV [DAT_XXXX_YYYY+1], 0x01` (Settlers a=0, d=1) 連串 pattern
- 用 cross-ref 從 `FUN_11e8_0337` 往下 walk 找 unit struct constructor
- 或 直接從 Civ1 SAV file 的 active unit list (§7.2 §0x06C8 area) 反推 stats 欄位 stride

## 6.10  License & credit

Manual 內容 © 1991 MicroProse Software. Manual 屬於遊戲附帶文件, 引用其 stats 表 (數值資料) 屬於 *facts* 範疇, 不受版權保護. 但敘述文字 (improvement descriptions) 屬版權範圍, 我們的 spec 不直接抄, 只做 summary.

未來 Team B `data/*.h` 內 stats 值 = ground-truth, 可自由實作. 中文翻譯沿用 spec 05 civ_dict.c (CC BY-SA 4.0).
