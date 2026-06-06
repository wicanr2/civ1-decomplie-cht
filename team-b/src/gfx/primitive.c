#include "primitive.h"

#include <stdlib.h>

void civ_hline(civ_surface_t *s, int x, int y, int w, uint8_t index)
{
    if (w <= 0) return;
    for (int i = 0; i < w; i++) civ_surface_put_pixel(s, x + i, y, index);
}

void civ_vline(civ_surface_t *s, int x, int y, int h, uint8_t index)
{
    if (h <= 0) return;
    for (int i = 0; i < h; i++) civ_surface_put_pixel(s, x, y + i, index);
}

void civ_line(civ_surface_t *s, int x0, int y0, int x1, int y1, uint8_t index)
{
    /* Bresenham 經典 */
    int dx =  abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;

    for (;;) {
        civ_surface_put_pixel(s, x0, y0, index);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

void civ_fill_rect(civ_surface_t *s, civ_rect_t r, uint8_t index)
{
    for (int y = 0; y < r.h; y++) {
        civ_hline(s, r.x, r.y + y, r.w, index);
    }
}

void civ_frame_rect(civ_surface_t *s, civ_rect_t r, uint8_t index)
{
    if (r.w <= 0 || r.h <= 0) return;
    civ_hline(s, r.x,         r.y,             r.w, index);
    civ_hline(s, r.x,         r.y + r.h - 1,   r.w, index);
    civ_vline(s, r.x,         r.y,             r.h, index);
    civ_vline(s, r.x + r.w-1, r.y,             r.h, index);
}
