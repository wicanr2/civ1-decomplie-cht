/*
 * test_demo_snapshot.c — M1/M2/M3 視覺驗證
 *
 * Headless dummy driver；產出 m3_demo.ppm（M3：含 EARTH worldmap blit）
 * 或 m2_demo.ppm（CIV1_DATA_DIR 未設時的 widget-only layout）。
 */
#include "civ_game.h"
#include "civ_widgets.h"

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

/* M3：若 CIV1_DATA_DIR 設了，把 EARTH worldmap blit 到主地圖區
 * 中央，作為 CvPc decoder 串到 widget render 的端到端驗證。 */
static int try_blit_earth(struct civ_game *g)
{
    const char *dir = getenv("CIV1_DATA_DIR");
    if (!dir || !*dir) return 0;

    char path[1024];
    snprintf(path, sizeof path, "%s/Civdata0.RSC", dir);
    civ_rsrc_t *r = civ_rsrc_open(path);
    if (!r) {
        snprintf(path, sizeof path, "%s/CIVDATA0.RSC", dir);
        r = civ_rsrc_open(path);
    }
    if (!r) return 0;

    civ_surface_t *earth = NULL;
    civ_palette_t  earth_pal = {0};
    civ_palette_default(&earth_pal);
    int rc = civ_load_cvpc_by_id(r, 137, &earth, &earth_pal);
    if (rc != 0 || !earth) { civ_rsrc_close(r); return 0; }

    /* 用 EARTH 的 palette 取代 framebuffer palette
     * （M5+ 會做正確 palette stomp 處理；M3 只證明 blit 通） */
    g->palette = earth_pal;

    /* 在主地圖中央 blit EARTH (320×200) */
    int dx = (g->map_w->rect.w - earth->w) / 2 + g->map_w->rect.x;
    int dy = (g->map_w->rect.h - earth->h) / 2 + g->map_w->rect.y;
    civ_surface_blit(g->framebuffer, dx, dy, earth, NULL);
    /* 框一條紅線標記 */
    civ_frame_rect(g->framebuffer,
                   (civ_rect_t){dx - 1, dy - 1, earth->w + 2, earth->h + 2},
                   12);
    /* 標題 */
    if (g->font_body) {
        civ_text_out(g->framebuffer, g->font_body,
                     dx, dy - 4, "EARTH.GIF (Civdata0 #137, 320×200)",
                     0, 15, CIV_TEXT_BK_TRANSPARENT);
    }

    civ_surface_free(earth);
    civ_rsrc_close(r);
    return 1;
}

static void simulate_user(struct civ_game *g)
{
    civ_map_state_t *ms = g->map_w->state;
    ms->has_mouse = true;
    ms->last_mouse_x = 240; ms->last_mouse_y = 250;
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
        g.font_body  = civ_font_open(CIV_DEFAULT_FONT_PATH, 16);
    }
    g.tick_count = 12345;

    if (civ_widgets_register(&g) != 0) {
        fprintf(stderr, "widgets register failed\n"); return 1;
    }

    paint_background(&g);
    simulate_user(&g);
    civ_widgets_render_all(&g);
    int blitted = try_blit_earth(&g);

    const char *out_path = (argc > 1) ? argv[1]
                                       : (blitted ? "m3_demo.ppm" : "m2_demo.ppm");
    if (write_ppm(out_path, g.framebuffer, &g.palette) < 0) {
        fprintf(stderr, "write %s failed\n", out_path);
        return 1;
    }
    printf("PASS test_demo_snapshot → %s (%d×%d, EARTH blit=%s)\n",
           out_path, FB_W, FB_H, blitted ? "yes" : "no");

    civ_widgets_unregister(&g);
    if (g.font_title) civ_font_close(g.font_title);
    if (g.font_body)  civ_font_close(g.font_body);
    civ_big5_cleanup();
    civ_surface_free(g.framebuffer);
    SDL_Quit();
    return 0;
}
