/*
 * gfx/surface.h — 8bpp indexed framebuffer
 *
 * 對應 SDL_IMPLEMENTATION_PLAN §5 與 spec 01 §1.4.3 GDI 替代邊界。
 * row-major、tightly packed (pitch == w)。所有繪圖最終落腳在這。
 */
#ifndef CIV_GFX_SURFACE_H
#define CIV_GFX_SURFACE_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

typedef struct civ_rect {
    int x, y, w, h;
} civ_rect_t;

typedef struct civ_surface {
    int        w, h;
    int        pitch;        /* bytes per row（M1 階段 = w） */
    uint8_t   *pixels;       /* w*h bytes，row-major */
    civ_rect_t clip;         /* 繪圖被裁切到此矩形 */
    bool       dirty;        /* present.c 監視，true 才上傳到 SDL_Texture */
} civ_surface_t;

civ_surface_t *civ_surface_new(int w, int h);
void           civ_surface_free(civ_surface_t *s);

/* clip 設定。NULL 表示重置為整個 surface。 */
void civ_surface_clip_reset(civ_surface_t *s);
void civ_surface_clip_set(civ_surface_t *s, civ_rect_t r);

/* 直接像素存取。座標被 clip 過濾；越界回 0 或 no-op。 */
uint8_t civ_surface_get_pixel(const civ_surface_t *s, int x, int y);
void    civ_surface_put_pixel(civ_surface_t *s, int x, int y, uint8_t index);

/* 整面清為單一 index。 */
void civ_surface_clear(civ_surface_t *s, uint8_t index);

/* 兩個 surface 之間 8bpp index-to-index blit。
 * dst_x/dst_y 為目的座標；src 整片或子矩形（src_rect NULL = 全部）。
 * 不做 raster-op，純拷貝。 */
void civ_surface_blit(civ_surface_t *dst, int dst_x, int dst_y,
                      const civ_surface_t *src, const civ_rect_t *src_rect);

#endif /* CIV_GFX_SURFACE_H */
