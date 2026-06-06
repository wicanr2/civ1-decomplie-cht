/*
 * main.c — 程式入口
 *
 * M2 demo：開 640×480 視窗、初始 palette + 字型 + 3 個 widget，
 * 進主迴圈。畫面分區由 widget 自繪。
 */
#include "civ_game.h"
#include "civ_loop.h"
#include "civ_widgets.h"

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
#define TITLE_H 40

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

    g->framebuffer = civ_surface_new(FB_W, FB_H);
    if (!g->framebuffer) return -1;
    civ_palette_default(&g->palette);
    if (civ_present_init(&g->present, g->renderer, FB_W, FB_H) < 0) return -1;

    const char *fpath = resolve_font_path();
    g->font_title = civ_font_open(fpath, 24);
    g->font_body  = civ_font_open(fpath, 16);
    if (!g->font_title || !g->font_body) {
        fprintf(stderr, "字型載入失敗：%s\n", fpath);
    }

    if (civ_widgets_register(g) < 0) {
        fprintf(stderr, "widget 註冊失敗\n");
        return -1;
    }
    return 0;
}

static void civ_app_shutdown(struct civ_game *g)
{
    civ_widgets_unregister(g);
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

/* M2：每 frame 都重畫整個 background + title，然後讓 widget 自繪
 * 各自區域。M3+ 改 dirty-rect。 */
void civ_render(struct civ_game *g)
{
    civ_surface_t *fb = g->framebuffer;
    civ_surface_clear(fb, 15);   /* 白底 */
    civ_fill_rect(fb, (civ_rect_t){0, 0, FB_W, TITLE_H}, 9);  /* 藍標題列 */
    civ_hline(fb, 0, TITLE_H, FB_W, 0);

    if (g->font_title) {
        const char *t = "文明帝國 視窗版 Civilization for Windows";
        int w = civ_text_measure(g->font_title, t);
        int x = (FB_W - w) / 2;
        civ_text_out(fb, g->font_title, x, 28, t, 15, 9,
                     CIV_TEXT_BK_TRANSPARENT);
    }

    /* civ_loop 已在每 frame present 之前呼叫 civ_widgets_render_all，
     * 這裡只負責 title bar / background。 */
}

int main(int argc, char **argv)
{
    (void)argc; (void)argv;

    struct civ_game game = {0};

    if (civ_app_init(&game) < 0) {
        civ_app_shutdown(&game);
        return 1;
    }

    civ_render(&game);    /* 初畫 title + background */

    civ_loop(&game);

    civ_app_shutdown(&game);
    return 0;
}
