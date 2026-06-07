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

/* R27-A: 接通 city.wonders_bitmap (22 bit 對齊 world/wonder.h).
 * spec 09 §9.3 公式裡 wonders_owned 算 player 當前持有 wonder 數
 * (即使被搶/搶來都算當前持有者). */
static int count_wonders(const struct civ_game *g, int player_slot)
{
    int count = 0;
    const civ_world_t *w = &g->world;
    for (int i = 0; i < w->cities_count; i++) {
        const civ_city_t *c = &w->cities[i];
        if (!c->alive) continue;
        if (c->owner != (uint8_t)player_slot) continue;
        uint32_t bm = c->wonders_bitmap;
        while (bm) {
            count += (bm & 1);
            bm >>= 1;
        }
    }
    return count;
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

    /* R27-A: 接通 world.tech_acquired bitmap (72 bit, tech.h enum).
     * future_tech = popcount(CIV_TECH_FUTURE_1..4 bits 68..71). */
    {
        int ft = 0;
        for (int i = 68; i < 72; i++) {
            if (g->world.tech_acquired[i / 64] & (1ULL << (i % 64))) ft++;
        }
        out->future_tech = ft;
    }
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

static int player_alive_city_count(const struct civ_game *g, int player_slot)
{
    int n = 0;
    const civ_world_t *w = &g->world;
    for (int i = 0; i < w->cities_count; i++) {
        if (!w->cities[i].alive) continue;
        if (w->cities[i].owner == (uint8_t)player_slot) n++;
    }
    return n;
}

/* spec 09 §9.7 civ_check_end v0.1: 接 YEAR_LIMIT + DESTROYED 兩 reason.
 * SPACE_WIN / CONQUEST_WIN / RETIRE / QUIT 待 victory flag 結構. */
civ_end_reason_t civ_check_end(const struct civ_game *g, int player_slot)
{
    if (!g) return CIV_END_NONE;

    /* YEAR_LIMIT: manual P22 西元 2100 強制 retire. */
    if (g->civ_year >= 2100) return CIV_END_YEAR_LIMIT;

    /* DESTROYED: player 已 found 過城但目前無 alive city.
     * 為避免新局尚未 found city 就觸發, 用「曾經 found」啟發: cities_count>0
     * 且 player 自己 alive city count == 0. */
    if (g->world.cities_count > 0 &&
        player_alive_city_count(g, player_slot) == 0) {
        return CIV_END_DESTROYED;
    }

    return CIV_END_NONE;
}

/* spec 09 §9.4 Hall of Fame 公式 v0.1.
 * difficulty factor: 1..5 (Chieftain..Emperor 對應 wizard cursor 0..4 + 1).
 * opponents factor: 1.0 + (opponents - 1) * 0.1, clip 0.5..1.5.
 *
 * 算法用 integer * integer / 10 來避免 float, factor 用 10×表示. */
int civ_hof_rank(int score, int difficulty, int opponents)
{
    if (score <= 0) return 0;
    if (difficulty < 1) difficulty = 1;
    if (difficulty > 5) difficulty = 5;
    if (opponents < 1) opponents = 1;
    if (opponents > 6) opponents = 6;

    int comp10 = 10 + (opponents - 1);   /* 10..15 (代表 1.0..1.5) */
    if (comp10 < 5) comp10 = 5;

    /* hof = score * difficulty * comp10 / 10 */
    long long t = (long long)score * difficulty * comp10 / 10;
    if (t > 2147483647LL) t = 2147483647LL;
    if (t < 0) t = 0;
    return (int)t;
}
