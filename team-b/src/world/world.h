/*
 * world/world.h — 世界地圖（terrain grid + view + cursor）
 *
 * Civ1 原版地圖 80×50 wrap-around；M5-lite 走較小尺寸 60×30 簡化。
 * terrain[] 每格存 sprite sheet 內的 tile 索引（0..N-1）。
 */
#ifndef CIV_WORLD_WORLD_H
#define CIV_WORLD_WORLD_H

#include <stdbool.h>
#include <stdint.h>

#define CIV_MAP_W 60
#define CIV_MAP_H 30

typedef struct civ_world {
    uint8_t terrain[CIV_MAP_H][CIV_MAP_W];
    int     view_x, view_y;       /* top-left tile shown in widget */
    int     cursor_x, cursor_y;   /* world coords of selected tile */
} civ_world_t;

/* 建簡易 demo 地圖：海包圍 + 草原 + 散布森林 / 山脈 / 沙漠 / 河流。 */
void civ_world_init_demo(civ_world_t *w);

/* 鍵盤移動 cursor，視 cursor 是否觸碰視窗邊緣自動 scroll。
 * cols/rows 是當前可顯示的 tile 數（widget 內）。 */
void civ_world_move_cursor(civ_world_t *w, int dx, int dy,
                           int cols, int rows);

#endif /* CIV_WORLD_WORLD_H */
