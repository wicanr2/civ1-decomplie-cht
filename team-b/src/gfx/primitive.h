/*
 * gfx/primitive.h — 繪圖原語
 *
 * 對應 GDI LINETO/MOVETO/FILLRECT/FRAMERECT/SETPIXEL/GETPIXEL。
 * 均直接寫入 civ_surface_t（8bpp indexed）。
 */
#ifndef CIV_GFX_PRIMITIVE_H
#define CIV_GFX_PRIMITIVE_H

#include "surface.h"

/* Bresenham 線：對應 LINETO 從 (x0,y0) 畫到 (x1,y1) */
void civ_line(civ_surface_t *s, int x0, int y0, int x1, int y1, uint8_t index);

/* 水平 / 垂直加速版 */
void civ_hline(civ_surface_t *s, int x, int y, int w, uint8_t index);
void civ_vline(civ_surface_t *s, int x, int y, int h, uint8_t index);

/* 填矩形（含 r 邊界內部）— 對應 FILLRECT */
void civ_fill_rect(civ_surface_t *s, civ_rect_t r, uint8_t index);

/* 1-pixel 框線 — 對應 FRAMERECT */
void civ_frame_rect(civ_surface_t *s, civ_rect_t r, uint8_t index);

#endif /* CIV_GFX_PRIMITIVE_H */
