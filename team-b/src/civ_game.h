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
#include "world/diplomat.h"
#include "world/sprite_sheet.h"
#include "world/tech.h"
#include "world/world.h"

#include <SDL.h>
#include <stdbool.h>
#include <stdint.h>

struct civ_widget;

struct civ_game {
    /* 主旗標 ------------------------------------------------------------ */
    bool       quit;             /* 原 DAT_12d8_24ee */
    uint32_t   timer_counter;    /* 原 DAT_12d8_24f0 */
    uint64_t   tick_count;       /* idle step 計次 */

    /* SDL --------------------------------------------------------------- */
    SDL_Window   *window;
    SDL_Renderer *renderer;

    /* 繪圖層 ----------------------------------------------------------- */
    civ_surface_t *framebuffer;
    civ_palette_t  palette;
    civ_present_t  present;

    /* 字型 ------------------------------------------------------------- */
    civ_font_t *font_body;
    civ_font_t *font_title;

    /* M2：widget 樹 ---------------------------------------------------- */
    struct civ_widget *map_w;
    struct civ_widget *minimap_w;
    struct civ_widget *status_w;
    struct civ_widget *focused_w;

    /* M2：modal lock — 對應 spec 02 §2.2.6 鎖定模式 (DAT_12b0_0000 /
     * 12b0_0004)。開啟時 map widget 只接受 RESIZE / CLOSE event，其他
     * widget 仍正常運作。 */
    bool       modal_lock;
    uint32_t   modal_lock_id;

    /* M5：世界 + sprite sheet ────────────────────────── */
    civ_world_t           world;
    civ_sprite_sheet_t    sprite_sheet;
    bool                  world_ready;

    /* M6-lite：回合 ──────────────────────────────────── */
    uint32_t              turn_number;     /* 起始 0；第 1 回合 = 4000 BC */
    int                   civ_year;        /* 西元；負數 = BC */
    uint64_t              ai_actions;      /* AI mock action 累計 */

    /* R6 M7-full: city screen modal ─────────────────── */
    bool       city_screen_open;          /* true 時 city_screen 蓋全螢幕 */
    int        city_screen_idx;           /* index into world.cities[] */

    /* R16 M-techscreen: tech discovery modal ────────── */
    bool                        tech_screen_open;   /* true 時 tech_screen 蓋全螢幕 */
    civ_tech_discovery_event_t  tech_screen_event;  /* 當前顯示的 event payload */

    /* R18 M-diplomat: diplomat visit modal ───────────── */
    bool                        diplomat_screen_open;  /* true 時 diplomat_screen 蓋全螢幕 */
    civ_diplomat_event_t        diplomat_screen_event; /* 當前顯示的 領袖 + mood */

    /* R19/R20: 領袖肖像 sprite cache — 從 CIVDATA2 KING00..13 (id 500..513) 載入.
     * leader_portraits[l] = NULL → 未載入, fallback 自畫.
     *
     * R20 改: 不再 cache 預算 LUT — 改 cache full king_pal (原版 sprite palette).
     * 渲染時可選擇:
     *   (a) 安裝 king_pal 為 g.palette → identity blit, 100% 顏色 (用於 diplomat
     *       全螢幕 modal showcase, 對齊 spec agent 確認的純色 skin/gold/紅天鵝絨)
     *   (b) 保持原 g.palette → in-place build_lut → 近似 nearest match (用於
     *       一般 widget 共存場景, 顏色會被 quantize 但結構保留)
     *
     * owner = civ_game, civ_widgets_unregister 時釋放 sprite (palette 是 value 不釋放). */
    civ_surface_t              *leader_portraits[CIV_LEADER_COUNT + 1];
    civ_palette_t               leader_king_palettes[CIV_LEADER_COUNT + 1];
};

#endif /* CIV_GAME_H */
