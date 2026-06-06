/*
 * widgets/status.h — 狀態 / 資訊面板 widget
 *
 * 對應 spec 02 §2.2.8 WDWSTATUSPROC：9-entry dispatch、無鎖定模式。
 * 與 WDWSMMAPPROC 共用同模板。
 */
#ifndef CIV_WIDGETS_STATUS_H
#define CIV_WIDGETS_STATUS_H

#include "widget.h"

typedef struct civ_status_state {
    /* M2 stub：把全域 tick count 顯示在面板上 */
    uint64_t last_tick_shown;
} civ_status_state_t;

civ_widget_t *civ_status_widget_new(struct civ_game *g, civ_rect_t rect);

#endif /* CIV_WIDGETS_STATUS_H */
