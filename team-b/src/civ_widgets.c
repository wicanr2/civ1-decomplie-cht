#include "civ_widgets.h"
#include "civ_game.h"
#include "widgets/map.h"
#include "widgets/minimap.h"
#include "widgets/status.h"

/* M5-B layout (2026-06-06 第二輪 C-B 修):
 *   原版 1991/1993 Civ1 小地圖在「左上」不在「右上」, 頂部是窄 menu bar
 *   (Game/Orders/Advisors/World/Civilopedia) 不是大紅 title bar. 改:
 *
 *     ┌─────────────────────────────┐ 0,0
 *     │  menu bar (16)              │
 *     ├──────────┬──────────────────┤ 0,16
 *     │ minimap  │                  │
 *     │ 160×120  │   主地圖 480     │
 *     ├──────────┤   ×448           │
 *     │ status   │                  │
 *     │ 160×344  │                  │
 *     └──────────┴──────────────────┘ 640,480
 */
#define MENU_H   16
#define MMAP_W   160
#define MMAP_H   120
#define MAP_W    480
#define SCREEN_W 640
#define SCREEN_H 480

int civ_widgets_register(struct civ_game *g)
{
    civ_rect_t minimap_rect = { 0,      MENU_H,         MMAP_W, MMAP_H };
    civ_rect_t status_rect  = { 0,      MENU_H + MMAP_H,
                                MMAP_W, SCREEN_H - MENU_H - MMAP_H };
    civ_rect_t map_rect     = { MMAP_W, MENU_H, MAP_W, SCREEN_H - MENU_H };

    g->map_w     = civ_map_widget_new(g, map_rect);
    g->minimap_w = civ_minimap_widget_new(g, minimap_rect);
    g->status_w  = civ_status_widget_new(g, status_rect);

    if (!g->map_w || !g->minimap_w || !g->status_w) {
        civ_widgets_unregister(g);
        return -1;
    }
    g->focused_w = g->map_w;
    return 0;
}

void civ_widgets_unregister(struct civ_game *g)
{
    if (g->map_w)     { g->map_w->vt->destroy(g->map_w);         g->map_w = NULL; }
    if (g->minimap_w) { g->minimap_w->vt->destroy(g->minimap_w); g->minimap_w = NULL; }
    if (g->status_w)  { g->status_w->vt->destroy(g->status_w);   g->status_w = NULL; }
    g->focused_w = NULL;
}

void civ_widgets_render_all(struct civ_game *g)
{
    if (!g->framebuffer) return;
    if (g->map_w && g->map_w->visible && g->map_w->vt->render)
        g->map_w->vt->render(g->map_w, g->framebuffer);
    if (g->minimap_w && g->minimap_w->visible && g->minimap_w->vt->render)
        g->minimap_w->vt->render(g->minimap_w, g->framebuffer);
    if (g->status_w && g->status_w->visible && g->status_w->vt->render)
        g->status_w->vt->render(g->status_w, g->framebuffer);
}
