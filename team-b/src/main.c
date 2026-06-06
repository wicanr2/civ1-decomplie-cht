/*
 * main.c — 程式入口
 *
 * 取代原版 Borland c0w stub + WinMain。
 * 對應 spec 02 §2.1 與 SDL_IMPLEMENTATION_PLAN §3。
 * M0 階段：開 640×480 視窗、進主迴圈、結束時清理。
 */
#include "civ_game.h"
#include "civ_loop.h"

#include <SDL.h>
#include <stdio.h>

static int civ_app_init(struct civ_game *g)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return -1;
    }

    /* TODO M1+: 內部 framebuffer 320×200 / 640×480 indexed，
     * 視窗用 SDL_RenderSetLogicalSize + bicubic 放大到任意 size。 */
    g->window = SDL_CreateWindow(
        "Civilization for Windows (zh-TW)",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        640, 480,
        SDL_WINDOW_SHOWN);
    if (!g->window) {
        fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
        return -1;
    }

    g->renderer = SDL_CreateRenderer(
        g->window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!g->renderer) {
        /* 沒硬體加速就退軟體 renderer */
        g->renderer = SDL_CreateRenderer(g->window, -1, SDL_RENDERER_SOFTWARE);
    }
    if (!g->renderer) {
        fprintf(stderr, "SDL_CreateRenderer failed: %s\n", SDL_GetError());
        return -1;
    }

    return 0;
}

static void civ_app_shutdown(struct civ_game *g)
{
    if (g->renderer) SDL_DestroyRenderer(g->renderer);
    if (g->window)   SDL_DestroyWindow(g->window);
    SDL_Quit();
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

    civ_loop(&game);

    civ_app_shutdown(&game);
    return 0;
}
