#include "turn.h"
#include "../civ_game.h"

void civ_turn_advance(struct civ_game *g)
{
    if (!g) return;
    g->turn_number++;
    g->ai_actions += 8;          /* 8 AI 玩家各 1 個 mock action */
    /* 西元年計算：每回合約 50 年（早期），50 年後縮為 25/10/5/2/1
     * （原版機制）；M6-lite 簡化為固定 50 年 / turn。 */
    g->civ_year = -4000 + (int)g->turn_number * 50;
}
