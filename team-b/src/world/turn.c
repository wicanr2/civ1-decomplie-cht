#include "turn.h"
#include "../civ_game.h"
#include "city.h"

/*
 * R7 M7: per-turn city tick.
 *
 * 算法簡化版 (對齊 Civ1 大致行為, spec 06 §6.2 building cost):
 *   shield_per_turn = 2 + (pop - 1)         (草原 city 一般 2-5 shields)
 *   food_per_turn   = pop                   (粗估)
 *   food_consumed   = pop * 2               (Civ1 每人 2 食物)
 *
 *   if building_target == -1: auto-pick (Granary 3 → Barracks 2 → Library 6 → Marketplace 5)
 *   shield_stock += shield_per_turn
 *   if shield_stock >= cost: set bit, reset stock, building_target = -1
 *
 *   food_stock += (food_per_turn - food_consumed)
 *   if food_stock < 0: food_stock = 0      (饑荒/淨負, 簡化不扣 pop)
 *   if food_stock >= (pop + 1) * 10: population++ + reset stock
 *     (Civ1 真公式較複雜含 Granary 50% 留存, 簡化版省略)
 */

static int pick_next_building(uint32_t buildings_bitmap)
{
    /* Granary (3) → Barracks (2) → Library (6) → Marketplace (5) → Walls (8) → Temple (4) */
    static const int PRIORITY[] = { 3, 2, 6, 5, 8, 4, 10, 11, 15 };
    for (size_t i = 0; i < sizeof PRIORITY / sizeof PRIORITY[0]; i++) {
        int idx = PRIORITY[i];
        if (!(buildings_bitmap & (1u << idx))) return idx;
    }
    return -1;
}

static void city_tick(civ_city_t *c)
{
    if (!c || !c->alive) return;
    int pop = c->population > 0 ? c->population : 1;

    /* 自動選 building target */
    if (c->building_target < 0) {
        c->building_target = (int8_t)pick_next_building(c->buildings_bitmap);
        c->shield_stock = 0;
    }

    /* shield 累積 */
    int shield_per_turn = 2 + (pop - 1);
    c->shield_stock += (int16_t)shield_per_turn;
    if (c->building_target >= 0) {
        int cost = civ_building_cost(c->building_target);
        if (cost > 0 && c->shield_stock >= cost) {
            c->buildings_bitmap |= (1u << c->building_target);
            c->shield_stock = 0;
            c->building_target = -1;
        }
    }

    /* food 累積 (粗估淨值) */
    int food_per_turn = pop;
    int food_consumed = pop * 2;
    int delta = food_per_turn - food_consumed;
    if (delta < 0) {
        /* 簡化版負時不扣 stock (避免 underflow) */
        if (c->food_stock + delta < 0) c->food_stock = 0;
        else c->food_stock = (int16_t)(c->food_stock + delta);
    } else {
        c->food_stock += (int16_t)delta;
    }

    /* 人口成長 (Civ1 阈值: (pop+1)*10 food = grow) */
    int grow_at = (pop + 1) * 10;
    if (c->food_stock >= grow_at) {
        c->food_stock = 0;
        if (c->population < 255) c->population++;
    }
}

void civ_turn_advance(struct civ_game *g)
{
    if (!g) return;
    g->turn_number++;
    g->ai_actions += 8;          /* 8 AI 玩家各 1 個 mock action */
    /* 西元年計算：每回合約 50 年（早期），50 年後縮為 25/10/5/2/1
     * （原版機制）；M6-lite 簡化為固定 50 年 / turn。 */
    g->civ_year = -4000 + (int)g->turn_number * 50;

    /* R7: city tick */
    if (g->world_ready) {
        for (int i = 0; i < g->world.cities_count; i++) {
            city_tick(&g->world.cities[i]);
        }
        /* unit moves_left reset (粗) */
        for (int i = 0; i < g->world.units_count; i++) {
            civ_unit_t *u = &g->world.units[i];
            if (!u->alive) continue;
            int atk, def, moves;
            civ_unit_stats(u->type, &atk, &def, &moves);
            u->moves_left = (uint8_t)moves;
        }
    }
}
