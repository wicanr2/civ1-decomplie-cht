/*
 * widgets/map.h — 主地圖 widget
 *
 * 對應 spec 02 §2.2.6 WDWMAPPROC：
 *   - 22-entry dispatch table（M2 階段用 stub handlers 佔位）
 *   - 「鎖定模式」(原 DAT_12b0_0000)：開啟時除 RESIZE / CLOSE 外
 *     都當 DefWindowProc（no-op）
 *   - 每次呼叫 call_count++（原 DAT_1420_4860 metric）
 */
#ifndef CIV_WIDGETS_MAP_H
#define CIV_WIDGETS_MAP_H

#include "widget.h"

typedef struct civ_map_state {
    /* 鎖定模式 short-circuit 次數（除錯 / test 用）。對應 spec 02
     * §2.2.6 鎖定模式分支。 */
    uint64_t modal_short_circuits;

    /* 最近一次的滑鼠座標（hover 視覺反饋用） */
    int last_mouse_x, last_mouse_y;
    bool has_mouse;

    /* 最近一次點擊（demo 用） */
    int  last_click_x, last_click_y;
    bool has_click;
} civ_map_state_t;

civ_widget_t *civ_map_widget_new(struct civ_game *g, civ_rect_t rect);

#endif /* CIV_WIDGETS_MAP_H */
