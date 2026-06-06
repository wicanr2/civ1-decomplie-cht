/*
 * test_unit_combat.c — M6-full-lite unit + combat 系統驗證
 *
 * 驗:
 *   1. civ_unit_stats 對 5 種 unit 回正確值
 *   2. civ_world_init_demo 生 11 個 starting unit (3 player 1 + 5 others + 2 barb)
 *   3. civ_world_unit_at 找對位置
 *   4. civ_world_move_selected 合法移動
 *   5. OCEAN block 移動
 *   6. 同 owner block 移動
 *   7. 敵 unit → combat, 有 winner
 *   8. cycle_selection 在 player 1 內循環
 *
 * 純單元測試, 不需要 CIV1_DATA_DIR.
 */
#include "world/world.h"
#include "world/unit.h"

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
    /* ── 1. stats 表 ─────────────────────────────── */
    int atk, def, moves;
    civ_unit_stats(CIV_UNIT_SETTLERS, &atk, &def, &moves);
    EXPECT(atk == 0 && def == 1 && moves == 1);
    civ_unit_stats(CIV_UNIT_PHALANX, &atk, &def, &moves);
    EXPECT(atk == 1 && def == 2 && moves == 1);
    civ_unit_stats(CIV_UNIT_LEGION, &atk, &def, &moves);
    EXPECT(atk == 4 && def == 2 && moves == 1);

    /* zh-TW 名稱 */
    EXPECT(strcmp(civ_unit_name_zh(CIV_UNIT_SETTLERS), "墾荒者") == 0);
    EXPECT(strcmp(civ_unit_name_zh(CIV_UNIT_LEGION),   "軍團兵") == 0);

    /* ── 2. init_demo 生 unit ─────────────────────────── */
    civ_world_t w;
    civ_world_init_demo(&w);

    /* M6-full-lite: 預期 11 個 starting unit (3+2+2+1+1+2) */
    EXPECT(w.units_count == 11);

    /* 預設 selected_unit = 0 = player 1 第一個 settler */
    EXPECT(w.selected_unit == 0);
    EXPECT(w.units[0].type == CIV_UNIT_SETTLERS);
    EXPECT(w.units[0].owner == 1);
    EXPECT(w.units[0].alive);

    /* ── 3. unit_at lookup ───────────────────────── */
    int idx = civ_world_unit_at(&w, 30, 15);
    EXPECT(idx == 0);
    idx = civ_world_unit_at(&w, 21, 12);     /* barbarian militia */
    EXPECT(idx >= 0);
    EXPECT(w.units[idx].owner == 0);
    idx = civ_world_unit_at(&w, 0, 0);       /* OCEAN, 無 unit */
    EXPECT(idx == -1);

    /* ── 4. 合法移動 ─────────────────────────────── */
    int orig_x = w.units[0].x;
    int orig_y = w.units[0].y;
    int orig_moves = w.units[0].moves_left;
    /* (30, 15) → (29, 15) 應該是空 GRASS, 合法移動。
     * 不能往 (31, 15) (blocked by settler2) 也不能往 (30, 16) (blocked by phalanx). */
    bool moved = civ_world_move_selected(&w, -1, 0, 15, 14);
    EXPECT(moved);
    EXPECT(w.units[0].x == orig_x - 1);
    EXPECT(w.units[0].y == orig_y);
    EXPECT(w.units[0].moves_left == orig_moves - 1);

    /* moves_left 用完, 再移動就拒絕 */
    EXPECT(!civ_world_move_selected(&w, -1, 0, 15, 14));

    /* ── 5. 重 init, 測 OCEAN block ──────────────── */
    civ_world_init_demo(&w);
    /* settler 在 (30, 15), 強制移到 OCEAN 外圈 (0..3) 路徑遠, 用簡化判定:
     * 直接 spawn 一個邊角的 settler 試 */
    int test_idx = civ_world_spawn_unit(&w, CIV_UNIT_SETTLERS, 1, 4, 14);
    EXPECT(test_idx >= 0);
    w.selected_unit = test_idx;
    w.units[test_idx].moves_left = 1;
    /* 往左是 OCEAN (x=3 仍 OCEAN ring), 應拒絕 */
    EXPECT(w.terrain[14][3] == CIV_TERRAIN_OCEAN);
    bool blocked = !civ_world_move_selected(&w, -1, 0, 15, 14);
    EXPECT(blocked);
    EXPECT(w.units[test_idx].x == 4);  /* 沒移動 */

    /* ── 6. 同 owner block ─────────────────────── */
    civ_world_init_demo(&w);
    w.selected_unit = 0;             /* (30, 15) settler1 */
    w.units[0].moves_left = 1;
    /* (31, 15) 是 settler2, 同 owner=1, 應拒絕 */
    bool blocked2 = !civ_world_move_selected(&w, 1, 0, 15, 14);
    EXPECT(blocked2);

    /* ── 7. 敵 combat ──────────────────────────── */
    civ_world_init_demo(&w);
    /* 找 player 1 unit 跟 barbarian (21,12) 鄰近. spawn 一個 player1 在 (22,12) */
    int p1 = civ_world_spawn_unit(&w, CIV_UNIT_LEGION, 1, 22, 12);
    EXPECT(p1 >= 0);
    w.selected_unit = p1;
    w.units[p1].moves_left = 1;
    int barb_idx = civ_world_unit_at(&w, 21, 12);
    EXPECT(barb_idx >= 0);

    /* fixed seed 讓 combat outcome deterministic */
    srand(42);
    bool combat = civ_world_move_selected(&w, -1, 0, 15, 14);
    EXPECT(combat);
    /* 至少有一方陣亡 */
    EXPECT(!w.units[p1].alive || !w.units[barb_idx].alive);
    /* last_combat_msg 應寫了內容 */
    EXPECT(w.last_combat_msg[0] != '\0');

    /* ── 8. cycle_selection ─────────────────────── */
    civ_world_init_demo(&w);
    w.selected_unit = -1;
    civ_world_cycle_selection(&w, 1);   /* player 1 */
    EXPECT(w.selected_unit >= 0);
    EXPECT(w.units[w.selected_unit].owner == 1);
    /* 再 cycle 一次, 應跳到下個 player 1 unit (sequence: 0/1/2 都是 player 1) */
    int first = w.selected_unit;
    civ_world_cycle_selection(&w, 1);
    EXPECT(w.selected_unit != first);
    EXPECT(w.units[w.selected_unit].owner == 1);

    printf("PASS test_unit_combat (8 子測, M6-full-lite)\n");
    return 0;
}
