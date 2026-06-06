/*
 * widgets/tech_screen.h — R16 M-techscreen modal UI
 *
 * 對齊 docs/TECH_DISCOVERY_SCREEN.md + reference 截圖
 *   docs/screenshots/reference/civ1_win_tech_discovery.png
 *
 * Layout:
 *   雙層花邊 frame (clean-room 自畫 pattern)
 *   左上 96×96 tech illustration 占位
 *   中上 大標題 (e.g. "青銅器")
 *   subtitle "文明進展"
 *   "(取自法國)" / "(自家研發)" 灰字
 *   Allows: 紫字 + 4 種 prereq DAG 反推:
 *     藍 後續可研發 tech / 紅 unit / 綠 wonder / 綠 improvement
 *
 * 開啟條件: civ_game.tech_screen_open = true. modal_lock 同步.
 * 觸發: R16 用 'T' 鍵 demo 觸發 BRONZE WORKING; 真 turn loop 整合留 R17.
 * 關閉: ESC / Enter.
 */
#ifndef CIV_WIDGETS_TECH_SCREEN_H
#define CIV_WIDGETS_TECH_SCREEN_H

#include "../gfx/surface.h"

struct civ_game;

void civ_tech_screen_render(struct civ_game *g, civ_surface_t *fb);

#endif /* CIV_WIDGETS_TECH_SCREEN_H */
