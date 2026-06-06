/*
 * widgets/diplomat_screen.h — R18 M-diplomat 外交訪問畫面
 *
 * 對齊 reference: civ1_diplomat_elizabeth_pc29.webp / civ1_diplomat_frederick_pc29.webp
 *
 * Layout (640×480):
 *   y 0..360  上半: 灰藍 sky + jagged mountain horizon + 中央領袖大頭像 +
 *                   左右兩側 advisor 占位
 *   y 360..480 下半: 左 spear (40 px 黑底紅斜紋) + 中 parchment 棕底
 *                    (對話文字 暗紅 serif) + 右 spear (40 px)
 *
 * 開啟條件: civ_game.diplomat_screen_open = true. modal_lock 同步.
 * 觸發: R18 用鍵盤 'D' demo trigger Elizabeth GREETING; 真 turn 整合留後.
 * 關閉: ESC / Enter.
 */
#ifndef CIV_WIDGETS_DIPLOMAT_SCREEN_H
#define CIV_WIDGETS_DIPLOMAT_SCREEN_H

#include "../gfx/surface.h"

struct civ_game;

void civ_diplomat_screen_render(struct civ_game *g, civ_surface_t *fb);

#endif /* CIV_WIDGETS_DIPLOMAT_SCREEN_H */
