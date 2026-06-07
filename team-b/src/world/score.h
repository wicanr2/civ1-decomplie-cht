/*
 * world/score.h — R26-D Civilization Score 公式 (對齊 spec 09 §9.3 / §9.7)
 *
 * 1991 manual P23 公式 ground-truth:
 *   score = 2 * happy_citizens
 *         + 1 * content_citizens
 *         + 20 * wonders_owned
 *         + 3 * turns_of_peace
 *         + 10 * future_tech_acquired
 *         - 10 * polluted_squares
 *         + space_bonus
 *         + conquest_bonus
 *
 * 本 v0.1 實作只接通 happy / content / wonders / peace / future / pollution
 * 對應的「現有 world 狀態」估算 — 不存在的欄位 (例: 個別 happy/content split)
 * 取保守近似. v0.2 (R27+) 隨 city 結構擴充再補. 詳見 §9.7 註解.
 */
#ifndef CIV_WORLD_SCORE_H
#define CIV_WORLD_SCORE_H

#include <stdbool.h>

struct civ_game;

typedef struct civ_score_breakdown {
    int happy_citizens;     /* 對齊 §9.3: 全城 happy citizen 總和 */
    int content_citizens;   /* 對齊 §9.3: 全城 content citizen 總和 */
    int wonders_owned;      /* player 當前持有 wonder 數 */
    int turns_of_peace;     /* 連續無戰回合 (v0.1: 用 turn_number 代為佔位) */
    int future_tech;        /* 已研發 future tech 數 (v0.1: 0) */
    int polluted_squares;   /* 污染地塊數 (v0.1: 0) */
    int space_bonus;        /* 太空勝利 bonus (v0.1: 0) */
    int conquest_bonus;     /* 征服勝利 bonus (v0.1: 0) */
    int total;              /* 套上 §9.3 加權後總分 (>= 0) */
} civ_score_breakdown_t;

/* 算 player_slot (1..14) 當前分數, 結果寫到 *out.
 *
 * out 不可為 NULL. g 不可為 NULL.
 * 若 player_slot 不在 1..14 範圍, 所有欄位歸 0 (total = 0).
 *
 * 公式總分以 §9.3 加權算; 若結果 < 0 (大量 pollution) clip 為 0
 * 對齊 manual "Hall of Fame requires non-negative score". */
void civ_score_compute(const struct civ_game *g, int player_slot,
                       civ_score_breakdown_t *out);

/* 快取 total only (= breakdown.total) — status panel 顯示用. */
int civ_score_total(const struct civ_game *g, int player_slot);

/* spec 09 §9.7 game-end reason. R27-C 接通 contract.
 *   QUIT     — 玩家主動結束 (Alt+Q from map)
 *   RETIRE   — 達到 game-year 上限 (manual 西元 2100 強制)
 *   DESTROYED— player 0 city alive
 *   SPACE_WIN — 任一 civ 太空船到 Alpha Centauri
 *   CONQUEST_WIN — player 消滅所有其他 civ
 *   YEAR_LIMIT — 西元 2100 強制 retire (跟 RETIRE 同效但 reason 不同) */
typedef enum {
    CIV_END_NONE         = 0,
    CIV_END_QUIT         = 1,
    CIV_END_RETIRE       = 2,
    CIV_END_DESTROYED    = 3,
    CIV_END_SPACE_WIN    = 4,
    CIV_END_CONQUEST_WIN = 5,
    CIV_END_YEAR_LIMIT   = 6,
} civ_end_reason_t;

/* 檢查當前 game state 是否觸發結束. 回 reason (CIV_END_NONE = 沒結束).
 *
 * R27-C v0.1 接通:
 *   YEAR_LIMIT — g->civ_year >= 2100 (對齊 manual P22 西元 2100 強制 retire)
 *   DESTROYED  — player_slot 沒任何 alive city (假設 player 已 found 過)
 *
 * v0.2 待補 SPACE_WIN / CONQUEST_WIN / RETIRE / QUIT (需 victory flag 結構). */
civ_end_reason_t civ_check_end(const struct civ_game *g, int player_slot);

/* Hall of Fame entry. 5 個存進 ~/.civ1-cht/hall_of_fame.json (v0.2). */
typedef struct civ_hof_entry {
    char     leader_zh[32];
    int      score;
    int      hof_rank;
    int      civ_year_ended;
    civ_end_reason_t end_reason;
} civ_hof_entry_t;

/* spec 09 §9.4 公式: hof_rank = score × difficulty_factor × competition_factor.
 *
 *   difficulty: 1..5 (CIV_DIFF_CHIEFTAIN..EMPEROR, 對應 wizard difficulty cursor)
 *     factor = difficulty (1..5 倍, manual P23 估算; v0.2 與 6 級 multiplier 對位)
 *   opponents: 對手 civ 數 (1..6, 1991 Civ1 7 player slot 含 player)
 *     factor ≈ 1.0 + (opponents - 1) × 0.1 (推測 0.5..1.5 range)
 *
 * 不超 OOM: 用 integer×integer 算後 / 10 對應 0.1 step factor; clip 為 0 if score < 0. */
int civ_hof_rank(int score, int difficulty, int opponents);

#endif /* CIV_WORLD_SCORE_H */
