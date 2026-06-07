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

/* R25: 對 palette transparent pixel idx — 只 skip idx 0.
 *
 * 對齊 spec 03 §3.5.1 Civ1 sentinel pixel convention + OpenCivOne 邏輯
 * (.NET 版 `Color.FromArgb(0, ...)` 對 palette[0] 設 alpha=0).
 *
 * R21 曾加 RGB scan magenta detection (R≥E0 && G≤40 && B≥E0) 是錯改動:
 *   - 真實透明色 = sprite 自身 palette idx 0 (sentinel pixel)
 *   - Elizabeth 紅華服深紅 (R≈B0, G≈10, B≈30) RGB 距離 magenta 不遠,
 *     palette 內若有 (E8, 20, E0) 級 entry 會被誤殺成透明 → 紅華服缺角
 *   - R21 之所以加 RGB scan 是因為 R20 前 g.palette 還沒切到 KING 自身 palette,
 *     導致 idx 0 mapping 不對; R20 之後 palette 已正確安裝, RGB scan 變多餘
 *
 * R25 改回單純只 skip idx 0. */
static void build_skip_mask(const civ_palette_t *pal, uint8_t skip[256])
{
    (void)pal;
    memset(skip, 0, 256);
    skip[0] = 1;
}

/* R19: scaled blit + remap + skip transparent. */
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

    /* R20: build LUT in-place (每次 render). king_pal → 當前 g.palette. */
    uint8_t lut[256];
    civ_palette_build_lut(g->leader_king_palettes[leader].entries, 256,
                          &g->palette, lut);

    /* R27-fix: KING sprite 的透明 sentinel 不在 idx 0 (R21 觀察到的大片粉紅
     * 是 KING 自身 palette 的背景 magenta entry). 但 4 角同色採樣不適用於
     * 領袖肖像 — 衣服往往延到 sprite 底部, 只有 top-left + top-right 是 bg.
     *
     * 改用 top-row 採樣: scan 整條 top row, 找最常出現 (>50%) 的 idx 認定為
     * background sentinel. 紅裙在中段, 不會在 top row 出現 majority, 安全. */
    uint8_t skip[256];
    build_skip_mask(&g->leader_king_palettes[leader], skip);
    {
        int row_y = src.y;
        if (row_y >= 0 && row_y < king->h) {
            int hist[256] = {0};
            int xstart = src.x < 0 ? 0 : src.x;
            int xend   = src.x + src.w;
            if (xend > king->w) xend = king->w;
            for (int xx = xstart; xx < xend; xx++) {
                hist[king->pixels[row_y * king->pitch + xx]]++;
            }
            int half = (xend - xstart) / 2;
            for (int i = 0; i < 256; i++) {
                if (hist[i] > half) { skip[i] = 1; break; }
            }
        }
    }

    blit_scaled_remap_skip(fb, dst_x, dst_y, dst_w, dst_h, king, src,
                           lut, skip);
}

/* R30 (C6 gap): blit one of 4 advisor figures from GOVT*M right half.
 * 對齊 _govt0m_dump.png 重新分析的真實 layout:
 *   - 上方 y=0..150: 8 個 expression mug-shot 動畫格 (talking advisor frames)
 *     → 完全不取, 避免 R23 拍到的「4 個小臉疊在 advisor 上方」bug
 *   - 下方 y=150..320: 4 個立姿 advisor (各約 100×170)
 *     idx 0 = 軍人 (army green, 帽子)
 *     idx 1 = 黑幫 (brown jacket, sunglasses)
 *     idx 2 = 紅衣金髮女
 *     idx 3 = 西裝戴墨鏡男
 *   - x 起始: 480 / 600 / 720 / 840 (各約 115 wide, 不重疊)
 *
 * R30 default 取 advisor 0 (軍人) 為左仕從, advisor 1 (黑幫) 為右仕從.
 * R23 原本左 0 右 3 跨度太大讓構圖右側多了不協調的「西裝男」, 改用 0+1
 * 兩個比較像 1993 reference 的 advisor pair. */
static void paint_govt_advisor(civ_surface_t *fb, struct civ_game *g,
                                int govt_idx, int advisor_idx,
                                int dst_x, int dst_y, int dst_w, int dst_h)
{
    if (govt_idx < 0 || govt_idx >= 3) govt_idx = 1;
    if (advisor_idx < 0 || advisor_idx >= 4) advisor_idx = 0;
    civ_surface_t *gb = g->govt_backdrops[govt_idx];
    if (!gb) return;

    /* R30: 4 advisor 在 GOVT*M 右半的 src coords (per 939×320 sheet).
     * x 從 480 起每 120 一格, y 從 150 起避開 mug-shot grid. */
    static const int ADV_X[4] = { 480, 600, 720, 840 };
    civ_rect_t src = { ADV_X[advisor_idx], 150, 100, 170 };
    if (src.x + src.w > gb->w) src.w = gb->w - src.x;
    if (src.y + src.h > gb->h) src.h = gb->h - src.y;

    uint8_t lut[256];
    civ_palette_build_lut(g->govt_palettes[govt_idx].entries, 256,
                          &g->palette, lut);
    uint8_t skip[256];
    build_skip_mask(&g->govt_palettes[govt_idx], skip);

    /* R27-fix: GOVT*M 右半 advisor 區的背景 sentinel 不是 palette idx 0
     * (左半 backdrop 用 idx 0, 但右半 advisor 用 cyan-ish 背景).
     * 採樣 source rect 4 角當該 region 的 background skip entry —
     * 避免 R25 後 cyan 大片漏進 dst.
     *
     * 為了避免採樣到非背景的角落 (e.g. advisor 衣袖伸到頂部) 而誤殺色彩,
     * 只當 4 角同色 (或至少 3 角同色) 時才認定為背景 sentinel. */
    {
        int cx0 = src.x, cx1 = src.x + src.w - 1;
        int cy0 = src.y, cy1 = src.y + src.h - 1;
        if (cx1 >= gb->w) cx1 = gb->w - 1;
        if (cy1 >= gb->h) cy1 = gb->h - 1;
        uint8_t corner[4] = {
            gb->pixels[cy0 * gb->pitch + cx0],
            gb->pixels[cy0 * gb->pitch + cx1],
            gb->pixels[cy1 * gb->pitch + cx0],
            gb->pixels[cy1 * gb->pitch + cx1],
        };
        for (int i = 0; i < 4; i++) {
            int hits = 0;
            for (int j = 0; j < 4; j++) if (corner[j] == corner[i]) hits++;
            if (hits >= 3) skip[corner[i]] = 1;
        }
    }

    blit_scaled_remap_skip(fb, dst_x, dst_y, dst_w, dst_h, gb, src, lut, skip);
}

/* R21: blit GOVT*M backdrop left half (scene + parchment + spear)
 * scaled to full 640x360 upper. 對應 spec 03 §3.1 內 939x320 sheet 結構:
 *   - 左半 (~0..460) = scene backdrop (sky/wall + parchment + spear ornament)
 *   - 右半 (~460..939) = advisor sprite sheet (animation + 4 standing figures)
 *
 * 政府 idx 選擇: 0=Despotism, 1=Monarchy, 2=Republic (per spec 06 §6.3).
 * R21 默認用 idx 1 (Monarchy) 因 reference webp Elizabeth/Frederick 兩位
 * 都在 monarchy era. R22 可依 player government 動態選. */
static void paint_govt_backdrop(civ_surface_t *fb, struct civ_game *g,
                                 int govt_idx)
{
    if (govt_idx < 0 || govt_idx >= 3) govt_idx = 1;
    civ_surface_t *gb = g->govt_backdrops[govt_idx];
    if (!gb) return;   /* fallback handled by caller */

    /* 左半 region: 約 0..460 × 0..320 (sheet 939×320, 左半=460 寬) */
    civ_rect_t src = { 0, 0, 460, 320 };
    if (src.w > gb->w) src.w = gb->w;
    if (src.h > gb->h) src.h = gb->h;

    /* Scale 到 640×360 upper area (DS_DIALOG_Y = 360) */
    int dst_w = DS_W;
    int dst_h = DS_DIALOG_Y;

    /* LUT govt_pal → current g.palette (caller 安裝 king_pal as g.palette,
     * 所以 govt 會 nearest-match to king_pal — 顏色稍有變化但結構正確) */
    uint8_t lut[256];
    civ_palette_build_lut(g->govt_palettes[govt_idx].entries, 256,
                          &g->palette, lut);

    /* R28-1: 使用者指正 — diplomat 背景應該完全不透明.
     * GOVT*M 左半 = full scene backdrop (sky/wall/throne/parchment), 沒有
     * sentinel pixel; idx 0 在這 region 是真實顏色不是透明標記.
     * 取消 skip mask, 每個 pixel 都 blit 過去 (idx 0 在 KING/advisor 才是
     * sentinel). 解 R27-fix 後 gandhi 背景仍漏底色問題. */
    uint8_t skip[256];
    memset(skip, 0, sizeof skip);

    blit_scaled_remap_skip(fb, 0, 0, dst_w, dst_h, gb, src, lut, skip);
}

/* R31 (C7 gap): clean-room 綠色刺繡 banner — 覆蓋 GOVT*M 底部 yellow banana
 * panel. 對齊 1993 reference webp: 領袖與對話框之間是綠地金 diamond 刺繡帶.
 *
 * pattern: 深綠底 + 中綠 diamond (8×8 重複) + 金 horizontal 邊條 + 隨意金點.
 * 完全 clean-room (Civdata 內沒有對應 sprite, 推測是原版 hardcoded paint). */
static void paint_diplomat_banner(civ_surface_t *fb, struct civ_game *g,
                                   int x0, int y0, int x1, int y1)
{
    uint8_t c_grn_dark = pn(g, 0x10, 0x40, 0x10);
    uint8_t c_grn_mid  = pn(g, 0x30, 0x70, 0x30);
    uint8_t c_grn_lt   = pn(g, 0x60, 0x90, 0x40);
    uint8_t c_gold     = pn(g, 0xE0, 0xC0, 0x40);

    /* 深綠底 */
    civ_fill_rect(fb, (civ_rect_t){x0, y0, x1 - x0, y1 - y0}, c_grn_dark);

    /* 8×8 diamond grid pattern */
    for (int yy = y0 + 4; yy < y1; yy += 8) {
        for (int xx = x0 + 4; xx < x1; xx += 16) {
            for (int dy = -3; dy <= 3; dy++) {
                int yt = yy + dy;
                if (yt < y0 || yt >= y1) continue;
                int dx_range = 3 - (dy < 0 ? -dy : dy);
                for (int dx = -dx_range; dx <= dx_range; dx++) {
                    int xt = xx + dx;
                    if (xt < x0 || xt >= x1) continue;
                    fb->pixels[yt * fb->pitch + xt] =
                        (dy == 0) ? c_grn_lt : c_grn_mid;
                }
            }
        }
    }

    /* 金邊 — 上下各一條 */
    civ_hline(fb, x0, y0,         x1 - x0, c_gold);
    civ_hline(fb, x0, y0 + 1,     x1 - x0, c_gold);
    civ_hline(fb, x0, y1 - 2,     x1 - x0, c_gold);
    civ_hline(fb, x0, y1 - 1,     x1 - x0, c_gold);
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

    /* === 上半 (y 0..360) ===
     * R21: 若 GOVT*M backdrop cached, 用原版 scene (含 parchment + spear +
     * 宮殿/山地 backdrop). 否則 fallback clean-room sky+mountain+advisors.
     * R23: 加 2 個原版 advisor 從 GOVT*M 右半切片 (左 idx 0, 右 idx 3).
     * R24: govt_idx 改用 civ_government_to_govt_idx(player_government) 動態選. */
    int govt_idx = 1;   /* fallback Monarchy if world 未 ready */
    if (g->world_ready) {
        govt_idx = civ_government_to_govt_idx(g->world.player_government);
    }
    if (g->govt_backdrops[govt_idx]) {
        /* R33: GOVT*A 左半已含完整 sky+mountain+banner+spears, 全幅 blit.
         * 不需要 R31 clean-room banner overlay (該 hack 是 R23 用錯
         * GOVT*M 時補的). */
        paint_govt_backdrop(fb, g, govt_idx);
        /* R30 (C6 gap): GOVT*A 右半 4 個 advisor (對齊 OpenCivOne
         * MeetWithKing.cs ScreenToBitmap 4 frame cache). 左 idx 0 / 右 idx 1
         * = 兩個 Despotism 古代戰士 (Greek/Egyptian 風格), 比舊 GOVT*M
         * 軍人+黑幫 配對更貼近 1993 reference. */
        paint_govt_advisor(fb, g, govt_idx, 0,  20, 110, 100, 230);
        paint_govt_advisor(fb, g, govt_idx, 1, 520, 110, 100, 230);
    } else {
        paint_sky_mountain(fb, g, 0, 0, DS_W, DS_DIALOG_Y);
        paint_advisor(fb, g, 90, 0x60, 0x60, 0x70);
        paint_advisor(fb, g, 550, 0x90, 0x80, 0x70);
    }

    /* R33: leader KING 最後畫, 確保不被任何 overlay 覆蓋成透明.
     * (R31 banner 之前在這上面, leader 透明區漏出綠色, 用者指 Elizabeth
     * 變透明 — root cause 是 R23 走錯 sheet, 用 GOVT*A 已自然解決) */
    paint_leader_portrait(fb, g, ev->leader);

    /* === 下半 (y 360..480) 對話區 === */
    /* R33: spear 回到原 y=360..480 (對話區). GOVT*A 左半已含上半部 spears. */
    paint_spear(fb, g, 0, DS_DIALOG_Y, DS_SPEAR_W, DS_H - DS_DIALOG_Y);
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
