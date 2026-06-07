/*
 * world/score.c — Civilization Score 公式 v0.1 (spec 09 §9.3)
 *
 * v0.1 把現存 city / turn / world 結構能算的部分接通; 未存的因子
 * (future tech / pollution / 連續和平 / 太空船 / 征服 flag) 留 0,
 * 等後續 R round 結構擴充再實作.
 *
 * 公式 (manual P23):
 *   total = 2*happy + 1*content + 20*wonders + 3*peace + 10*future
 *         - 10*pollution + space_bonus + conquest_bonus
 */
#include "score.h"

#include "../civ_game.h"
#include "world.h"

#include <string.h>

/* v0.1 近似: city.population 全部當 content citizen
 * (Civ1 原版 happy/content/unhappy 三分; 我們現在只有 population 一個欄位).
 * 之後 R 補 city.happy / city.content 後再拆. */
static void count_citizens(const struct civ_game *g, int player_slot,
                           int *happy, int *content)
{
    int total_pop = 0;
    const civ_world_t *w = &g->world;
    for (int i = 0; i < w->cities_count; i++) {
        const civ_city_t *c = &w->cities[i];
        if (!c->alive) continue;
        if (c->owner != (uint8_t)player_slot) continue;
        total_pop += c->population;
    }
    *happy   = 0;            /* v0.1 沒拆, 之後接 city.happy_count */
    *content = total_pop;    /* 全部視為 content */
}

/* v0.1: buildings_bitmap 25 bit 暫不區分「Wonder vs 一般 building」—
 * 對齊 spec 06 §6.2 待 wonder slot RE 完成後拆.
 * 暫回 0 (合 §9.3 保守). 之後接 player wonders bitmap. */
static int count_wonders(const struct civ_game *g, int player_slot)
{
    (void)g; (void)player_slot;
    return 0;
}

void civ_score_compute(const struct civ_game *g, int player_slot,
                       civ_score_breakdown_t *out)
{
    memset(out, 0, sizeof *out);

    if (player_slot < 1 || player_slot > 14) return;

    int happy = 0, content = 0;
    count_citizens(g, player_slot, &happy, &content);

    out->happy_citizens   = happy;
    out->content_citizens = content;
    out->wonders_owned    = count_wonders(g, player_slot);

    /* v0.1: 沒專屬 war flag, 用 turn_number 代為「連續和平回合」估算上限.
     * 之後接 civ_world.war_with_bitmap 後改成「自上次戰鬥起 turns」. */
    out->turns_of_peace = (int)g->turn_number;

    out->future_tech      = 0;   /* v0.1: 沒追蹤 tech_acquired set */
    out->polluted_squares = 0;   /* v0.1: 沒 pollution overlay */
    out->space_bonus      = 0;   /* v0.1: 沒太空船 */
    out->conquest_bonus   = 0;   /* v0.1: 沒征服 flag */

    int t = 2 * out->happy_citizens
          + 1 * out->content_citizens
          + 20 * out->wonders_owned
          + 3 * out->turns_of_peace
          + 10 * out->future_tech
          - 10 * out->polluted_squares
          + out->space_bonus
          + out->conquest_bonus;

    if (t < 0) t = 0;            /* HoF requires non-negative */
    out->total = t;
}

int civ_score_total(const struct civ_game *g, int player_slot)
{
    civ_score_breakdown_t b;
    civ_score_compute(g, player_slot, &b);
    return b.total;
}
