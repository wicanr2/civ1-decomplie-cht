#include "city_screen.h"
#include "../civ_game.h"
#include "../gfx/palette.h"
#include "../gfx/primitive.h"
#include "../text/text_out.h"
#include "../world/city.h"
#include "../world/world.h"
#include "../world/sprite_sheet.h"

#include <stdio.h>
#include <string.h>

/* R10: 用 palette_nearest 找 Win16 真色, 不用 sheet palette idx 直填 */
/* R32 (C4/C5 gap): 5 種 16×16 城市資源圖示 — clean-room 自畫.
 * 對齊 1993 reference civ1_win_city_screen.png CITY RESOURCES 區的
 * 麥穗 / 盾 / 金幣 / 笑臉 / 怒臉 row. SPR16x16 我們沒載, 全部 hardcoded
 * 8×8 pixel pattern + scale 2x. */
typedef enum {
    CIV_RES_ICON_WHEAT  = 0,   /* 食物 — 黃色麥穗 */
    CIV_RES_ICON_SHIELD = 1,   /* 生產 — 灰盾形 */
    CIV_RES_ICON_TRADE  = 2,   /* 貿易 — 金幣 */
    CIV_RES_ICON_HAPPY  = 3,   /* 快樂 — 笑臉 (黃底黑點) */
    CIV_RES_ICON_UNHAPPY= 4,   /* 不滿 — 怒臉 (紅底黑點) */
} civ_res_icon_t;

/* 5 × 8×8 bit pattern (1 = 主色, 0 = 透明) */
static const uint8_t RES_ICON_PAT[5][8] = {
    /* wheat — 麥穗 */
    { 0x18, 0x3C, 0x7E, 0x7E, 0x3C, 0x18, 0x18, 0x18 },
    /* shield — 盾 */
    { 0x7E, 0xFF, 0xFF, 0xFF, 0xFF, 0x7E, 0x3C, 0x18 },
    /* trade — 金幣 */
    { 0x3C, 0x7E, 0xFF, 0xDB, 0xDB, 0xFF, 0x7E, 0x3C },
    /* happy — 笑臉 */
    { 0x3C, 0x42, 0xA5, 0x81, 0xA5, 0x99, 0x42, 0x3C },
    /* unhappy — 怒臉 */
    { 0x3C, 0x42, 0xA5, 0x81, 0x99, 0xA5, 0x42, 0x3C },
};

static uint8_t pn(struct civ_game *g, uint8_t r, uint8_t g_, uint8_t b)
{
    return civ_palette_nearest_rgb(&g->palette, r, g_, b);
}

/* 藍色 stipple dither (2x2 checker) — 對齊原版 city screen 底色
 * 仍保留作 fallback (sprite sheet 未載入時用) */
/* R32: 畫 1 個 16×16 城市資源圖示. (dx, dy) = dst 左上角.
 * pattern 8×8 scale 2x → 16×16, fg = 圖示色, bg-transparent. */
static void paint_resource_icon(civ_surface_t *fb, struct civ_game *g,
                                 civ_res_icon_t icon, int dx, int dy)
{
    uint8_t fg;
    switch (icon) {
    case CIV_RES_ICON_WHEAT:   fg = pn(g, 0xE0, 0xC0, 0x40); break; /* 黃 */
    case CIV_RES_ICON_SHIELD:  fg = pn(g, 0xA0, 0xA0, 0xA0); break; /* 灰 */
    case CIV_RES_ICON_TRADE:   fg = pn(g, 0xE0, 0xC0, 0x20); break; /* 金 */
    case CIV_RES_ICON_HAPPY:   fg = pn(g, 0xF0, 0xE0, 0x40); break; /* 黃笑 */
    case CIV_RES_ICON_UNHAPPY: fg = pn(g, 0xE0, 0x40, 0x40); break; /* 紅怒 */
    default: return;
    }
    const uint8_t *pat = RES_ICON_PAT[icon];
    for (int py = 0; py < 8; py++) {
        for (int px = 0; px < 8; px++) {
            if (!((pat[py] >> (7 - px)) & 1)) continue;
            /* 2x scale */
            for (int sy = 0; sy < 2; sy++) {
                for (int sx = 0; sx < 2; sx++) {
                    int xx = dx + px * 2 + sx;
                    int yy = dy + py * 2 + sy;
                    if (xx < 0 || xx >= fb->w || yy < 0 || yy >= fb->h) continue;
                    fb->pixels[yy * fb->pitch + xx] = fg;
                }
            }
        }
    }
}

/* R32: 一排 N 個圖示 (max 12 in 一行). 用於 CITY RESOURCES + FOOD STORAGE. */
static void paint_icon_row(civ_surface_t *fb, struct civ_game *g,
                            civ_res_icon_t icon, int n,
                            int x, int y, int icon_w)
{
    if (n < 0) n = 0;
    if (n > 12) n = 12;
    for (int i = 0; i < n; i++) {
        paint_resource_icon(fb, g, icon, x + i * icon_w, y);
    }
}

static void paint_stipple(civ_surface_t *fb, civ_rect_t r,
                          uint8_t c_hi, uint8_t c_lo)
{
    for (int yy = r.y; yy < r.y + r.h; yy++) {
        for (int xx = r.x; xx < r.x + r.w; xx++) {
            uint8_t c = ((xx + yy) & 1) ? c_hi : c_lo;
            if (xx >= 0 && xx < fb->w && yy >= 0 && yy < fb->h)
                fb->pixels[yy * fb->pitch + xx] = c;
        }
    }
}

/* R17: paint_fine_speckle — 4x4 pattern, 取代 R15 paint_tile_repeat 的 ocean.
 * 對齊原版 ROME city screen reference: 整版底色實際是「純藍 + 細顆粒 dither」,
 * 不是 ocean wave tile (R15 R10 使用者再次指正). 4x4 內 13/16 pixel 是 base 藍,
 * 3/16 pixel 是 highlight 較亮藍, 形成均勻 speckle 質感.
 *
 *   pattern:
 *     . . . .
 *     . X . .
 *     . . . X
 *     . . X .
 */
static void paint_fine_speckle(civ_surface_t *fb, civ_rect_t r,
                                uint8_t c_base, uint8_t c_dot)
{
    static const uint8_t DOT[16] = {
        0, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 0, 1,
        0, 0, 1, 0,
    };
    for (int yy = r.y; yy < r.y + r.h; yy++) {
        for (int xx = r.x; xx < r.x + r.w; xx++) {
            if (xx < 0 || xx >= fb->w || yy < 0 || yy >= fb->h) continue;
            int idx = (yy & 3) * 4 + (xx & 3);
            fb->pixels[yy * fb->pitch + xx] = DOT[idx] ? c_dot : c_base;
        }
    }
}

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

/* R10 改: panel 改用 stipple bg + 深藍 title bar 對齊原版 city screen */
static void draw_panel(civ_surface_t *fb, civ_rect_t r,
                       struct civ_game *g, const char *title)
{
    uint8_t bg_hi    = pn(g, 0x60, 0x80, 0xD0);
    uint8_t bg_lo    = pn(g, 0x50, 0x70, 0xC0);
    uint8_t border   = pn(g, 0x00, 0x00, 0x80);
    uint8_t title_bg = pn(g, 0x00, 0x00, 0x80);
    uint8_t title_fg = pn(g, 0xFF, 0xFF, 0xFF);

    /* R15 v2: panel 內仍用 stipple (text 易讀), 外大底才用 ocean tile 紋路.
     * 對齊使用者 reference: panel 跟外背景對比明顯 (panel 較柔). */
    paint_stipple(fb, r, bg_hi, bg_lo);
    civ_frame_rect(fb, r, border);
    if (title && g->font_body) {
        /* 內嵌深藍 title strip (高 14 px) */
        civ_rect_t tb = { r.x, r.y, r.w, 14 };
        civ_fill_rect(fb, tb, title_bg);
        civ_text_out(fb, g->font_body, r.x + 6, r.y + 11, title,
                     title_fg, title_bg, CIV_TEXT_BK_TRANSPARENT);
    }
}

void civ_city_screen_render(struct civ_game *g, civ_surface_t *fb)
{
    if (!g || !g->city_screen_open) return;
    if (g->city_screen_idx < 0 ||
        g->city_screen_idx >= g->world.cities_count) return;
    const civ_city_t *c = &g->world.cities[g->city_screen_idx];
    if (!c->alive) return;

    /* R10 改: 整版藍 stipple (原版 city screen 配色) */
    uint8_t c_bg_hi   = pn(g, 0x60, 0x80, 0xD0);
    uint8_t c_bg_lo   = pn(g, 0x50, 0x70, 0xC0);
    uint8_t c_yellow  = pn(g, 0xFF, 0xFF, 0x00);
    uint8_t c_black   = pn(g, 0x00, 0x00, 0x00);
    uint8_t c_white   = pn(g, 0xFF, 0xFF, 0xFF);
    uint8_t c_red     = pn(g, 0xC0, 0x00, 0x00);
    uint8_t c_blue    = pn(g, 0x00, 0x00, 0x80);
    /* R17 v2: 整版背景純藍 (Win16 #000080 nearest) + 微微 stipple 反白點
     * (4x4 中 1/16 高亮 pixel) — 對齊 ROME reference 純藍質感.
     *
     * 之前 paint_fine_speckle 用 3/16 密度 + 兩個藍色, 但 sheet palette
     * 找不到對應 RGB 跳到 terrain 雜色. v2 改用 c_blue base + c_white 1/16
     * 點, 兩色都是穩定 nearest match. */
    {
        uint8_t c_dot = pn(g, 0xC0, 0xC0, 0xC0);
        civ_fill_rect(fb, (civ_rect_t){0, 0, CS_W, CS_H}, c_blue);
        for (int yy = 0; yy < CS_H; yy += 4) {
            for (int xx = 2; xx < CS_W; xx += 8) {
                if (xx < fb->w && yy < fb->h)
                    fb->pixels[yy * fb->pitch + xx] = c_dot;
            }
        }
    }

    /* === 標題列 y 0..30 (黑底黃字 — 原版風格) === */
    civ_fill_rect(fb, (civ_rect_t){0, 0, CS_W, 30}, c_black);
    if (g->font_body) {
        char buf[96];
        snprintf(buf, sizeof buf, "%s (人口: %d,000)", c->name, c->population * 10);
        int w = civ_text_measure(g->font_body, buf);
        int x = (CS_W - w) / 2;
        civ_text_out(fb, g->font_body, x, 22, buf, c_yellow, c_black, CIV_TEXT_BK_TRANSPARENT);
    }

    /* 標題左角 leader icon 占位 */
    civ_fill_rect(fb, (civ_rect_t){8, 4, 22, 22}, c_blue);
    civ_frame_rect(fb, (civ_rect_t){8, 4, 22, 22}, c_white);

    /* === CITY RESOURCES panel (左上) — R32 (C5 gap): 改成 icon row + 數字 ===
     * 對齊 1993 reference: 食物 / 生產 / 貿易 / 人民 各一排 16×16 icon. */
    draw_panel(fb, (civ_rect_t){8, 34, 184, 170}, g, "CITY RESOURCES");
    {
        int food   = c->population * 2;
        int shield = 2 + c->population;
        int trade  = c->population;
        int happy  = c->population;
        int rx     = 16, ry = 60;
        int icon_w = 18;   /* 16 icon + 2 spacing */
        paint_icon_row(fb, g, CIV_RES_ICON_WHEAT,  food,   rx, ry, icon_w);
        ry += 22;
        paint_icon_row(fb, g, CIV_RES_ICON_SHIELD, shield, rx, ry, icon_w);
        ry += 22;
        paint_icon_row(fb, g, CIV_RES_ICON_TRADE,  trade,  rx, ry, icon_w);
        ry += 22;
        paint_icon_row(fb, g, CIV_RES_ICON_HAPPY,  happy,  rx, ry, icon_w);
        if (g->font_body) {
            char buf[32];
            snprintf(buf, sizeof buf, "  %d", food);
            civ_text_out(fb, g->font_body, rx + 12 * icon_w - 4, 60 + 12,
                         buf, c_white, c_bg_lo, CIV_TEXT_BK_TRANSPARENT);
        }
    }

    /* === Center small map (中上) - city 周圍 5x5 tiles 縮圖 === */
    civ_fill_rect(fb, (civ_rect_t){200, 34, 232, 170}, c_black);
    civ_frame_rect(fb, (civ_rect_t){200, 34, 232, 170}, c_blue);
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
    draw_panel(fb, (civ_rect_t){440, 34, 192, 170}, g, "PALACE");
    if (g->font_body) {
        civ_text_out(fb, g->font_body,
                     460, 104, "(主城)", c_yellow, c_bg_lo, CIV_TEXT_BK_TRANSPARENT);
    }

    /* === FOOD STORAGE (左下) — R32 (C4 gap): 加 wheat icon row 顯示存量 === */
    draw_panel(fb, (civ_rect_t){8, 210, 184, 200}, g, "FOOD STORAGE");
    {
        int cap = (c->population + 1) * 10;
        int stock = c->food_stock;
        if (stock < 0) stock = 0;
        if (stock > cap) stock = cap;
        /* 算 N icon = stock / 5 (max ~8 icons per row); 多 row 顯示 */
        int icons = stock / 5;
        if (icons < 0) icons = 0;
        if (icons > 24) icons = 24;
        int icon_w = 18;
        for (int i = 0; i < icons; i++) {
            int row = i / 8;
            int col = i % 8;
            paint_resource_icon(fb, g, CIV_RES_ICON_WHEAT,
                                 16 + col * icon_w, 244 + row * 20);
        }
        if (g->font_body) {
            char buf[64];
            snprintf(buf, sizeof buf, "%d / %d", stock, cap);
            civ_text_out(fb, g->font_body, 16, 320, buf,
                         c_white, c_bg_lo, CIV_TEXT_BK_TRANSPARENT);
        }
    }

    /* === Build panel (中下) — INFO/HAPPY/MAP/VIEW tabs + CHANGE/BUY === */
    paint_stipple(fb, (civ_rect_t){200, 210, 432, 200}, c_bg_hi, c_bg_lo);
    civ_frame_rect(fb, (civ_rect_t){200, 210, 432, 200}, c_blue);
    if (g->font_body) {
        /* tabs — INFO 黑底白字 (反白選中), 其餘灰底黑字 */
        const char *tabs[] = { "INFO", "HAPPY", "MAP", "VIEW" };
        int tx = 210;
        uint8_t c_grey = pn(g, 0xC0, 0xC0, 0xC0);
        for (size_t i = 0; i < 4; i++) {
            int tw = civ_text_measure(g->font_body, tabs[i]) + 12;
            uint8_t bg = (i == 0) ? c_black : c_grey;
            uint8_t fg = (i == 0) ? c_white : c_black;
            civ_fill_rect(fb, (civ_rect_t){tx, 218, tw, 18}, bg);
            civ_frame_rect(fb, (civ_rect_t){tx, 218, tw, 18}, c_black);
            civ_text_out(fb, g->font_body, tx + 6, 232, tabs[i],
                         fg, bg, CIV_TEXT_BK_TRANSPARENT);
            tx += tw + 4;
        }

        /* 建造資訊 (INFO tab 預設) */
        int by = 256;
        if (c->building_target >= 0) {
            char buf[96];
            snprintf(buf, sizeof buf, "正在建造: %s",
                     civ_building_name_zh(c->building_target));
            civ_text_out(fb, g->font_body, 216, by, buf, c_yellow, c_bg_lo, CIV_TEXT_BK_TRANSPARENT);
            by += 16;
            snprintf(buf, sizeof buf, "  進度: %d / %d 盾",
                     c->shield_stock,
                     civ_building_cost(c->building_target));
            civ_text_out(fb, g->font_body, 216, by, buf, c_white, c_bg_lo, CIV_TEXT_BK_TRANSPARENT);
            by += 16;
            int remain = civ_building_cost(c->building_target) - c->shield_stock;
            int per_turn = 2 + c->population;
            int turns = per_turn > 0 ? (remain + per_turn - 1) / per_turn : 999;
            snprintf(buf, sizeof buf, "  約 %d 回合完成", turns);
            civ_text_out(fb, g->font_body, 216, by, buf, c_white, c_bg_lo, CIV_TEXT_BK_TRANSPARENT);
        } else {
            civ_text_out(fb, g->font_body, 216, by,
                         "(待選擇建造項目)", c_yellow, c_bg_lo, CIV_TEXT_BK_TRANSPARENT);
        }

        /* CHANGE + BUY 按鈕 — Win16 灰底黑字 + 3D bevel */
        uint8_t c_btn = pn(g, 0xC0, 0xC0, 0xC0);
        civ_fill_rect(fb, (civ_rect_t){460, 360, 60, 20}, c_btn);
        civ_frame_rect(fb, (civ_rect_t){460, 360, 60, 20}, c_black);
        civ_text_out(fb, g->font_body, 470, 374, "CHANGE", c_black, c_btn,
                     CIV_TEXT_BK_TRANSPARENT);
        civ_fill_rect(fb, (civ_rect_t){530, 360, 50, 20}, c_btn);
        civ_frame_rect(fb, (civ_rect_t){530, 360, 50, 20}, c_black);
        civ_text_out(fb, g->font_body, 545, 374, "BUY", c_black, c_btn,
                     CIV_TEXT_BK_TRANSPARENT);
    }

    /* === EXIT 按鈕 (右下) — 紅底白字 + hint === */
    civ_fill_rect(fb, (civ_rect_t){560, 440, 70, 28}, c_red);
    civ_frame_rect(fb, (civ_rect_t){560, 440, 70, 28}, c_white);
    if (g->font_body) {
        civ_text_out(fb, g->font_body, 580, 460, "EXIT", c_white, c_red,
                     CIV_TEXT_BK_TRANSPARENT);
        civ_text_out(fb, g->font_body, 8, 460,
                     "ESC 鍵 離開", c_yellow, c_bg_lo, CIV_TEXT_BK_TRANSPARENT);
    }
}
