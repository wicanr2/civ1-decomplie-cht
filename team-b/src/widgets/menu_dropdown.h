/*
 * widgets/menu_dropdown.h — R29 (C1 gap fix) 主畫面 menu dropdown 占位
 *
 * 1993 原版 menu bar 8 items 點下後展開 dropdown 子選單.
 * 對齊 docs/screenshots/gametest_2026-06-07/orig/civ1_win_civilopedia_dropdown.png
 * 視覺: 灰底 (#C0C0C0) + 黑邊框 + 黑字; active 項目 = 深藍底白字.
 *
 * R29 v0.1 範圍:
 * - 8 menu × items list 全 zh-TW
 * - 純 render (modal_lock + 鍵盤 navigation 留 R30+)
 * - test_world_snapshot 加 menu-N 模式 demo
 */
#ifndef CIV_WIDGETS_MENU_DROPDOWN_H
#define CIV_WIDGETS_MENU_DROPDOWN_H

#include "../gfx/surface.h"

struct civ_game;

#define CIV_MENU_COUNT 8        /* 檔案/編輯/命令/顧問/世界/百科/城市/說明 */
#define CIV_MENU_MAX_ITEMS 8    /* 最多一個 menu 8 個 item (Civilopedia=6 是最寬) */

/* 取 menu_idx (0..7) 的中文名稱. */
const char *civ_menu_name_zh(int menu_idx);

/* 取 menu_idx 的 item 個數 (0 = 該 menu 沒實作或空). */
int civ_menu_item_count(int menu_idx);

/* 取 menu_idx 第 item_idx 個 item 的中文文字 (NULL = 越界). */
const char *civ_menu_item_zh(int menu_idx, int item_idx);

/* 算 menu bar 內 menu_idx 的 x 起始座標 (相對 fb).
 *   font 需要量字寬. menu_x_left = 8 + sum(prev menus + spacing 16). */
int civ_menu_bar_x(struct civ_game *g, int menu_idx);

/* 在 menu bar 上畫 menu_idx 的「active highlight」(深藍底白字),
 * 跑在 paint_background menu bar 之後. y / h 同 menu bar.  */
void civ_menu_bar_highlight(civ_surface_t *fb, struct civ_game *g,
                             int menu_idx);

/* 在 menu bar 下方畫 dropdown box (menu_idx 的所有 items).
 *   cursor_item = 反白項 (-1 = 都不反白)
 *   x_left = menu bar item 左邊 (用 civ_menu_bar_x())
 *   y_top  = menu bar 底 (= 32 = TITLE_H + MENU_H) */
void civ_menu_dropdown_render(civ_surface_t *fb, struct civ_game *g,
                               int menu_idx, int cursor_item,
                               int x_left, int y_top);

#endif /* CIV_WIDGETS_MENU_DROPDOWN_H */
