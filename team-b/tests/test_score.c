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

    printf("PASS test_score (7 sub-tests, spec 09 §9.3 v0.1)\n");
    return 0;
}
