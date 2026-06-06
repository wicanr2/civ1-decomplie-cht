/*
 * test_palette_remap.c — M5 驗證：RGB-nearest LUT + remap blit
 *
 * 對應 spec 03 §12 #3 palette stomp 雷的解法。
 */
#include "gfx/palette.h"
#include "gfx/primitive.h"
#include "gfx/surface.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EXPECT(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL %s:%d  %s\n", __FILE__, __LINE__, #cond); \
        exit(1); \
    } \
} while (0)

int main(void)
{
    /* base palette = VGA 16 + 漸層 + 6×6×6 cube */
    civ_palette_t base = {0};
    civ_palette_default(&base);

    /* 源 palette：8 個顏色，選與 VGA 16 接近的 RGB */
    civ_color_t src[8] = {
        {  0,   0,   0, 255}, /* 0: pure black -> base index 0 (black) */
        {255, 255, 255, 255}, /* 1: pure white -> base index 15 (white) */
        {200,   0,   0, 255}, /* 2: red       -> base index 12 (255,85,85) 或 32-47 漸層 */
        {  0, 200,   0, 255}, /* 3: green     -> base index 10 (85,255,85) */
        {  0,   0, 200, 255}, /* 4: blue      -> base index 9 (85,85,255) */
        {255, 255,   0, 255}, /* 5: yellow    -> base index 14 (255,255,85) */
        {128, 128, 128, 255}, /* 6: gray      -> base index 7 (170,170,170) 或灰階漸層 */
        {255,   0, 255, 255}, /* 7: magenta   -> base index 13 (255,85,255) */
    };

    uint8_t lut[256] = {0};
    civ_palette_build_lut(src, 8, &base, lut);

    printf("LUT mapping for 8 source colors:\n");
    for (int i = 0; i < 8; i++) {
        civ_color_t s = src[i];
        civ_color_t d = base.entries[lut[i]];
        printf("  src[%d]=(%3d,%3d,%3d) → base[%3d]=(%3d,%3d,%3d)\n",
               i, s.r, s.g, s.b, lut[i], d.r, d.g, d.b);
    }

    /* 黑色映黑色 */
    EXPECT(lut[0] == 0);
    EXPECT(base.entries[lut[0]].r == 0 && base.entries[lut[0]].g == 0
        && base.entries[lut[0]].b == 0);
    /* 白色映白色 (base index 15) */
    EXPECT(lut[1] == 15);

    /* 越界 src 的 LUT slot 應為 0 */
    for (int i = 8; i < 256; i++) EXPECT(lut[i] == 0);

    /* 建一個小 src surface 用上面 8 種顏色填，然後 remap-blit */
    civ_surface_t *src_surf = civ_surface_new(8, 1);
    EXPECT(src_surf);
    for (int i = 0; i < 8; i++) civ_surface_put_pixel(src_surf, i, 0, (uint8_t)i);

    civ_surface_t *dst = civ_surface_new(16, 4);
    EXPECT(dst);
    civ_surface_clear(dst, 5);   /* dst 預先填 dark magenta */

    civ_surface_blit_remap(dst, 2, 1, src_surf, NULL, lut);

    /* dst[2..9, 1] 應該被 8 個 remap 後的 index 填滿 */
    EXPECT(civ_surface_get_pixel(dst, 0, 1) == 5);     /* 未被覆蓋 */
    EXPECT(civ_surface_get_pixel(dst, 2, 1) == lut[0]);
    EXPECT(civ_surface_get_pixel(dst, 9, 1) == lut[7]);
    EXPECT(civ_surface_get_pixel(dst, 10, 1) == 5);    /* 未被覆蓋 */

    civ_surface_free(src_surf);
    civ_surface_free(dst);

    printf("PASS test_palette_remap\n");
    return 0;
}
