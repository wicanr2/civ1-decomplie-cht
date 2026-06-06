#include "status.h"
#include "../civ_game.h"
#include "../gfx/primitive.h"
#include "../text/text_out.h"

#include <stdio.h>
#include <stdlib.h>

static const civ_widget_msg_entry_t STATUS_TABLE[] = {
    /* M2：暫無 handler — paint-only widget */
    { 0, NULL },
};

static civ_evt_result_t status_dispatch(civ_widget_t *w, SDL_Event *ev)
{
    (void)ev;
    w->call_count++;
    return 0;   /* paint-only，所有 event 都吃下不擋 */
}

static void status_render(civ_widget_t *w, civ_surface_t *fb)
{
    /* C-B-3 改 (2026-06-06 第二輪):
     *
     * 原版 1993 Civ1 status panel 顯示玩家當前 game state - 西元年 / 金庫
     * / 稅率三色條 (Tax/Lux/Sci) / 政府型態 / 選中單位 panel. 之前用 M0-M7
     * milestone 進度當占位是 RE 期 debug; 改成原版欄位以對位視覺.
     * 真實的 player state hook (金庫, 稅率, 政府) 等 M6-full / M7 才會接.
     */

    /* R17: status 重整 — 對齊使用者指正 (文字排版跑掉/雙層雙背景).
     *
     * Reference layout (civ1_win_civilopedia_dropdown.png 左下 Status panel):
     *   ┌────────────────┐
     *   │   Status (青)  │  <- title bar
     *   ├────────────────┤
     *   │ [青底矩形圖]   │  <- 上半: 城市人口/年代/金庫 (Cyan box)
     *   │ 40,000人       │
     *   │ 3420 BC        │
     *   │ 25@ 0.5.5      │
     *   ├────────────────┤
     *   │ Indian (灰底)  │  <- 下半: 玩家/單位 info
     *   │ Militia        │
     *   │ Moves: 1       │
     *   │ Delhi          │
     *   │ (Grassland)    │
     *   └────────────────┘
     *
     * 我們的版本中文化:
     *   標題: 狀態  ;  上半青底: 人口/年代/金庫;  下半灰底: 單位資訊/城市
     */
    if (!w->game) return;
    civ_font_t *font = w->game->font_body;
    if (!font) return;

    civ_palette_t *pal = &w->game->palette;
    uint8_t c_title_bg = civ_palette_nearest_rgb(pal, 0x00, 0x00, 0x80);
    uint8_t c_title_fg = civ_palette_nearest_rgb(pal, 0xFF, 0xFF, 0xFF);
    uint8_t c_top_bg   = civ_palette_nearest_rgb(pal, 0x40, 0xB0, 0xC0);  /* 青 */
    uint8_t c_body_bg  = civ_palette_nearest_rgb(pal, 0xC0, 0xC0, 0xC0);  /* Win16 灰 */
    uint8_t c_border   = civ_palette_nearest_rgb(pal, 0x40, 0x40, 0x40);
    uint8_t c_black    = civ_palette_nearest_rgb(pal, 0, 0, 0);
    uint8_t c_yellow   = civ_palette_nearest_rgb(pal, 0xC0, 0x80, 0);
    uint8_t c_blue     = civ_palette_nearest_rgb(pal, 0, 0, 0x80);
    uint8_t c_red      = civ_palette_nearest_rgb(pal, 0xC0, 0, 0);
    uint8_t c_lux      = civ_palette_nearest_rgb(pal, 0xE0, 0xE0, 0);
    uint8_t c_sci      = civ_palette_nearest_rgb(pal, 0, 0x80, 0xC0);
    uint8_t c_grey_txt = civ_palette_nearest_rgb(pal, 0x40, 0x40, 0x40);

    enum { SUB_TITLE_H = 12 };
    civ_fill_rect(fb, (civ_rect_t){w->rect.x, w->rect.y, w->rect.w, SUB_TITLE_H},
                  c_title_bg);
    {
        const char *title = "狀態";
        int tw = civ_text_measure(font, title);
        int tx = w->rect.x + (w->rect.w - tw) / 2;
        civ_text_out(fb, font, tx, w->rect.y + SUB_TITLE_H - 2, title,
                     c_title_fg, c_title_bg, CIV_TEXT_BK_TRANSPARENT);
    }

    /* === 上半青底: 人口 / 年代 / 金庫 / 稅率 / 政府 ===
     * top_h 預留 上半總高 (含小頭像 + 4 行文字) */
    int top_y = w->rect.y + SUB_TITLE_H;
    int top_h = 110;
    civ_fill_rect(fb, (civ_rect_t){w->rect.x, top_y, w->rect.w, top_h}, c_top_bg);
    civ_frame_rect(fb, (civ_rect_t){w->rect.x, top_y, w->rect.w, top_h}, c_border);

    int x = w->rect.x + 8;
    int y = top_y + 16;
    char buf[64];

    /* 1. 年代 (Civ1 字串 e.g. "3420 BC") */
    int year = w->game->civ_year;
    if (year < 0) snprintf(buf, sizeof buf, "西元前 %d 年", -year);
    else          snprintf(buf, sizeof buf, "西元 %d 年", year);
    civ_text_out(fb, font, x, y, buf, c_black, c_top_bg, CIV_TEXT_BK_TRANSPARENT);
    y += 20;

    /* 2. 金庫 (placeholder) */
    snprintf(buf, sizeof buf, "金庫: $0");
    civ_text_out(fb, font, x, y, buf, c_black, c_top_bg, CIV_TEXT_BK_TRANSPARENT);
    y += 20;

    /* 3. 稅率三色 bar — bar 上方 留 14 px 給 label 文字 */
    int tax_pct = 60, lux_pct = 20, sci_pct = 20;
    snprintf(buf, sizeof buf, "稅%d 樂%d 科%d", tax_pct, lux_pct, sci_pct);
    civ_text_out(fb, font, x, y, buf, c_black, c_top_bg, CIV_TEXT_BK_TRANSPARENT);
    y += 14;
    int bar_x = x, bar_y = y, bar_w = w->rect.w - 16, bar_h = 8;
    int tax_w = bar_w * tax_pct / 100;
    int lux_w = bar_w * lux_pct / 100;
    int sci_w = bar_w - tax_w - lux_w;
    civ_fill_rect(fb, (civ_rect_t){bar_x,                bar_y, tax_w, bar_h}, c_red);
    civ_fill_rect(fb, (civ_rect_t){bar_x + tax_w,        bar_y, lux_w, bar_h}, c_lux);
    civ_fill_rect(fb, (civ_rect_t){bar_x + tax_w + lux_w, bar_y, sci_w, bar_h}, c_sci);
    civ_frame_rect(fb, (civ_rect_t){bar_x, bar_y, bar_w, bar_h}, c_black);
    y += bar_h + 6;

    /* 4. 政府 */
    civ_text_out(fb, font, x, y, "君主制", c_blue, c_top_bg, CIV_TEXT_BK_TRANSPARENT);

    /* === 下半灰底: 單位 / 城市 info === */
    int bot_y = top_y + top_h;
    int bot_h = w->rect.h - SUB_TITLE_H - top_h;
    civ_fill_rect(fb, (civ_rect_t){w->rect.x, bot_y, w->rect.w, bot_h}, c_body_bg);
    civ_frame_rect(fb, (civ_rect_t){w->rect.x, bot_y, w->rect.w, bot_h}, c_border);

    int yb = bot_y + 14;

    /* 5. 選中單位 */
    civ_text_out(fb, font, x, yb, "選定單位:", c_black, c_body_bg, CIV_TEXT_BK_TRANSPARENT);
    yb += 18;
    if (w->game->world_ready && w->game->world.selected_unit >= 0 &&
        w->game->world.selected_unit < w->game->world.units_count) {
        const civ_unit_t *u = &w->game->world.units[w->game->world.selected_unit];
        if (u->alive) {
            const char *uname = civ_unit_name_zh(u->type);
            civ_text_out(fb, font, x + 6, yb, uname, c_yellow, c_body_bg,
                         CIV_TEXT_BK_TRANSPARENT);
            yb += 18;
            int atk, def, moves;
            civ_unit_stats(u->type, &atk, &def, &moves);
            snprintf(buf, sizeof buf, "攻%d 防%d", atk, def);
            civ_text_out(fb, font, x + 6, yb, buf, c_black, c_body_bg,
                         CIV_TEXT_BK_TRANSPARENT);
            yb += 16;
            snprintf(buf, sizeof buf, "移動 %d/%d", u->moves_left, moves);
            civ_text_out(fb, font, x + 6, yb, buf, c_black, c_body_bg,
                         CIV_TEXT_BK_TRANSPARENT);
            yb += 16;
            snprintf(buf, sizeof buf, "位置 (%d,%d)", u->x, u->y);
            civ_text_out(fb, font, x + 6, yb, buf, c_black, c_body_bg,
                         CIV_TEXT_BK_TRANSPARENT);
            yb += 16;
            snprintf(buf, sizeof buf, "生命 %d", u->hp);
            civ_text_out(fb, font, x + 6, yb, buf, c_black, c_body_bg,
                         CIV_TEXT_BK_TRANSPARENT);
            yb += 18;
        } else {
            civ_text_out(fb, font, x + 6, yb, "(已陣亡)", c_red, c_body_bg,
                         CIV_TEXT_BK_TRANSPARENT);
            yb += 18;
        }
    } else {
        civ_text_out(fb, font, x + 6, yb, "(未選定)", c_grey_txt, c_body_bg,
                     CIV_TEXT_BK_TRANSPARENT);
        yb += 18;
    }

    /* 6. cursor 上 city */
    if (w->game->world_ready) {
        int cidx = civ_world_city_at(&w->game->world,
                                     w->game->world.cursor_x,
                                     w->game->world.cursor_y);
        if (cidx >= 0) {
            const civ_city_t *c = &w->game->world.cities[cidx];
            civ_text_out(fb, font, x, yb, "游標城市:", c_black, c_body_bg,
                         CIV_TEXT_BK_TRANSPARENT);
            yb += 18;
            civ_text_out(fb, font, x + 6, yb, c->name, c_yellow, c_body_bg,
                         CIV_TEXT_BK_TRANSPARENT);
            yb += 16;
            snprintf(buf, sizeof buf, "人口 %d 萬", c->population);
            civ_text_out(fb, font, x + 6, yb, buf, c_black, c_body_bg,
                         CIV_TEXT_BK_TRANSPARENT);
            yb += 16;
            if (c->building_target >= 0) {
                snprintf(buf, sizeof buf, "建造: %s",
                         civ_building_name_zh(c->building_target));
                civ_text_out(fb, font, x + 6, yb, buf, c_black, c_body_bg,
                             CIV_TEXT_BK_TRANSPARENT);
                yb += 16;
            }
        }
    }

    /* 7. 戰鬥訊息 */
    if (w->game->world_ready && w->game->world.last_combat_msg[0]) {
        civ_text_out(fb, font, x, yb, w->game->world.last_combat_msg,
                     c_red, c_body_bg, CIV_TEXT_BK_TRANSPARENT);
    }

    /* 8. 底部回合計數 (debug) */
    int y_bottom = w->rect.y + w->rect.h - 28;
    snprintf(buf, sizeof buf, "回合 %u", (unsigned)w->game->turn_number);
    civ_text_out(fb, font, x, y_bottom, buf, c_grey_txt, c_body_bg,
                 CIV_TEXT_BK_TRANSPARENT);
    snprintf(buf, sizeof buf, "AI %llu",
             (unsigned long long)w->game->ai_actions);
    civ_text_out(fb, font, x, y_bottom + 14, buf, c_grey_txt, c_body_bg,
                 CIV_TEXT_BK_TRANSPARENT);
}

static void status_destroy(civ_widget_t *w)
{
    free(w->state);
    free(w);
}

static const civ_widget_vtable_t STATUS_VTABLE = {
    .name     = "status",
    .dispatch = status_dispatch,
    .render   = status_render,
    .destroy  = status_destroy,
};

civ_widget_t *civ_status_widget_new(struct civ_game *g, civ_rect_t rect)
{
    civ_widget_t *w = calloc(1, sizeof *w);
    if (!w) return NULL;
    w->vt      = &STATUS_VTABLE;
    w->rect    = rect;
    w->visible = true;
    w->game    = g;
    w->state   = calloc(1, sizeof(civ_status_state_t));
    if (!w->state) { free(w); return NULL; }
    (void)STATUS_TABLE;   /* 暫無 handler，silence warning */
    return w;
}
