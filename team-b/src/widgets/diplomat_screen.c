#include "diplomat_screen.h"
#include "../civ_game.h"
#include "../gfx/palette.h"
#include "../gfx/primitive.h"
#include "../text/text_out.h"
#include "../world/diplomat.h"

#include <stdio.h>
#include <string.h>

#define DS_W 640
#define DS_H 480
#define DS_DIALOG_Y 360   /* 對話區起點 y */
#define DS_SPEAR_W  44

static uint8_t pn(struct civ_game *g, uint8_t r, uint8_t g_, uint8_t b)
{
    return civ_palette_nearest_rgb(&g->palette, r, g_, b);
}

/* R18: 山地 horizon — 自畫 jagged mountain silhouette + sky gradient.
 * 對齊 reference 圖片山坡輪廓 (起伏 + 遠處 fade). */
static void paint_sky_mountain(civ_surface_t *fb, struct civ_game *g,
                                int x0, int y0, int x1, int y1)
{
    uint8_t c_sky_top    = pn(g, 0xA0, 0xB0, 0xC0);   /* 灰藍 */
    uint8_t c_sky_mid    = pn(g, 0xB0, 0xB0, 0xB0);   /* 中間 grey */
    uint8_t c_sky_low    = pn(g, 0x80, 0x90, 0xA0);
    uint8_t c_mnt_far    = pn(g, 0x60, 0x70, 0x80);   /* 遠山 fade */
    uint8_t c_mnt_near   = pn(g, 0x40, 0x50, 0x60);   /* 近山 深 */
    uint8_t c_grass      = pn(g, 0x60, 0x70, 0x40);

    int h = y1 - y0;

    /* 1. sky gradient (3 段) — top → mid → low */
    for (int yy = y0; yy < y0 + h * 30 / 100; yy++) {
        civ_hline(fb, x0, yy, x1 - x0, c_sky_top);
    }
    for (int yy = y0 + h * 30 / 100; yy < y0 + h * 50 / 100; yy++) {
        civ_hline(fb, x0, yy, x1 - x0, c_sky_mid);
    }
    for (int yy = y0 + h * 50 / 100; yy < y0 + h * 60 / 100; yy++) {
        civ_hline(fb, x0, yy, x1 - x0, c_sky_low);
    }

    /* 2. 遠山 silhouette — jagged 波形 */
    int horizon = y0 + h * 60 / 100;
    int mnt_h = 36;
    for (int xx = x0; xx < x1; xx++) {
        /* triangular wave (2 個峰) */
        int phase = (xx - x0) * 360 / (x1 - x0);   /* 0..359 */
        int wave;
        if (phase < 90)       wave = phase * 100 / 90;
        else if (phase < 180) wave = (180 - phase) * 100 / 90;
        else if (phase < 270) wave = (phase - 180) * 100 / 90;
        else                  wave = (360 - phase) * 100 / 90;
        int mtop = horizon - mnt_h * wave / 100;
        for (int yy = mtop; yy < horizon + 12; yy++) {
            if (yy >= y0 && yy < y1)
                fb->pixels[yy * fb->pitch + xx] = c_mnt_far;
        }
    }

    /* 3. 近山 silhouette — 較低 + 較深 */
    int horizon2 = y0 + h * 75 / 100;
    int mnt2_h = 28;
    for (int xx = x0; xx < x1; xx++) {
        int phase = (xx - x0 + 60) * 540 / (x1 - x0);
        int wave;
        if (phase < 90)       wave = phase * 100 / 90;
        else if (phase < 180) wave = (180 - phase) * 100 / 90;
        else if (phase < 270) wave = (phase - 180) * 100 / 90;
        else                  wave = ((phase % 360) - 180) * 100 / 90;
        if (wave < 0) wave = -wave;
        if (wave > 100) wave = 100;
        int mtop = horizon2 - mnt2_h * wave / 100;
        for (int yy = mtop; yy < y1; yy++) {
            if (yy >= y0 && yy < y1)
                fb->pixels[yy * fb->pitch + xx] = c_mnt_near;
        }
    }

    /* 4. foreground 草地 — 最後 10% */
    for (int yy = y0 + h * 92 / 100; yy < y1; yy++) {
        civ_hline(fb, x0, yy, x1 - x0, c_grass);
    }
}

/* R18: leader 大頭像占位 — 中央矩形 + 服裝色 + 大字 icon (e.g. "英") +
 * 簡化頭部外框. 對齊 reference 領袖中央構圖. */
static void paint_leader_portrait(civ_surface_t *fb, struct civ_game *g,
                                   civ_leader_id_t leader)
{
    uint8_t lr, lg, lb;
    civ_leader_palette(leader, &lr, &lg, &lb);

    uint8_t c_body   = pn(g, lr, lg, lb);
    uint8_t c_skin   = pn(g, 0xE0, 0xC0, 0xA0);
    uint8_t c_hair   = leader == CIV_LEADER_FREDERICK
                          ? pn(g, 0xF0, 0xF0, 0xE0)   /* 銀白假髮 */
                          : pn(g, 0xC0, 0x60, 0x20);  /* Elizabeth 紅髮 */
    uint8_t c_dark   = pn(g, 0x20, 0x20, 0x20);
    uint8_t c_gold   = pn(g, 0xE0, 0xC0, 0x40);

    /* 服裝大方塊 — 中央偏下 */
    civ_rect_t body_r = { 220, 200, 200, 160 };
    civ_fill_rect(fb, body_r, c_body);
    civ_frame_rect(fb, body_r, c_dark);

    /* 領口 — 三角形簡化用兩個 rect */
    civ_fill_rect(fb, (civ_rect_t){260, 200, 120, 18}, c_skin);
    civ_fill_rect(fb, (civ_rect_t){280, 200, 80, 12}, c_body);

    /* 頭部 — 圓形近似用 ellipse-ish rect + 4 corner mask */
    civ_rect_t head_r = { 270, 90, 100, 120 };
    civ_fill_rect(fb, head_r, c_skin);
    civ_frame_rect(fb, head_r, c_dark);

    /* 髮 — 頭頂半圓 */
    civ_fill_rect(fb, (civ_rect_t){head_r.x - 10, head_r.y - 10,
                                    head_r.w + 20, 36}, c_hair);
    civ_frame_rect(fb, (civ_rect_t){head_r.x - 10, head_r.y - 10,
                                     head_r.w + 20, 36}, c_dark);

    /* 服飾 ornament (中央徽章 — Frederick 銀星 / Elizabeth 寶石) */
    civ_fill_rect(fb, (civ_rect_t){308, 260, 24, 24}, c_gold);
    civ_frame_rect(fb, (civ_rect_t){308, 260, 24, 24}, c_dark);

    /* R18: 中央放 1 字 icon (e.g. "英" 在徽章下方) — 對齊 m9 tech showcase */
    if (g->font_title) {
        const char *ic = civ_leader_icon_char_zh(leader);
        int iw = civ_text_measure(g->font_title, ic);
        int ix = body_r.x + (body_r.w - iw) / 2;
        int iy = body_r.y + body_r.h - 20;
        /* drop shadow */
        civ_text_out(fb, g->font_title, ix + 1, iy + 1, ic,
                     c_dark, c_body, CIV_TEXT_BK_TRANSPARENT);
        civ_text_out(fb, g->font_title, ix, iy, ic,
                     c_gold, c_body, CIV_TEXT_BK_TRANSPARENT);
    }
}

/* R18: 兩側 advisor 占位 — 較小頭像, 對齊 reference 圖左右兵士 */
static void paint_advisor(civ_surface_t *fb, struct civ_game *g,
                           int cx, uint8_t robe_r, uint8_t robe_g, uint8_t robe_b)
{
    uint8_t c_robe = pn(g, robe_r, robe_g, robe_b);
    uint8_t c_skin = pn(g, 0xE0, 0xC0, 0xA0);
    uint8_t c_dark = pn(g, 0x20, 0x20, 0x20);
    uint8_t c_hair = pn(g, 0x40, 0x30, 0x20);

    /* 服 */
    civ_rect_t body = { cx - 36, 200, 72, 160 };
    civ_fill_rect(fb, body, c_robe);
    civ_frame_rect(fb, body, c_dark);

    /* 頭 */
    civ_rect_t head = { cx - 24, 130, 48, 70 };
    civ_fill_rect(fb, head, c_skin);
    civ_frame_rect(fb, head, c_dark);

    /* 髮 */
    civ_fill_rect(fb, (civ_rect_t){head.x - 4, head.y - 4, head.w + 8, 16}, c_hair);
}

/* R18: 左右 spear ornament — 黑底 + 紅斜紋 + 中央矛 (banded color) */
static void paint_spear(civ_surface_t *fb, struct civ_game *g,
                         int x, int y, int w, int h)
{
    uint8_t c_black  = pn(g, 0x00, 0x00, 0x00);
    uint8_t c_red    = pn(g, 0x80, 0x00, 0x00);
    uint8_t c_brown  = pn(g, 0x80, 0x50, 0x20);
    uint8_t c_silver = pn(g, 0xC0, 0xC0, 0xC0);

    /* 黑底 */
    civ_fill_rect(fb, (civ_rect_t){x, y, w, h}, c_black);

    /* 斜紋 — 每 8 像素紅斜線 (對角) */
    for (int yy = y; yy < y + h; yy++) {
        for (int xx = x; xx < x + w; xx++) {
            if (((xx - yy) & 7) == 0) {
                if (xx >= 0 && xx < fb->w && yy >= 0 && yy < fb->h)
                    fb->pixels[yy * fb->pitch + xx] = c_red;
            }
        }
    }

    /* 中央矛桿 — vertical brown line + tip silver */
    int sx = x + w / 2;
    civ_vline(fb, sx, y + 12, h - 24, c_brown);
    civ_vline(fb, sx + 1, y + 12, h - 24, c_brown);
    /* 矛頭 — 銀色三角形簡化用 small rect */
    civ_fill_rect(fb, (civ_rect_t){sx - 4, y + 6, 9, 12}, c_silver);
    civ_frame_rect(fb, (civ_rect_t){sx - 4, y + 6, 9, 12}, c_black);
}

/* R18: parchment 對話框 — 棕底 + 立體邊 + 暗紅 serif 對話文字 + 底部劍紋 */
static void paint_parchment(civ_surface_t *fb, struct civ_game *g,
                             int x0, int y0, int x1, int y1,
                             const char *dialog)
{
    uint8_t c_p_base = pn(g, 0xD0, 0xA0, 0x60);   /* 棕 parchment */
    uint8_t c_p_dark = pn(g, 0x80, 0x50, 0x20);   /* 棕邊 */
    uint8_t c_text   = pn(g, 0x80, 0x10, 0x10);   /* 暗紅 serif */
    uint8_t c_text2  = pn(g, 0x40, 0x10, 0x10);   /* drop shadow */
    uint8_t c_orn    = pn(g, 0xA0, 0x70, 0x30);   /* 底部劍紋 */

    /* 主底 */
    civ_fill_rect(fb, (civ_rect_t){x0, y0, x1 - x0, y1 - y0}, c_p_base);
    /* 外框 */
    civ_frame_rect(fb, (civ_rect_t){x0, y0, x1 - x0, y1 - y0}, c_p_dark);
    civ_frame_rect(fb,
        (civ_rect_t){x0 + 1, y0 + 1, x1 - x0 - 2, y1 - y0 - 2}, c_p_dark);

    /* 底部劍紋 ornament — 重複交叉劍簡化版 */
    int orn_y = y1 - 28;
    for (int i = 0; i < 8; i++) {
        int ox = x0 + 20 + i * ((x1 - x0 - 40) / 8);
        /* 劍身 — vertical 細線 */
        civ_vline(fb, ox,     orn_y, 20, c_orn);
        civ_vline(fb, ox + 12, orn_y, 20, c_orn);
        /* 劍柄 — horizontal */
        civ_hline(fb, ox - 4, orn_y + 4,  20, c_orn);
        civ_hline(fb, ox - 4, orn_y + 5,  20, c_orn);
    }

    /* 對話文字 — 雙寫加粗 + drop shadow */
    if (g->font_body && dialog && dialog[0]) {
        int tx = x0 + 24;
        int ty = y0 + 28;
        /* shadow 1px */
        civ_text_out(fb, g->font_body, tx + 1, ty + 1, dialog,
                     c_text2, c_p_base, CIV_TEXT_BK_TRANSPARENT);
        /* main */
        civ_text_out(fb, g->font_body, tx, ty, dialog,
                     c_text, c_p_base, CIV_TEXT_BK_TRANSPARENT);
        /* bold 雙寫: stamp 1px x offset */
        civ_text_out(fb, g->font_body, tx + 1, ty, dialog,
                     c_text, c_p_base, CIV_TEXT_BK_TRANSPARENT);
    }
}

void civ_diplomat_screen_render(struct civ_game *g, civ_surface_t *fb)
{
    if (!g || !g->diplomat_screen_open) return;
    const civ_diplomat_event_t *ev = &g->diplomat_screen_event;

    /* === 上半 (y 0..360) sky + mountain + leader + advisors === */
    paint_sky_mountain(fb, g, 0, 0, DS_W, DS_DIALOG_Y);

    /* 兩側 advisor — 左 grey robe / 右 blue robe (對齊 reference) */
    paint_advisor(fb, g, 110, 0x60, 0x60, 0x70);
    paint_advisor(fb, g, 530, 0x90, 0x80, 0x70);

    /* 中央領袖 */
    paint_leader_portrait(fb, g, ev->leader);

    /* === 下半 (y 360..480) 對話區 === */
    /* 左 spear (x 0..44) */
    paint_spear(fb, g, 0, DS_DIALOG_Y, DS_SPEAR_W, DS_H - DS_DIALOG_Y);
    /* 右 spear (x 596..640) */
    paint_spear(fb, g, DS_W - DS_SPEAR_W, DS_DIALOG_Y,
                DS_SPEAR_W, DS_H - DS_DIALOG_Y);
    /* 中央 parchment */
    const char *dialog = civ_diplomat_dialog_zh(ev);
    paint_parchment(fb, g, DS_SPEAR_W, DS_DIALOG_Y,
                    DS_W - DS_SPEAR_W, DS_H, dialog);

    /* 底部 hint (parchment 下緣) */
    if (g->font_body) {
        uint8_t c_dim = pn(g, 0x40, 0x20, 0x10);
        const char *hint = "按 Enter / ESC 繼續";
        int hw = civ_text_measure(g->font_body, hint);
        civ_text_out(fb, g->font_body,
                     (DS_W - hw) / 2, DS_H - 8,
                     hint, c_dim, c_dim, CIV_TEXT_BK_TRANSPARENT);
    }
}
