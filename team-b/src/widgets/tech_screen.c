#include "tech_screen.h"
#include "../civ_game.h"
#include "../gfx/palette.h"
#include "../gfx/primitive.h"
#include "../text/text_out.h"
#include "../world/tech.h"
#include "../world/unit.h"
#include "../world/sprite_sheet.h"
#include "../world/city.h"

#include <stdio.h>
#include <string.h>

#define TS_W 640
#define TS_H 480

static uint8_t pn(struct civ_game *g, uint8_t r, uint8_t g_, uint8_t b)
{
    return civ_palette_nearest_rgb(&g->palette, r, g_, b);
}

/* R16-2: 花邊 frame 自畫. 8×8 pattern: 綠底 + 紅圓點 + 紅斜紋.
 * border 寬度 tw px. inner 用 outer + tw 縮排.
 *
 * pattern data: 8x8 bool array. 1 = 紅, 0 = 綠.
 *   . . . X . . . .
 *   . . X X X . . .
 *   . . . X . . . .
 *   . . . . . . . X
 *   . . . . . . X .
 *   . . . . . X . .
 *   . . . . X . . .
 *   . . . X . . . .
 *
 * 圓點放中央, 斜紋從右上往左下劃過.  */
static void paint_frame_border(civ_surface_t *fb, struct civ_game *g,
                                civ_rect_t r, int tw)
{
    static const uint8_t PAT[8] = {
        0x10, 0x38, 0x10, 0x01, 0x02, 0x04, 0x08, 0x10
    };
    uint8_t c_green = pn(g, 0x00, 0x80, 0x00);
    uint8_t c_red   = pn(g, 0xC0, 0x00, 0x00);

    /* 4 條 border rectangle: 上 / 下 / 左 / 右 */
    civ_rect_t bands[4] = {
        { r.x, r.y, r.w, tw },                          /* top */
        { r.x, r.y + r.h - tw, r.w, tw },               /* bottom */
        { r.x, r.y, tw, r.h },                          /* left */
        { r.x + r.w - tw, r.y, tw, r.h },               /* right */
    };
    for (int b = 0; b < 4; b++) {
        civ_rect_t bd = bands[b];
        for (int yy = bd.y; yy < bd.y + bd.h; yy++) {
            for (int xx = bd.x; xx < bd.x + bd.w; xx++) {
                if (xx < 0 || xx >= fb->w || yy < 0 || yy >= fb->h) continue;
                int py = yy & 7;
                int px = xx & 7;
                uint8_t bit = (PAT[py] >> (7 - px)) & 1;
                fb->pixels[yy * fb->pitch + xx] = bit ? c_red : c_green;
            }
        }
    }
}

/* R16-5: 16x16 mini sprite blit (從 SPR32X32 取 32x32, 縮一半).
 * 簡化: 直接 sub-sample 偶數 row/col. SPR32X32 is 32x32 per tile. */
static void blit_mini_sprite(civ_surface_t *fb, struct civ_game *g,
                              int dst_x, int dst_y, int col, int row)
{
    civ_sprite_sheet_t *sh = &g->sprite_sheet;
    if (!sh->sheet) return;
    civ_rect_t src = civ_sprite_rect(sh, col, row);
    int tw = sh->tile_w, th = sh->tile_h;
    for (int yy = 0; yy < 16; yy++) {
        for (int xx = 0; xx < 16; xx++) {
            int sx = src.x + xx * tw / 16;
            int sy = src.y + yy * th / 16;
            if (sx < 0 || sx >= sh->sheet->w ||
                sy < 0 || sy >= sh->sheet->h) continue;
            uint8_t sidx = sh->sheet->pixels[sy * sh->sheet->pitch + sx];
            if (sidx == 0) continue;  /* transparent */
            uint8_t out = sh->lut_built ? sh->lut[sidx] : sidx;
            int dx = dst_x + xx, dy = dst_y + yy;
            if (dx < 0 || dx >= fb->w || dy < 0 || dy >= fb->h) continue;
            fb->pixels[dy * fb->pitch + dx] = out;
        }
    }
}

/* SPR32X32 mini sprite (col, row) 對 unit / wonder / improvement.
 * 值是 best-effort (對齊 spec 06 §6.1-6.4 推測), 真位置待 R17 ground-truth. */
static void unit_sprite_coord(int unit_id, int *col, int *row)
{
    /* unit row = 10 (M6-full 使用), col 推測 = unit_id-1 */
    *row = 10;
    *col = (unit_id - 1) & 0xF;
    if (*col < 0) *col = 0;
}

static void building_sprite_coord(int building_id, int *col, int *row)
{
    /* improvement icons 推測在 row 12 (待 R17 ground-truth) */
    *row = 12;
    *col = (building_id - 1) & 0xF;
    if (*col < 0) *col = 0;
}

static void wonder_sprite_coord(int wonder_id, int *col, int *row)
{
    /* wonder icons 推測在 row 11 */
    *row = 11;
    *col = wonder_id & 0xF;
}

/* 取 civ 名稱 (R16-3 source phrase 用). slot 0 = barbarian, 1..7 = civ. */
static const char *civ_short_name(int slot)
{
    /* 用 spec 05 14 文明索引第 1..14, slot 0..7 直接 mod 取代表 */
    if (slot <= 0 || slot >= 8) return "他國";
    /* civs.c 內有完整 14 文明資料; 簡化用 hardcoded 7 (player slot 內常見) */
    static const char *SHORT[8] = {
        "野蠻", "羅馬", "巴比倫", "德國", "埃及", "美國", "希臘", "印度"
    };
    return SHORT[slot];
}

void civ_tech_screen_render(struct civ_game *g, civ_surface_t *fb)
{
    if (!g || !g->tech_screen_open) return;
    const civ_tech_discovery_event_t *ev = &g->tech_screen_event;

    /* 顏色 */
    uint8_t c_white  = pn(g, 0xFF, 0xFF, 0xFF);
    uint8_t c_black  = pn(g, 0x00, 0x00, 0x00);
    uint8_t c_grey   = pn(g, 0xA0, 0xA0, 0xA0);
    uint8_t c_purple = pn(g, 0x80, 0x00, 0x80);
    uint8_t c_blue   = pn(g, 0x00, 0x40, 0xC0);
    uint8_t c_red    = pn(g, 0xC0, 0x00, 0x00);
    uint8_t c_green  = pn(g, 0x00, 0x80, 0x00);
    uint8_t c_bg     = pn(g, 0xF0, 0xE8, 0xD0);   /* 米色 wallpaper bg */
    uint8_t c_yellow = pn(g, 0xFF, 0xE0, 0x00);

    /* 整版 米色 wallpaper 底 */
    civ_fill_rect(fb, (civ_rect_t){0, 0, TS_W, TS_H}, c_bg);

    /* R16-2: 雙層花邊 frame */
    paint_frame_border(fb, g, (civ_rect_t){8, 8, TS_W - 16, TS_H - 16}, 16);
    civ_frame_rect(fb, (civ_rect_t){24, 24, TS_W - 48, TS_H - 48}, c_black);
    /* 內框留 1px 黑邊就好 */

    /* R16-3: 左上 96x96 tech illustration 占位 (BRONZE WORKING = 大鍋火) */
    civ_rect_t pic_r = { 56, 80, 96, 96 };
    civ_fill_rect(fb, pic_r, c_black);
    civ_frame_rect(fb, pic_r, c_grey);
    if (g->font_body) {
        const char *icon_hint = "(圖示)";
        int w = civ_text_measure(g->font_body, icon_hint);
        civ_text_out(fb, g->font_body,
                     pic_r.x + (pic_r.w - w) / 2,
                     pic_r.y + pic_r.h / 2,
                     icon_hint, c_white, c_black,
                     CIV_TEXT_BK_TRANSPARENT);
    }

    /* R16-3: 大標題 (tech 中文名). 雙寫達到「粗黑大字」效果. */
    if (g->font_title) {
        const char *tname = civ_tech_name_zh(ev->tech_id);
        int tw = civ_text_measure(g->font_title, tname);
        int tx = 180;
        int ty = 110;
        /* drop shadow 1px */
        civ_text_out(fb, g->font_title, tx + 1, ty + 1,
                     tname, c_grey, c_bg, CIV_TEXT_BK_TRANSPARENT);
        civ_text_out(fb, g->font_title, tx, ty,
                     tname, c_black, c_bg, CIV_TEXT_BK_TRANSPARENT);
        (void)tw;
    } else if (g->font_body) {
        civ_text_out(fb, g->font_body, 180, 110,
                     civ_tech_name_zh(ev->tech_id), c_black, c_bg,
                     CIV_TEXT_BK_TRANSPARENT);
    }

    /* subtitle "文明進展" */
    if (g->font_body) {
        civ_text_out(fb, g->font_body, 180, 140,
                     civ_tech_subtitle_zh(), c_grey, c_bg,
                     CIV_TEXT_BK_TRANSPARENT);

        /* "(取自 XXX)" / "(自家研發)" */
        const char *src = civ_tech_source_phrase_zh(
            ev->source, civ_short_name(ev->from_civ_slot));
        civ_text_out(fb, g->font_body, 180, 162,
                     src, c_grey, c_bg, CIV_TEXT_BK_TRANSPARENT);
    }

    /* R16-4: Allows 列表 */
    if (g->font_body) {
        int ay = 210;
        civ_text_out(fb, g->font_body, 80, ay,
                     "解鎖:", c_purple, c_bg, CIV_TEXT_BK_TRANSPARENT);
        ay += 22;

        /* 後續可研發 tech (藍) */
        for (int i = 0; i < 8 && ev->unlocked_techs[i]; i++) {
            civ_text_out(fb, g->font_body, 110, ay,
                         civ_tech_name_zh(ev->unlocked_techs[i]),
                         c_blue, c_bg, CIV_TEXT_BK_TRANSPARENT);
            ay += 20;
        }

        /* unit (紅) + sprite */
        for (int i = 0; i < 4 && ev->unlocked_units[i]; i++) {
            int col = 0, row = 0;
            unit_sprite_coord(ev->unlocked_units[i], &col, &row);
            blit_mini_sprite(fb, g, 100, ay - 14, col, row);
            char buf[64];
            snprintf(buf, sizeof buf, "%s 單位",
                     civ_unit_name_zh((civ_unit_type_t)ev->unlocked_units[i]));
            civ_text_out(fb, g->font_body, 120, ay, buf,
                         c_red, c_bg, CIV_TEXT_BK_TRANSPARENT);
            ay += 20;
        }

        /* improvement (綠) + sprite */
        for (int i = 0; i < 4 && ev->unlocked_imp[i]; i++) {
            int col = 0, row = 0;
            building_sprite_coord(ev->unlocked_imp[i], &col, &row);
            blit_mini_sprite(fb, g, 100, ay - 14, col, row);
            char buf[64];
            snprintf(buf, sizeof buf, "%s",
                     civ_building_name_zh(ev->unlocked_imp[i]));
            civ_text_out(fb, g->font_body, 120, ay, buf,
                         c_green, c_bg, CIV_TEXT_BK_TRANSPARENT);
            ay += 20;
        }

        /* wonder (綠) + sprite — R16 只 placeholder, 真名 R17 補 wonder 表 */
        for (int i = 0; i < 4 && ev->unlocked_wonder[i]; i++) {
            int col = 0, row = 0;
            wonder_sprite_coord(ev->unlocked_wonder[i], &col, &row);
            blit_mini_sprite(fb, g, 100, ay - 14, col, row);
            const char *wname = "巨像 Wonder";
            if (ev->unlocked_wonder[i] == 2) wname = "金字塔 Wonder";
            civ_text_out(fb, g->font_body, 120, ay, wname,
                         c_green, c_bg, CIV_TEXT_BK_TRANSPARENT);
            ay += 20;
        }
    }

    /* 底部 hint */
    if (g->font_body) {
        civ_text_out(fb, g->font_body, 50, TS_H - 40,
                     "按 Enter / ESC 繼續",
                     c_yellow, c_bg, CIV_TEXT_BK_TRANSPARENT);
    }
}
