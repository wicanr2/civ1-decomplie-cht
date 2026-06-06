/*
 * world/world.h — 世界地圖（terrain grid + view + cursor）
 *
 * Civ1 原版地圖 80×50 wrap-around；M5-lite 走較小尺寸 60×30 簡化。
 * terrain[] 每格存 civ_terrain_kind_t 語意 (0..CIV_TERRAIN_COUNT-1)，
 * 渲染時 widgets/map.c 透過 civ_terrain_sprite_coord() 查 SPR32X32
 * 內的 (col, row) 真實貼圖位置。
 */
#ifndef CIV_WORLD_WORLD_H
#define CIV_WORLD_WORLD_H

#include <stdbool.h>
#include <stdint.h>

#define CIV_MAP_W 60
#define CIV_MAP_H 30

typedef enum {
    CIV_TERRAIN_OCEAN    = 0,
    CIV_TERRAIN_GRASS    = 1,
    CIV_TERRAIN_PLAINS   = 2,
    CIV_TERRAIN_FOREST   = 3,
    CIV_TERRAIN_MOUNTAIN = 4,
    CIV_TERRAIN_HILLS    = 5,
    CIV_TERRAIN_DESERT   = 6,
    CIV_TERRAIN_JUNGLE   = 7,
    CIV_TERRAIN_SWAMP    = 8,
    CIV_TERRAIN_TUNDRA   = 9,
    CIV_TERRAIN_ARCTIC   = 10,
    CIV_TERRAIN_RIVER    = 11,
    CIV_TERRAIN_COUNT    = 12,
} civ_terrain_kind_t;

typedef struct civ_world {
    uint8_t terrain[CIV_MAP_H][CIV_MAP_W];  /* civ_terrain_kind_t 值 */
    int     view_x, view_y;       /* top-left tile shown in widget */
    int     cursor_x, cursor_y;   /* world coords of selected tile */
} civ_world_t;

/* 把 terrain enum 對應到 SPR32X32 內的 (col, row)。
 *
 * 對位來源: docs/screenshots/cvpc_spr32x32_decoded.png 視覺辨認。
 * SPR32X32 cols 14..29 是 terrain band，每 row 一種地形 (含 16 個變體
 * for blending — M5-lite 只取每帶中央代表 col=22)。cols 0..13 含 forest
 * / river 等獨立 sprite。座標表為近似；spec 06 ground-truth pass 再對齊。 */
void civ_terrain_sprite_coord(civ_terrain_kind_t kind, int *out_col, int *out_row);

/* 建簡易 demo 地圖：外圈海洋包圍 + 中央大陸 + 隨機 forest/mountain/hills/desert blob。 */
void civ_world_init_demo(civ_world_t *w);

/* 鍵盤移動 cursor，視 cursor 是否觸碰視窗邊緣自動 scroll。
 * cols/rows 是當前可顯示的 tile 數（widget 內）。 */
void civ_world_move_cursor(civ_world_t *w, int dx, int dy,
                           int cols, int rows);

#endif /* CIV_WORLD_WORLD_H */
