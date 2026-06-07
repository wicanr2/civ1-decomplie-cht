/*
 * test_score.c — R26-D Civilization Score 公式驗算 (spec 09 §9.3)
 *
 * 不開 SDL window — 純結構單測.
 */
#include "civ_game.h"
#include "world/score.h"
#include "world/world.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EXPECT(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL %s:%d  %s\n", __FILE__, __LINE__, #cond); \
        exit(1); \
    } \
} while (0)

static void seed_city(struct civ_game *g, int x, int y,
                      uint8_t owner, int pop, const char *name)
{
    civ_world_t *w = &g->world;
    int idx = w->cities_count++;
    civ_city_t *c = &w->cities[idx];
    memset(c, 0, sizeof *c);
    snprintf(c->name, sizeof c->name, "%s", name);
    c->x = x; c->y = y;
    c->owner = owner;
    c->population = pop;
    c->alive = true;
}

int main(void)
{
    /* 1. invalid player_slot → 全 0 */
    {
        struct civ_game g = {0};
        civ_score_breakdown_t b;
        civ_score_compute(&g, 0, &b);
        EXPECT(b.total == 0);
        EXPECT(b.content_citizens == 0);

        civ_score_compute(&g, 99, &b);
        EXPECT(b.total == 0);
    }

    /* 2. 沒城市 + turn 0 → total = 0 (peace 也 0) */
    {
        struct civ_game g = {0};
        g.turn_number = 0;
        civ_score_breakdown_t b;
        civ_score_compute(&g, 1, &b);
        EXPECT(b.happy_citizens == 0);
        EXPECT(b.content_citizens == 0);
        EXPECT(b.turns_of_peace == 0);
        EXPECT(b.total == 0);
    }

    /* 3. 1 城 pop=5 + turn=10 → content 5 + peace 10
     *    total = 1*5 + 3*10 = 35 */
    {
        struct civ_game g = {0};
        g.turn_number = 10;
        seed_city(&g, 5, 5, /*owner=*/1, /*pop=*/5, "Rome");
        civ_score_breakdown_t b;
        civ_score_compute(&g, 1, &b);
        EXPECT(b.content_citizens == 5);
        EXPECT(b.turns_of_peace == 10);
        EXPECT(b.total == 35);
    }

    /* 4. 多城 + 不同 owner: 只算指定 player_slot */
    {
        struct civ_game g = {0};
        g.turn_number = 4;
        seed_city(&g, 1, 1, /*owner=*/1, /*pop=*/3, "Rome");
        seed_city(&g, 2, 2, /*owner=*/1, /*pop=*/2, "Pisa");
        seed_city(&g, 3, 3, /*owner=*/2, /*pop=*/9, "Berlin");
        /* slot 1: content = 3+2 = 5, peace = 4 → 5 + 12 = 17 */
        EXPECT(civ_score_total(&g, 1) == 17);
        /* slot 2: content = 9, peace = 4 → 9 + 12 = 21 */
        EXPECT(civ_score_total(&g, 2) == 21);
        /* slot 3: 沒城 → 0 + 12 = 12 (peace 仍計) */
        EXPECT(civ_score_total(&g, 3) == 12);
    }

    /* 5. dead city 不算 */
    {
        struct civ_game g = {0};
        g.turn_number = 0;
        seed_city(&g, 1, 1, 1, 5, "Ghost");
        g.world.cities[0].alive = false;
        EXPECT(civ_score_total(&g, 1) == 0);
    }

    /* 6. 大 pop sanity: 5 city × 8 pop @ slot 1, turn 20
     *    content = 40, peace = 20 → 40 + 60 = 100 */
    {
        struct civ_game g = {0};
        g.turn_number = 20;
        for (int i = 0; i < 5; i++) seed_city(&g, i, i, 1, 8, "C");
        civ_score_breakdown_t b;
        civ_score_compute(&g, 1, &b);
        EXPECT(b.content_citizens == 40);
        EXPECT(b.turns_of_peace == 20);
        EXPECT(b.total == 100);
    }

    /* 7. breakdown 寫進去都應 >= 0 */
    {
        struct civ_game g = {0};
        g.turn_number = 7;
        seed_city(&g, 0, 0, 1, 1, "Tiny");
        civ_score_breakdown_t b;
        civ_score_compute(&g, 1, &b);
        EXPECT(b.future_tech == 0);
        EXPECT(b.polluted_squares == 0);
        EXPECT(b.space_bonus == 0);
        EXPECT(b.conquest_bonus == 0);
        EXPECT(b.wonders_owned == 0);
        EXPECT(b.total > 0);
    }

    /* 8. R27-A: wonders bitmap 接通. 3 wonder in 2 cities (player slot 1)
     * → wonders_owned = 3, +20*3 = 60 加到 base */
    {
        struct civ_game g = {0};
        g.turn_number = 0;
        seed_city(&g, 0, 0, 1, 1, "Rome");
        seed_city(&g, 1, 1, 1, 1, "Pisa");
        g.world.cities[0].wonders_bitmap = (1u << 1) | (1u << 3); /* Pyramids + Colossus */
        g.world.cities[1].wonders_bitmap = (1u << 5);             /* Great Library */
        civ_score_breakdown_t b;
        civ_score_compute(&g, 1, &b);
        EXPECT(b.wonders_owned == 3);
        /* content = 1+1 = 2; peace = 0; wonders = 3; total = 2 + 60 = 62 */
        EXPECT(b.total == 62);
    }

    /* 9. R27-A: future_tech bitmap 接通. 2 future tech 設成 acquired
     * → future_tech = 2, +10*2 = 20 加到 base */
    {
        struct civ_game g = {0};
        g.turn_number = 0;
        /* set bits 68 (FUTURE_1) + 71 (FUTURE_4) */
        g.world.tech_acquired[1] = (1ULL << (68 - 64)) | (1ULL << (71 - 64));
        civ_score_breakdown_t b;
        civ_score_compute(&g, 1, &b);
        EXPECT(b.future_tech == 2);
        EXPECT(b.total == 20);
    }

    /* 10. R27-A: 完整公式 — 2 city + 3 wonder + 1 future + turn 5
     * = 2 content + 60 wonders + 10 future + 15 peace = 87 */
    {
        struct civ_game g = {0};
        g.turn_number = 5;
        seed_city(&g, 0, 0, 1, 1, "A");
        seed_city(&g, 1, 1, 1, 1, "B");
        g.world.cities[0].wonders_bitmap = (1u << 1) | (1u << 2) | (1u << 3);
        g.world.tech_acquired[1] = 1ULL << (68 - 64);
        EXPECT(civ_score_total(&g, 1) == 87);
    }

    /* 11. R27-C: civ_check_end YEAR_LIMIT — civ_year >= 2100 → YEAR_LIMIT */
    {
        struct civ_game g = {0};
        g.civ_year = 2099;
        EXPECT(civ_check_end(&g, 1) == CIV_END_NONE);
        g.civ_year = 2100;
        EXPECT(civ_check_end(&g, 1) == CIV_END_YEAR_LIMIT);
        g.civ_year = 2500;   /* 越過上限仍 YEAR_LIMIT */
        EXPECT(civ_check_end(&g, 1) == CIV_END_YEAR_LIMIT);
    }

    /* 12. R27-C: civ_check_end DESTROYED — 玩家 found 過城但全亡 */
    {
        struct civ_game g = {0};
        g.civ_year = -3000;
        /* 沒城 → NONE (新局尚未 found) */
        EXPECT(civ_check_end(&g, 1) == CIV_END_NONE);
        /* found 過 1 城, alive → NONE */
        seed_city(&g, 0, 0, 1, 1, "Rome");
        EXPECT(civ_check_end(&g, 1) == CIV_END_NONE);
        /* 城被消滅 → DESTROYED */
        g.world.cities[0].alive = false;
        EXPECT(civ_check_end(&g, 1) == CIV_END_DESTROYED);
        /* 但 player_slot 2 不是被消滅 (他根本沒城過) — 但因為 cities_count>0
         * 跟 §9.7 v0.1 啟發, 我們會誤判. 標 v0.1 limitation, v0.2 加 ever_founded
         * flag 解. 暫不 test player 2. */
    }

    /* 13. R27-C: civ_hof_rank — 公式 score×diff×comp/10 */
    {
        /* score 100, diff 3 (Prince), opponents 4
         * comp10 = 13 → rank = 100 * 3 * 13 / 10 = 390 */
        EXPECT(civ_hof_rank(100, 3, 4) == 390);
        /* score 0 → 0 */
        EXPECT(civ_hof_rank(0, 5, 6) == 0);
        /* 負 score → 0 */
        EXPECT(civ_hof_rank(-50, 3, 4) == 0);
        /* diff/opponents out-of-range → clip */
        EXPECT(civ_hof_rank(100, 99, 99) == civ_hof_rank(100, 5, 6));
        /* min difficulty + min opponents: 100 * 1 * 10 / 10 = 100 */
        EXPECT(civ_hof_rank(100, 1, 1) == 100);
        /* max: 100 * 5 * 15 / 10 = 750 */
        EXPECT(civ_hof_rank(100, 5, 6) == 750);
    }

    printf("PASS test_score (13 sub-tests, spec 09 §9.3-9.4 v0.2 + §9.7 v0.1)\n");
    return 0;
}
