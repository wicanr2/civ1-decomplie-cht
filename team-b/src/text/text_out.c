#include "text_out.h"
#include "utf8.h"

#include <string.h>

/* 把 1-bit mask blit 到 surface。
 * (px, py) 是 mask 左上角的 surface 座標。 */
static void glyph_blit(civ_surface_t *fb,
                       const civ_glyph_t *g,
                       int px, int py,
                       uint8_t text_color, uint8_t bk_color,
                       civ_text_bk_mode_t bk_mode)
{
    size_t pitch = (size_t)(g->w + 7) / 8;
    for (int row = 0; row < g->h; row++) {
        const uint8_t *bits = g->mask + (size_t)row * pitch;
        for (int col = 0; col < g->w; col++) {
            int bit = (bits[col / 8] >> (7 - (col % 8))) & 1;
            if (bit) {
                civ_surface_put_pixel(fb, px + col, py + row, text_color);
            } else if (bk_mode == CIV_TEXT_BK_OPAQUE) {
                civ_surface_put_pixel(fb, px + col, py + row, bk_color);
            }
        }
    }
}

int civ_text_out(civ_surface_t *fb,
                 civ_font_t *font,
                 int x, int y,
                 const char *str,
                 uint8_t text_color, uint8_t bk_color,
                 civ_text_bk_mode_t bk_mode)
{
    if (!str) return 0;
    size_t len = strlen(str);
    size_t pos = 0;
    int    cur_x = x;

    while (pos < len) {
        uint32_t cp = civ_utf8_walk((const uint8_t *)str, len, &pos);
        if (cp == 0) break;
        if (cp == ' ' || cp == '\t') {
            cur_x += font->space_advance;
            continue;
        }
        const civ_glyph_t *g = civ_glyph_get(font, cp);
        if (!g) {
            cur_x += font->space_advance;
            continue;
        }
        /* baseline 模型：(x,y) 是 baseline 左端；glyph bearing_y 從 baseline 向上 */
        int px = cur_x + g->bearing_x;
        int py = y - g->bearing_y;
        glyph_blit(fb, g, px, py, text_color, bk_color, bk_mode);
        cur_x += g->advance_x;
    }
    return cur_x - x;
}

int civ_text_measure(civ_font_t *font, const char *str)
{
    if (!str) return 0;
    size_t len = strlen(str);
    size_t pos = 0;
    int    w   = 0;
    while (pos < len) {
        uint32_t cp = civ_utf8_walk((const uint8_t *)str, len, &pos);
        if (cp == 0) break;
        if (cp == ' ' || cp == '\t') { w += font->space_advance; continue; }
        const civ_glyph_t *g = civ_glyph_get(font, cp);
        if (!g) { w += font->space_advance; continue; }
        w += g->advance_x;
    }
    return w;
}
