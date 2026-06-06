# Spec 09 — 結束條件 / 勝利 / Civilization Score / Hall of Fame

> **Team A 規格 (v0.1, 2026-06-06 R9)**.
>
> **主要來源**: 1991 Civ1 manual P22-23 *"Ending the Game and Winning"* (使用者提供 PDF).
>
> **次要來源**: STR# 155 (Space 1) / STR# 156 (Space 2) / STR# 157 (Archeologist) / STR# 136 idx 11 "Hall of Fame" — 翻譯側已 ship (civ_dict.c).

## 9.0  本 spec 解答什麼

- 一場 Civilization 怎麼結束的 5 種方式
- 2 種勝利條件 (征服 / 太空)
- Civilization Score 完整公式
- Hall of Fame ranking 算法 + 5 entry 儲存
- M7 / M8 milestone 的 game-end trigger

## 9.1  5 種結束方式 (manual P22)

| # | 觸發 | Hall of Fame? | 後續可繼續玩? |
|---:|---|---|---|
| 1 | **Quit** (Alt+Q from map) | ❌ 不入 HoF | 不能 |
| 2 | **Retire** (Game menu) | ✅ 計分入 HoF | 不能 |
| 3 | **被消滅** (所有 city 被滅) | ❌ 不入 HoF | 不能, 可看 replay |
| 4 | **太空船抵達 Alpha Centauri** (任一 civ) | ✅ 計分 | 可繼續玩 (不計分) |
| 5 | **征服世界** (消滅其他全 civ) | ✅ 計分 | 可繼續玩 (不計分) |

註: Civ1 manual 沒明示 "西元 2100 強制結束" 規則, 但 OpenCivOne 跟其他考古證實 1991 DOS 版 v475.05 有此規則 (year >= 2100 自動 retire). 1993 Win port 行為待 spec 09 v0.2 確認.

## 9.2  2 種勝利條件 (manual P23 "Winning")

### 9.2.1  征服世界 (Conquest Victory)

- **條件**: 場上只剩你自己的 civilization, 所有其他 civ 都被消滅 (含 barbarian — 但 barbarian 不算 civ)
- **manual 評語**: "very hard to accomplish"
- **bonus**: 高達 **+1000 civilization points** + 年代 bonus (越早征服越多)

### 9.2.2  太空殖民 (Space Race Victory)

- **條件**: 任一 civ 太空船含 colonists 抵達 Alpha Centauri
- **不一定要你的太空船** — 即使他國太空船先到, 你存活到那時也得 partial credit (但 bonus 0)
- **manual 評語**: "much more likely to win by being in existence when colonists reach Alpha Centauri"
- **bonus** (只給造船+先到的 civ):
  - 50 × (colonists / 10000) × success_percentage / 100
  - e.g. 10000 colonists × 80% success → 50 × 1 × 0.8 = 40 bonus

### 9.2.3  太空船零件 (對應 spec 06 §6.2 + STR# 150)

需先建 Apollo Program Wonder. 然後在城市生產:
- **SS Structural** (cost 80, spec 06 idx 22) — 推測上限 ~8 個
- **SS Component** (cost 160, idx 23) — 推測上限 ~4 個
- **SS Module** (cost 320, idx 24) — 推測上限 ~12 個

完成所有零件 → 發射. 抵達時間取決於船速 (含 module 數量, 比例計算). v0.2 待補確切公式.

## 9.3  Civilization Score 公式 (manual P23)

```
score = 2 × happy_citizens
      + 1 × content_citizens
      + 20 × wonders_owned
      + 3 × turns_of_peace (沒有任何 war)
      + 10 × future_tech_acquired
      - 10 × polluted_squares
      + space_colonists_bonus (若太空勝利)
      + conquest_bonus       (若征服勝利)
```

**Notes**:
- happy/content citizen 從 city 計, status panel 顯示分數
- "no war anywhere" = 沒 active war 跟任何 civ. 若 turn 內有戰 → 該 turn 不計 peace
- Wonder 計到構築的 civ; 即使被搶/搶來都算當前持有者
- pollution 算 currently polluted, 不算歷史曾污染
- future tech 只算超過 72 個 base tech 後 (對齊 spec 06 §6.5 NewFutureTech idx 127)

## 9.4  Hall of Fame Ranking (manual P23)

```
hof_rank = score × difficulty_factor × competition_factor
```

| 因子 | 範圍 | 來源 |
|---|---|---|
| `score` | 0..∞ | §9.3 |
| `difficulty_factor` | 1 (Chieftain) .. 6 (Emperor) | spec 06 §6.3 governments? 待 v0.2 確認 |
| `competition_factor` | 隨對手 civ 數 + AI personality 加權 | 推測 0.5..1.5 |

存 top 5 entries. 對應 STR# 136 idx 11 "Hall of Fame" 翻譯 = "名人堂" (civ_dict.c 已 ship).

## 9.5  與 STR# (spec 05) 對位

| STR# | 內容 | spec 09 用途 |
|---|---|---|
| 136 idx 11 | "Hall of Fame" → "名人堂" | UI 標籤 |
| 137 | Credits 10 條 | game-end 後可選顯示開發者 |
| 138 | Report Titles 7 條 | 含 SCIENCE REPORT (進度) / MILITARY (戰力) |
| 145 | King Text 8 條 (Attila/Caesar/Shaka/Genghis/Montezuma/Xerxes/Saladin/Suppiluliumas) | HoF 可能顯示參考歷史名君比較 |
| 146 | King Title 13 條 (Mr./Emperor/King/...) | HoF entry 含 title |
| **155** | **Space 1** 19 條 (太空勝利文字) | game-end 勝利 narrative |
| **156** | **Space 2** 19 條 (替代版本) | 同上 |
| **157** | **Archeologist** 11 條 (失敗結局: "Centuries later, archeologists discover the remains...") | game-end 失敗 narrative |

STR# 155/156/157 文字側已 RE (spec 05), 但翻譯未補 (待 D-#3 v2 round).

## 9.6  與 SAV file (spec 07 §7.2) 對位

SAV 可能存:
- current score (LE16 or LE32, 待 SAV §7.2 v0.2 對位)
- turns_of_peace counter
- future_tech_count
- polluted_squares_count
- game_over_flag (= 5 種結束方式之一)
- victory_type (= conquest / space / etc.)

對齊 SAV §7.2 v0.1 已標出的 V-category bytes (game state 變化處), v0.2 R10 補完.

## 9.7  Team B 整合介面契約

```c
/* team-b/src/game/score.h */
typedef struct civ_score_breakdown {
    int happy_citizens;
    int content_citizens;
    int wonders_owned;
    int turns_of_peace;
    int future_tech;
    int polluted_squares;
    int space_bonus;
    int conquest_bonus;
    int total;
} civ_score_breakdown_t;

void civ_score_compute(const struct civ_game *g, int player_slot,
                       civ_score_breakdown_t *out);

typedef enum {
    CIV_END_NONE = 0,
    CIV_END_QUIT,
    CIV_END_RETIRE,
    CIV_END_DESTROYED,
    CIV_END_SPACE_WIN,     /* 任一 civ 太空船到 Alpha Centauri */
    CIV_END_CONQUEST_WIN,  /* player 消滅所有其他 civ */
    CIV_END_YEAR_LIMIT,    /* 西元 2100 強制 retire */
} civ_end_reason_t;

/* 檢查當前 game state 是否觸發結束. 回觸發理由 (CIV_END_NONE = 沒結束). */
civ_end_reason_t civ_check_end(const struct civ_game *g);

/* Hall of Fame entry, 5 個存進 ~/.civ1-cht/hall_of_fame.json (or 對應 path). */
typedef struct civ_hof_entry {
    char     leader_zh[32];
    int      score;
    int      hof_rank;
    int      civ_year_ended;
    civ_end_reason_t end_reason;
} civ_hof_entry_t;

int civ_hof_load(civ_hof_entry_t entries[5]);
int civ_hof_save(const civ_hof_entry_t entries[5]);
int civ_hof_insert(civ_hof_entry_t entries[5], const civ_hof_entry_t *new_entry);
```

Team B M8 milestone 可以實作 §9.7 後 ship "M8 — game-end UI" 對齊 manual P23.

## 9.8  與 OpenCivOne 對位

OpenCivOne `src/Game/State/SpaceshipCell.cs` 含太空船零件 cell 結構 (Apollo Program 後可建). 完整 spaceship state 跨多個 City. v0.2 R10 整合.

## 9.9  v0.2 (待後 round) 待補

- 西元 2100 強制 retire 規則 (1993 Win port 行為確認)
- Hall of Fame 實際 storage path + format (是 RSC resource 還是獨立檔)
- difficulty_factor 6 級 × score multiplier 數值
- 太空船完整零件構築規則 (zone constraints, success% formula)
- STR# 155-157 中文化補完 (D-#3 v2)
- 對 OpenCivOne `SpaceshipCell.cs` 完整 layout

## 9.10  License & credit

Manual 內容 © 1991 MicroProse — 引用其分數公式 + 規則屬於 facts. OpenCivOne 對位是次要 ground-truth.

對應 STR# 155/156/157 翻譯 → 採 CC BY-SA 4.0 (civ_dict.c 翻譯沿用).
