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

/* R19: leader 大頭像 — 優先用原版 KING00..13 CvPc sprite (從 CIVDATA2 載入,
 * 緩存在 g->leader_portraits[]). sprite 未載入時 fallback clean-room 自畫.
 *
 * 對齊 reference 構圖: 中央領袖佔上半 60-70% 高度. 原版 sprite size 約 240×200
 * (320×200 CvPc 是全幅 splash, KING 是肖像剪裁版). blit 居中. */
static void paint_leader_portrait_clean_room(civ_surface_t *fb, struct civ_game *g,
                                              civ_leader_id_t leader)
{
    uint8_t lr, lg, lb;
    civ_leader_palette(leader, &lr, &lg, &lb);

    uint8_t c_body   = pn(g, lr, lg, lb);
    uint8_t c_skin   = pn(g, 0xE0, 0xC0, 0xA0);
    uint8_t c_hair   = leader == CIV_LEADER_FREDERICK
                          ? pn(g, 0xF0, 0xF0, 0xE0)
                          : pn(g, 0xC0, 0x60, 0x20);
    uint8_t c_dark   = pn(g, 0x20, 0x20, 0x20);
    uint8_t c_gold   = pn(g, 0xE0, 0xC0, 0x40);

    civ_rect_t body_r = { 220, 200, 200, 160 };
    civ_fill_rect(fb, body_r, c_body);
    civ_frame_rect(fb, body_r, c_dark);
    civ_fill_rect(fb, (civ_rect_t){260, 200, 120, 18}, c_skin);
    civ_fill_rect(fb, (civ_rect_t){280, 200, 80, 12}, c_body);

    civ_rect_t head_r = { 270, 90, 100, 120 };
    civ_fill_rect(fb, head_r, c_skin);
    civ_frame_rect(fb, head_r, c_dark);
    civ_fill_rect(fb, (civ_rect_t){head_r.x - 10, head_r.y - 10,
                                    head_r.w + 20, 36}, c_hair);
    civ_frame_rect(fb, (civ_rect_t){head_r.x - 10, head_r.y - 10,
                                     head_r.w + 20, 36}, c_dark);

    civ_fill_rect(fb, (civ_rect_t){308, 260, 24, 24}, c_gold);
    civ_frame_rect(fb, (civ_rect_t){308, 260, 24, 24}, c_dark);

    if (g->font_title) {
        const char *ic = civ_leader_icon_char_zh(leader);
        int iw = civ_text_measure(g->font_title, ic);
        int ix = body_r.x + (body_r.w - iw) / 2;
        int iy = body_r.y + body_r.h - 20;
        civ_text_out(fb, g->font_title, ix + 1, iy + 1, ic,
                     c_dark, c_body, CIV_TEXT_BK_TRANSPARENT);
        civ_text_out(fb, g->font_title, ix, iy, ic,
                     c_gold, c_body, CIV_TEXT_BK_TRANSPARENT);
    }
}

/* R19: scaled blit + remap + skip transparent.
 * KING sprite layout: spec 03 §3.1 內 427×320 = 5col × 4row 動畫 frame +
 * 右下角 1 個大主肖像 (約 165×235 @ (260,80)). 對話畫面顯示大主肖像. */
static void blit_scaled_remap_skip(civ_surface_t *fb, int dst_x, int dst_y,
                                    int dst_w, int dst_h,
                                    const civ_surface_t *src, civ_rect_t src_r,
                                    const uint8_t lut[256],
                                    const uint8_t skip[256])
{
    for (int yy = 0; yy < dst_h; yy++) {
        int sy = src_r.y + yy * src_r.h / dst_h;
        if (sy < 0 || sy >= src->h) continue;
        for (int xx = 0; xx < dst_w; xx++) {
            int sx = src_r.x + xx * src_r.w / dst_w;
            if (sx < 0 || sx >= src->w) continue;
            int dx = dst_x + xx, dy = dst_y + yy;
            if (dx < 0 || dx >= fb->w || dy < 0 || dy >= fb->h) continue;
            uint8_t sidx = src->pixels[sy * src->pitch + sx];
            if (skip[sidx]) continue;
            fb->pixels[dy * fb->pitch + dx] = lut[sidx];
        }
    }
}

static void paint_leader_portrait(civ_surface_t *fb, struct civ_game *g,
                                   civ_leader_id_t leader)
{
    civ_surface_t *king = NULL;
    if ((int)leader >= 1 && (int)leader <= CIV_LEADER_COUNT) {
        king = g->leader_portraits[leader];
    }
    if (!king) {
        paint_leader_portrait_clean_room(fb, g, leader);
        return;
    }

    /* R20: KING sprite 427×320 layout (對齊 spec 03 + agent 視覺辨識):
     *   - 5 col × 4 row 動畫 modular frame (head/eyes/mouth/expression)
     *     在 左側 + 上方 (x=0..250, y=0..280)
     *   - **大主肖像** 位於 right-bottom 區 (x=250..420, y=110..315)
     *     ≈ 170×205 (對 Elizabeth 已視覺驗證)
     *   - 透明背景 = palette idx 0 (Civ1 sentinel pixel, spec 03 §3.5.1)
     *
     * R20 改用 in-place build LUT: 對齊當前 g.palette (test 已在 diplomat
     * mode 安裝 king palette → LUT identity, 100% accurate). */
    civ_rect_t src = { 250, 110, 170, 205 };

    /* 1.5x scale → 255×308. 居中放上半 */
    int dst_h = 308;
    int dst_w = dst_h * src.w / src.h;
    if (dst_h > DS_DIALOG_Y - 10) {
        dst_h = DS_DIALOG_Y - 10;
        dst_w = dst_h * src.w / src.h;
    }
    int dst_x = (DS_W - dst_w) / 2;
    int dst_y = DS_DIALOG_Y - dst_h - 4;

    /* R20: build LUT in-place (每次 render). king_pal → 當前 g.palette.
     * 若 g.palette 已切換為 king palette (R20 diplomat showcase mode),
     * LUT 為 identity (1:1). 若 g.palette 還是其他 (e.g. sprite_sheet 共用),
     * LUT 是 nearest match (顏色會被 quantize 但結構保留). */
    uint8_t lut[256];
    civ_palette_build_lut(g->leader_king_palettes[leader].entries, 256,
                          &g->palette, lut);

    /* skip mask: idx 0 是 Civ1 sentinel transparent (spec 03 §3.5.1) */
    uint8_t skip[256];
    memset(skip, 0, sizeof skip);
    skip[0] = 1;

    blit_scaled_remap_skip(fb, dst_x, dst_y, dst_w, dst_h, king, src,
                           lut, skip);
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

    /* R19: 若原版 KING sprite 已 cache, 直接 blit 大幅 PC_29 場景 (內含 advisors
     * + 領袖 + 山地 horizon, 約 320×200 或全幅). 否則 fallback 三層 clean-room. */
    civ_surface_t *king = NULL;
    if ((int)ev->leader >= 1 && (int)ev->leader <= CIV_LEADER_COUNT)
        king = g->leader_portraits[ev->leader];

    /* === 上半 (y 0..360) sky + mountain horizon + 左右 advisor + leader === */
    paint_sky_mountain(fb, g, 0, 0, DS_W, DS_DIALOG_Y);
    paint_advisor(fb, g, 90, 0x60, 0x60, 0x70);
    paint_advisor(fb, g, 550, 0x90, 0x80, 0x70);
    /* leader 用真 KING sprite (R19) 或 fallback clean-room */
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
