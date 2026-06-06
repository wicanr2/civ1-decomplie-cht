/*
 * test_turn.c — M6-lite 驗證：turn counter + 西元年計算
 */
#include "civ_game.h"
#include "world/turn.h"

#include <stdio.h>
#include <stdlib.h>

#define EXPECT(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL %s:%d  %s\n", __FILE__, __LINE__, #cond); \
        exit(1); \
    } \
} while (0)

int main(void)
{
    struct civ_game g = {0};
    g.turn_number = 0;
    g.civ_year    = -4000;

    /* 第 1 回合 */
    civ_turn_advance(&g);
    EXPECT(g.turn_number == 1);
    EXPECT(g.civ_year == -3950);
    EXPECT(g.ai_actions == 8);

    /* 跑 10 回合 */
    for (int i = 0; i < 10; i++) civ_turn_advance(&g);
    EXPECT(g.turn_number == 11);
    EXPECT(g.civ_year == -3450);   /* -4000 + 11*50 */
    EXPECT(g.ai_actions == 88);    /* 11 * 8 */

    /* 跑到西元元年附近 — 4000/50 = 80 回合 (turn 80 -> 0 年) */
    for (int i = 11; i < 80; i++) civ_turn_advance(&g);
    EXPECT(g.turn_number == 80);
    EXPECT(g.civ_year == 0);

    civ_turn_advance(&g);
    EXPECT(g.civ_year == 50);      /* AD 50 */

    printf("PASS test_turn (turn -> 西元年 + AI counter 驗證)\n");
    return 0;
}
