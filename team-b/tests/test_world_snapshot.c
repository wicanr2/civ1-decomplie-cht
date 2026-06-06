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

#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define FB_W 640
#define FB_H 480
#define MENU_H 16    /* C-B-2: 窄 menu bar 取代大紅標題列 */

#ifndef CIV_DEFAULT_FONT_PATH
#define CIV_DEFAULT_FONT_PATH "/usr/share/fonts/truetype/arphic/uming.ttc"
#endif

static int file_exists(const char *p) { struct stat s; return stat(p,&s)==0; }

static void paint_background(struct civ_game *g)
{
    civ_surface_t *fb = g->framebuffer;
    civ_surface_clear(fb, 15);
    /* 窄 menu bar - 對應原版 Civ1 1993 Win 頂部 dropdown 列 */
    civ_fill_rect(fb, (civ_rect_t){0, 0, FB_W, MENU_H}, 15);  /* 亮灰底 */
    civ_hline(fb, 0, MENU_H - 1, FB_W, 0);                    /* 底部隔線 */
    if (g->font_body) {
        /* 五個 menu item 從左到右排,原版選項 */
        const char *items[] = {
            "Game", "Orders", "Advisors", "World", "Civilopedia"
        };
        int x = 8;
        for (size_t i = 0; i < sizeof items / sizeof items[0]; i++) {
            civ_text_out(fb, g->font_body, x, MENU_H - 4, items[i],
                         0, 15, CIV_TEXT_BK_TRANSPARENT);
            x += civ_text_measure(g->font_body, items[i]) + 14;
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

    paint_background(&g);
    civ_widgets_render_all(&g);

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
