#include "surface.h"

#include <stdlib.h>
#include <string.h>

static inline int civ_clamp(int v, int lo, int hi)
{
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

civ_surface_t *civ_surface_new(int w, int h)
{
    if (w <= 0 || h <= 0) return NULL;
    civ_surface_t *s = calloc(1, sizeof *s);
    if (!s) return NULL;
    s->w     = w;
    s->h     = h;
    s->pitch = w;
    s->pixels = calloc((size_t)w * (size_t)h, 1);
    if (!s->pixels) { free(s); return NULL; }
    civ_surface_clip_reset(s);
    s->dirty = true;
    return s;
}

void civ_surface_free(civ_surface_t *s)
{
    if (!s) return;
    free(s->pixels);
    free(s);
}

void civ_surface_clip_reset(civ_surface_t *s)
{
    s->clip.x = 0;
    s->clip.y = 0;
    s->clip.w = s->w;
    s->clip.h = s->h;
}

void civ_surface_clip_set(civ_surface_t *s, civ_rect_t r)
{
    /* 與 surface 整體交集 */
    int x1 = civ_clamp(r.x, 0, s->w);
    int y1 = civ_clamp(r.y, 0, s->h);
    int x2 = civ_clamp(r.x + r.w, 0, s->w);
    int y2 = civ_clamp(r.y + r.h, 0, s->h);
    s->clip.x = x1;
    s->clip.y = y1;
    s->clip.w = x2 - x1;
    s->clip.h = y2 - y1;
}

uint8_t civ_surface_get_pixel(const civ_surface_t *s, int x, int y)
{
    if (x < 0 || y < 0 || x >= s->w || y >= s->h) return 0;
    return s->pixels[(size_t)y * (size_t)s->pitch + (size_t)x];
}

void civ_surface_put_pixel(civ_surface_t *s, int x, int y, uint8_t index)
{
    if (x < s->clip.x || y < s->clip.y) return;
    if (x >= s->clip.x + s->clip.w) return;
    if (y >= s->clip.y + s->clip.h) return;
    s->pixels[(size_t)y * (size_t)s->pitch + (size_t)x] = index;
    s->dirty = true;
}

void civ_surface_clear(civ_surface_t *s, uint8_t index)
{
    memset(s->pixels, index, (size_t)s->w * (size_t)s->h);
    s->dirty = true;
}

void civ_surface_blit(civ_surface_t *dst, int dst_x, int dst_y,
                      const civ_surface_t *src, const civ_rect_t *src_rect)
{
    civ_rect_t r;
    if (src_rect) {
        r = *src_rect;
    } else {
        r.x = 0; r.y = 0; r.w = src->w; r.h = src->h;
    }
    /* 對 src clip 內檢查 */
    if (r.x < 0)         { r.w += r.x; dst_x -= r.x; r.x = 0; }
    if (r.y < 0)         { r.h += r.y; dst_y -= r.y; r.y = 0; }
    if (r.x + r.w > src->w) r.w = src->w - r.x;
    if (r.y + r.h > src->h) r.h = src->h - r.y;
    if (r.w <= 0 || r.h <= 0) return;

    /* 對 dst clip */
    int x1 = dst_x;
    int y1 = dst_y;
    int x2 = dst_x + r.w;
    int y2 = dst_y + r.h;
    if (x1 < dst->clip.x) { r.x += dst->clip.x - x1; r.w -= dst->clip.x - x1; x1 = dst->clip.x; }
    if (y1 < dst->clip.y) { r.y += dst->clip.y - y1; r.h -= dst->clip.y - y1; y1 = dst->clip.y; }
    if (x2 > dst->clip.x + dst->clip.w) r.w -= x2 - (dst->clip.x + dst->clip.w);
    if (y2 > dst->clip.y + dst->clip.h) r.h -= y2 - (dst->clip.y + dst->clip.h);
    if (r.w <= 0 || r.h <= 0) return;

    for (int row = 0; row < r.h; row++) {
        memcpy(dst->pixels + (size_t)(y1 + row) * dst->pitch + x1,
               src->pixels + (size_t)(r.y + row) * src->pitch + r.x,
               (size_t)r.w);
    }
    dst->dirty = true;
}
