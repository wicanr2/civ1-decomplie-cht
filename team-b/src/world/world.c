#include "world.h"

#include <string.h>

/*
 * world.c — terrain enum → SPR32X32 (col, row) lookup + PoC demo 地圖
 *
 * M5-真落地 (2026-06-06 第二輪)：
 *   terrain[][] 從原本「row*46+col 的 sprite linear index」改為存
 *   civ_terrain_kind_t enum 值，map.c 透過 civ_terrain_sprite_coord()
 *   查 SPR32X32 內代表 tile (col, row)。
 *
 * SPR32X32 layout (1472×400 = 46 cols × 12 rows, 視覺辨認):
 *   cols  0..13 = rivers / forests / units / icons (個別 sprite)
 *   cols 14..29 = 純 terrain band, 每 row 一種地形 16 變體
 *   cols 30..45 = 城市 / wonder / 領袖肖像 / 軍事單位
 *
 * 中央代表 col 取 22 (相當於 band 中段「正常 tile」非邊界)。
 * 確切座標表是 best-guess，spec 06 ground-truth pass 會修。
 */

static const struct {
    int col, row;
} TERRAIN_SPRITE[CIV_TERRAIN_COUNT] = {
    [CIV_TERRAIN_OCEAN]    = { 22, 5 },  /* 底部水帶 */
    [CIV_TERRAIN_GRASS]    = { 22, 2 },  /* 純綠草原 */
    [CIV_TERRAIN_PLAINS]   = { 22, 0 },  /* 黃綠平原 */
    [CIV_TERRAIN_FOREST]   = {  2, 2 },  /* 左半深綠樹叢 */
    [CIV_TERRAIN_MOUNTAIN] = { 22, 3 },  /* 灰岩山脈 */
    [CIV_TERRAIN_HILLS]    = { 22, 3 },  /* 暫共用 mountain band */
    [CIV_TERRAIN_DESERT]   = { 22, 1 },  /* 黃土沙漠 */
    [CIV_TERRAIN_JUNGLE]   = {  3, 2 },  /* 雜叢深綠 */
    [CIV_TERRAIN_SWAMP]    = {  4, 2 },  /* 潛色綠 */
    [CIV_TERRAIN_TUNDRA]   = { 22, 4 },  /* 灰白冷帶 */
    [CIV_TERRAIN_ARCTIC]   = { 22, 4 },  /* 同 tundra band */
    [CIV_TERRAIN_RIVER]    = {  6, 1 },  /* 藍色蜿蜒 */
};

void civ_terrain_sprite_coord(civ_terrain_kind_t kind, int *out_col, int *out_row)
{
    if (kind < 0 || kind >= CIV_TERRAIN_COUNT) kind = CIV_TERRAIN_OCEAN;
    if (out_col) *out_col = TERRAIN_SPRITE[kind].col;
    if (out_row) *out_row = TERRAIN_SPRITE[kind].row;
}

/* 60×30 PoC 地圖:
 *   外圈 2 格 OCEAN 包圍
 *   中央大陸 GRASS 為主
 *   散布若干 hardcoded blob (mountain / forest / hills / desert / river)
 *   不需要 Perlin/雜訊,讓視覺辨認得出原版各 terrain 即可
 */

static void fill_blob(civ_world_t *w, int cx, int cy, int rx, int ry,
                      civ_terrain_kind_t kind)
{
    for (int y = cy - ry; y <= cy + ry; y++) {
        for (int x = cx - rx; x <= cx + rx; x++) {
            if (x < 0 || x >= CIV_MAP_W || y < 0 || y >= CIV_MAP_H) continue;
            /* 橢圓判定 */
            int dx = x - cx, dy = y - cy;
            if (rx == 0 || ry == 0) {
                w->terrain[y][x] = (uint8_t)kind;
            } else if (dx*dx*ry*ry + dy*dy*rx*rx <= rx*rx*ry*ry) {
                w->terrain[y][x] = (uint8_t)kind;
            }
        }
    }
}

void civ_world_init_demo(civ_world_t *w)
{
    if (!w) return;
    memset(w, 0, sizeof *w);

    /* 1. 全 OCEAN base */
    for (int y = 0; y < CIV_MAP_H; y++)
        for (int x = 0; x < CIV_MAP_W; x++)
            w->terrain[y][x] = (uint8_t)CIV_TERRAIN_OCEAN;

    /* 2. 中央大陸 GRASS (排除外圍 ~3 格海岸) */
    for (int y = 3; y < CIV_MAP_H - 3; y++) {
        for (int x = 4; x < CIV_MAP_W - 4; x++) {
            /* 橢圓大陸 — 簡單裁形,讓海岸線不齊整 */
            int dx = x - CIV_MAP_W / 2;
            int dy = y - CIV_MAP_H / 2;
            int Rx = CIV_MAP_W / 2 - 4;
            int Ry = CIV_MAP_H / 2 - 3;
            if (dx*dx*Ry*Ry + dy*dy*Rx*Rx <= Rx*Rx*Ry*Ry)
                w->terrain[y][x] = (uint8_t)CIV_TERRAIN_GRASS;
        }
    }

    /* 3. 散布 terrain blob */
    fill_blob(w, 12,  8, 3, 2, CIV_TERRAIN_FOREST);
    fill_blob(w, 20, 12, 2, 2, CIV_TERRAIN_MOUNTAIN);
    fill_blob(w, 32, 10, 3, 2, CIV_TERRAIN_HILLS);
    fill_blob(w, 45, 14, 3, 2, CIV_TERRAIN_DESERT);
    fill_blob(w, 18, 20, 4, 2, CIV_TERRAIN_PLAINS);
    fill_blob(w, 38, 22, 3, 2, CIV_TERRAIN_JUNGLE);
    fill_blob(w, 28, 18, 2, 1, CIV_TERRAIN_SWAMP);

    /* 4. 北極帶 — 最上一橫排陸地改 ARCTIC */
    for (int x = 0; x < CIV_MAP_W; x++) {
        if (w->terrain[3][x] != (uint8_t)CIV_TERRAIN_OCEAN)
            w->terrain[3][x] = (uint8_t)CIV_TERRAIN_ARCTIC;
        if (CIV_MAP_H - 4 >= 0 &&
            w->terrain[CIV_MAP_H - 4][x] != (uint8_t)CIV_TERRAIN_OCEAN)
            w->terrain[CIV_MAP_H - 4][x] = (uint8_t)CIV_TERRAIN_TUNDRA;
    }

    /* 5. 一條從北至南河流 (column 25 大陸範圍內) */
    for (int y = 4; y < CIV_MAP_H - 4; y++) {
        int rx = 25 + ((y % 4 == 0) ? 1 : 0);
        if (rx >= 0 && rx < CIV_MAP_W &&
            w->terrain[y][rx] != (uint8_t)CIV_TERRAIN_OCEAN)
            w->terrain[y][rx] = (uint8_t)CIV_TERRAIN_RIVER;
    }

    /* 視窗預設置中於大陸 (而非從 (0,0) 看 OCEAN 邊角):
     *   map widget 15 cols × 13 rows visible @ 32×32 tile
     *   world 60×30 → center view_x ≈ 22, view_y ≈ 8 */
    w->view_x   = 22;
    w->view_y   = 8;
    w->cursor_x = CIV_MAP_W / 2;
    w->cursor_y = CIV_MAP_H / 2;
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
