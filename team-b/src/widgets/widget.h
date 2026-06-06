/*
 * widgets/widget.h — base widget type + dispatch table 模型
 *
 * 對應 spec 02 §2.2.6 的 22-entry parallel array dispatch（原版用
 * `static const WORD msgs[22]` + `static const FARPROC handlers[22]`）。
 * Team B 翻成 `{sdl_event_type, handler_fn}` 表，主迴圈把 SDL_Event
 * 分派到對的 widget，widget 再從自己的 table 找對應 handler。
 *
 * Mac shim（Mac Memory Manager / Resource Manager port）依 SDL_PLAN
 * §2 規範壓平：widget 沒有 NewPtr / Handle 抽象，直接 plain pointer。
 */
#ifndef CIV_WIDGETS_WIDGET_H
#define CIV_WIDGETS_WIDGET_H

#include "../gfx/surface.h"

#include <SDL.h>
#include <stdbool.h>
#include <stdint.h>

struct civ_game;
struct civ_widget;

/* 與 Win16 LRESULT 對應；0 = 已處理；> 0 視 caller 約定。M2 階段
 * 多半回 0。 */
typedef int civ_evt_result_t;

typedef civ_evt_result_t (*civ_widget_handler_fn)(struct civ_widget *w,
                                                  SDL_Event *ev);

typedef struct civ_widget_msg_entry {
    Uint32                sdl_type;
    civ_widget_handler_fn fn;
} civ_widget_msg_entry_t;

typedef civ_evt_result_t (*civ_widget_dispatch_fn)(struct civ_widget *w,
                                                   SDL_Event *ev);
typedef void (*civ_widget_render_fn)(struct civ_widget *w,
                                     civ_surface_t *fb);
typedef void (*civ_widget_destroy_fn)(struct civ_widget *w);

typedef struct civ_widget_vtable {
    const char            *name;          /* 偵錯用；對齊 spec 02 §2.2 命名 */
    civ_widget_dispatch_fn dispatch;
    civ_widget_render_fn   render;
    civ_widget_destroy_fn  destroy;
} civ_widget_vtable_t;

typedef struct civ_widget {
    const civ_widget_vtable_t *vt;
    civ_rect_t               rect;       /* surface 內絕對座標 */
    bool                     visible;
    bool                     focused;
    uint64_t                 call_count; /* 對應 spec 02 §2.2.6 DAT_1420_4860 */
    void                    *state;      /* per-widget 私有狀態 */
    struct civ_game         *game;       /* 回指主結構（modal_lock 等讀寫） */
} civ_widget_t;

/* point-in-rect helper（rect.w / h 為「寬高」非「右下座標」） */
static inline bool civ_rect_contains(civ_rect_t r, int x, int y)
{
    return x >= r.x && y >= r.y && x < r.x + r.w && y < r.y + r.h;
}

/* 跑 widget 自己的 dispatch table，找不到回 0。 */
civ_evt_result_t civ_widget_table_run(civ_widget_t *w,
                                      const civ_widget_msg_entry_t *table,
                                      size_t n,
                                      SDL_Event *ev);

#endif /* CIV_WIDGETS_WIDGET_H */
