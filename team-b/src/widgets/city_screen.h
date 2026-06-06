/*
 * widgets/city_screen.h — R6 M7-full city screen 模態 UI
 *
 * 對齊 docs/screenshots/reference/civ1_win_city_screen.png:
 *   標題 PARIS (POP: 10,000)         + PALACE icon @ 右上
 *   CITY RESOURCES panel @ 左上
 *   center small map @ 中上
 *   FOOD STORAGE @ 左下
 *   INFO / HAPPY / MAP / VIEW 4 tab @ 中下
 *   CHANGE / BUY 按鈕 + 單位 icon @ 右下
 *   EXIT 按鈕 @ 右下角
 *
 * 開啟條件: civ_game.city_screen_open = true. modal_lock 同步.
 * 觸發: cursor 上 city 時按 Enter (見 widgets/map.c).
 * 關閉: ESC.
 */
#ifndef CIV_WIDGETS_CITY_SCREEN_H
#define CIV_WIDGETS_CITY_SCREEN_H

#include "../gfx/surface.h"

struct civ_game;

/* 若 game->city_screen_open 開啟, 在 fb 上 render 整版 city screen
 * 覆蓋所有 widget. 否則 no-op. */
void civ_city_screen_render(struct civ_game *g, civ_surface_t *fb);

#endif /* CIV_WIDGETS_CITY_SCREEN_H */
