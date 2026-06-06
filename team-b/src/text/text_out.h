/*
 * text/text_out.h — 把字串繪到 palette framebuffer
 *
 * 對應 GDI TEXTOUT / DRAWTEXT。
 * 走 civ_big5_walk → civ_glyph_get → blit mask 到 surface。
 */
#ifndef CIV_TEXT_OUT_H
#define CIV_TEXT_OUT_H

#include "../gfx/surface.h"
#include "glyph_cache.h"

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

/* 對應 SETBKMODE */
typedef enum {
    CIV_TEXT_BK_TRANSPARENT = 0,
    CIV_TEXT_BK_OPAQUE      = 1,
} civ_text_bk_mode_t;

/* 在 (x, y) 為基線位置畫 Big5+ASCII 字串。
 * text_color / bk_color 是 palette index。
 * 回傳字串的總寬度（pixel）。 */
int civ_text_out(civ_surface_t *fb,
                 civ_font_t *font,
                 int x, int y,
                 const char *str,
                 uint8_t text_color, uint8_t bk_color,
                 civ_text_bk_mode_t bk_mode);

/* 度量一段字串的寬度（不繪）。對應 GETTEXTEXTENT。 */
int civ_text_measure(civ_font_t *font, const char *str);

#endif /* CIV_TEXT_OUT_H */
