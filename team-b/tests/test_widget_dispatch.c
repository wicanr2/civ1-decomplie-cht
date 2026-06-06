/*
 * test_widget_dispatch.c — M2 驗證點
 *
 * 對應 SDL_IMPLEMENTATION_PLAN §13 M2：
 *   1. push SDL_MOUSEMOTION 到主地圖區，map.call_count == 1、
 *      minimap.call_count == 0；
 *   2. push 到小地圖區，minimap.call_count == 1；
 *   3. modal_lock=true 後 mouse click 不會進 normal handler
 *      （但 dispatch 仍進入 short-circuit branch）。
 */
#include "civ_event.h"
#include "civ_game.h"
#include "civ_widgets.h"
#include "widgets/map.h"
#include "widgets/widget.h"

#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EXPECT(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL %s:%d  %s\n", __FILE__, __LINE__, #cond); \
        exit(1); \
    } \
} while (0)

/* layout 與 civ_widgets.c 同步：
 *   title 0..40
 *   map     (0, 40) 480×440
 *   minimap (480, 40) 160×120
 *   status  (480, 160) 160×320
 */

static void mk_motion(SDL_Event *ev, int x, int y)
{
    memset(ev, 0, sizeof *ev);
    ev->type = SDL_MOUSEMOTION;
    ev->motion.x = x;
    ev->motion.y = y;
}

static void mk_click(SDL_Event *ev, int x, int y)
{
    memset(ev, 0, sizeof *ev);
    ev->type = SDL_MOUSEBUTTONDOWN;
    ev->button.x = x;
    ev->button.y = y;
    ev->button.button = SDL_BUTTON_LEFT;
}

int main(void)
{
    SDL_setenv("SDL_VIDEODRIVER", "dummy", 0);
    EXPECT(SDL_Init(SDL_INIT_VIDEO) == 0);

    struct civ_game g = {0};
    EXPECT(civ_widgets_register(&g) == 0);

    /* 三個 widget call_count 初始 0 */
    EXPECT(g.map_w->call_count == 0);
    EXPECT(g.minimap_w->call_count == 0);
    EXPECT(g.status_w->call_count == 0);

    /* R4 layout (2026-06-06): chrome 從 16 → 32 (title + menu)
     *   chrome:    y < 32 (title 16 + menu 16)
     *   minimap:   x 0..159,   y 32..159  (含 W16 sub-title)
     *   status:    x 0..159,   y 160..479 (含 W16 sub-title)
     *   map:       x 160..639, y 32..479
     */

    /* ── 測 1：mouse motion 落在主地圖區 ───────────────── */
    SDL_Event ev;
    mk_motion(&ev, 300, 100);   /* 在 map (160..640, 32..480) 內 */
    civ_dispatch_event(&g, &ev);
    EXPECT(g.map_w->call_count == 1);
    EXPECT(g.minimap_w->call_count == 0);
    EXPECT(g.status_w->call_count == 0);

    /* 驗證 map state 收到正確座標 */
    civ_map_state_t *ms = g.map_w->state;
    EXPECT(ms->has_mouse);
    EXPECT(ms->last_mouse_x == 300);
    EXPECT(ms->last_mouse_y == 100);

    /* ── 測 2：mouse motion 落在小地圖區 ─────────────── */
    mk_motion(&ev, 80, 80);     /* 在 minimap (0..160, 32..160) 內 */
    civ_dispatch_event(&g, &ev);
    EXPECT(g.minimap_w->call_count == 1);
    EXPECT(g.map_w->call_count == 1);    /* 沒漏出去 */
    EXPECT(g.status_w->call_count == 0);

    /* ── 測 3：modal_lock 開啟後 click 走 short-circuit ─── */
    uint64_t map_calls_pre = g.map_w->call_count;
    uint64_t map_short_pre = ms->modal_short_circuits;
    int  click_x_pre = ms->last_click_x;
    int  click_y_pre = ms->last_click_y;

    g.modal_lock = true;
    mk_click(&ev, 400, 200);     /* 在 map 內 */
    civ_dispatch_event(&g, &ev);

    /* dispatch 仍進去（call_count 仍 ++），但 short-circuit 而非
     * 真的執行 on_mouse_down → last_click_x/y 沒變、modal_short_circuits++ */
    EXPECT(g.map_w->call_count == map_calls_pre + 1);
    EXPECT(ms->modal_short_circuits == map_short_pre + 1);
    EXPECT(ms->last_click_x == click_x_pre);
    EXPECT(ms->last_click_y == click_y_pre);

    /* ── 測 4：modal_lock 解除後 click 正常處理 ────────── */
    g.modal_lock = false;
    mk_click(&ev, 450, 250);
    civ_dispatch_event(&g, &ev);
    EXPECT(ms->last_click_x == 450);
    EXPECT(ms->last_click_y == 250);

    /* ── 測 5：座標越界（落在 widget 之外）→ 無 widget 被呼叫 ── */
    /* R4 layout: 唯一沒覆蓋的區域是 chrome (title + menu, y < 32)。 */
    uint64_t all_calls_pre = g.map_w->call_count +
                             g.minimap_w->call_count +
                             g.status_w->call_count;
    mk_motion(&ev, 300, 20);    /* menu bar 內 (y=20 < 32) */
    civ_dispatch_event(&g, &ev);
    uint64_t all_calls_post = g.map_w->call_count +
                              g.minimap_w->call_count +
                              g.status_w->call_count;
    EXPECT(all_calls_post == all_calls_pre);

    civ_widgets_unregister(&g);
    SDL_Quit();
    printf("PASS test_widget_dispatch (5 sub-tests)\n");
    return 0;
}
