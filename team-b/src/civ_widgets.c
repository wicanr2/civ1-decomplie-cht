#include "civ_widgets.h"
#include "civ_game.h"
#include "widgets/city_screen.h"
#include "widgets/map.h"
#include "widgets/minimap.h"
#include "widgets/status.h"
#include "widgets/tech_screen.h"

/* R4 layout (2026-06-06): 對齊原版 1993 Civ Windows
 *   docs/screenshots/reference/civ1_win_civilopedia_dropdown.png
 *
 *     ┌─────────────────────────────┐ 0,0
 *     │  CIVILIZATION title bar (16)│
 *     │  File Edit Orders ... (16)  │  menu bar 8 items
 *     ├──────────┬──────────────────┤ 0,32   ← CHROME_H=32
 *     │ minimap  │                  │
 *     │ 160×128  │   主地圖 480     │
 *     ├──────────┤   ×448           │
 *     │ status   │                  │
 *     │ 160×320  │                  │
 *     └──────────┴──────────────────┘ 640,480
 */
#define TITLE_H  16
#define MENU_H   16
#define CHROME_H (TITLE_H + MENU_H)
#define MMAP_W   160
#define MMAP_H   128
#define MAP_W    480
#define SCREEN_W 640
#define SCREEN_H 480

int civ_widgets_register(struct civ_game *g)
{
    civ_rect_t minimap_rect = { 0,      CHROME_H,         MMAP_W, MMAP_H };
    civ_rect_t status_rect  = { 0,      CHROME_H + MMAP_H,
                                MMAP_W, SCREEN_H - CHROME_H - MMAP_H };
    civ_rect_t map_rect     = { MMAP_W, CHROME_H, MAP_W, SCREEN_H - CHROME_H };

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

    /* R6 + R16: modal overlays — 蓋在所有 widget 上 */
    if (g->city_screen_open)
        civ_city_screen_render(g, g->framebuffer);
    if (g->tech_screen_open)
        civ_tech_screen_render(g, g->framebuffer);
}
