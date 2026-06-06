#include "palette.h"

#include <string.h>

/* 標準 VGA 16 色（IBM CGA / EGA / VGA text mode） */
static const civ_color_t vga16[16] = {
    {  0,   0,   0, 255}, /*  0 black        */
    {  0,   0, 170, 255}, /*  1 dark blue    */
    {  0, 170,   0, 255}, /*  2 dark green   */
    {  0, 170, 170, 255}, /*  3 dark cyan    */
    {170,   0,   0, 255}, /*  4 dark red     */
    {170,   0, 170, 255}, /*  5 dark magenta */
    {170,  85,   0, 255}, /*  6 brown        */
    {170, 170, 170, 255}, /*  7 light gray   */
    { 85,  85,  85, 255}, /*  8 dark gray    */
    { 85,  85, 255, 255}, /*  9 blue         */
    { 85, 255,  85, 255}, /* 10 green        */
    { 85, 255, 255, 255}, /* 11 cyan         */
    {255,  85,  85, 255}, /* 12 red          */
    {255,  85, 255, 255}, /* 13 magenta      */
    {255, 255,  85, 255}, /* 14 yellow       */
    {255, 255, 255, 255}, /* 15 white        */
};

void civ_palette_zero(civ_palette_t *p)
{
    memset(p->entries, 0, sizeof p->entries);
    for (int i = 0; i < 256; i++) p->entries[i].a = 255;
    p->generation++;
}

void civ_palette_default(civ_palette_t *p)
{
    civ_palette_zero(p);

    /* 0-15：VGA 16 */
    for (int i = 0; i < 16; i++) p->entries[i] = vga16[i];

    /* 16-31：深藍漸層 */
    for (int i = 0; i < 16; i++) {
        int v = 16 + i * 14;
        p->entries[16 + i] = (civ_color_t){0, 0, (uint8_t)v, 255};
    }

    /* 32-47：深紅漸層 */
    for (int i = 0; i < 16; i++) {
        int v = 16 + i * 14;
        p->entries[32 + i] = (civ_color_t){(uint8_t)v, 0, 0, 255};
    }

    /* 48-63：灰階漸層 */
    for (int i = 0; i < 16; i++) {
        int v = i * 16 + 8;
        p->entries[48 + i] = (civ_color_t){(uint8_t)v, (uint8_t)v, (uint8_t)v, 255};
    }

    /* 64-255：6×6×6 RGB cube = 216 + 多餘填色 */
    int idx = 64;
    for (int r = 0; r < 6; r++) {
        for (int g = 0; g < 6; g++) {
            for (int b = 0; b < 6; b++) {
                if (idx >= 256) break;
                p->entries[idx++] = (civ_color_t){
                    (uint8_t)(r * 51), (uint8_t)(g * 51), (uint8_t)(b * 51), 255
                };
            }
        }
    }
    /* 剩下 256-idx 個填灰色，避免 garbage */
    for (; idx < 256; idx++) {
        p->entries[idx] = (civ_color_t){128, 128, 128, 255};
    }
    p->generation++;
}

void civ_palette_set(civ_palette_t *p, int index, civ_color_t c)
{
    if (index < 0 || index > 255) return;
    p->entries[index] = c;
    p->generation++;
}

void civ_palette_animate(civ_palette_t *p, int start, int count, const civ_color_t *src)
{
    if (start < 0) { count += start; src -= start; start = 0; }
    if (start + count > 256) count = 256 - start;
    if (count <= 0) return;
    memcpy(p->entries + start, src, (size_t)count * sizeof *src);
    p->generation++;
}

void civ_palette_build_lut(const civ_color_t *src, int src_count,
                           const civ_palette_t *dst, uint8_t lut[256])
{
    if (src_count > 256) src_count = 256;
    if (src_count < 0)   src_count = 0;

    for (int i = 0; i < src_count; i++) {
        int sr = src[i].r, sg = src[i].g, sb = src[i].b;
        int best_idx = 0;
        int best_d   = 256 * 256 * 4;   /* > 任何實際 d² */
        for (int j = 0; j < 256; j++) {
            int dr = sr - dst->entries[j].r;
            int dg = sg - dst->entries[j].g;
            int db = sb - dst->entries[j].b;
            int d  = dr * dr + dg * dg + db * db;
            if (d < best_d) { best_d = d; best_idx = j; }
        }
        lut[i] = (uint8_t)best_idx;
    }
    for (int i = src_count; i < 256; i++) lut[i] = 0;
}
