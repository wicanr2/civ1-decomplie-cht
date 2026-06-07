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

#endif /* CIV_WORLD_SCORE_H */
