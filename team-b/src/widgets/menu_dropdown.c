/*
 * widgets/menu_dropdown.c — 8 menu × items dropdown render
 *
 * Layout 對齊 1993 reference webp:
 *   menu bar item: y=16..32, x 從 left 累加 + 16px spacing
 *   active 反白: 深藍底 + 白字
 *   dropdown: 灰底 + 黑邊框, 每行 16 px 高, 內含 8px L/R padding
 *   active item: 深藍底 + 白字
 */
#include "menu_dropdown.h"
#include "../civ_game.h"
#include "../gfx/palette.h"
#include "../gfx/primitive.h"
#include "../text/text_out.h"

#include <string.h>

#define TITLE_H 16
#define MENU_H  16
#define DD_ITEM_H 18
#define DD_PAD_X 12
#define DD_PAD_Y 4

/* 8 menu 名稱 (對齊 R17 test_world_snapshot menu bar). */
static const char *MENU_NAMES[CIV_MENU_COUNT] = {
    "檔案", "編輯", "命令", "顧問",
    "世界", "百科", "城市", "說明",
};

/* 各 menu 的 items. 對齊 1993 原版 Civilopedia/Help dropdown reference,
 * 其餘 menu 按 manual + spec 04 §dialog_chain 推測. v0.1 範圍.
 *
 * 順序對齊 reference: 檔案/編輯/命令/顧問/世界/百科/城市/說明
 *
 * NULL = 該 slot 無 item.  */
static const char *MENU_ITEMS[CIV_MENU_COUNT][CIV_MENU_MAX_ITEMS] = {
    /* 檔案 (File): 開新局/載入/存檔/退出 */
    { "開新局", "載入舊檔", "存檔", "結束遊戲", NULL, NULL, NULL, NULL },
    /* 編輯 (Edit): Civ1 沒實作太多, 用 v0.1 placeholder */
    { "復原", "重做", NULL, NULL, NULL, NULL, NULL, NULL },
    /* 命令 (Orders): 對齊 manual P15 unit orders */
    { "建造城市", "建造道路", "灌溉", "礦山", "築堡", "睡眠", "等候", "解除任務" },
    /* 顧問 (Advisors): 對齊 manual P36 5 顧問 */
    { "城市報告", "軍事報告", "外交報告", "貿易報告", "科技進度", NULL, NULL, NULL },
    /* 世界 (World): 對齊 manual P19 World menu */
    { "縮放地圖", "顯示版圖", "切換 grid", "找城市", NULL, NULL, NULL, NULL },
    /* 百科 (Civilopedia): reference 確認 6 items */
    { "完整", "文明進展", "城市建設", "軍事單位", "地形類型", "其他", NULL, NULL },
    /* 城市 (City): manual P29 城市選單 */
    { "城市畫面", "改變生產", "重新命名", NULL, NULL, NULL, NULL, NULL },
    /* 說明 (Help): reference 確認 */
    { "主題", "搜尋", "使用 Help", NULL, "關於文明帝國", NULL, NULL, NULL },
};

const char *civ_menu_name_zh(int menu_idx)
{
    if (menu_idx < 0 || menu_idx >= CIV_MENU_COUNT) return "";
    return MENU_NAMES[menu_idx];
}

int civ_menu_item_count(int menu_idx)
{
    if (menu_idx < 0 || menu_idx >= CIV_MENU_COUNT) return 0;
    int n = 0;
    for (int i = 0; i < CIV_MENU_MAX_ITEMS; i++) {
        if (MENU_ITEMS[menu_idx][i]) n++;
        else break;   /* 第一個 NULL 之後不算 (即使後面又有) */
    }
    return n;
}

const char *civ_menu_item_zh(int menu_idx, int item_idx)
{
    if (menu_idx < 0 || menu_idx >= CIV_MENU_COUNT) return NULL;
    if (item_idx < 0 || item_idx >= CIV_MENU_MAX_ITEMS) return NULL;
    return MENU_ITEMS[menu_idx][item_idx];
}

int civ_menu_bar_x(struct civ_game *g, int menu_idx)
{
    if (menu_idx < 0 || menu_idx >= CIV_MENU_COUNT) return 0;
    int x = 8;
    if (!g || !g->font_body) return x;
    for (int i = 0; i < menu_idx; i++) {
        x += civ_text_measure(g->font_body, MENU_NAMES[i]) + 16;
    }
    return x;
}

void civ_menu_bar_highlight(civ_surface_t *fb, struct civ_game *g,
                             int menu_idx)
{
    if (!fb || !g || !g->font_body) return;
    if (menu_idx < 0 || menu_idx >= CIV_MENU_COUNT) return;

    int x = civ_menu_bar_x(g, menu_idx);
    int w = civ_text_measure(g->font_body, MENU_NAMES[menu_idx]) + 8;

    uint8_t c_hi_bg = civ_palette_nearest_rgb(&g->palette, 0x00, 0x00, 0x80);
    uint8_t c_hi_fg = civ_palette_nearest_rgb(&g->palette, 0xFF, 0xFF, 0xFF);

    civ_fill_rect(fb, (civ_rect_t){x - 4, TITLE_H, w, MENU_H}, c_hi_bg);
    civ_text_out(fb, g->font_body, x, TITLE_H + MENU_H - 3,
                 MENU_NAMES[menu_idx], c_hi_fg, c_hi_bg,
                 CIV_TEXT_BK_TRANSPARENT);
}

void civ_menu_dropdown_render(civ_surface_t *fb, struct civ_game *g,
                               int menu_idx, int cursor_item,
                               int x_left, int y_top)
{
    if (!fb || !g || !g->font_body) return;
    int n = civ_menu_item_count(menu_idx);
    if (n <= 0) return;

    /* 算最寬 item 文字 + padding 當 dropdown 寬度 */
    int max_w = 0;
    for (int i = 0; i < n; i++) {
        const char *t = civ_menu_item_zh(menu_idx, i);
        if (!t) continue;
        int w = civ_text_measure(g->font_body, t);
        if (w > max_w) max_w = w;
    }
    int dd_w = max_w + DD_PAD_X * 2;
    int dd_h = n * DD_ITEM_H + DD_PAD_Y * 2;

    /* 確保不超出 fb 右邊 */
    if (x_left + dd_w > fb->w) x_left = fb->w - dd_w - 2;
    if (x_left < 0) x_left = 0;

    uint8_t c_bg     = civ_palette_nearest_rgb(&g->palette, 0xC0, 0xC0, 0xC0);
    uint8_t c_border = civ_palette_nearest_rgb(&g->palette, 0x00, 0x00, 0x00);
    uint8_t c_fg     = c_border;
    uint8_t c_hi_bg  = civ_palette_nearest_rgb(&g->palette, 0x00, 0x00, 0x80);
    uint8_t c_hi_fg  = civ_palette_nearest_rgb(&g->palette, 0xFF, 0xFF, 0xFF);

    /* dropbox 灰底 + 黑邊框 */
    civ_fill_rect(fb, (civ_rect_t){x_left, y_top, dd_w, dd_h}, c_bg);
    civ_frame_rect(fb, (civ_rect_t){x_left, y_top, dd_w, dd_h}, c_border);

    /* 渲染 items */
    for (int i = 0; i < n; i++) {
        int item_y = y_top + DD_PAD_Y + i * DD_ITEM_H;
        bool is_cursor = (i == cursor_item);
        if (is_cursor) {
            civ_fill_rect(fb,
                (civ_rect_t){x_left + 1, item_y, dd_w - 2, DD_ITEM_H},
                c_hi_bg);
        }
        const char *t = civ_menu_item_zh(menu_idx, i);
        if (t) {
            civ_text_out(fb, g->font_body,
                         x_left + DD_PAD_X, item_y + DD_ITEM_H - 4,
                         t,
                         is_cursor ? c_hi_fg : c_fg,
                         is_cursor ? c_hi_bg : c_bg,
                         CIV_TEXT_BK_TRANSPARENT);
        }
    }
}
