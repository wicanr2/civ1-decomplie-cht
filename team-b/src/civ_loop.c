/*
 * civ_loop.c — 主迴圈實作
 *
 * 對應 spec 02 §2.1.2 H 段 + SDL_IMPLEMENTATION_PLAN §3。
 * M0 階段：用 SDL_PollEvent 非阻塞 poll、~60Hz 灰底 present、ESC 與視窗
 * 關閉退出。idle/render path 在 M1+ 接 framebuffer 後擴充。
 */
#include "civ_loop.h"
#include "civ_game.h"

#include <SDL.h>

/* SDL2 < 2.0.18 沒 GetTicks64；fallback 到 32-bit GetTicks */
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
        /* M1+ 在這裡推進 timer_counter、AI、地圖動畫 */

        /* ── ~60 Hz present cap ────────────────────────────── */
        Uint64 now = SDL_GetTicks64();
        if (now - last_present >= 16) {
            /* M0: 灰底；M1+ 換成 framebuffer present */
            if (g->renderer) {
                SDL_SetRenderDrawColor(g->renderer, 64, 64, 64, 255);
                SDL_RenderClear(g->renderer);
                SDL_RenderPresent(g->renderer);
            }
            last_present = now;
        } else {
            SDL_Delay(1);   /* 還電給 OS，不忙等 */
        }
    }
}
