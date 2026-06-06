/*
 * test_demo_snapshot.c — M1 視覺驗證：跑 demo_paint 後 dump framebuffer
 * 成 PPM 給 docs/ 留證。
 *
 * 用 dummy SDL driver 跑 headless；產出 m1_demo.ppm（RGB binary，
 * 任何看圖工具都能開）。執行不要求字型存在；若字型缺，仍會出灰底
 * + 矩形圖。
 */
#include "civ_game.h"
#include "civ_loop.h"
#include "gfx/palette.h"
#include "gfx/present.h"
#include "gfx/primitive.h"
#include "gfx/surface.h"
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

#ifndef CIV_DEFAULT_FONT_PATH
#define CIV_DEFAULT_FONT_PATH "/usr/share/fonts/truetype/arphic/uming.ttc"
#endif

static int file_exists(const char *p) { struct stat s; return stat(p,&s)==0; }

/* 與 main.c 的 demo_paint 同步 — 改一處要兩邊改。 */
static void demo_paint(civ_surface_t *fb,
                       civ_font_t *font_title,
                       civ_font_t *font_body)
{
    civ_surface_clear(fb, 15);
    civ_fill_rect(fb, (civ_rect_t){0, 0, FB_W, 56}, 9);
    civ_hline(fb, 0, 56, FB_W, 0);
    civ_frame_rect(fb, (civ_rect_t){32, 80, FB_W - 64, FB_H - 120}, 8);
    civ_line(fb, 32, 80, FB_W - 32, FB_H - 40, 12);
    civ_line(fb, FB_W - 32, 80, 32, FB_H - 40, 10);

    if (font_title) {
        const char *t = "文明帝國 視窗版 Civilization for Windows";
        int w = civ_text_measure(font_title, t);
        int x = (FB_W - w) / 2;
        civ_text_out(fb, font_title, x, 38, t,
                     15, 9, CIV_TEXT_BK_TRANSPARENT);
    }
    if (font_body) {
        const char *lines[] = {
            "M0 SDL 視窗 + 主迴圈：完成",
            "M1 palette framebuffer + CJK 字模：本次",
            "M2 三個 widget + dispatch table",
            "M3 載入 .RSC + CvPc decode + blit",
            "M4 載入 14 文明 + 新局精靈",
            "M5 地圖視窗 + 地形繪製 + 滾動",
            "M6 turn loop + AI + 存讀檔",
            "M7 奇蹟 + 外交 + 勝利條件",
            "",
            "ESC 或關閉視窗離開",
        };
        int y = 120;
        for (size_t i = 0; i < sizeof lines/sizeof lines[0]; i++) {
            civ_text_out(fb, font_body, 60, y, lines[i],
                         0, 15, CIV_TEXT_BK_TRANSPARENT);
            y += 24;
        }
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
    SDL_setenv("SDL_VIDEODRIVER", "dummy", 0);
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
        return 1;
    }

    civ_surface_t *fb = civ_surface_new(FB_W, FB_H);
    if (!fb) { fprintf(stderr, "surface_new failed\n"); return 1; }
    civ_palette_t pal = {0};
    civ_palette_default(&pal);

    civ_font_t *ft = NULL, *fb_font = NULL;
    if (file_exists(CIV_DEFAULT_FONT_PATH)) {
        ft       = civ_font_open(CIV_DEFAULT_FONT_PATH, 24);
        fb_font  = civ_font_open(CIV_DEFAULT_FONT_PATH, 16);
    } else {
        printf("note: 字型 %s 不存在，demo 不畫文字\n", CIV_DEFAULT_FONT_PATH);
    }

    demo_paint(fb, ft, fb_font);

    const char *out_path = (argc > 1) ? argv[1] : "m1_demo.ppm";
    if (write_ppm(out_path, fb, &pal) < 0) {
        fprintf(stderr, "write %s failed\n", out_path);
        return 1;
    }
    printf("PASS test_demo_snapshot → %s (%d×%d)\n", out_path, FB_W, FB_H);

    if (ft)      civ_font_close(ft);
    if (fb_font) civ_font_close(fb_font);
    civ_big5_cleanup();
    civ_surface_free(fb);
    SDL_Quit();
    return 0;
}
