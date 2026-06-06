/*
 * civ_loop.c — 主迴圈實作
 *
 * 對應 spec 02 §2.1.2 H 段 + SDL_IMPLEMENTATION_PLAN §3。
 * M1：用 civ_present_frame 把 palette framebuffer 上到視窗。
 */
#include "civ_loop.h"
#include "civ_game.h"

#include <SDL.h>

#if !SDL_VERSION_ATLEAST(2, 0, 18)
static Uint64 SDL_GetTicks64(void) { return (Uint64)SDL_GetTicks(); }
#endif

void civ_loop(struct civ_game *g)
{
    Uint64 last_present = SDL_GetTicks64();

    while (!g->quit) {
        SDL_Event ev;

        /* ── 對應原版 PEEKMESSAGE 非阻塞 poll ────────────────── */
        if (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) {
                g->quit = true;
                break;
            }
            if (ev.type == SDL_KEYDOWN &&
                ev.key.keysym.sym == SDLK_ESCAPE) {
                g->quit = true;
                break;
            }
            continue;   /* 把佇列裡的 event 全吃完再 idle */
        }

        /* ── idle path ─────────────────────────────────────── */
        g->tick_count++;

        /* ── ~60 Hz present cap ────────────────────────────── */
        Uint64 now = SDL_GetTicks64();
        if (now - last_present >= 16) {
            if (g->framebuffer && g->renderer) {
                civ_present_frame(&g->present, g->framebuffer, &g->palette);
                SDL_RenderPresent(g->renderer);
            }
            last_present = now;
        } else {
            SDL_Delay(1);
        }
    }
}
