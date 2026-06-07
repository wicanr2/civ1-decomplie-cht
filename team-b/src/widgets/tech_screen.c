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

/* R28-2: 科技官員立像 — Civdata3 內 discovr1.gif (512×320). 1993 原版
 * 科技發現畫面右側站個科學家介紹新科技. R16 之前完全沒接, 純文字+花邊
 * frame. R28 對齊原版加上人物 sprite.
 *
 * 透明度: 採用 paint_leader_portrait 同套 top-row >50% dominant sampling
 * 推導 sentinel idx (sprite 邊角是背景, 內部人物是實心), 不會誤殺臉色. */
static void paint_tech_advisor(civ_surface_t *fb, struct civ_game *g,
                                int dst_x, int dst_y, int dst_w, int dst_h)
{
    civ_surface_t *adv = g->tech_advisor[0];
    if (!adv) return;

    civ_rect_t src = { 0, 0, adv->w, adv->h };

    uint8_t lut[256];
    civ_palette_build_lut(g->tech_advisor_palettes[0].entries, 256,
                          &g->palette, lut);

    uint8_t skip[256];
    memset(skip, 0, sizeof skip);
    skip[0] = 1;
    /* top-row dominant-pixel sample */
    {
        int hist[256] = {0};
        for (int xx = 0; xx < src.w; xx++) {
            hist[adv->pixels[0 * adv->pitch + xx]]++;
        }
        int half = src.w / 2;
        for (int i = 0; i < 256; i++) {
            if (hist[i] > half) { skip[i] = 1; break; }
        }
    }

    for (int yy = 0; yy < dst_h; yy++) {
        int sy = yy * src.h / dst_h;
        if (sy < 0 || sy >= adv->h) continue;
        for (int xx = 0; xx < dst_w; xx++) {
            int sx = xx * src.w / dst_w;
            if (sx < 0 || sx >= adv->w) continue;
            int dx = dst_x + xx, dy = dst_y + yy;
            if (dx < 0 || dx >= fb->w || dy < 0 || dy >= fb->h) continue;
            uint8_t si = adv->pixels[sy * adv->pitch + sx];
            if (skip[si]) continue;
            fb->pixels[dy * fb->pitch + dx] = lut[si];
        }
    }
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

/* R17: 每個 tech 對應一個代表性中文字 (icon 占位之代替), 對齊 reference
 * 圖示 — 1991 manual 每 tech 對應一個 cause/effect 圖. */
static const char *tech_icon_char_zh(civ_tech_id_t t)
{
    switch (t) {
        case CIV_TECH_ALPHABET:         return "文";
        case CIV_TECH_BRONZE_WORKING:   return "銅";
        case CIV_TECH_HORSEBACK_RIDING: return "馬";
        case CIV_TECH_POTTERY:          return "陶";
        case CIV_TECH_CURRENCY:         return "幣";
        case CIV_TECH_IRON_WORKING:     return "鐵";
        case CIV_TECH_WRITING:          return "書";
        case CIV_TECH_MAP_MAKING:       return "圖";
        case CIV_TECH_MASONRY:          return "石";
        case CIV_TECH_RELIGION:         return "神";
        case CIV_TECH_LITERACY:         return "文";
        case CIV_TECH_MATHEMATICS:      return "算";
        case CIV_TECH_PHILOSOPHY:       return "哲";
        case CIV_TECH_PHYSICS:          return "理";
        case CIV_TECH_CHEMISTRY:        return "化";
        case CIV_TECH_ELECTRICITY:      return "電";
        case CIV_TECH_COMPUTERS:        return "腦";
        case CIV_TECH_NUCLEAR_FISSION:  return "核";
        case CIV_TECH_SPACE_FLIGHT:     return "太";
        default:                        return "?";
    }
}

void civ_tech_screen_render(struct civ_game *g, civ_surface_t *fb)
{
    if (!g || !g->tech_screen_open) return;
    const civ_tech_discovery_event_t *ev = &g->tech_screen_event;

    /* R17 升級色盤 — 對齊原版 BRONZE WORKING reference 紙質感 */
    uint8_t c_white  = pn(g, 0xFF, 0xFF, 0xFF);
    uint8_t c_black  = pn(g, 0x00, 0x00, 0x00);
    uint8_t c_grey   = pn(g, 0x80, 0x80, 0x80);
    uint8_t c_dgrey  = pn(g, 0x40, 0x40, 0x40);
    uint8_t c_purple = pn(g, 0x80, 0x00, 0x80);
    uint8_t c_blue   = pn(g, 0x00, 0x40, 0xC0);
    uint8_t c_red    = pn(g, 0xC0, 0x00, 0x00);
    uint8_t c_green  = pn(g, 0x00, 0x80, 0x00);
    uint8_t c_bg     = pn(g, 0xF0, 0xE8, 0xD0);   /* 米色 wallpaper bg */
    uint8_t c_dark   = pn(g, 0x60, 0x30, 0x20);   /* tech icon 深褐 (青銅色) */
    uint8_t c_gold   = pn(g, 0xE0, 0xC0, 0x40);   /* tech icon 金 highlight */

    /* 整版 米色 wallpaper 底 */
    civ_fill_rect(fb, (civ_rect_t){0, 0, TS_W, TS_H}, c_bg);

    /* R16-2: 雙層花邊 frame (外厚 + 內細 + 黑線) */
    paint_frame_border(fb, g, (civ_rect_t){8, 8, TS_W - 16, TS_H - 16}, 16);
    civ_frame_rect(fb, (civ_rect_t){24, 24, TS_W - 48, TS_H - 48}, c_black);
    civ_frame_rect(fb, (civ_rect_t){25, 25, TS_W - 50, TS_H - 50}, c_dgrey);

    /* R28-2: 科技官員立像 (Civdata3 discovr1) — 對齊原版科技發現右側人物.
     * src 512×320 → 右側 200×250 dst (x=410 y=80, 接到 inner frame 邊). */
    paint_tech_advisor(fb, g, 410, 80, 200, 250);

    /* R17: 左上 tech illustration — 用 1 個代表性中文字 (大字) + 深褐底 */
    civ_rect_t pic_r = { 60, 80, 110, 110 };
    civ_fill_rect(fb, pic_r, c_dark);
    civ_frame_rect(fb, pic_r, c_gold);
    civ_frame_rect(fb,
                   (civ_rect_t){pic_r.x + 1, pic_r.y + 1, pic_r.w - 2, pic_r.h - 2},
                   c_black);
    /* 圖示大字 — 用 font_title 寫 1 個代表字, 雙寫粗體 */
    if (g->font_title) {
        const char *ic = tech_icon_char_zh(ev->tech_id);
        int iw = civ_text_measure(g->font_title, ic);
        int ix = pic_r.x + (pic_r.w - iw) / 2;
        int iy = pic_r.y + pic_r.h / 2 + 16;
        civ_text_out(fb, g->font_title, ix + 1, iy + 1, ic, c_dark, c_dark,
                     CIV_TEXT_BK_TRANSPARENT);
        civ_text_out(fb, g->font_title, ix, iy, ic, c_gold, c_dark,
                     CIV_TEXT_BK_TRANSPARENT);
        /* 雙寫粗體 */
        civ_text_out(fb, g->font_title, ix + 1, iy, ic, c_gold, c_dark,
                     CIV_TEXT_BK_TRANSPARENT);
    }

    /* R17: 大標題區 — tname 中央偏右, 雙寫多次 達到粗黑大字效果 */
    int title_x = 200;
    int title_y = 130;
    if (g->font_title) {
        const char *tname = civ_tech_name_zh(ev->tech_id);
        /* drop shadow 3px (gives 3D effect) */
        civ_text_out(fb, g->font_title, title_x + 2, title_y + 2,
                     tname, c_grey, c_bg, CIV_TEXT_BK_TRANSPARENT);
        /* bold 雙寫: stamp twice with 1px x offset */
        civ_text_out(fb, g->font_title, title_x,     title_y,
                     tname, c_black, c_bg, CIV_TEXT_BK_TRANSPARENT);
        civ_text_out(fb, g->font_title, title_x + 1, title_y,
                     tname, c_black, c_bg, CIV_TEXT_BK_TRANSPARENT);
    } else if (g->font_body) {
        civ_text_out(fb, g->font_body, title_x, title_y,
                     civ_tech_name_zh(ev->tech_id), c_black, c_bg,
                     CIV_TEXT_BK_TRANSPARENT);
    }

    /* subtitle + source phrase */
    if (g->font_body) {
        civ_text_out(fb, g->font_body, title_x, title_y + 26,
                     civ_tech_subtitle_zh(), c_dgrey, c_bg,
                     CIV_TEXT_BK_TRANSPARENT);

        const char *src = civ_tech_source_phrase_zh(
            ev->source, civ_short_name(ev->from_civ_slot));
        civ_text_out(fb, g->font_body, title_x, title_y + 46,
                     src, c_grey, c_bg, CIV_TEXT_BK_TRANSPARENT);
    }

    /* R16-4: Allows 列表 — 加深裝飾, 對齊 reference layout */
    if (g->font_body) {
        int ay = 240;
        int ax = 80;
        civ_text_out(fb, g->font_body, ax, ay,
                     "解鎖:", c_purple, c_bg, CIV_TEXT_BK_TRANSPARENT);
        ay += 24;

        /* 後續可研發 tech (藍) */
        for (int i = 0; i < 8 && ev->unlocked_techs[i]; i++) {
            civ_text_out(fb, g->font_body, ax + 30, ay,
                         civ_tech_name_zh(ev->unlocked_techs[i]),
                         c_blue, c_bg, CIV_TEXT_BK_TRANSPARENT);
            ay += 22;
        }

        /* unit (紅) + sprite */
        for (int i = 0; i < 4 && ev->unlocked_units[i]; i++) {
            int col = 0, row = 0;
            unit_sprite_coord(ev->unlocked_units[i], &col, &row);
            blit_mini_sprite(fb, g, ax + 12, ay - 14, col, row);
            char buf[64];
            snprintf(buf, sizeof buf, "%s 單位",
                     civ_unit_name_zh((civ_unit_type_t)ev->unlocked_units[i]));
            civ_text_out(fb, g->font_body, ax + 32, ay, buf,
                         c_red, c_bg, CIV_TEXT_BK_TRANSPARENT);
            ay += 22;
        }

        /* improvement (綠) + sprite */
        for (int i = 0; i < 4 && ev->unlocked_imp[i]; i++) {
            int col = 0, row = 0;
            building_sprite_coord(ev->unlocked_imp[i], &col, &row);
            blit_mini_sprite(fb, g, ax + 12, ay - 14, col, row);
            civ_text_out(fb, g->font_body, ax + 32, ay,
                         civ_building_name_zh(ev->unlocked_imp[i]),
                         c_green, c_bg, CIV_TEXT_BK_TRANSPARENT);
            ay += 22;
        }

        /* wonder (綠) + sprite */
        for (int i = 0; i < 4 && ev->unlocked_wonder[i]; i++) {
            int col = 0, row = 0;
            wonder_sprite_coord(ev->unlocked_wonder[i], &col, &row);
            blit_mini_sprite(fb, g, ax + 12, ay - 14, col, row);
            const char *wname = "巨像 奇蹟";
            if (ev->unlocked_wonder[i] == 2) wname = "金字塔 奇蹟";
            civ_text_out(fb, g->font_body, ax + 32, ay, wname,
                         c_green, c_bg, CIV_TEXT_BK_TRANSPARENT);
            ay += 22;
        }
    }

    /* 底部 hint (對齊 reference 右下劍形 cursor 位置) */
    if (g->font_body) {
        const char *hint = "按 Enter / ESC 繼續";
        int hw = civ_text_measure(g->font_body, hint);
        civ_text_out(fb, g->font_body,
                     (TS_W - hw) / 2, TS_H - 38,
                     hint, c_dgrey, c_bg, CIV_TEXT_BK_TRANSPARENT);
    }
    (void)c_white;
}
