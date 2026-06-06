#include "map.h"
#include "../civ_game.h"
#include "../gfx/primitive.h"
#include "../gfx/surface.h"
#include "../text/text_out.h"
#include "../world/diplomat.h"
#include "../world/sprite_sheet.h"
#include "../world/tech.h"
#include "../world/world.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ── 22-entry dispatch table（M2 stub：handler 只更新 state，不
 *    做真實遊戲邏輯） ────────────────────────────────── */

static civ_evt_result_t on_mouse_down(civ_widget_t *w, SDL_Event *ev);
static civ_evt_result_t on_mouse_up  (civ_widget_t *w, SDL_Event *ev);
static civ_evt_result_t on_mouse_move(civ_widget_t *w, SDL_Event *ev);
static civ_evt_result_t on_mouse_wheel(civ_widget_t *w, SDL_Event *ev);
static civ_evt_result_t on_key_down  (civ_widget_t *w, SDL_Event *ev);
static civ_evt_result_t on_key_up    (civ_widget_t *w, SDL_Event *ev);
static civ_evt_result_t on_text_input(civ_widget_t *w, SDL_Event *ev);

/* 對應原版 1420:0634 那張 22-entry table。Win16 WM_* 與 SDL_Event
 * type 之間是 1:N（例 WM_PAINT 對應 SDL_WINDOWEVENT_EXPOSED + 主迴圈
 * present），我們列 SDL 端的事件。M3+ 隨著子系統真實實作再補滿 22。 */
static const civ_widget_msg_entry_t MAP_TABLE[] = {
    { SDL_MOUSEBUTTONDOWN, on_mouse_down  },
    { SDL_MOUSEBUTTONUP,   on_mouse_up    },
    { SDL_MOUSEMOTION,     on_mouse_move  },
    { SDL_MOUSEWHEEL,      on_mouse_wheel },
    { SDL_KEYDOWN,         on_key_down    },
    { SDL_KEYUP,           on_key_up      },
    { SDL_TEXTINPUT,       on_text_input  },
    /* 8–22：保留給 M3+ 補 — WM_TIMER / WM_VSCROLL / WM_HSCROLL /
     * WM_COMMAND / WM_SETCURSOR / WM_ACTIVATE / WM_PAINT 之 idle
     * tick 等。佔位欄位不寫 entry，table run 找不到回 0。 */
};
#define MAP_TABLE_LEN (sizeof MAP_TABLE / sizeof MAP_TABLE[0])

/* ── handlers ───────────────────────────────────────── */

static civ_evt_result_t on_mouse_down(civ_widget_t *w, SDL_Event *ev)
{
    civ_map_state_t *s = w->state;
    s->last_click_x = ev->button.x;
    s->last_click_y = ev->button.y;
    s->has_click    = true;
    return 0;
}

static civ_evt_result_t on_mouse_up(civ_widget_t *w, SDL_Event *ev)
{
    (void)w; (void)ev;
    return 0;
}

static civ_evt_result_t on_mouse_move(civ_widget_t *w, SDL_Event *ev)
{
    civ_map_state_t *s = w->state;
    s->last_mouse_x = ev->motion.x;
    s->last_mouse_y = ev->motion.y;
    s->has_mouse    = true;
    return 0;
}

static civ_evt_result_t on_mouse_wheel(civ_widget_t *w, SDL_Event *ev)
{
    (void)w; (void)ev;
    return 0;
}

static civ_evt_result_t on_key_down(civ_widget_t *w, SDL_Event *ev)
{
    /* M6-full-lite: 方向鍵 = 移動 selected unit; TAB = 循環下一個 unit;
     * 空白鍵 = 跳過此 unit (consume moves_left). */
    if (!w->game || !w->game->world_ready) return 0;
    civ_world_t *wd = &w->game->world;
    int tile_w = 32, tile_h = 32;
    if (w->game->sprite_sheet.sheet) {
        tile_w = w->game->sprite_sheet.tile_w;
        tile_h = w->game->sprite_sheet.tile_h;
    }
    int cols = w->rect.w / tile_w;
    int rows = w->rect.h / tile_h;

    switch (ev->key.keysym.sym) {
        case SDLK_UP:
        case SDLK_KP_8:    civ_world_move_selected(wd,  0, -1, cols, rows); break;
        case SDLK_DOWN:
        case SDLK_KP_2:    civ_world_move_selected(wd,  0,  1, cols, rows); break;
        case SDLK_LEFT:
        case SDLK_KP_4:    civ_world_move_selected(wd, -1,  0, cols, rows); break;
        case SDLK_RIGHT:
        case SDLK_KP_6:    civ_world_move_selected(wd,  1,  0, cols, rows); break;
        case SDLK_KP_7:    civ_world_move_selected(wd, -1, -1, cols, rows); break;
        case SDLK_KP_9:    civ_world_move_selected(wd,  1, -1, cols, rows); break;
        case SDLK_KP_1:    civ_world_move_selected(wd, -1,  1, cols, rows); break;
        case SDLK_KP_3:    civ_world_move_selected(wd,  1,  1, cols, rows); break;
        case SDLK_TAB:     civ_world_cycle_selection(wd, 1); break;  /* player 1 */
        case SDLK_SPACE:
            if (wd->selected_unit >= 0 && wd->selected_unit < wd->units_count)
                wd->units[wd->selected_unit].moves_left = 0;
            break;
        case SDLK_RETURN:
        case SDLK_KP_ENTER: {
            /* R18: diplomat_screen 開啟時 Enter 關閉 */
            if (w->game->diplomat_screen_open) {
                w->game->diplomat_screen_open = false;
                w->game->modal_lock           = false;
                break;
            }
            /* R16: tech_screen 開啟時 Enter 關閉 */
            if (w->game->tech_screen_open) {
                w->game->tech_screen_open = false;
                w->game->modal_lock       = false;
                break;
            }
            /* R6: cursor 上有 city → 打開 city screen */
            int cidx = civ_world_city_at(wd, wd->cursor_x, wd->cursor_y);
            if (cidx >= 0) {
                w->game->city_screen_open = true;
                w->game->city_screen_idx  = cidx;
                w->game->modal_lock       = true;
            }
            break;
        }
        case SDLK_t: {
            /* R16-6 demo: 按 T → BRONZE WORKING 研發完成 modal */
            civ_tech_discovery_event_t *ev = &w->game->tech_screen_event;
            memset(ev, 0, sizeof *ev);
            ev->tech_id       = CIV_TECH_BRONZE_WORKING;
            ev->source        = CIV_TECH_LEARN_SELF;
            ev->from_civ_slot = 0;
            civ_tech_discovery_fill_unlocked(ev);
            w->game->tech_screen_open = true;
            w->game->modal_lock       = true;
            break;
        }
        case SDLK_d: {
            /* R18 demo: 按 D → 伊莉莎白 GREETING modal */
            civ_diplomat_event_t *dev = &w->game->diplomat_screen_event;
            memset(dev, 0, sizeof *dev);
            dev->leader = CIV_LEADER_ELIZABETH;
            dev->mood   = CIV_DIPLOMAT_GREETING;
            w->game->diplomat_screen_open = true;
            w->game->modal_lock           = true;
            break;
        }
        case SDLK_ESCAPE:
            if (w->game->diplomat_screen_open) {
                w->game->diplomat_screen_open = false;
                w->game->modal_lock           = false;
            } else if (w->game->tech_screen_open) {
                w->game->tech_screen_open = false;
                w->game->modal_lock       = false;
            } else if (w->game->city_screen_open) {
                w->game->city_screen_open = false;
                w->game->modal_lock       = false;
            }
            break;
        default: break;
    }
    return 0;
}

static civ_evt_result_t on_key_up(civ_widget_t *w, SDL_Event *ev)
{
    (void)w; (void)ev;
    return 0;
}

static civ_evt_result_t on_text_input(civ_widget_t *w, SDL_Event *ev)
{
    (void)w; (void)ev;
    return 0;
}

/* ── dispatch（含 modal_lock 短路） ─────────────────── */

static civ_evt_result_t map_dispatch(civ_widget_t *w, SDL_Event *ev)
{
    civ_map_state_t *s = w->state;
    w->call_count++;          /* 對應 DAT_1420_4860 */

    if (w->game && w->game->modal_lock) {
        s->modal_short_circuits++;
        /* 對應原版：鎖定模式只接受 RESIZE / CLOSE，餘交 DefWindowProc */
        if (ev->type == SDL_WINDOWEVENT &&
            (ev->window.event == SDL_WINDOWEVENT_RESIZED ||
             ev->window.event == SDL_WINDOWEVENT_SIZE_CHANGED ||
             ev->window.event == SDL_WINDOWEVENT_CLOSE)) {
            /* M3+：把幾何 / quit flag 寫入 game */
            return 0;
        }
        return 0;             /* DefWindowProc 等價 — no-op */
    }

    return civ_widget_table_run(w, MAP_TABLE, MAP_TABLE_LEN, ev);
}

/* ── render ─────────────────────────────────────────── */

static void map_render(civ_widget_t *w, civ_surface_t *fb)
{
    /* M5：用 sprite sheet + world 資料畫 terrain，沒 sheet 時 fallback 綠底
     *
     * 修 (2026-06-06 第二輪): terrain[][] 改存 civ_terrain_kind_t enum，
     * 用 civ_terrain_sprite_coord() 查代表 (col, row) 餵 civ_sprite_rect()。
     * 舊版直接把 row*46+col 當 col 傳 + clamp >= 46 為 0 是錯的。 */
    if (w->game && w->game->world_ready && w->game->sprite_sheet.sheet) {
        civ_sprite_sheet_t *sh = &w->game->sprite_sheet;
        civ_world_t        *wd = &w->game->world;
        int tile_w = sh->tile_w, tile_h = sh->tile_h;
        int cols = w->rect.w / tile_w;
        int rows = w->rect.h / tile_h;
        /* R14: 雙 layer render — base GRASS 先打底 (對 land tiles), 再 overlay
         * 用 skip0 模式不蓋 transparent. 解 RIVER/MOUNTAIN sprite 露 palette-0
         * 紅的問題. OCEAN 仍直接 blit (本身就是水底). */
        int grass_col = 22, grass_row = 2;  /* spec 06 §6.6 GRASS sprite */
        civ_rect_t grass_src = civ_sprite_rect(sh, grass_col, grass_row);

        for (int ry = 0; ry < rows; ry++) {
            for (int rx = 0; rx < cols; rx++) {
                int wx = wd->view_x + rx;
                int wy = wd->view_y + ry;
                if (wx < 0 || wy < 0 || wx >= CIV_MAP_W || wy >= CIV_MAP_H)
                    continue;
                civ_terrain_kind_t kind =
                    (civ_terrain_kind_t)wd->terrain[wy][wx];
                int sc = 0, sr = 0;
                civ_terrain_sprite_coord(kind, &sc, &sr);
                if (sc < 0 || sc >= sh->cols) sc = 0;
                if (sr < 0 || sr >= sh->rows) sr = 0;
                int dx = w->rect.x + rx * tile_w;
                int dy = w->rect.y + ry * tile_h;

                /* Step 1: 對非 OCEAN tile, 先 blit GRASS base 打底 */
                if (kind != CIV_TERRAIN_OCEAN) {
                    if (sh->lut_built)
                        civ_surface_blit_remap(fb, dx, dy, sh->sheet, &grass_src, sh->lut);
                    else
                        civ_surface_blit(fb, dx, dy, sh->sheet, &grass_src);
                }

                /* Step 2: blit terrain sprite. GRASS / PLAINS / DESERT / OCEAN
                 * 用 full blit (本身就是 base color); 其他 (FOREST/MOUNTAIN/
                 * HILLS/JUNGLE/SWAMP/RIVER 等) 用 skip0 不蓋 transparent */
                civ_rect_t src = civ_sprite_rect(sh, sc, sr);
                bool is_base = (kind == CIV_TERRAIN_OCEAN ||
                                kind == CIV_TERRAIN_GRASS ||
                                kind == CIV_TERRAIN_PLAINS ||
                                kind == CIV_TERRAIN_DESERT);
                if (is_base) {
                    if (sh->lut_built)
                        civ_surface_blit_remap(fb, dx, dy, sh->sheet, &src, sh->lut);
                    else
                        civ_surface_blit(fb, dx, dy, sh->sheet, &src);
                } else {
                    /* overlay: skip palette-0 transparent */
                    if (sh->lut_built)
                        civ_surface_blit_remap_skip0(fb, dx, dy, sh->sheet, &src, sh->lut);
                    else {
                        /* fallback: 沒 LUT 時 build identity LUT 後用 skip0 */
                        static uint8_t identity_lut[256];
                        static int identity_built = 0;
                        if (!identity_built) {
                            for (int i = 0; i < 256; i++) identity_lut[i] = (uint8_t)i;
                            identity_built = 1;
                        }
                        civ_surface_blit_remap_skip0(fb, dx, dy, sh->sheet, &src, identity_lut);
                    }
                }
            }
        }
        /* R5 M6-full: render cities (overlay terrain, before units).
         *
         * Civ1 city sprite 在 SPR32X32 row 0 cols 30..43 (14 個成長 stage).
         * 我們依 population 選 col (1..2 = 小村莊 / 3..5 = 城鎮 / 6+ = 大城).
         * 對齊 spec 03 §3.5.1 sentinel — owner color 是 runtime 替換,
         * 但 sheet 上 cols 30+ city sprite 用 idx 1-4 (national + barb) 為主色,
         * 我們直接 blit 不重映色, 各 owner 視覺相似可接受 (M7 polish 再修). */
        for (int i = 0; i < wd->cities_count; i++) {
            const civ_city_t *c = &wd->cities[i];
            if (!c->alive) continue;
            int rx = c->x - wd->view_x;
            int ry = c->y - wd->view_y;
            if (rx < 0 || ry < 0 || rx >= cols || ry >= rows) continue;
            int dx = w->rect.x + rx * tile_w;
            int dy = w->rect.y + ry * tile_h;

            int city_col = 30;  /* 小村莊 */
            if (c->population >= 6) city_col = 32;       /* 大城 */
            else if (c->population >= 3) city_col = 31;  /* 城鎮 */
            civ_rect_t src = civ_sprite_rect(sh, city_col, 0);
            if (sh->lut_built)
                civ_surface_blit_remap(fb, dx, dy, sh->sheet, &src, sh->lut);
            else
                civ_surface_blit(fb, dx, dy, sh->sheet, &src);

            /* owner 色框 (跟 unit owner 一致) */
            static const uint8_t CITY_OWNER_COLOR[CIV_NUM_PLAYERS] = {
                12, 14, 9, 10, 11, 13, 6, 15,
            };
            uint8_t col = CITY_OWNER_COLOR[c->owner < CIV_NUM_PLAYERS ? c->owner : 0];
            civ_frame_rect(fb, (civ_rect_t){dx, dy, tile_w, tile_h}, col);

            /* city name label 浮在 sprite 下方 */
            if (w->game->font_body && c->name[0]) {
                int name_w = civ_text_measure(w->game->font_body, c->name);
                int nx = dx + (tile_w - name_w) / 2;
                int ny = dy + tile_h + 12;
                /* 黑色底 (idx 0 typically 紅 in sheet palette,
                 * 但 frame_rect 用 0 還是黑 fallback) */
                civ_fill_rect(fb, (civ_rect_t){nx - 2, ny - 11, name_w + 4, 13}, 0);
                civ_text_out(fb, w->game->font_body, nx, ny, c->name,
                             15, 0, CIV_TEXT_BK_TRANSPARENT);
            }
        }

        /* M6-full-lite: render units (overlay on terrain).
         *
         * Unit sprite 取 SPR32X32 row 10 cols 0..15 (軍事單位橘框 icon),
         * Settlers 用 row 7 col 14 (人物站立 icon, 草色). 真實 unit→sprite
         * 對位是 spec 06 後續工作; 本輪先用 placeholder 對位讓 unit 看得到.
         *
         * Owner 0 = barbarian 用紅框, 1 = 黃, 2 = 藍, 3..7 各色. 對應原版
         * civilization 色 (Civ1 玩家色). */
        static const uint8_t OWNER_COLOR[CIV_NUM_PLAYERS] = {
            12,  /* 0 barbarian = 紅 */
            14,  /* 1 player    = 黃 */
            9,   /* 2 = 藍 */
            10,  /* 3 = 綠 */
            11,  /* 4 = 青 */
            13,  /* 5 = 紫 */
            6,   /* 6 = 灰 */
            15,  /* 7 = 白 */
        };
        for (int i = 0; i < wd->units_count; i++) {
            const civ_unit_t *u = &wd->units[i];
            if (!u->alive) continue;
            int rx = u->x - wd->view_x;
            int ry = u->y - wd->view_y;
            if (rx < 0 || ry < 0 || rx >= cols || ry >= rows) continue;
            int dx = w->rect.x + rx * tile_w;
            int dy = w->rect.y + ry * tile_h;

            int sc = 0, sr = 10;  /* 預設軍事單位 row */
            switch (u->type) {
                case CIV_UNIT_SETTLERS:   sc = 0;  sr = 10; break;
                case CIV_UNIT_MILITIA:    sc = 1;  sr = 10; break;
                case CIV_UNIT_PHALANX:    sc = 2;  sr = 10; break;
                case CIV_UNIT_LEGION:     sc = 3;  sr = 10; break;
                case CIV_UNIT_MUSKETEERS: sc = 4;  sr = 10; break;
                default: sc = 0; sr = 10; break;
            }
            civ_rect_t src = civ_sprite_rect(sh, sc, sr);
            if (sh->lut_built)
                civ_surface_blit_remap(fb, dx, dy, sh->sheet, &src, sh->lut);
            else
                civ_surface_blit(fb, dx, dy, sh->sheet, &src);

            /* owner 色框 (3 px 雙環顯眼) */
            uint8_t col = OWNER_COLOR[u->owner < CIV_NUM_PLAYERS ? u->owner : 0];
            civ_frame_rect(fb, (civ_rect_t){dx,     dy,     tile_w,     tile_h}, col);
            civ_frame_rect(fb, (civ_rect_t){dx + 1, dy + 1, tile_w - 2, tile_h - 2}, col);
        }

        /* cursor */
        int cx = (wd->cursor_x - wd->view_x) * tile_w + w->rect.x;
        int cy = (wd->cursor_y - wd->view_y) * tile_h + w->rect.y;
        if (cx >= w->rect.x && cy >= w->rect.y &&
            cx + tile_w <= w->rect.x + w->rect.w &&
            cy + tile_h <= w->rect.y + w->rect.h) {
            /* selected unit 用閃爍白框 (drawn 比 owner 框更外圈);
             * 非 selected 用一般 cursor 黃框 */
            uint8_t cur_col = (wd->selected_unit >= 0) ? 15 : 14;
            civ_frame_rect(fb, (civ_rect_t){cx,     cy,     tile_w, tile_h}, cur_col);
            civ_frame_rect(fb, (civ_rect_t){cx - 1, cy - 1, tile_w + 2, tile_h + 2}, cur_col);
        }
        civ_frame_rect(fb, w->rect, 0);
        return;
    }

    /* M0..M4 fallback：placeholder 地形草原綠 */
    uint8_t terrain = 64 + 0 * 36 + 4 * 6 + 1;
    civ_fill_rect(fb, w->rect, terrain);
    civ_frame_rect(fb, w->rect, 0);
    civ_map_state_t *s = w->state;
    if (s->has_mouse) {
        int cx = s->last_mouse_x, cy = s->last_mouse_y;
        civ_hline(fb, cx - 3, cy, 7, 12);
        civ_vline(fb, cx, cy - 3, 7, 12);
    }
    if (s->has_click) {
        int cx = s->last_click_x, cy = s->last_click_y;
        civ_frame_rect(fb, (civ_rect_t){cx - 4, cy - 4, 9, 9}, 14);
    }
    if (w->game && w->game->font_body) {
        civ_text_out(fb, w->game->font_body,
                     w->rect.x + 8, w->rect.y + 24,
                     "WDWMAPPROC — 主地圖", 15, 0,
                     CIV_TEXT_BK_TRANSPARENT);
    }
}

static void map_destroy(civ_widget_t *w)
{
    free(w->state);
    free(w);
}

static const civ_widget_vtable_t MAP_VTABLE = {
    .name     = "map",
    .dispatch = map_dispatch,
    .render   = map_render,
    .destroy  = map_destroy,
};

civ_widget_t *civ_map_widget_new(struct civ_game *g, civ_rect_t rect)
{
    civ_widget_t *w = calloc(1, sizeof *w);
    if (!w) return NULL;
    w->vt      = &MAP_VTABLE;
    w->rect    = rect;
    w->visible = true;
    w->game    = g;
    w->state   = calloc(1, sizeof(civ_map_state_t));
    if (!w->state) { free(w); return NULL; }
    return w;
}
