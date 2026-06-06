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

    /* R4+R10: Win16 子視窗 chrome 用 nearest 解 sheet palette idx 對不上 */
    enum { SUB_TITLE_H = 12 };
    uint8_t c_t_bg = w->game ? civ_palette_nearest_rgb(&w->game->palette, 0x00,0x00,0x80) : 1;
    uint8_t c_t_fg = w->game ? civ_palette_nearest_rgb(&w->game->palette, 0xFF,0xFF,0xFF) : 15;
    civ_fill_rect(fb, (civ_rect_t){w->rect.x, w->rect.y, w->rect.w, SUB_TITLE_H}, c_t_bg);
    if (w->game && w->game->font_body) {
        const char *title = "Status";
        int tw = civ_text_measure(w->game->font_body, title);
        int tx = w->rect.x + (w->rect.w - tw) / 2;
        civ_text_out(fb, w->game->font_body, tx, w->rect.y + SUB_TITLE_H - 3,
                     title, c_t_fg, c_t_bg, CIV_TEXT_BK_TRANSPARENT);
    }
    civ_rect_t inner_rect = { w->rect.x, w->rect.y + SUB_TITLE_H,
                              w->rect.w, w->rect.h - SUB_TITLE_H };

    /* 灰底 (用 inner_rect 不擋 title bar) */
    civ_fill_rect(fb, inner_rect, 7);
    civ_frame_rect(fb, inner_rect, 8);

    if (!w->game) return;
    civ_font_t *font = w->game->font_body;
    if (!font) return;

    char buf[64];
    int x = inner_rect.x + 6;
    int y = inner_rect.y + 12;

    /* 1. 西元年 */
    int year = w->game->civ_year;
    if (year < 0)
        snprintf(buf, sizeof buf, "%d BC", -year);
    else
        snprintf(buf, sizeof buf, "AD %d", year);
    civ_text_out(fb, font, x, y, buf, 14, 7, CIV_TEXT_BK_TRANSPARENT);
    y += 18;

    /* 2. 金庫 (placeholder $0, M7 接 player.treasury) */
    snprintf(buf, sizeof buf, "Gold: $0");
    civ_text_out(fb, font, x, y, buf, 1, 7, CIV_TEXT_BK_TRANSPARENT);
    y += 16;

    /* 3. 稅率三色條 - 60% Tax / 20% Lux / 20% Sci 預設 (placeholder) */
    civ_text_out(fb, font, x, y, "Rate:", 0, 7, CIV_TEXT_BK_TRANSPARENT);
    y += 14;
    int bar_x = x, bar_y = y, bar_w = w->rect.w - 12, bar_h = 8;
    int tax_pct = 60, lux_pct = 20, sci_pct = 20;
    int tax_w = bar_w * tax_pct / 100;
    int lux_w = bar_w * lux_pct / 100;
    int sci_w = bar_w - tax_w - lux_w;
    civ_fill_rect(fb, (civ_rect_t){bar_x,                bar_y, tax_w, bar_h}, 12); /* tax 紅 */
    civ_fill_rect(fb, (civ_rect_t){bar_x + tax_w,        bar_y, lux_w, bar_h}, 14); /* lux 黃 */
    civ_fill_rect(fb, (civ_rect_t){bar_x + tax_w + lux_w, bar_y, sci_w, bar_h}, 9); /* sci 藍 */
    civ_frame_rect(fb, (civ_rect_t){bar_x, bar_y, bar_w, bar_h}, 0);
    y += bar_h + 4;
    snprintf(buf, sizeof buf, "T%d L%d S%d", tax_pct, lux_pct, sci_pct);
    civ_text_out(fb, font, x, y, buf, 0, 7, CIV_TEXT_BK_TRANSPARENT);
    y += 16;

    /* 4. 政府型態 (placeholder, M7 接 player.government) */
    civ_text_out(fb, font, x, y, "Despotism", 1, 7, CIV_TEXT_BK_TRANSPARENT);
    y += 18;

    /* 5. 分隔線 */
    civ_hline(fb, x, y, bar_w, 8);
    y += 4;

    /* 6. 選中單位 panel — M6-full-lite hook 進 world.selected_unit */
    civ_text_out(fb, font, x, y, "Unit:", 0, 7, CIV_TEXT_BK_TRANSPARENT);
    y += 14;
    if (w->game->world_ready && w->game->world.selected_unit >= 0 &&
        w->game->world.selected_unit < w->game->world.units_count) {
        const civ_unit_t *u = &w->game->world.units[w->game->world.selected_unit];
        if (u->alive) {
            const char *uname = civ_unit_name_zh(u->type);
            civ_text_out(fb, font, x, y, uname, 14, 7, CIV_TEXT_BK_TRANSPARENT);
            y += 14;
            int atk, def, moves;
            civ_unit_stats(u->type, &atk, &def, &moves);
            snprintf(buf, sizeof buf, "A%d D%d M%d/%d", atk, def,
                     u->moves_left, moves);
            civ_text_out(fb, font, x, y, buf, 0, 7, CIV_TEXT_BK_TRANSPARENT);
            y += 14;
            snprintf(buf, sizeof buf, "(%d,%d) HP %d", u->x, u->y, u->hp);
            civ_text_out(fb, font, x, y, buf, 0, 7, CIV_TEXT_BK_TRANSPARENT);
            y += 14;
        } else {
            civ_text_out(fb, font, x, y, "(陣亡)", 12, 7, CIV_TEXT_BK_TRANSPARENT);
            y += 14;
        }
    } else {
        civ_text_out(fb, font, x, y, "(無)", 8, 7, CIV_TEXT_BK_TRANSPARENT);
        y += 14;
    }

    /* 7. 最近戰鬥訊息 */
    if (w->game->world_ready && w->game->world.last_combat_msg[0]) {
        civ_text_out(fb, font, x, y, w->game->world.last_combat_msg,
                     12, 7, CIV_TEXT_BK_TRANSPARENT);
        y += 14;
    }

    /* 7b. R5 M6-full: cursor 上 city info */
    if (w->game->world_ready) {
        int cidx = civ_world_city_at(&w->game->world,
                                     w->game->world.cursor_x,
                                     w->game->world.cursor_y);
        if (cidx >= 0) {
            const civ_city_t *c = &w->game->world.cities[cidx];
            civ_text_out(fb, font, x, y, "城市:", 0, 7, CIV_TEXT_BK_TRANSPARENT);
            y += 12;
            civ_text_out(fb, font, x, y, c->name, 14, 7, CIV_TEXT_BK_TRANSPARENT);
            y += 12;
            snprintf(buf, sizeof buf, "人口: %d", c->population);
            civ_text_out(fb, font, x, y, buf, 0, 7, CIV_TEXT_BK_TRANSPARENT);
            y += 12;
            if (c->building_target >= 0) {
                snprintf(buf, sizeof buf, "正在建造: %s",
                         civ_building_name_zh(c->building_target));
                civ_text_out(fb, font, x, y, buf, 0, 7, CIV_TEXT_BK_TRANSPARENT);
                y += 12;
                snprintf(buf, sizeof buf, "  %d/%d 盾",
                         c->shield_stock,
                         civ_building_cost(c->building_target));
                civ_text_out(fb, font, x, y, buf, 0, 7, CIV_TEXT_BK_TRANSPARENT);
                y += 12;
            }
        }
    }

    /* 8. 回合計數 (底部, RE 期保留) — 用 inner_rect 算 (w->rect 含 title bar) */
    int y_bottom = inner_rect.y + inner_rect.h - 30;
    snprintf(buf, sizeof buf, "Turn %u  Tick %llu",
             (unsigned)w->game->turn_number,
             (unsigned long long)w->game->tick_count);
    civ_text_out(fb, font, x, y_bottom, buf, 8, 7, CIV_TEXT_BK_TRANSPARENT);

    snprintf(buf, sizeof buf, "AI moves: %llu",
             (unsigned long long)w->game->ai_actions);
    civ_text_out(fb, font, x, y_bottom + 14, buf, 8, 7, CIV_TEXT_BK_TRANSPARENT);
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
