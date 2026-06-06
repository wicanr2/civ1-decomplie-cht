#include "city_screen.h"
#include "../civ_game.h"
#include "../gfx/primitive.h"
#include "../text/text_out.h"
#include "../world/city.h"
#include "../world/world.h"
#include "../world/sprite_sheet.h"

#include <stdio.h>
#include <string.h>

/* 對齊 reference/civ1_win_city_screen.png. 全螢幕 modal.
 *
 * Layout (640x480):
 *   標題列      y 0..30   ── "PARIS (POP: ...)" 居中 + 邊角 icon
 *   主區        y 30..380 ── 三大 panel:
 *                              左上 CITY RESOURCES   x 0..200
 *                              中上 small map        x 200..440
 *                              右上 PALACE area      x 440..640
 *                              左下 FOOD STORAGE     x 0..200, y 200..420
 *                              中下 INFO/HAPPY/MAP/VIEW tabs
 *                                                     x 200..440, y 380..400
 *                              右下 CHANGE/BUY/UNIT  x 440..640, y 380..420
 *   底部        y 420..480 ── EXIT 按鈕 + version
 */

#define CS_W 640
#define CS_H 480

static void draw_panel(civ_surface_t *fb, civ_rect_t r,
                       uint8_t bg, uint8_t border, const char *title,
                       struct civ_game *g)
{
    civ_fill_rect(fb, r, bg);
    civ_frame_rect(fb, r, border);
    if (title && g->font_body) {
        civ_text_out(fb, g->font_body,
                     r.x + 6, r.y + 12,
                     title, 15, bg,
                     CIV_TEXT_BK_TRANSPARENT);
    }
}

void civ_city_screen_render(struct civ_game *g, civ_surface_t *fb)
{
    if (!g || !g->city_screen_open) return;
    if (g->city_screen_idx < 0 ||
        g->city_screen_idx >= g->world.cities_count) return;
    const civ_city_t *c = &g->world.cities[g->city_screen_idx];
    if (!c->alive) return;

    /* 整個 screen 藍色背景 (對齊原版 city screen 藍底) */
    civ_fill_rect(fb, (civ_rect_t){0, 0, CS_W, CS_H}, 9);  /* 9 = palette idx for blue-ish */

    /* === 標題列 y 0..30 === */
    civ_fill_rect(fb, (civ_rect_t){0, 0, CS_W, 30}, 1);
    if (g->font_body) {
        char buf[96];
        snprintf(buf, sizeof buf, "%s (人口: %d,000)", c->name, c->population * 10);
        int w = civ_text_measure(g->font_body, buf);
        int x = (CS_W - w) / 2;
        civ_text_out(fb, g->font_body, x, 22, buf, 15, 1, CIV_TEXT_BK_TRANSPARENT);
    }

    /* 標題左角 leader icon 占位 */
    civ_fill_rect(fb, (civ_rect_t){8, 4, 22, 22}, 7);
    civ_frame_rect(fb, (civ_rect_t){8, 4, 22, 22}, 15);

    /* === CITY RESOURCES panel (左上) === */
    draw_panel(fb, (civ_rect_t){8, 34, 184, 170}, 9, 15, "CITY RESOURCES", g);
    if (g->font_body) {
        int rx = 16, ry = 60;
        char buf[64];
        snprintf(buf, sizeof buf, "食物: %d", c->population * 2);
        civ_text_out(fb, g->font_body, rx, ry, buf, 14, 9, CIV_TEXT_BK_TRANSPARENT);
        ry += 16;
        snprintf(buf, sizeof buf, "生產: %d 盾/turn", 2 + c->population);
        civ_text_out(fb, g->font_body, rx, ry, buf, 14, 9, CIV_TEXT_BK_TRANSPARENT);
        ry += 16;
        snprintf(buf, sizeof buf, "貿易: %d", c->population);
        civ_text_out(fb, g->font_body, rx, ry, buf, 14, 9, CIV_TEXT_BK_TRANSPARENT);
        ry += 24;
        civ_text_out(fb, g->font_body, rx, ry, "人民:", 15, 9, CIV_TEXT_BK_TRANSPARENT);
        ry += 16;
        snprintf(buf, sizeof buf, "  快樂 %d / 不滿 0", c->population);
        civ_text_out(fb, g->font_body, rx, ry, buf, 10, 9, CIV_TEXT_BK_TRANSPARENT);
    }

    /* === Center small map (中上) - city 周圍 5x5 tiles 縮圖 === */
    draw_panel(fb, (civ_rect_t){200, 34, 232, 170}, 0, 15, NULL, g);
    if (g->sprite_sheet.sheet) {
        civ_sprite_sheet_t *sh = &g->sprite_sheet;
        /* 5x5 grid, 32x32 = 160x160 px (160 fits in panel) */
        int gx0 = 200 + (232 - 5*32) / 2;
        int gy0 = 34  + (170 - 5*32) / 2;
        for (int dy = -2; dy <= 2; dy++) {
            for (int dx = -2; dx <= 2; dx++) {
                int wx = c->x + dx;
                int wy = c->y + dy;
                if (wx < 0 || wx >= CIV_MAP_W || wy < 0 || wy >= CIV_MAP_H) continue;
                civ_terrain_kind_t kind = (civ_terrain_kind_t)g->world.terrain[wy][wx];
                int sc = 0, sr = 0;
                civ_terrain_sprite_coord(kind, &sc, &sr);
                civ_rect_t src = civ_sprite_rect(sh, sc, sr);
                int px = gx0 + (dx + 2) * 32;
                int py = gy0 + (dy + 2) * 32;
                if (sh->lut_built)
                    civ_surface_blit_remap(fb, px, py, sh->sheet, &src, sh->lut);
                else
                    civ_surface_blit(fb, px, py, sh->sheet, &src);
            }
        }
        /* 中央 city sprite */
        int city_col = c->population >= 6 ? 32 : (c->population >= 3 ? 31 : 30);
        civ_rect_t city_src = civ_sprite_rect(sh, city_col, 0);
        int cx = gx0 + 2 * 32;
        int cy = gy0 + 2 * 32;
        if (sh->lut_built)
            civ_surface_blit_remap(fb, cx, cy, sh->sheet, &city_src, sh->lut);
        else
            civ_surface_blit(fb, cx, cy, sh->sheet, &city_src);
    }

    /* === PALACE area (右上) === */
    draw_panel(fb, (civ_rect_t){440, 34, 192, 170}, 9, 15, "PALACE", g);
    if (g->font_body) {
        civ_text_out(fb, g->font_body,
                     460, 100, "(主城)", 14, 9, CIV_TEXT_BK_TRANSPARENT);
    }

    /* === FOOD STORAGE (左下) === */
    draw_panel(fb, (civ_rect_t){8, 210, 184, 200}, 9, 15, "FOOD STORAGE", g);
    if (g->font_body) {
        char buf[64];
        snprintf(buf, sizeof buf, "%d / %d", c->food_stock,
                 (c->population + 1) * 10);
        civ_text_out(fb, g->font_body, 16, 240, buf, 14, 9, CIV_TEXT_BK_TRANSPARENT);
    }

    /* === Build panel (中下) — INFO/HAPPY/MAP/VIEW tabs + CHANGE/BUY === */
    draw_panel(fb, (civ_rect_t){200, 210, 432, 200}, 9, 15, NULL, g);
    if (g->font_body) {
        /* tabs */
        const char *tabs[] = { "INFO", "HAPPY", "MAP", "VIEW" };
        int tx = 210;
        for (size_t i = 0; i < 4; i++) {
            int tw = civ_text_measure(g->font_body, tabs[i]) + 12;
            civ_fill_rect(fb, (civ_rect_t){tx, 218, tw, 18}, 7);
            civ_frame_rect(fb, (civ_rect_t){tx, 218, tw, 18}, 0);
            civ_text_out(fb, g->font_body, tx + 6, 232, tabs[i],
                         0, 7, CIV_TEXT_BK_TRANSPARENT);
            tx += tw + 4;
        }

        /* 建造資訊 (INFO tab 預設) */
        int by = 250;
        if (c->building_target >= 0) {
            char buf[96];
            snprintf(buf, sizeof buf, "正在建造: %s",
                     civ_building_name_zh(c->building_target));
            civ_text_out(fb, g->font_body, 216, by, buf, 15, 9, CIV_TEXT_BK_TRANSPARENT);
            by += 16;
            snprintf(buf, sizeof buf, "  進度: %d / %d 盾",
                     c->shield_stock,
                     civ_building_cost(c->building_target));
            civ_text_out(fb, g->font_body, 216, by, buf, 14, 9, CIV_TEXT_BK_TRANSPARENT);
            by += 16;
            int remain = civ_building_cost(c->building_target) - c->shield_stock;
            int per_turn = 2 + c->population;
            int turns = per_turn > 0 ? (remain + per_turn - 1) / per_turn : 999;
            snprintf(buf, sizeof buf, "  約 %d 回合完成", turns);
            civ_text_out(fb, g->font_body, 216, by, buf, 14, 9, CIV_TEXT_BK_TRANSPARENT);
        } else {
            civ_text_out(fb, g->font_body, 216, by,
                         "(待選擇建造項目)", 12, 9, CIV_TEXT_BK_TRANSPARENT);
        }

        /* CHANGE + BUY 按鈕 */
        civ_fill_rect(fb, (civ_rect_t){460, 360, 60, 20}, 7);
        civ_frame_rect(fb, (civ_rect_t){460, 360, 60, 20}, 0);
        civ_text_out(fb, g->font_body, 470, 374, "CHANGE", 0, 7,
                     CIV_TEXT_BK_TRANSPARENT);
        civ_fill_rect(fb, (civ_rect_t){530, 360, 50, 20}, 7);
        civ_frame_rect(fb, (civ_rect_t){530, 360, 50, 20}, 0);
        civ_text_out(fb, g->font_body, 545, 374, "BUY", 0, 7,
                     CIV_TEXT_BK_TRANSPARENT);
    }

    /* === EXIT 按鈕 (右下) + hint === */
    civ_fill_rect(fb, (civ_rect_t){560, 440, 70, 28}, 12);
    civ_frame_rect(fb, (civ_rect_t){560, 440, 70, 28}, 15);
    if (g->font_body) {
        civ_text_out(fb, g->font_body, 580, 460, "EXIT", 15, 12,
                     CIV_TEXT_BK_TRANSPARENT);
        civ_text_out(fb, g->font_body, 8, 460,
                     "ESC 鍵 離開", 15, 9, CIV_TEXT_BK_TRANSPARENT);
    }
}
