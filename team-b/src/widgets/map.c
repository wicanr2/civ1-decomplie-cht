#include "map.h"
#include "../civ_game.h"
#include "../gfx/primitive.h"
#include "../text/text_out.h"

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
    (void)w; (void)ev;
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
    /* placeholder 地形：草原綠（6×6×6 cube 內 R=0 G=4 B=1 ≈ 0,204,51） */
    uint8_t terrain = 64 + 0 * 36 + 4 * 6 + 1;   /* palette §3 計算 */
    civ_fill_rect(fb, w->rect, terrain);

    /* 1px 黑邊框 */
    civ_frame_rect(fb, w->rect, 0);

    /* 中央 marker — hover 時亮紅 */
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

    /* 標籤 */
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
