/*
 * main.c — 程式入口
 *
 * M1 demo：開 640×480 視窗、初始 palette framebuffer、載入 CJK 字型、
 * 畫一個白底場景含矩形 / 線段 / 「文明帝國 Civilization」標題與內文。
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

#define FB_W 640
#define FB_H 480

#ifndef CIV_DEFAULT_FONT_PATH
#define CIV_DEFAULT_FONT_PATH "/usr/share/fonts/truetype/arphic/uming.ttc"
#endif

static const char *resolve_font_path(void)
{
    const char *env = getenv("CIV_FONT");
    if (env && *env) return env;
    return CIV_DEFAULT_FONT_PATH;
}

static int civ_app_init(struct civ_game *g)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return -1;
    }

    g->window = SDL_CreateWindow(
        "Civilization for Windows (zh-TW)",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        FB_W, FB_H,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (!g->window) {
        fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
        return -1;
    }
    g->renderer = SDL_CreateRenderer(g->window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!g->renderer) {
        g->renderer = SDL_CreateRenderer(g->window, -1, SDL_RENDERER_SOFTWARE);
    }
    if (!g->renderer) {
        fprintf(stderr, "SDL_CreateRenderer failed: %s\n", SDL_GetError());
        return -1;
    }

    /* ── palette framebuffer ─────────────────────────────── */
    g->framebuffer = civ_surface_new(FB_W, FB_H);
    if (!g->framebuffer) return -1;
    civ_palette_default(&g->palette);
    if (civ_present_init(&g->present, g->renderer, FB_W, FB_H) < 0) return -1;

    /* ── CJK 字型 ───────────────────────────────────────── */
    const char *fpath = resolve_font_path();
    g->font_title = civ_font_open(fpath, 24);
    g->font_body  = civ_font_open(fpath, 16);
    if (!g->font_title || !g->font_body) {
        fprintf(stderr, "字型載入失敗：%s\n", fpath);
        /* 不致命；只是 text 畫不出來 */
    }
    return 0;
}

static void civ_app_shutdown(struct civ_game *g)
{
    if (g->font_title) civ_font_close(g->font_title);
    if (g->font_body)  civ_font_close(g->font_body);
    civ_big5_cleanup();
    if (g->framebuffer) {
        civ_present_destroy(&g->present);
        civ_surface_free(g->framebuffer);
    }
    if (g->renderer) SDL_DestroyRenderer(g->renderer);
    if (g->window)   SDL_DestroyWindow(g->window);
    SDL_Quit();
}

static void demo_paint(struct civ_game *g)
{
    civ_surface_t *fb = g->framebuffer;

    /* 白底 */
    civ_surface_clear(fb, 15);

    /* 上方藍底標題列 */
    civ_fill_rect(fb, (civ_rect_t){0, 0, FB_W, 56}, 9);
    civ_hline(fb, 0, 56, FB_W, 0);     /* 黑色 1px 分隔線 */

    /* 中央外框 */
    civ_frame_rect(fb, (civ_rect_t){32, 80, FB_W - 64, FB_H - 120}, 8);

    /* 對角線示範 */
    civ_line(fb, 32, 80, FB_W - 32, FB_H - 40, 12);   /* 紅 */
    civ_line(fb, FB_W - 32, 80, 32, FB_H - 40, 10);    /* 綠 */

    /* 標題 — 24 px 中文 */
    if (g->font_title) {
        int w = civ_text_measure(g->font_title, "文明帝國 視窗版 Civilization for Windows");
        int x = (FB_W - w) / 2;
        int y = 38;
        civ_text_out(fb, g->font_title, x, y,
                     "文明帝國 視窗版 Civilization for Windows",
                     15, 9, CIV_TEXT_BK_TRANSPARENT);
    }

    /* 內文 — 16 px 中英混排 */
    if (g->font_body) {
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
        for (size_t i = 0; i < sizeof lines / sizeof lines[0]; i++) {
            civ_text_out(fb, g->font_body, 60, y, lines[i],
                         0, 15, CIV_TEXT_BK_TRANSPARENT);
            y += 24;
        }
    }
}

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    struct civ_game game = {0};

    if (civ_app_init(&game) < 0) {
        civ_app_shutdown(&game);
        return 1;
    }

    demo_paint(&game);

    civ_loop(&game);

    civ_app_shutdown(&game);
    return 0;
}
