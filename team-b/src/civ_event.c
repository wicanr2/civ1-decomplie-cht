#include "civ_event.h"
#include "civ_game.h"
#include "widgets/widget.h"

struct civ_widget *civ_widget_at(struct civ_game *g, int x, int y)
{
    /* 順序：minimap → status → map（前兩個面積小、優先；map 兜底） */
    if (g->minimap_w && g->minimap_w->visible &&
        civ_rect_contains(g->minimap_w->rect, x, y)) return g->minimap_w;
    if (g->status_w && g->status_w->visible &&
        civ_rect_contains(g->status_w->rect, x, y)) return g->status_w;
    if (g->map_w && g->map_w->visible &&
        civ_rect_contains(g->map_w->rect, x, y)) return g->map_w;
    return NULL;
}

int civ_dispatch_event(struct civ_game *g, SDL_Event *ev)
{
    struct civ_widget *target = NULL;
    int x = 0, y = 0;

    switch (ev->type) {
    case SDL_MOUSEMOTION:
        x = ev->motion.x; y = ev->motion.y;
        target = civ_widget_at(g, x, y);
        break;
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
        x = ev->button.x; y = ev->button.y;
        target = civ_widget_at(g, x, y);
        break;
    case SDL_MOUSEWHEEL:
        SDL_GetMouseState(&x, &y);
        target = civ_widget_at(g, x, y);
        break;
    case SDL_KEYDOWN:
    case SDL_KEYUP:
    case SDL_TEXTINPUT:
        target = g->focused_w ? g->focused_w : g->map_w;
        break;
    case SDL_WINDOWEVENT:
        /* 視窗事件廣播到三個 widget，由各自 dispatch 決定接不接。
         * M2 階段 map 在 modal_lock 時專門處理 RESIZE / CLOSE。 */
        if (g->map_w     && g->map_w->visible)     g->map_w->vt->dispatch(g->map_w, ev);
        if (g->minimap_w && g->minimap_w->visible) g->minimap_w->vt->dispatch(g->minimap_w, ev);
        if (g->status_w  && g->status_w->visible)  g->status_w->vt->dispatch(g->status_w, ev);
        return 0;
    default:
        target = NULL;
        break;
    }

    if (target && target->vt && target->vt->dispatch) {
        return target->vt->dispatch(target, ev);
    }
    return 0;
}
