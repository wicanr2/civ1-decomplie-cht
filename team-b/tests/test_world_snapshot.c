/*
 * test_world_snapshot.c — M5 視覺驗證：terrain rendering + cursor + scroll
 *
 * 載入 SPR32X32 sprite sheet (CIVDATA4 #200, 1472×400)，切成 32×32
 * tile grid，把 world.terrain[] 畫進主地圖 widget。
 */
#include "civ_game.h"
#include "civ_widgets.h"

#include "gfx/palette.h"
#include "gfx/primitive.h"
#include "gfx/surface.h"
#include "res/loader.h"
#include "res/rsrcfork.h"
#include "text/big5.h"
#include "text/glyph_cache.h"
#include "text/text_out.h"

#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define FB_W 640
#define FB_H 480
#define TITLE_H 40

#ifndef CIV_DEFAULT_FONT_PATH
#define CIV_DEFAULT_FONT_PATH "/usr/share/fonts/truetype/arphic/uming.ttc"
#endif

static int file_exists(const char *p) { struct stat s; return stat(p,&s)==0; }

static void paint_background(struct civ_game *g)
{
    civ_surface_t *fb = g->framebuffer;
    civ_surface_clear(fb, 15);
    civ_fill_rect(fb, (civ_rect_t){0, 0, FB_W, TITLE_H}, 9);
    civ_hline(fb, 0, TITLE_H, FB_W, 0);
    if (g->font_title) {
        const char *t = "文明帝國 視窗版 Civilization for Windows";
        int w = civ_text_measure(g->font_title, t);
        int x = (FB_W - w) / 2;
        civ_text_out(fb, g->font_title, x, 28, t, 15, 9,
                     CIV_TEXT_BK_TRANSPARENT);
    }
}

static int write_ppm(const char *path,
                     const civ_surface_t *fb,
                     const civ_palette_t *pal)
{
    FILE *f = fopen(path, "wb");
    if (!f) return -1;
    fprintf(f, "P6\n%d %d\n255\n", fb->w, fb->h);
    for (int y = 0; y < fb->h; y++) {
        const uint8_t *row = fb->pixels + (size_t)y * fb->pitch;
        for (int x = 0; x < fb->w; x++) {
            civ_color_t c = pal->entries[row[x]];
            uint8_t rgb[3] = { c.r, c.g, c.b };
            fwrite(rgb, 1, 3, f);
        }
    }
    fclose(f);
    return 0;
}

int main(int argc, char **argv)
{
    const char *data_dir = getenv("CIV1_DATA_DIR");
    if (!data_dir || !*data_dir) {
        printf("SKIP test_world_snapshot（CIV1_DATA_DIR 未設）\n");
        return 0;
    }

    SDL_setenv("SDL_VIDEODRIVER", "dummy", 0);
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return 1;

    struct civ_game g = {0};
    g.framebuffer = civ_surface_new(FB_W, FB_H);
    civ_palette_default(&g.palette);
    if (file_exists(CIV_DEFAULT_FONT_PATH)) {
        g.font_title = civ_font_open(CIV_DEFAULT_FONT_PATH, 24);
        g.font_body  = civ_font_open(CIV_DEFAULT_FONT_PATH, 14);
    }
    civ_widgets_register(&g);

    /* 載 SPR32X32 (CIVDATA4 #200) */
    char path[1024];
    snprintf(path, sizeof path, "%s/CIVDATA4.RSC", data_dir);
    civ_rsrc_t *r4 = civ_rsrc_open(path);
    if (!r4) {
        snprintf(path, sizeof path, "%s/Civdata4.RSC", data_dir);
        r4 = civ_rsrc_open(path);
    }
    if (!r4) { fprintf(stderr, "CIVDATA4.RSC 找不到\n"); return 1; }

    if (civ_sprite_sheet_load(r4, 200, 32, 32, &g.sprite_sheet) != 0) {
        fprintf(stderr, "sprite_sheet_load failed\n");
        return 1;
    }
    civ_rsrc_close(r4);

    printf("SPR32X32 loaded: %d×%d sheet, %d cols × %d rows of 32×32 tiles\n",
           g.sprite_sheet.sheet->w, g.sprite_sheet.sheet->h,
           g.sprite_sheet.cols, g.sprite_sheet.rows);

    /* M5 demo（debug 模式）：直接把整張 SPR32X32 sheet 切片 blit 到主地圖
     * widget 內，使用 sheet 自有 palette 當 framebuffer palette。這讓我們
     * 視覺確認 sheet 內哪些 (col, row) 是 terrain，做 spec 06 校準的 ground
     * truth。一般 M5 路線會走 LUT remap（widget 背景保持）但 demo 先看
     * 全貌。 */
    g.palette = g.sprite_sheet.pal;   /* 用 sheet 自有 palette */

    civ_world_init_demo(&g.world);
    g.world_ready = false;            /* skip widget render path，下面手動 blit */

    paint_background(&g);
    civ_widgets_render_all(&g);

    /* 在 map widget rect 內畫 sheet 一片區域 — 從左上開始一直放滿 */
    SDL_Rect dest = {0, TITLE_H, 480, FB_H - TITLE_H};
    civ_rect_t src = {0, 0, 480, FB_H - TITLE_H};
    if (src.w > g.sprite_sheet.sheet->w) src.w = g.sprite_sheet.sheet->w;
    if (src.h > g.sprite_sheet.sheet->h) src.h = g.sprite_sheet.sheet->h;
    civ_surface_blit(g.framebuffer, 0, TITLE_H,
                     g.sprite_sheet.sheet, &src);
    (void)dest;

    /* 32×32 grid 線（讓肉眼可數 col/row） */
    for (int x = 0; x <= 480; x += 32)
        civ_vline(g.framebuffer, x, TITLE_H, src.h, 0);
    for (int y = TITLE_H; y <= TITLE_H + src.h; y += 32)
        civ_hline(g.framebuffer, 0, y, src.w, 0);

    /* 標籤覆蓋資訊 */
    if (g.font_body) {
        char buf[128];
        snprintf(buf, sizeof buf,
                 "M5 World Demo  view=(%d,%d) cursor=(%d,%d)",
                 g.world.view_x, g.world.view_y,
                 g.world.cursor_x, g.world.cursor_y);
        civ_text_out(g.framebuffer, g.font_body, 8, FB_H - 6, buf,
                     14, 0, CIV_TEXT_BK_TRANSPARENT);
    }

    const char *out_path = argc > 1 ? argv[1] : "m5_world.ppm";
    write_ppm(out_path, g.framebuffer, &g.palette);
    printf("PASS test_world_snapshot → %s\n", out_path);

    civ_sprite_sheet_free(&g.sprite_sheet);
    civ_widgets_unregister(&g);
    if (g.font_title) civ_font_close(g.font_title);
    if (g.font_body)  civ_font_close(g.font_body);
    civ_big5_cleanup();
    civ_surface_free(g.framebuffer);
    SDL_Quit();
    return 0;
}
