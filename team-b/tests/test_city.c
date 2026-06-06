/*
 * test_city.c — R5 M6-full city 系統驗證
 *
 * 6 子測:
 *   1. building info table 全 25 條 lookup
 *   2. building cost 對齊 spec 06 §6.2
 *   3. building zh-TW name 對齊 civ_dict.c STR# 131
 *   4. civ_world_init_demo 生 4 城市
 *   5. civ_world_spawn_city 邊界 (negative coords / 超 cap)
 *   6. civ_world_city_at lookup
 */
#include "civ_game.h"
#include "world/city.h"
#include "world/world.h"
#include "world/turn.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EXPECT(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL %s:%d  %s\n", __FILE__, __LINE__, #cond); \
        exit(1); \
    } \
} while (0)

int main(void)
{
    /* ── 1. building info table 全範圍 lookup ───────────── */
    EXPECT(civ_building_cost(0) == 80);
    EXPECT(civ_building_cost(1) == 200);    /* Palace */
    EXPECT(civ_building_cost(2) == 40);     /* Barracks */
    EXPECT(civ_building_cost(15) == 200);   /* Factory */
    EXPECT(civ_building_cost(16) == 320);   /* Mfg Plant */
    EXPECT(civ_building_cost(24) == 320);   /* SS Module */
    EXPECT(civ_building_cost(25) == 0);     /* OOR */
    EXPECT(civ_building_cost(-1) == 0);

    /* ── 2. zh-TW name 對齊 spec 05 civ_dict.c STR# 131 ─── */
    EXPECT(strcmp(civ_building_name_zh(1), "宮殿") == 0);
    EXPECT(strcmp(civ_building_name_zh(2), "兵營") == 0);
    EXPECT(strcmp(civ_building_name_zh(8), "城牆") == 0);
    EXPECT(strcmp(civ_building_name_zh(15), "工廠") == 0);

    /* ── 3. init_demo 4 cities ─────────────────────────── */
    civ_world_t w;
    civ_world_init_demo(&w);
    EXPECT(w.cities_count == 4);

    EXPECT(strcmp(w.cities[0].name, "羅馬") == 0);
    EXPECT(w.cities[0].owner == 1);
    EXPECT(w.cities[0].population == 3);
    EXPECT((w.cities[0].buildings_bitmap & (1u << 1)) != 0);  /* Palace */

    EXPECT(strcmp(w.cities[1].name, "巴比倫") == 0);
    EXPECT(w.cities[1].owner == 2);

    /* ── 4. spawn 邊界 ────────────────────────────────── */
    int idx = civ_world_spawn_city(&w, "測試", 1, -1, 0, 1);
    EXPECT(idx == -1);  /* x < 0 拒絕 */
    idx = civ_world_spawn_city(&w, "測試", 1, 0, 99, 1);
    EXPECT(idx == -1);  /* y > MAP_H 拒絕 */

    /* ── 5. spawn 超 cap ─────────────────────────────── */
    civ_world_t w2;
    civ_world_init_demo(&w2);
    /* 先打滿到 CIV_MAX_CITIES */
    while (w2.cities_count < CIV_MAX_CITIES) {
        int x = (w2.cities_count % 50) + 5;
        int y = (w2.cities_count % 20) + 5;
        int r = civ_world_spawn_city(&w2, "X", 1, x, y, 1);
        if (r < 0) break;
    }
    EXPECT(w2.cities_count == CIV_MAX_CITIES);
    /* 再 spawn 應該失敗 */
    int over = civ_world_spawn_city(&w2, "Y", 1, 10, 10, 1);
    EXPECT(over == -1);

    /* ── 6. city_at lookup ─────────────────────────────── */
    EXPECT(civ_world_city_at(&w, 28, 14) == 0);   /* 羅馬 */
    EXPECT(civ_world_city_at(&w, 17, 11) == 1);   /* 巴比倫 */
    EXPECT(civ_world_city_at(&w, 99, 99) == -1);  /* 無 */
    EXPECT(civ_world_city_at(&w, 0, 0) == -1);    /* OCEAN, 無 city */

    /* ── 7. R7 M7: turn-tick city progression ─────────── */
    {
        struct civ_game g = {0};
        civ_world_init_demo(&g.world);
        g.world_ready = true;

        /* 羅馬 default: pop=3, Palace built */
        civ_city_t *roma = &g.world.cities[0];
        EXPECT(roma->population == 3);
        EXPECT(roma->building_target == -1);  /* init_demo 沒設, tick 會自動 pick */

        /* 跑 1 turn → 應自動 pick Granary (idx 3) */
        civ_turn_advance(&g);
        EXPECT(roma->building_target == 3);   /* Granary */
        /* shield 1 turn: 2 + (3-1) = 4 */
        EXPECT(roma->shield_stock == 4);

        /* Granary cost = 60. 4 shield/turn → 完工後 target=-1.
         * 跑 14 turn 後 shield = 15*4 = 60 → 完工. */
        for (int i = 0; i < 14; i++) civ_turn_advance(&g);
        EXPECT((roma->buildings_bitmap & (1u << 3)) != 0);  /* Granary built */
        EXPECT(roma->building_target == -1);  /* 剛完工尚未 pick 下個 */

        /* 再 1 turn → 應自動 pick Barracks (bit 2 未 set) */
        civ_turn_advance(&g);
        EXPECT(roma->building_target == 2);   /* Auto-pick next: Barracks */
    }

    printf("PASS test_city (7 子測, R5+R7)\n");
    return 0;
}
