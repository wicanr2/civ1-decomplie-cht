/*
 * test_palette_blit.c — M1 驗證：surface 寫入後讀回正確 index、
 * blit 不越界、palette generation 變動。
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

static void test_put_get(void)
{
    civ_surface_t *s = civ_surface_new(16, 8);
    EXPECT(s);
    civ_surface_put_pixel(s, 3, 4, 99);
    EXPECT(civ_surface_get_pixel(s, 3, 4) == 99);
    EXPECT(s->dirty == true);
    civ_surface_free(s);
}

static void test_clear(void)
{
    civ_surface_t *s = civ_surface_new(4, 4);
    civ_surface_clear(s, 7);
    for (int y = 0; y < 4; y++)
        for (int x = 0; x < 4; x++)
            EXPECT(civ_surface_get_pixel(s, x, y) == 7);
    civ_surface_free(s);
}

static void test_fill_rect(void)
{
    civ_surface_t *s = civ_surface_new(10, 10);
    civ_surface_clear(s, 0);
    civ_fill_rect(s, (civ_rect_t){2, 2, 4, 4}, 5);
    EXPECT(civ_surface_get_pixel(s, 1, 1) == 0);
    EXPECT(civ_surface_get_pixel(s, 2, 2) == 5);
    EXPECT(civ_surface_get_pixel(s, 5, 5) == 5);
    EXPECT(civ_surface_get_pixel(s, 6, 6) == 0);  /* 邊界外 */
    civ_surface_free(s);
}

static void test_frame_rect(void)
{
    civ_surface_t *s = civ_surface_new(8, 8);
    civ_surface_clear(s, 0);
    civ_frame_rect(s, (civ_rect_t){1, 1, 6, 6}, 9);
    EXPECT(civ_surface_get_pixel(s, 1, 1) == 9);   /* 左上角 */
    EXPECT(civ_surface_get_pixel(s, 6, 1) == 9);   /* 右上角 */
    EXPECT(civ_surface_get_pixel(s, 1, 6) == 9);   /* 左下角 */
    EXPECT(civ_surface_get_pixel(s, 6, 6) == 9);   /* 右下角 */
    EXPECT(civ_surface_get_pixel(s, 3, 3) == 0);   /* 中心未填 */
    civ_surface_free(s);
}

static void test_line_bresenham(void)
{
    civ_surface_t *s = civ_surface_new(8, 8);
    civ_surface_clear(s, 0);
    civ_line(s, 0, 0, 7, 7, 12);
    /* 對角線經過 (0,0) 與 (7,7) 與每個 (i,i) */
    for (int i = 0; i < 8; i++) EXPECT(civ_surface_get_pixel(s, i, i) == 12);
    civ_surface_free(s);
}

static void test_clip(void)
{
    civ_surface_t *s = civ_surface_new(10, 10);
    civ_surface_clear(s, 0);
    civ_surface_clip_set(s, (civ_rect_t){2, 2, 4, 4});
    civ_fill_rect(s, (civ_rect_t){0, 0, 10, 10}, 1);
    /* 應該只有 (2..5, 2..5) 被寫入 */
    EXPECT(civ_surface_get_pixel(s, 1, 1) == 0);
    EXPECT(civ_surface_get_pixel(s, 2, 2) == 1);
    EXPECT(civ_surface_get_pixel(s, 5, 5) == 1);
    EXPECT(civ_surface_get_pixel(s, 6, 6) == 0);
    civ_surface_free(s);
}

static void test_palette_default(void)
{
    civ_palette_t p = {0};
    civ_palette_default(&p);
    EXPECT(p.entries[0].r == 0  && p.entries[0].g == 0  && p.entries[0].b == 0);
    EXPECT(p.entries[15].r == 255 && p.entries[15].g == 255 && p.entries[15].b == 255);
    EXPECT(p.generation >= 1);
}

static void test_palette_animate(void)
{
    civ_palette_t p = {0};
    civ_palette_default(&p);
    int gen0 = p.generation;
    civ_color_t band[3] = {
        {255, 0, 0, 255}, {0, 255, 0, 255}, {0, 0, 255, 255}
    };
    civ_palette_animate(&p, 100, 3, band);
    EXPECT(p.generation > gen0);
    EXPECT(p.entries[100].r == 255 && p.entries[100].g == 0);
    EXPECT(p.entries[101].g == 255);
    EXPECT(p.entries[102].b == 255);
}

static void test_blit(void)
{
    civ_surface_t *src = civ_surface_new(4, 4);
    civ_surface_t *dst = civ_surface_new(8, 8);
    civ_surface_clear(src, 7);
    civ_surface_clear(dst, 0);
    civ_surface_blit(dst, 2, 2, src, NULL);
    EXPECT(civ_surface_get_pixel(dst, 1, 1) == 0);
    EXPECT(civ_surface_get_pixel(dst, 2, 2) == 7);
    EXPECT(civ_surface_get_pixel(dst, 5, 5) == 7);
    EXPECT(civ_surface_get_pixel(dst, 6, 6) == 0);
    civ_surface_free(src);
    civ_surface_free(dst);
}

int main(void)
{
    test_put_get();
    test_clear();
    test_fill_rect();
    test_frame_rect();
    test_line_bresenham();
    test_clip();
    test_palette_default();
    test_palette_animate();
    test_blit();
    printf("PASS test_palette_blit (9 sub-tests)\n");
    return 0;
}
