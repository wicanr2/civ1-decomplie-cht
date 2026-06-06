/*
 * test_demo_snapshot.c — M0..M4 視覺驗證
 *
 * Headless dummy driver；產出對應 milestone 的 PPM：
 *   m4_demo.ppm — CIV1_DATA_DIR 設定時：14 文明 + 領袖肖像 + 中文名
 *   m3_demo.ppm — EARTH worldmap blit（CIV1_DATA_DIR 設但無 STR#）
 *   m2_demo.ppm — widget-only layout（無 CIV1_DATA_DIR）
 */
#include "civ_game.h"
#include "civ_widgets.h"

#include "data/civs.h"
#include "data/strings.h"
#include "gfx/palette.h"
#include "gfx/present.h"
#include "gfx/primitive.h"
#include "gfx/surface.h"
#include "res/loader.h"
#include "res/rsrcfork.h"
#include "text/big5.h"
#include "text/glyph_cache.h"
#include "text/text_out.h"
#include "widgets/map.h"

#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define FB_W 640
#define FB_H 480
#define TITLE_H 40

#ifndef CIV_DEFAULT_FONT_PATH
#define CIV_DEFAULT_FONT_PATH "/usr/share/fonts/truetype/arphic/uming.ttc"
#endif

static int file_exists(const char *p) { struct stat s; return stat(p,&s)==0; }

static void paint_background(struct civ_game *g)
{
    civ_surface_t *fb = g->framebuffer;
    civ_surface_clear(fb, 15);
    civ_fill_rect(fb, (civ_rect_t){0, 0, FB_W, TITLE_H}, 9);
    civ_hline(fb, 0, TITLE_H, FB_W, 0);
    if (g->font_title) {
        const char *t = "文明帝國 視窗版 Civilization for Windows";
        int w = civ_text_measure(g->font_title, t);
        int x = (FB_W - w) / 2;
        civ_text_out(fb, g->font_title, x, 28, t, 15, 9,
                     CIV_TEXT_BK_TRANSPARENT);
    }
}

/* M4：載入 14 文明 master table + KING* 領袖肖像 + 中文名 ─────── */

#define FOURCC_STR_HASH CIV_FOURCC('S','T','R','#')

static int try_m4_civ_list(struct civ_game *g)
{
    const char *dir = getenv("CIV1_DATA_DIR");
    if (!dir || !*dir) return 0;

    /* 開 Civdata0.RSC 抽 STR# */
    char path[1024];
    snprintf(path, sizeof path, "%s/Civdata0.RSC", dir);
    civ_rsrc_t *r0 = civ_rsrc_open(path);
    if (!r0) {
        snprintf(path, sizeof path, "%s/CIVDATA0.RSC", dir);
        r0 = civ_rsrc_open(path);
    }
    if (!r0) return 0;

    civ_strlist_t leaders = {0}, army_sing = {0}, army_plur = {0};
    civ_strlist_t civ_sing = {0}, civ_plur = {0};
    const civ_rsrc_entry_t *e;
    if (!(e = civ_rsrc_find(r0, FOURCC_STR_HASH, CIV_STR_LEADERS))) goto fail;
    civ_strlist_parse(e->data, e->len, CIV_STR_LEADERS, &leaders);
    if (!(e = civ_rsrc_find(r0, FOURCC_STR_HASH, CIV_STR_ARMY_SING))) goto fail;
    civ_strlist_parse(e->data, e->len, CIV_STR_ARMY_SING, &army_sing);
    if (!(e = civ_rsrc_find(r0, FOURCC_STR_HASH, CIV_STR_ARMY_PLUR))) goto fail;
    civ_strlist_parse(e->data, e->len, CIV_STR_ARMY_PLUR, &army_plur);
    if (!(e = civ_rsrc_find(r0, FOURCC_STR_HASH, CIV_STR_LEADERS_CIV_SING))) goto fail;
    civ_strlist_parse(e->data, e->len, CIV_STR_LEADERS_CIV_SING, &civ_sing);
    if (!(e = civ_rsrc_find(r0, FOURCC_STR_HASH, CIV_STR_LEADERS_CIV_PLUR))) goto fail;
    civ_strlist_parse(e->data, e->len, CIV_STR_LEADERS_CIV_PLUR, &civ_plur);

    civ_civ_entry_t civs[CIV_NUM_CIVS] = {0};
    civ_civs_build(&leaders, &army_sing, &army_plur, &civ_sing, &civ_plur, civs);

    /* 開 CIVDATA2.RSC 載 KING00..13 領袖肖像 */
    snprintf(path, sizeof path, "%s/CIVDATA2.RSC", dir);
    civ_rsrc_t *r2 = civ_rsrc_open(path);
    if (!r2) {
        snprintf(path, sizeof path, "%s/Civdata2.RSC", dir);
        r2 = civ_rsrc_open(path);
    }

    /* layout：主地圖區內畫一個 7×2 grid，每格放領袖頭像 + 中文文明名 */
    int gx0 = g->map_w->rect.x + 12;
    int gy0 = g->map_w->rect.y + 12;
    int cw  = (g->map_w->rect.w - 24) / 7;
    int ch  = (g->map_w->rect.h - 24) / 2;

    for (int i = 0; i < CIV_NUM_CIVS; i++) {
        int col = i % 7;
        int row = i / 7;
        int cx  = gx0 + col * cw;
        int cy  = gy0 + row * ch;

        /* 載入肖像：CIVDATA2 #500..513 = KING00..13 */
        civ_surface_t *king = NULL;
        civ_palette_t  king_pal = {0};
        if (r2) {
            int16_t king_id = (int16_t)(500 + civs[i].king_sprite_idx);
            civ_load_cvpc_by_id(r2, king_id, &king, &king_pal);
        }

        if (king) {
            /* M5：用 RGB-nearest LUT remap，**不蓋掉 g->palette** —
             * widget background 顏色保持，肖像像素逐個翻譯到 base palette */
            uint8_t lut[256];
            civ_palette_build_lut(king_pal.entries, 256, &g->palette, lut);

            civ_rect_t src = {0, 0,
                              king->w < cw - 4   ? king->w : cw - 4,
                              king->h < ch - 32  ? king->h : ch - 32};
            civ_surface_blit_remap(g->framebuffer, cx + 2, cy + 2,
                                   king, &src, lut);
            civ_surface_free(king);
        } else {
            civ_fill_rect(g->framebuffer,
                          (civ_rect_t){cx + 2, cy + 2, cw - 4, ch - 32}, 8);
        }

        /* 框 */
        civ_frame_rect(g->framebuffer, (civ_rect_t){cx, cy, cw, ch}, 0);

        /* 標籤 */
        if (g->font_body) {
            const char *zh_leader   = civ_civs_zh(civs[i].slot, "leader");
            const char *zh_civ_sing = civ_civs_zh(civs[i].slot, "civ_sing");
            char buf[64];
            if (zh_leader && zh_civ_sing) {
                snprintf(buf, sizeof buf, "%s %s", zh_civ_sing, zh_leader);
                civ_text_out(g->framebuffer, g->font_body,
                             cx + 4, cy + ch - 14, buf,
                             15, 0, CIV_TEXT_BK_TRANSPARENT);
            }
            /* 英文名小字 — 顯示原 STR# 內容 */
            if (civs[i].leader_en) {
                civ_text_out(g->framebuffer, g->font_body,
                             cx + 4, cy + ch - 2,
                             civs[i].leader_en,
                             7, 0, CIV_TEXT_BK_TRANSPARENT);
            }
        }
    }

    civ_strlist_free(&leaders);
    civ_strlist_free(&army_sing);
    civ_strlist_free(&army_plur);
    civ_strlist_free(&civ_sing);
    civ_strlist_free(&civ_plur);
    if (r2) civ_rsrc_close(r2);
    civ_rsrc_close(r0);
    return 1;

fail:
    civ_strlist_free(&leaders);
    civ_strlist_free(&army_sing);
    civ_strlist_free(&army_plur);
    civ_strlist_free(&civ_sing);
    civ_strlist_free(&civ_plur);
    civ_rsrc_close(r0);
    return 0;
}

static int write_ppm(const char *path,
                     const civ_surface_t *fb,
                     const civ_palette_t *pal)
{
    FILE *f = fopen(path, "wb");
    if (!f) return -1;
    fprintf(f, "P6\n%d %d\n255\n", fb->w, fb->h);
    for (int y = 0; y < fb->h; y++) {
        const uint8_t *row = fb->pixels + (size_t)y * fb->pitch;
        for (int x = 0; x < fb->w; x++) {
            civ_color_t c = pal->entries[row[x]];
            uint8_t rgb[3] = { c.r, c.g, c.b };
            fwrite(rgb, 1, 3, f);
        }
    }
    fclose(f);
    return 0;
}

int main(int argc, char **argv)
{
    SDL_setenv("SDL_VIDEODRIVER", "dummy", 0);
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
        return 1;
    }

    struct civ_game g = {0};
    g.framebuffer = civ_surface_new(FB_W, FB_H);
    if (!g.framebuffer) { fprintf(stderr, "surface_new\n"); return 1; }
    civ_palette_default(&g.palette);
    if (file_exists(CIV_DEFAULT_FONT_PATH)) {
        g.font_title = civ_font_open(CIV_DEFAULT_FONT_PATH, 24);
        g.font_body  = civ_font_open(CIV_DEFAULT_FONT_PATH, 14);
    }
    g.tick_count = 12345;

    if (civ_widgets_register(&g) != 0) {
        fprintf(stderr, "widgets register failed\n"); return 1;
    }

    paint_background(&g);
    civ_widgets_render_all(&g);
    int m4 = try_m4_civ_list(&g);

    const char *out_path = (argc > 1) ? argv[1]
                                       : (m4 ? "m4_demo.ppm" : "m2_demo.ppm");
    if (write_ppm(out_path, g.framebuffer, &g.palette) < 0) {
        fprintf(stderr, "write %s failed\n", out_path);
        return 1;
    }
    printf("PASS test_demo_snapshot → %s (%d×%d, M4 civ list=%s)\n",
           out_path, FB_W, FB_H, m4 ? "yes" : "no");

    civ_widgets_unregister(&g);
    if (g.font_title) civ_font_close(g.font_title);
    if (g.font_body)  civ_font_close(g.font_body);
    civ_big5_cleanup();
    civ_surface_free(g.framebuffer);
    SDL_Quit();
    return 0;
}
