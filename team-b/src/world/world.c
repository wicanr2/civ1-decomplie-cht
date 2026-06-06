#include "world.h"

#include <string.h>

/*
 * M5 注：tile_idx 編碼為 row*46+col。SPR32X32 是 46 cols × 12 rows。
 *
 * 從 SPR32X32 視覺校驗（docs/screenshots/cvpc_spr32x32_decoded.png）：
 *   cols 0..13   = 紅色 background + 工具列 sprite
 *   cols 14..29  = TERRAIN 區（草原 / 森林 / 水 / 沙漠 / 山等）
 *   cols 30..45  = 領袖肖像 / 城市 / 圖示
 *   rows 9..11   = 步兵 / 海軍單位
 *
 * uint8_t terrain[][] 最多表 idx 0..255 = row 0 全部 + row 1 col 0..209。
 * 本 demo 用 row 1 cols 14..29 (idx 60..75), row 2 cols 14..29
 * (idx 106..121), row 3 cols 14..29 (idx 152..167), row 4 cols 14..29
 * (idx 198..213), 全部都在 uint8_t 範圍。
 */

void civ_world_init_demo(civ_world_t *w)
{
    if (!w) return;
    memset(w, 0, sizeof *w);

    /* 預設 row 2 col 14 */
    for (int y = 0; y < CIV_MAP_H; y++)
        for (int x = 0; x < CIV_MAP_W; x++)
            w->terrain[y][x] = (uint8_t)(2 * 46 + 14);    /* = 106 */

    struct band { int y0, y1, row, col_lo, col_hi; };
    struct band bands[] = {
        { 1,  6,  1, 14, 29},   /* row 1: terrain edges */
        { 7, 12,  2, 14, 29},   /* row 2: grass-ish */
        {13, 18,  3, 14, 29},   /* row 3: more terrain */
        {19, 24,  4, 14, 29},   /* row 4: hills/mountain */
        {25, 29,  5, 14, 19},   /* row 5: 部分 (col 19 = idx 249, col 20+ 溢出) */
    };
    for (size_t b = 0; b < sizeof bands / sizeof bands[0]; b++) {
        for (int y = bands[b].y0; y < bands[b].y1; y++) {
            for (int x = 1; x < CIV_MAP_W - 1; x++) {
                int col = bands[b].col_lo +
                          (x % (bands[b].col_hi - bands[b].col_lo + 1));
                int idx = bands[b].row * 46 + col;
                if (idx > 255) idx = 255;
                w->terrain[y][x] = (uint8_t)idx;
            }
        }
    }

    w->view_x   = 0;
    w->view_y   = 0;
    w->cursor_x = 7;
    w->cursor_y = 6;
}

void civ_world_move_cursor(civ_world_t *w, int dx, int dy,
                           int cols, int rows)
{
    if (!w) return;
    int nx = w->cursor_x + dx;
    int ny = w->cursor_y + dy;
    if (nx < 0)            nx = 0;
    if (ny < 0)            ny = 0;
    if (nx >= CIV_MAP_W)   nx = CIV_MAP_W - 1;
    if (ny >= CIV_MAP_H)   ny = CIV_MAP_H - 1;
    w->cursor_x = nx;
    w->cursor_y = ny;

    int margin = 2;
    if (nx - w->view_x < margin && w->view_x > 0) w->view_x--;
    if (nx - w->view_x >= cols - margin && w->view_x + cols < CIV_MAP_W) w->view_x++;
    if (ny - w->view_y < margin && w->view_y > 0) w->view_y--;
    if (ny - w->view_y >= rows - margin && w->view_y + rows < CIV_MAP_H) w->view_y++;
}
