/*
 * civ_game.h — 主遊戲狀態結構
 *
 * 對應 spec 02 §2.1.2 與 SDL_IMPLEMENTATION_PLAN §10。
 * 原版 1228:* / 12b0:* / 12d8:* / 1420:* 的上百個 global 統一收歸於此。
 * 每個 milestone 持續擴充。
 */
#ifndef CIV_GAME_H
#define CIV_GAME_H

#include "gfx/palette.h"
#include "gfx/present.h"
#include "gfx/surface.h"
#include "text/glyph_cache.h"

#include <SDL.h>
#include <stdbool.h>
#include <stdint.h>

struct civ_game {
    /* 主旗標 ------------------------------------------------------------ */
    bool       quit;             /* 原 DAT_12d8_24ee：主迴圈 quit flag */
    uint32_t   timer_counter;    /* 原 DAT_12d8_24f0：TIMERPROC sleep token */
    uint64_t   tick_count;       /* idle step 計次 */

    /* SDL --------------------------------------------------------------- */
    SDL_Window   *window;
    SDL_Renderer *renderer;

    /* M1：palette framebuffer + present ------------------------------- */
    civ_surface_t *framebuffer;   /* 640×480 indexed 8bpp */
    civ_palette_t  palette;
    civ_present_t  present;

    /* M1：CJK 字型 ------------------------------------------------------ */
    civ_font_t *font_body;        /* 內文 16 px */
    civ_font_t *font_title;       /* 標題 24 px */
};

#endif /* CIV_GAME_H */
