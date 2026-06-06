/*
 * test_world_snapshot.c — M5 視覺驗證：terrain rendering + cursor + scroll
 *
 * 載入 SPR32X32 sprite sheet (CIVDATA4 #200, 1472×400)，切成 32×32
 * tile grid，把 world.terrain[] 畫進主地圖 widget。
 */
#include "civ_game.h"
#include "civ_widgets.h"

#include "gfx/palette.h"
#include "gfx/primitive.h"
#include "gfx/surface.h"
#include "res/loader.h"
#include "res/rsrcfork.h"
#include "text/big5.h"
#include "text/glyph_cache.h"
#include "text/text_out.h"
#include "widgets/city_screen.h"
#include "widgets/tech_screen.h"
#include "world/tech.h"

#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define FB_W 640
#define FB_H 480

/* R4 (2026-06-06): 對齊原版 1993 Civ Windows layout
 *   docs/screenshots/reference/civ1_win_civilopedia_dropdown.png
 *
 *   主視窗 title bar  16 px  "CIVILIZATION" (Win16 blue)
 *   主視窗 menu bar   16 px  8 items (File/Edit/Orders/Advisors/World/
 *                                    Civilopedia/City/Help)
 *   total chrome     32 px
 */
#define TITLE_H 16
#define MENU_H  16
#define CHROME_H (TITLE_H + MENU_H)

#ifndef CIV_DEFAULT_FONT_PATH
#define CIV_DEFAULT_FONT_PATH "/usr/share/fonts/truetype/arphic/uming.ttc"
#endif

static int file_exists(const char *p) { struct stat s; return stat(p,&s)==0; }

static void paint_background(struct civ_game *g)
{
    civ_surface_t *fb = g->framebuffer;

    /* R10: 用 palette_nearest 解 chrome UI 在 sheet palette idx 對不上問題.
     * 對應設計師 R10 audit P0 修法 (palette idx 1/9/0 在 sheet palette 意義
     * 不是 Win16 標準). */
    uint8_t c_title_bg = civ_palette_nearest_rgb(&g->palette, 0x00, 0x00, 0x80);
    uint8_t c_title_fg = civ_palette_nearest_rgb(&g->palette, 0xFF, 0xFF, 0xFF);
    uint8_t c_menu_bg  = civ_palette_nearest_rgb(&g->palette, 0xC0, 0xC0, 0xC0);
    uint8_t c_menu_fg  = civ_palette_nearest_rgb(&g->palette, 0x00, 0x00, 0x00);

    civ_surface_clear(fb, c_menu_bg);

    /* === Win16 主視窗 title bar @ y=0 (16 px) — Win16 active blue === */
    civ_fill_rect(fb, (civ_rect_t){0, 0, FB_W, TITLE_H}, c_title_bg);
    civ_hline(fb, 0, TITLE_H - 1, FB_W, c_menu_fg);
    if (g->font_body) {
        /* R17: 主標題中文化 — "文明帝國" 對齊使用者中文化目標 */
        const char *t = "文明帝國";
        int w = civ_text_measure(g->font_body, t);
        int x = (FB_W - w) / 2;
        civ_text_out(fb, g->font_body, x, TITLE_H - 3, t,
                     c_title_fg, c_title_bg, CIV_TEXT_BK_TRANSPARENT);
        /* min/max/close 三個小框右上角 (Win16 暗示) */
        for (int i = 0; i < 3; i++) {
            int bx = FB_W - 4 - (3 - i) * 14;
            civ_frame_rect(fb, (civ_rect_t){bx, 3, 10, 10}, c_title_fg);
        }
        /* system menu (左上角) */
        civ_frame_rect(fb, (civ_rect_t){4, 3, 10, 10}, c_title_fg);
    }

    /* === menu bar @ y=16 (16 px) — Win16 灰底黑字 ===
     * R17: 8 items 中文化 (對齊使用者指正) — 對齊 1993 英文版 menu
     *   File → 檔案;  Edit → 編輯;  Orders → 命令;  Advisors → 顧問
     *   World → 世界; Civilopedia → 百科; City → 城市;   Help → 說明 */
    civ_fill_rect(fb, (civ_rect_t){0, TITLE_H, FB_W, MENU_H}, c_menu_bg);
    civ_hline(fb, 0, CHROME_H - 1, FB_W, c_menu_fg);
    if (g->font_body) {
        const char *items[] = {
            "檔案", "編輯", "命令", "顧問",
            "世界", "百科", "城市", "說明",
        };
        int x = 8;
        for (size_t i = 0; i < sizeof items / sizeof items[0]; i++) {
            civ_text_out(fb, g->font_body, x, TITLE_H + MENU_H - 3, items[i],
                         c_menu_fg, c_menu_bg, CIV_TEXT_BK_TRANSPARENT);
            x += civ_text_measure(g->font_body, items[i]) + 16;
        }
    }
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
    const char *data_dir = getenv("CIV1_DATA_DIR");
    if (!data_dir || !*data_dir) {
        printf("SKIP test_world_snapshot（CIV1_DATA_DIR 未設）\n");
        return 0;
    }

    SDL_setenv("SDL_VIDEODRIVER", "dummy", 0);
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return 1;

    struct civ_game g = {0};
    g.framebuffer = civ_surface_new(FB_W, FB_H);
    civ_palette_default(&g.palette);
    if (file_exists(CIV_DEFAULT_FONT_PATH)) {
        g.font_title = civ_font_open(CIV_DEFAULT_FONT_PATH, 24);
        g.font_body  = civ_font_open(CIV_DEFAULT_FONT_PATH, 14);
    }
    civ_widgets_register(&g);

    /* 載 SPR32X32 (CIVDATA4 #200) */
    char path[1024];
    snprintf(path, sizeof path, "%s/CIVDATA4.RSC", data_dir);
    civ_rsrc_t *r4 = civ_rsrc_open(path);
    if (!r4) {
        snprintf(path, sizeof path, "%s/Civdata4.RSC", data_dir);
        r4 = civ_rsrc_open(path);
    }
    if (!r4) { fprintf(stderr, "CIVDATA4.RSC 找不到\n"); return 1; }

    if (civ_sprite_sheet_load(r4, 200, 32, 32, &g.sprite_sheet) != 0) {
        fprintf(stderr, "sprite_sheet_load failed\n");
        return 1;
    }
    civ_rsrc_close(r4);

    printf("SPR32X32 loaded: %d×%d sheet, %d cols × %d rows of 32×32 tiles\n",
           g.sprite_sheet.sheet->w, g.sprite_sheet.sheet->h,
           g.sprite_sheet.cols, g.sprite_sheet.rows);

    /* M5 真落地 (2026-06-06 第二輪):
     *   舊版這裡是 debug 模式直接把整張 SPR32X32 sheet raw blit + grid 線
     *   做資產校驗,結果看起來像「sprite atlas 平鋪占位」不像真實 game world。
     *   現在改走 widgets/map.c 的真實 render path: world_ready=true 後
     *   widgets render 會跑 terrain enum lookup 把每格 terrain 對應到
     *   SPR32X32 的代表 (col, row) blit 到 widget rect 內。 */
    g.palette = g.sprite_sheet.pal;   /* 用 sheet 自有 palette */

    civ_world_init_demo(&g.world);

    /* 建 sheet→game palette LUT (sheet 已用 g.palette 同表故 identity,但
     * blit_remap 仍要 lut_built=true 才不會跳到 fallback 綠底) */
    civ_sprite_sheet_build_lut(&g.sprite_sheet, &g.palette);

    g.world_ready = true;

    /* R6 demo: 若 argv 帶 "city" 則打開 city screen 展示 modal */
    if (argc > 2 && strcmp(argv[2], "city") == 0) {
        int cidx = civ_world_city_at(&g.world,
                                     g.world.cities[0].x,
                                     g.world.cities[0].y);
        if (cidx >= 0) {
            g.city_screen_open = true;
            g.city_screen_idx  = cidx;
            g.modal_lock       = true;
            /* 讓羅馬建造 Granary 為 demo */
            g.world.cities[0].building_target = 3;  /* Granary */
            g.world.cities[0].shield_stock    = 20;
        }
    }

    /* R16 demo: 若 argv 帶 "tech" 則打開 tech discovery modal — BRONZE WORKING
     * R17: 同時把 font_title 改用 36px (大字效果, 對齊原版 reference) */
    if (argc > 2 && strcmp(argv[2], "tech") == 0) {
        if (file_exists(CIV_DEFAULT_FONT_PATH)) {
            if (g.font_title) civ_font_close(g.font_title);
            g.font_title = civ_font_open(CIV_DEFAULT_FONT_PATH, 36);
        }
        civ_tech_discovery_event_t *ev = &g.tech_screen_event;
        memset(ev, 0, sizeof *ev);
        ev->tech_id       = CIV_TECH_BRONZE_WORKING;
        ev->source        = CIV_TECH_LEARN_DIPLOMAT;
        ev->from_civ_slot = 4;  /* "埃及" */
        civ_tech_discovery_fill_unlocked(ev);
        g.tech_screen_open = true;
        g.modal_lock       = true;
    }

    paint_background(&g);
    civ_widgets_render_all(&g);
    civ_city_screen_render(&g, g.framebuffer);
    civ_tech_screen_render(&g, g.framebuffer);

    const char *out_path = argc > 1 ? argv[1] : "m5_world.ppm";
    write_ppm(out_path, g.framebuffer, &g.palette);
    printf("PASS test_world_snapshot → %s\n", out_path);

    civ_sprite_sheet_free(&g.sprite_sheet);
    civ_widgets_unregister(&g);
    if (g.font_title) civ_font_close(g.font_title);
    if (g.font_body)  civ_font_close(g.font_body);
    civ_big5_cleanup();
    civ_surface_free(g.framebuffer);
    SDL_Quit();
    return 0;
}
