/*
 * civ_event.h — 把 SDL_Event 分派到對的 widget
 *
 * 對應 SDL_IMPLEMENTATION_PLAN §3 末的 civ_dispatch_event 與 §4 的
 * widget 模型。取代 Win16 TRANSLATEMESSAGE / DISPATCHMESSAGE 路徑。
 */
#ifndef CIV_EVENT_H
#define CIV_EVENT_H

#include <SDL.h>

struct civ_game;
struct civ_widget;

/* 找出 (x, y) 落在哪個 widget；找不到回 NULL。 */
struct civ_widget *civ_widget_at(struct civ_game *g, int x, int y);

/* 主迴圈用：把 ev 派到對的 widget；回 0 表示已處理，> 0 為 widget
 * 自訂結果（M2 階段一律回 0）。 */
int civ_dispatch_event(struct civ_game *g, SDL_Event *ev);

#endif /* CIV_EVENT_H */
