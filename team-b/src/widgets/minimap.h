/*
 * widgets/minimap.h — 小地圖 widget
 *
 * 對應 spec 02 §2.2.7 WDWSMMAPPROC：9-entry dispatch table、無鎖定
 * 模式。spec 02 已確認與 WDWSTATUSPROC 兩個 callback 結構完全相同
 * （77 byte body），同模板複製 + 不同 table。
 */
#ifndef CIV_WIDGETS_MINIMAP_H
#define CIV_WIDGETS_MINIMAP_H

#include "widget.h"

typedef struct civ_minimap_state {
    int  view_x, view_y;     /* 主地圖目前 view 中心對應到小地圖內的點 */
    bool has_view;
} civ_minimap_state_t;

civ_widget_t *civ_minimap_widget_new(struct civ_game *g, civ_rect_t rect);

#endif /* CIV_WIDGETS_MINIMAP_H */
