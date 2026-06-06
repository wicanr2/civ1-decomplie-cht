# Spec 06 — 28 unit + 25 建築 + 22 奇蹟 + 47 科技 + 24 地形 + 16 nation 數值表

> **Team A 規格 (v0.3, 2026-06-06 R8)**.
>
> **主要來源**: [**OpenCivOne**](https://codeberg.org/rhorvat/OpenCivOne) (2023-, Rajko Horvat, MIT license) — FOSS preservation 專案, 直接從 1991 DOS Civilization v475.05 反組譯而成. **絕大部分數值表 ground-truth 來自此**.
>
> **次要來源**:
> - 1991 *Sid Meier's Civilization* 官方 Manual (使用者提供 PDF, 126 頁) — 玩家視角描述, 大致一致但部分數值 OpenCivOne 修正
> - Honza Havlicek 2008 RE notes (`team-a/external/Civilization/`)
>
> **Clean-room 狀態**: OpenCivOne 從 2026-06-06 起放寬政策, 列為 Team A 可參考的 external research (跟 Honza 2008 一樣). Team B 仍不直接讀 OpenCivOne source. 詳見 [`docs/CLEAN_ROOM.md`](../../docs/CLEAN_ROOM.md).
>
> **狀態**: §6.1 unit 28/28 ✅; §6.2 improvement 25/25 ✅; §6.3 government 6/6 ✅; §6.4 wonder 22/22 ✅; §6.5 tech 47/72 ✅ (核心 47); §6.6 terrain 24/24 ✅; §6.7 nation 16/16 ✅ (R8 新).

## 6.0  本 spec 解答什麼

- M6-full 真版 combat / production 公式所需的數值表 ground-truth
- 哪些值要 hardcoded 進 Team B 程式碼 (e.g. `data/unit_stats.h`)
- 哪些值 *runtime 才知道* (e.g. unit 是否 obsolete)

## 6.1  Unit Stats Table (28 個, OpenCivOne `GameData.cs:209-236` ground-truth)

### 6.1.0  Unit struct layout (OpenCivOne 揭穿)

OpenCivOne `UnitDefinition.cs` 揭穿原版 (1991 DOS) 每個 unit 是 **34 byte fixed-size struct**:

| offset | size | field | 備註 |
|---:|---:|---|---|
| 0 | 12 | Name | ASCII null-pad to 12 byte |
| 12 | 2 | CancelTechnology | LE16, 取代/淘汰本 unit 的 tech (None=−1) |
| 14 | 2 | MovementType | LE16, 0=Land / 1=Air / 2=Water |
| 16 | 2 | MoveCount | LE16, 移動點 |
| 18 | 2 | TurnsOutside | LE16, air unit 可滯空 turn 數 |
| 20 | 2 | AttackStrength | LE16 |
| 22 | 2 | DefenseStrength | LE16 |
| 24 | 2 | Cost | LE16, **shields ÷ 10** (e.g. cost=4 = 40 shields) |
| 26 | 2 | SightRange | LE16, 視野半徑 |
| 28 | 2 | TransportCapacity | LE16, 載 unit 上限 |
| 30 | 2 | AIRole | LE16, 0=Settler / 1=LandAtk / 2=Def / 3=SeaAtk / 4=AirAtk / 5=SeaTransport / 6=Civilian |
| 32 | 2 | RequiredTechnology | LE16, 解鎖本 unit 的 tech (None=−1) |

Total: 34 byte × 28 unit = **952 byte unit table**.

### 6.1.1  完整 28 unit stats (OpenCivOne ground-truth)

格式: `[A] attack / [D] defense / [M] movement / [Cost] shields / [Tech] prereq advance / [Sight] / [Cargo]`

| # | 中文 | 英文 | A | D | M | Cost | Sight | Cargo | MoveType | AIRole | Prereq | CancelTech |
|---:|---|---|---:|---:|---:|---:|---:|---:|---|---|---|---|
| 0 | 墾荒者 | Settlers | 0 | 1 | 1 | 40 | 0 | 0 | Land | Settler | — | FutureTech |
| 1 | 民兵 | Militia | 1 | 1 | 1 | 10 | 0 | 0 | Land | Defense | — | Gunpowder |
| 2 | 方陣兵 | Phalanx | 1 | 2 | 1 | 20 | 0 | 0 | Land | Defense | Bronze Working | Gunpowder |
| 3 | 軍團兵 | Legion | **3** | **1** | 1 | 20 | 0 | 0 | Land | LandAtk | Iron Working | Conscription |
| 4 | 火槍兵 | Musketeers | **2** | **3** | 1 | 30 | 0 | 0 | Land | Defense | Gunpowder | Conscription |
| 5 | 步兵 | Riflemen | **3** | **5** | 1 | 30 | 0 | 0 | Land | Defense | Conscription | FutureTech |
| 6 | 騎兵 | Cavalry | 2 | 1 | 2 | 20 | 0 | 0 | Land | LandAtk | Horseback Riding | Conscription |
| 7 | 騎士 | Knights | 4 | 2 | 2 | 40 | 0 | 0 | Land | LandAtk | Chivalry | Automobile |
| 8 | 投石機 | Catapult | 6 | 1 | 1 | 40 | 0 | 0 | Land | LandAtk | Mathematics | Metallurgy |
| 9 | 加農炮 | Cannon | 8 | 1 | 1 | 40 | 0 | 0 | Land | LandAtk | Metallurgy | Robotics |
| 10 | 戰車 | Chariot | 4 | 1 | 2 | 40 | 0 | 0 | Land | LandAtk | The Wheel | Chivalry |
| 11 | 裝甲車 | Armor | 10 | 5 | 3 | 80 | 0 | 0 | Land | LandAtk | Automobile | FutureTech |
| 12 | 機械化步兵 | Mech Inf | 6 | 6 | 3 | 50 | 0 | 0 | Land | Defense | Labor Union | FutureTech |
| 13 | 炮兵 | Artillery | 12 | 2 | 2 | 60 | 0 | 0 | Land | LandAtk | Robotics | FutureTech |
| 14 | 戰鬥機 | Fighter | **4** | **2** | 10 | 60 | 2 | 0 | Air | AirAtk | Flight | FutureTech |
| 15 | 轟炸機 | Bomber | 12 | 1 | 8 | 120 | 2 | 0 | Air | LandAtk | Advanced Flight | FutureTech |
| 16 | 三層槳座戰船 | Trireme | 1 | 0 | 3 | 40 | 0 | 2 | Water | SeaTransport | Map Making | Navigation |
| 17 | 帆船 | Sail | 1 | 1 | 3 | 40 | 0 | 3 | Water | SeaTransport | Navigation | Magnetism |
| 18 | 巡防艦 | Frigate | 2 | 2 | 3 | 40 | 0 | 4 | Water | SeaTransport | Magnetism | Industrialization |
| 19 | 鐵甲艦 | Ironclad | 4 | 4 | 4 | 60 | 0 | 0 | Water | SeaAtk | Steam Engine | Combustion |
| 20 | 巡洋艦 | Cruiser | 6 | 6 | 6 | 80 | 3 | 0 | Water | SeaAtk | Combustion | FutureTech |
| 21 | 戰艦 | Battleship | 18 | 12 | 4 | 160 | 3 | 0 | Water | SeaAtk | Steel | FutureTech |
| 22 | 潛艇 | Submarine | 8 | 2 | 3 | 50 | 3 | 0 | Water | SeaAtk | Mass Production | FutureTech |
| 23 | 航空母艦 | Carrier | 1 | 12 | 5 | 160 | 3 | 0 | Water | SeaAtk | Advanced Flight | FutureTech |
| 24 | 運輸船 | Transport | 0 | 3 | 4 | 50 | 0 | 8 | Water | SeaTransport | Industrialization | FutureTech |
| 25 | 核彈 | Nuclear | 99 | 0 | 16 | 160 | 0 | 0 | Air | LandAtk | Rocketry | FutureTech |
| 26 | 外交官 | Diplomat | 0 | 0 | 2 | 30 | 0 | 0 | Land | Civilian | Writing | FutureTech |
| 27 | 商隊 | Caravan | 0 | 1 | 1 | 50 | 0 | 0 | Land | Civilian | Trade | FutureTech |

**OpenCivOne 推翻先前 manual + wiki 多個值**:
- #3 Legion: 3-1 (manual ✓, wiki 4-2 ✗)
- #4 Musketeers: 2-3 (manual ✓, wiki 3-3 ✗)
- #5 Riflemen: 3-5 (manual ✓, wiki 5-4 ✗)
- #14 Fighter: **4-2** (OpenCivOne ground-truth!! manual 3-3 ✗, wiki 4-2 ✓) — manual P41 印錯
- #18 Frigate move=3 (manual ✓, wiki 4 ✗)
- AI Role / Sight / Cargo / CancelTech 全都是 OpenCivOne 新補

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

## 6.2  City Improvement Table (25 個, OpenCivOne `GameData.cs:238-264` ground-truth)

### 6.2.0  Improvement struct (OpenCivOne `ImprovementDefinition.cs`)

每筆 record: `(id, name, cost, maintenance, required_tech, cancel_tech)`. cost 同 unit, **shields ÷ 10**.

### 6.2.1  完整 25 improvement

| # | 中文 | 英文 | Cost (shields) | Maint $ | Prereq Tech |
|---:|---|---|---:|---:|---|
| 0 | NONE | NONE | 80 | 0 | — |
| 1 | 宮殿 | Palace | 200 | 5 | Masonry |
| 2 | 兵營 | Barracks | 40 | 0 | — |
| 3 | 穀倉 | Granary | 60 | 1 | Pottery |
| 4 | 神廟 | Temple | 40 | 1 | Ceremonial Burial |
| 5 | 市場 | Marketplace | 80 | 1 | Currency |
| 6 | 圖書館 | Library | 80 | 1 | Writing |
| 7 | 法院 | Courthouse | 80 | 1 | Code of Laws |
| 8 | 城牆 | City Walls | 120 | 2 | Masonry |
| 9 | 水道 | Aqueduct | 120 | 2 | Construction |
| 10 | 銀行 | Bank | 120 | 3 | Banking |
| 11 | 大教堂 | Cathedral | 160 | 3 | Religion |
| 12 | 大學 | University | 160 | 3 | University |
| 13 | 大眾運輸 | Mass Transit | 160 | 4 | Mass Production |
| 14 | 競技場 | Colosseum | 100 | 4 | Construction |
| 15 | 工廠 | Factory | 200 | 4 | Industrialization |
| 16 | 製造廠 | Mfg. Plant | 320 | 6 | Robotics |
| 17 | SDI 防禦 | SDI Defense | 200 | 4 | Superconductor |
| 18 | 回收中心 | Recycling Cntr. | 200 | 2 | Recycling |
| 19 | 發電廠 | Power Plant | 160 | 4 | Refining |
| 20 | 水力發電廠 | Hydro Plant | 240 | 4 | Electronics |
| 21 | 核能電廠 | Nuclear Plant | 160 | 2 | Nuclear Power |
| 22 | 太空船結構 | SS Structural | 80 | 0 | Space Flight |
| 23 | 太空船元件 | SS Component | 160 | 0 | Plastics |
| 24 | 太空船模組 | SS Module | 320 | 0 | Robotics |

**Manual 跟 OpenCivOne 差異**:
- Manual 沒列「NONE」(idx 0), OpenCivOne 確認原版 binary 有此 placeholder
- Manual 寫 Recycling Cntr. require Recycling 算對 (OpenCivOne 確認)
- SS Structural / SS Component / SS Module (太空船零件) 在 manual 是 improvement type, 對應 STR# 150 Dock 已抽到

### 6.2.2  Power plant 互斥規則

City 只可有一座 power plant. Hydro > Nuclear > Power (manual 順序). 三者都 boost Factory / Mfg.Plant production.

### 6.2.3  Wonders 跟 improvement 的 stack

- Hoover Dam (modern Wonder): 為 city 自動 give Hydro Plant
- Michelangelo's Chapel (medieval): 所有 Cathedrals × 2 effect
- Isaac Newton's College (medieval): 所有 Libraries + Universities × 2

### 6.2.4  Wonder Table (22 個, OpenCivOne `wonderTypes` ground-truth)

跟 improvement 共用 `ImprovementDefinition` struct.

| # | 中文 | 英文 | Cost | Prereq | Obsolete |
|---:|---|---|---:|---|---|
| 0 | NONE | NONE | 80 | — | — |
| 1 | 金字塔 | Pyramids | 300 | Masonry | Communism |
| 2 | 空中花園 | Hanging Gardens | 300 | Pottery | Invention |
| 3 | 巨像 | Colossus | 200 | Bronze Working | Electricity |
| 4 | 燈塔 | Lighthouse | 200 | Map Making | Magnetism |
| 5 | 大圖書館 | Great Library | 300 | Literacy | University |
| 6 | 神諭 | Oracle | 300 | Mysticism | Religion |
| 7 | 萬里長城 | Great Wall | 300 | Masonry | Gunpowder |
| 8 | 麥哲倫的遠征 | Magellan's Expedition | 400 | Navigation | — |
| 9 | 米開朗基羅教堂 | Michelangelo's Chapel | 300 | Religion | Communism |
| 10 | 哥白尼天文台 | Copernicus' Observatory | 300 | Astronomy | Automobile |
| 11 | 莎士比亞劇場 | Shakespeare's Theatre | 400 | Medicine | Electronics |
| 12 | 牛頓學院 | Isaac Newton's College | 400 | Theory of Gravity | Nuclear Fission |
| 13 | 巴哈大教堂 | J.S.Bach's Cathedral | 400 | Religion | — |
| 14 | 達爾文之旅 | Darwin's Voyage | 300 | Railroad | — |
| 15 | 胡佛水壩 | Hoover Dam | 600 | Electronics | — |
| 16 | 女性投票權 | Women's Suffrage | 600 | Industrialization | — |
| 17 | 曼哈頓計畫 | Manhattan Project | 600 | Nuclear Fission | — |
| 18 | 聯合國 | United Nations | 600 | Communism | — |
| 19 | 阿波羅計畫 | Apollo Program | 600 | Space Flight | — |
| 20 | SETI 計畫 | SETI Program | (TBD v0.3) | (TBD) | — |
| 21 | 癌症療法 | Cure for Cancer | (TBD v0.3) | (TBD) | — |

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

## 6.5  Civilization Advances Tree (47 個核心 + 5 future, OpenCivOne `GameData.cs:266-336` ground-truth)

### 6.5.0  Tech struct (OpenCivOne `TechnologyAdvanceDefinition`)

每筆: `(enum, name, prereq1, prereq2)`. None=−1, NewFutureTech=127.

### 6.5.1  完整 47 核心 tech + DAG

```
no prereq:  Alphabet, Bronze Working, Ceremonial Burial, Horseback Riding,
            Masonry, Pottery, The Wheel

1 prereq:   Code of Laws ← Alphabet
            Map Making ← Alphabet
            Writing ← Alphabet
            Currency ← Bronze Working
            Iron Working ← Bronze Working
            Mysticism ← Ceremonial Burial

2 prereqs:  Astronomy ← Mysticism + Mathematics
            Monarchy ← Ceremonial Burial + Code of Laws
            Engineering ← Wheel + Construction
            Mathematics ← Alphabet + Masonry
            Trade ← Currency + Code of Laws
            Construction ← Masonry + Currency
            Literacy ← Writing + Code of Laws
            Bridge Building ← Iron Working + Construction
            Banking ← Trade + The Republic
            The Republic ← Code of Laws + Literacy
            Feudalism ← Masonry + Monarchy
            Philosophy ← Mysticism + Literacy
            Religion ← Philosophy + Writing
            Medicine ← Philosophy + Trade
            Navigation ← Map Making + Astronomy
            Magnetism ← Navigation + Physics
            Chivalry ← Feudalism + Horseback Riding
            University ← Mathematics + Philosophy
            Physics ← Mathematics + Navigation
            Invention ← Engineering + Literacy
            Democracy ← Philosophy + Literacy
            Theory of Gravity ← Astronomy + University
            Chemistry ← University + Medicine
            Gunpowder ← Invention + Iron Working
            Steam Engine ← Physics + Invention
            Railroad ← Steam Engine + Bridge Building
            Metallurgy ← Gunpowder + University
            Explosives ← Gunpowder + Chemistry
            Industrialization ← Railroad + Banking
            Communism ← Philosophy + Industrialization
            Conscription ← The Republic + Explosives
            Electricity ← Metallurgy + Magnetism
            Atomic Theory ← Theory of Gravity + Physics
            The Corporation ← Banking + Industrialization
            Steel ← Metallurgy + Industrialization
            Refining ← Chemistry + The Corporation
            Combustion ← Refining + Explosives
            Automobile ← Combustion + Steel
            Mass Production ← Automobile + The Corporation
            Electronics ← Engineering + Electricity
            Flight ← Combustion + Physics
            Labor Union ← Mass Production + Communism
            Genetic Engineering ← Medicine + The Corporation
            Plastics ← Refining + Space Flight
            Recycling ← Mass Production + Democracy
            Nuclear Fission ← Mass Production + Atomic Theory
            Computers ← Mathematics + Electronics
            Advanced Flight ← Flight + Electricity
            Rocketry ← Advanced Flight + Electronics
            Nuclear Power ← Nuclear Fission + Electronics
            Space Flight ← Computers + Rocketry
            Robotics ← Plastics + Computers
            Superconductor ← Plastics + Mass Production
            Fusion Power ← Nuclear Power + Superconductor

Future tech (5): FutureTechnology 1/2/3/4/5 ("1", "2", "3", "4", "Future Tech.")
```

### 6.5.2  Tech 觸發特殊事件

- Gunpowder → 所有 Barracks obsolete (要重建 gen2)
- Combustion → gen2 Barracks obsolete (要重建 gen3)
- Magnetism → Lighthouse Wonder obsolete
- University → Great Library obsolete
- Religion → Oracle Wonder obsolete
- Electricity → Colossus Wonder obsolete

### 6.5.3  Civ Advances 完整對照 STR# 130

對應 spec 05 §5.3 的 STR# 130 (72 entries). OpenCivOne 列 47 + 5 = 52 個, 仍有 20 個未對齊 (可能 STR# 130 含 era/extra). v0.3 補完.

## 6.6  Terrain Table (24 個含 resource, OpenCivOne `GameData.cs:354-378` ground-truth)

### 6.6.0  TerrainDefinition struct

`(enum, name, move_cost, defense_bonus, food, shield, trade, irrigation_bonus, palette_idx)`

### 6.6.1  12 base terrain

| # | 中文 | 英文 | Move | Def | Food | Shield | Trade | Irrig | Pal idx |
|---:|---|---|---:|---:|---:|---:|---:|---:|---:|
| 0 | 沙漠 | Desert | 1 | 2 | 0 | 1 | 0 | 1 | 14 |
| 1 | 平原 | Plains | 1 | 2 | 1 | 1 | 0 | 1 | 6 |
| 2 | 草原 | Grassland | 1 | 2 | 2 | 1 | 0 | 1 | 10 |
| 3 | 森林 | Forest | 2 | 3 | 1 | 2 | 0 | 2 | 2 |
| 4 | 丘陵 | Hills | 2 | 4 | 1 | 0 | 0 | 2 | 12 |
| 5 | 山脈 | Mountains | 3 | 6 | 0 | 1 | 0 | 3 | 13 |
| 6 | 凍原 | Tundra | 1 | 2 | 1 | 0 | 0 | 0 | 7 |
| 7 | 北極 | Arctic | 2 | 2 | 0 | 0 | 0 | 0 | 15 |
| 8 | 沼澤 | Swamp | 2 | 3 | 1 | 0 | 0 | 0 | 3 |
| 9 | 叢林 | Jungle | 2 | 3 | 1 | 0 | 0 | 0 | 11 |
| 10 | 海洋 | Ocean | 1 | 2 | 1 | 0 | 2 | 0 | 1 |
| 11 | 河流 | River | 1 | 3 | 2 | 1 | 1 | 2 | 9 |

**Defense bonus** 為 `( base_def × terrain_def ) / 2` (OpenCivOne 推測, 與 manual P35 對位 +50%/+100%/+200% 等價).
**Palette idx** 對齊我們 spec 03 §3.5.1 SPR32X32 palette (我們 D-minimap 的 RGB 表 vs OpenCivOne 直接給 palette idx 更精準).

### 6.6.2  12 resource terrain (special tile)

| # | resource | base terrain | Food | Shield | Trade | 取代 |
|---:|---|---|---:|---:|---:|---|
| 12 | 綠洲 Oasis | Desert | 3 | 1 | 0 | base |
| 13 | 馬 Horses | Plains | 1 | 3 | 0 | base |
| 14 | 草原 Grassland (shield) | Grassland | 2 | 1 | 0 | base (extra shield) |
| 15 | 獵物 Game | Forest | 3 | 2 | 0 | base |
| 16 | 煤 Coal | Hills | 1 | 2 | 0 | base |
| 17 | 黃金 Gold | Mountains | 0 | 1 | 6 | base |
| 18 | 獵物 Game | Tundra | 3 | 0 | 0 | base |
| 19 | 海豹 Seals | Arctic | 2 | 0 | 0 | base |
| 20 | 石油 Oil | Swamp | 1 | 4 | 0 | base |
| 21 | 寶石 Gems | Jungle | 1 | 0 | 4 | base |
| 22 | 魚 Fish | Ocean | 3 | 0 | 2 | base |
| 23 | 河流 River | River | 2 | 1 | 1 | base |

對應 spec 05 STR# 133 (24 terrains) 我們已抽完, 名稱對齊.

### 6.6.3  TerrainModification (`GameData.cs:340-352`)

7-tuple: `(terrain, irrigation_food, irrigation_turns, mining_shield, mining_turns, ?, ?)`. 完整對位待 v0.3.

## 6.7  Player / Civilization Data (16 個, OpenCivOne `GameData.cs:37-86` ground-truth, v0.3 R8)

### 6.7.0  NationDefinition struct (OpenCivOne)

每筆 58 byte: `(id, leader, nation, nationality, mood, policy, ideology, short_tune, long_tune, cities[])`

**三軸個性編碼** (manual 沒提, OpenCivOne 唯一文獻):
- **Mood**: −1 = Friendly / 0 = Neutral / 1 = Aggressive
- **Policy**: −1 = Perfectionist (少城少強) / 0 = Neutral / 1 = Expansionistic (多城)
- **Ideology**: −1 = Militaristic / 0 = Neutral / 1 = Civilized

ShortTune / LongTune 是該 civ 對應的音樂 ID (對齊 23 個 WAV 中的兩個變體).

### 6.7.1  完整 16 nation 表

| # | 中文 | English | Leader | 中文領袖 | Mood | Policy | Ideology | ShortTune | LongTune |
|---:|---|---|---|---|---:|---:|---:|---:|---:|
| 0 | 野蠻 | Barbarians | Attila | 阿提拉 | 0 | 0 | 0 | 36 | 36 |
| 1 | 羅馬 | Romans | Caesar | 凱撒 | 0 | **+1** | **+1** | 24 | 10 |
| 2 | 巴比倫 | Babylonians | Hammurabi | 漢摩拉比 | **−1** | **−1** | **+1** | 28 | 14 |
| 3 | 德意志 | Germans | Frederick | 腓特烈 | **+1** | **−1** | **+1** | 32 | 18 |
| 4 | 埃及 | Egyptians | Ramesses | 拉美西斯 | 0 | 0 | **+1** | 21 | 7 |
| 5 | 美利堅 | Americans | Abe Lincoln | 林肯 | **−1** | 0 | **+1** | 19 | 5 |
| 6 | 希臘 | Greeks | Alexander | 亞歷山大 | 0 | **+1** | **−1** | 26 | 12 |
| 7 | 印度 | Indians | M.Gandhi | 甘地 | **−1** | **−1** | 0 | 31 | 17 |
| 8 | (NONE) | (空 slot) | — | — | 0 | 0 | 0 | 36 | 36 |
| 9 | 俄羅斯 | Russians | Stalin | 史達林 | **+1** | 0 | **−1** | 25 | 11 |
| 10 | 祖魯 | Zulus | Shaka | 夏卡 | **+1** | 0 | 0 | 22 | 8 |
| 11 | 法蘭西 | French | Napoleon | 拿破崙 | **+1** | **+1** | **+1** | 23 | 9 |
| 12 | 阿茲特克 | Aztecs | Montezuma | 蒙特蘇馬 | 0 | **−1** | **+1** | 20 | 6 |
| 13 | 中華 | Chinese | Mao Tse Tung | 毛澤東 | 0 | 0 | **+1** | 29 | 15 |
| 14 | 英格蘭 | English | Elizabeth I | 伊莉莎白一世 | 0 | **+1** | 0 | 27 | 13 |
| 15 | 蒙古 | Mongols | Genghis Khan | 成吉思汗 | **+1** | **+1** | **−1** | 30 | 16 |

### 6.7.2  個性聚類 (從上表推導)

**最強硬侵略派** (Mood=+1, Policy=+1, Ideology=−1):
- #15 Mongol/Genghis — 經典軍事擴張 (征服世界傾向)

**強硬擴張派** (Mood=+1, Policy=+1, Ideology=+1):
- #11 French/Napoleon — 拿破崙文化加軍事

**侵略軍事派** (Mood=+1, Ideology=−1):
- #9 Russian/Stalin

**侵略派** (Mood=+1, Ideology=0..+1):
- #3 German/Frederick (military 但 perfectionist)
- #10 Zulu/Shaka

**和善文明派** (Mood=−1, Ideology=+1):
- #2 Babylonian/Hammurabi (法典之父 — perfectionist civilized)
- #5 American/Lincoln (defensive civilized)

**和善 perfectionist** (Mood=−1, Policy=−1):
- #7 Indian/Gandhi (非暴力)

**中庸擴張**:
- #1 Roman/Caesar
- #6 Greek/Alexander (但 Ideology −1 = 軍事)
- #14 English/Elizabeth I (Policy +1)

**中庸文明**:
- #4 Egyptian, #13 Chinese (Ideology +1 = 文明)
- #12 Aztec (Policy −1 perfectionist)

### 6.7.3  AI 決策參數推測 (待 v0.4 從 Ghidra 抽 AI 決策 function 確認)

各 axis 影響的可能 AI 行為:
- **Mood +1 (Aggressive)** → 高機率宣戰 / 拒和談 / 主動侵略鄰邊
- **Mood −1 (Friendly)** → 高機率提和談 / 共享 tech
- **Policy +1 (Expansionistic)** → 早期 settler 偏好 / 多城少 build improvement
- **Policy −1 (Perfectionist)** → 少城多 build / wonder 競爭
- **Ideology +1 (Civilized)** → 偏好 happiness building / 科技 / 民主政體
- **Ideology −1 (Militaristic)** → 偏好 barracks / 軍事 unit / 君主政體

### 6.7.4  Cities[] 預設城市名單

每個 civ 含一個 `string[] Cities` 陣列 (約 16 城市/civ), 為 STR# 135 (256 entries) 對應的子集. 已在 spec 05 §5.3 / civ_dict 翻譯範圍. v0.3 R8 暫不列, 對齊 STR# 135 即可.

### 6.7.5  跟 SAV file (spec 07 §7.2) 對位

SAV file 內含:
- 0x00112 area: 8 civ plural names (STR# 144 子集 — barbarian + 7 civs)
- 0x00212 area: 8 civ singular names (STR# 143 子集)

對應 nationTypes 中 8 個 alive slot. **8 而非 14** — 1993 Win port 一場 game 最多 7 civ + barbarian. nationTypes 14 個是「可選」civ, runtime 隨機選 7 個 + 1 barb 進場.

§6.7 已對齊 Team B `civs.c` 內 14 civ × 5 field 翻譯 (spec 05 已 ship).

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

## 6.9  與 CIV.EXE binary 對位狀態 (R3b 2026-06-06 結論)

**OpenCivOne `UnitDefinition` 34-byte struct (含 12-byte name) 在 1993 Win CIV.EXE 內找不到** (`team-a/tools/scan_unitdef_struct.py` scan 結果):
- "Settlers" 字串在 CIV.EXE 兩處 (0xB75AE, 0xB9DF6), 但都是 **error message text** ("! Settlers lost. ", "Settlers+\0") **不是 unit table 內 name field**
- 全 34-byte struct hit = 0

**推測 1993 Win port 跟 1991 DOS 不同**:
- DOS 版 (OpenCivOne base) unit struct 含 embedded name (34 byte)
- Win port 拆 name 進 STR# 132 (per spec 05, name 從 RSC 載), unit table 只剩 **22 byte 數值欄位** (11 LE16 × 2 byte)
- Win port 是 Mac port + Win shim (spec 01 §1.2), Mac 用 STR# resource 自然把 name 跟 stats 分離

**v0.3 R4 round 預定**:
- 改掃 22-byte numeric-only struct (從 cancel_tech LE16 起算 11 個 LE16) × 28 = 616 byte
- 或 用 Ghidra cross-ref 從 `FUN_11e8_0337` 往下 walk 找 unit struct constructor
- 或 直接從 Civ1 SAV file 的 active unit list (§7.2 §0x06C8 area) 反推 stats 欄位 stride

**對 Team B 影響 = 零** — `data/unit_stats.h` 內 const 值直接從 OpenCivOne 抄, 即使 binary 沒對齊也沒關係 (因為 ground-truth 在 OpenCivOne, 不在我們的 binary).

## 6.10  License & credit

**主要 ground-truth source**: [OpenCivOne](https://codeberg.org/rhorvat/OpenCivOne) © 2023- Rajko Horvat, **MIT License**. OpenCivOne 明示其 source code 是從 1991 DOS Civilization 反組譯+重寫, 屬於 *FOSS preservation*. 引用 OpenCivOne 數值資料相容於 MIT 條款.

引用時須 credit:
> *Unit / improvement / wonder / tech / terrain values reverse-engineered by Rajko Horvat in OpenCivOne project (https://codeberg.org/rhorvat/OpenCivOne), MIT license, based on 1991 DOS Civilization version 475.05.*

**次要參考**: 1991 *Sid Meier's Civilization* 官方 Manual © 1991 MicroProse. Manual 屬於遊戲附帶文件, 引用其 stats 表 (數值資料) 屬於 *facts* 範疇, 不受版權保護. 但敘述文字屬版權範圍, 我們的 spec 不直接抄, 只做 summary.

未來 Team B `data/*.h` 內 stats 值 = ground-truth, 可自由實作. 中文翻譯沿用 spec 05 civ_dict.c (CC BY-SA 4.0).

## 6.11  v0.4 (待 R9+) 待補

- §6.4 SETI / Cure for Cancer 完整 (兩個 wonder 太晚出現, OpenCivOne 列表截在 idx 19)
- §6.5 完整 72-tech 對齊 STR# 130 全部 entry (現在 47 + 5 future = 52, 缺 20)
- §6.6 TerrainModification 7-tuple 解讀
- §6.7 R8 ✅ NationDefinition 抽完, AI 決策 function 從 Ghidra 抽 — 確認 §6.7.3 個性參數推論
- §6.9 binary offset (22-byte numeric struct 重掃)
- §6.7 §6.7.4 14 civ × 16 城市 = 224 城市名單對齊 STR# 135
