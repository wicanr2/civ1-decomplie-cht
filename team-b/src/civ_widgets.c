#include "civ_widgets.h"
#include "civ_game.h"
#include "widgets/map.h"
#include "widgets/minimap.h"
#include "widgets/status.h"

#define TITLE_H 40
#define MAP_W   480
#define MMAP_H  120
#define SCREEN_W 640
#define SCREEN_H 480

int civ_widgets_register(struct civ_game *g)
{
    /* layout（640×480 base canvas）：
     *   ┌─────────────────────────────┐ 0,0
     *   │  title bar (40)             │
     *   ├──────────────────┬──────────┤ 0,40
     *   │                  │ minimap  │
     *   │   主地圖 480     │ 160×120  │
     *   │   ×440           ├──────────┤ 480,160
     *   │                  │ status   │
     *   │                  │ 160×320  │
     *   └──────────────────┴──────────┘ 640,480
     */
    civ_rect_t map_rect     = { 0,         TITLE_H, MAP_W,        SCREEN_H - TITLE_H };
    civ_rect_t minimap_rect = { MAP_W,     TITLE_H, SCREEN_W - MAP_W, MMAP_H };
    civ_rect_t status_rect  = { MAP_W,     TITLE_H + MMAP_H,
                                SCREEN_W - MAP_W, SCREEN_H - TITLE_H - MMAP_H };

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
