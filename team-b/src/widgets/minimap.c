#include "minimap.h"
#include "../civ_game.h"
#include "../gfx/primitive.h"
#include "../text/text_out.h"

#include <stdlib.h>

static civ_evt_result_t on_click(civ_widget_t *w, SDL_Event *ev);

/* 9-entry dispatch — spec 02 §2.2.7 推測：WM_PAINT / WM_LBUTTONDOWN
 * / WM_RBUTTONDOWN / WM_MOUSEMOVE / WM_TIMER / WM_CREATE / WM_DESTROY
 * / WM_SIZE / WM_SETFOCUS。M2 stub 只實作可立即測的 3 個。 */
static const civ_widget_msg_entry_t MINIMAP_TABLE[] = {
    { SDL_MOUSEBUTTONDOWN, on_click },
};
#define MINIMAP_TABLE_LEN (sizeof MINIMAP_TABLE / sizeof MINIMAP_TABLE[0])

static civ_evt_result_t on_click(civ_widget_t *w, SDL_Event *ev)
{
    civ_minimap_state_t *s = w->state;
    s->view_x = ev->button.x;
    s->view_y = ev->button.y;
    s->has_view = true;
    return 0;
}

static civ_evt_result_t minimap_dispatch(civ_widget_t *w, SDL_Event *ev)
{
    w->call_count++;
    /* 無鎖定模式 */
    return civ_widget_table_run(w, MINIMAP_TABLE, MINIMAP_TABLE_LEN, ev);
}

static void minimap_render(civ_widget_t *w, civ_surface_t *fb)
{
    /* 深藍底（palette 9） */
    civ_fill_rect(fb, w->rect, 9);
    civ_frame_rect(fb, w->rect, 7);   /* 灰邊框 */

    /* view marker */
    civ_minimap_state_t *s = w->state;
    if (s->has_view) {
        civ_frame_rect(fb, (civ_rect_t){s->view_x - 3, s->view_y - 3, 7, 7}, 14);
    }

    if (w->game && w->game->font_body) {
        civ_text_out(fb, w->game->font_body,
                     w->rect.x + 6, w->rect.y + 20,
                     "WDWSMMAP", 15, 0,
                     CIV_TEXT_BK_TRANSPARENT);
        civ_text_out(fb, w->game->font_body,
                     w->rect.x + 6, w->rect.y + 40,
                     "小地圖", 15, 0,
                     CIV_TEXT_BK_TRANSPARENT);
    }
}

static void minimap_destroy(civ_widget_t *w)
{
    free(w->state);
    free(w);
}

static const civ_widget_vtable_t MINIMAP_VTABLE = {
    .name     = "minimap",
    .dispatch = minimap_dispatch,
    .render   = minimap_render,
    .destroy  = minimap_destroy,
};

civ_widget_t *civ_minimap_widget_new(struct civ_game *g, civ_rect_t rect)
{
    civ_widget_t *w = calloc(1, sizeof *w);
    if (!w) return NULL;
    w->vt      = &MINIMAP_VTABLE;
    w->rect    = rect;
    w->visible = true;
    w->game    = g;
    w->state   = calloc(1, sizeof(civ_minimap_state_t));
    if (!w->state) { free(w); return NULL; }
    return w;
}
