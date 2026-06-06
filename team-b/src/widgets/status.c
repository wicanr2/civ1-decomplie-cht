#include "status.h"
#include "../civ_game.h"
#include "../gfx/primitive.h"
#include "../text/text_out.h"

#include <stdio.h>
#include <stdlib.h>

static const civ_widget_msg_entry_t STATUS_TABLE[] = {
    /* M2：暫無 handler — paint-only widget */
    { 0, NULL },
};

static civ_evt_result_t status_dispatch(civ_widget_t *w, SDL_Event *ev)
{
    (void)ev;
    w->call_count++;
    return 0;   /* paint-only，所有 event 都吃下不擋 */
}

static void status_render(civ_widget_t *w, civ_surface_t *fb)
{
    /* 灰底 */
    civ_fill_rect(fb, w->rect, 7);
    civ_frame_rect(fb, w->rect, 8);

    if (!w->game) return;
    civ_font_t *font = w->game->font_body;
    if (!font) return;

    civ_text_out(fb, font,
                 w->rect.x + 8, w->rect.y + 22,
                 "WDWSTATUS — 狀態", 0, 7,
                 CIV_TEXT_BK_TRANSPARENT);

    /* M0–M7 milestone 列表 */
    const char *lines[] = {
        "M0 SDL 視窗",
        "M1 palette + CJK",
        "M2 widget skeleton",
        "M3 .RSC + CvPc",
        "M4 14 文明 + 新局",
        "M5 地圖 + 地形",
        "M6 turn + AI + 存讀",
        "M7 奇蹟 / 外交 / 勝利",
    };
    int y = w->rect.y + 50;
    for (size_t i = 0; i < sizeof lines / sizeof lines[0]; i++) {
        civ_text_out(fb, font, w->rect.x + 8, y, lines[i], 0, 7,
                     CIV_TEXT_BK_TRANSPARENT);
        y += 20;
    }

    /* 動態：目前 tick count */
    char buf[64];
    snprintf(buf, sizeof buf, "tick=%llu",
             (unsigned long long)w->game->tick_count);
    civ_text_out(fb, font, w->rect.x + 8, w->rect.y + w->rect.h - 24,
                 buf, 1, 7, CIV_TEXT_BK_TRANSPARENT);
}

static void status_destroy(civ_widget_t *w)
{
    free(w->state);
    free(w);
}

static const civ_widget_vtable_t STATUS_VTABLE = {
    .name     = "status",
    .dispatch = status_dispatch,
    .render   = status_render,
    .destroy  = status_destroy,
};

civ_widget_t *civ_status_widget_new(struct civ_game *g, civ_rect_t rect)
{
    civ_widget_t *w = calloc(1, sizeof *w);
    if (!w) return NULL;
    w->vt      = &STATUS_VTABLE;
    w->rect    = rect;
    w->visible = true;
    w->game    = g;
    w->state   = calloc(1, sizeof(civ_status_state_t));
    if (!w->state) { free(w); return NULL; }
    (void)STATUS_TABLE;   /* 暫無 handler，silence warning */
    return w;
}
