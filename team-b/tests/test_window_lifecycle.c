/*
 * test_window_lifecycle.c — M0 驗證點
 *
 * 對應 SDL_IMPLEMENTATION_PLAN §13 M0：headless 模式下 SDL_INIT_VIDEO
 * 用 dummy driver、civ_loop 收到 push 的 SDL_QUIT 後 1 tick 內結束。
 */
#include "civ_game.h"
#include "civ_loop.h"

#include <SDL.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EXPECT(cond, msg) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL %s: %s\n", msg, SDL_GetError()); \
        exit(1); \
    } \
} while (0)

int main(void)
{
    /* dummy driver 由 ctest 透過 ENVIRONMENT 注入，但為保險再設一次 */
    SDL_setenv("SDL_VIDEODRIVER", "dummy", 0);

    EXPECT(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) == 0, "SDL_Init");

    struct civ_game game = {0};
    game.window = SDL_CreateWindow(
        "test", 0, 0, 320, 200, SDL_WINDOW_HIDDEN);
    EXPECT(game.window != NULL, "SDL_CreateWindow");

    game.renderer = SDL_CreateRenderer(game.window, -1, SDL_RENDERER_SOFTWARE);
    EXPECT(game.renderer != NULL, "SDL_CreateRenderer");

    /* push SDL_QUIT 進佇列 — civ_loop 第一次 poll 就應該結束 */
    SDL_Event quit_ev;
    memset(&quit_ev, 0, sizeof quit_ev);
    quit_ev.type = SDL_QUIT;
    EXPECT(SDL_PushEvent(&quit_ev) >= 0, "SDL_PushEvent");

    Uint64 t0 = SDL_GetTicks();
    civ_loop(&game);
    Uint64 dt = SDL_GetTicks() - t0;

    /* 確認 quit 旗標確實被設 */
    if (!game.quit) {
        fprintf(stderr, "FAIL game.quit was not set after civ_loop\n");
        return 1;
    }

    /* 1 tick 內結束 = 應該遠少於 1 秒 */
    if (dt > 1000) {
        fprintf(stderr, "FAIL civ_loop took %llu ms, expected <1000\n",
                (unsigned long long)dt);
        return 1;
    }

    SDL_DestroyRenderer(game.renderer);
    SDL_DestroyWindow(game.window);
    SDL_Quit();

    printf("PASS test_window_lifecycle (loop exit in %llu ms)\n",
           (unsigned long long)dt);
    return 0;
}
