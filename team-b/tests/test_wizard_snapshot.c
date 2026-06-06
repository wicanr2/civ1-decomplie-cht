/*
 * test_wizard_snapshot.c — M4-full 視覺驗證
 *
 * 渲染新局精靈三頁分別到 PPM：m4full_p1.ppm (難度) / m4full_p2.ppm (文明) /
 * m4full_p3.ppm (名稱)。
 */
#include "civ_game.h"
#include "civ_widgets.h"
#include "dialog/dialog.h"
#include "dialog/wizard.h"

#include "gfx/palette.h"
#include "gfx/primitive.h"
#include "gfx/surface.h"
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

static void render_snapshot(struct civ_game *g, civ_dialog_stack_t *stk,
                            const char *path)
{
    paint_background(g);
    civ_widgets_render_all(g);
    civ_dialog_render(stk, g->framebuffer, g);
    write_ppm(path, g->framebuffer, &g->palette);
    printf("  → %s\n", path);
}

static void push_key(civ_dialog_stack_t *s, struct civ_game *g, SDL_Keycode k)
{
    SDL_Event ev;
    memset(&ev, 0, sizeof ev);
    ev.type = SDL_KEYDOWN;
    ev.key.keysym.sym = k;
    civ_dialog_handle_event(s, &ev, g);
}

int main(int argc, char **argv)
{
    SDL_setenv("SDL_VIDEODRIVER", "dummy", 0);
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return 1;

    struct civ_game g = {0};
    g.framebuffer = civ_surface_new(FB_W, FB_H);
    civ_palette_default(&g.palette);
    if (file_exists(CIV_DEFAULT_FONT_PATH)) {
        g.font_title = civ_font_open(CIV_DEFAULT_FONT_PATH, 24);
        g.font_body  = civ_font_open(CIV_DEFAULT_FONT_PATH, 16);
    }
    civ_widgets_register(&g);

    civ_dialog_stack_t stk = {0};
    civ_wizard_open(&stk, &g);

    const char *base = argc > 1 ? argv[1] : "m4full";
    char path[256];

    /* Page 1: Difficulty (預設 cursor = Prince) */
    snprintf(path, sizeof path, "%s_p1.ppm", base);
    render_snapshot(&g, &stk, path);

    /* 按 ↓ 移到 King 然後 Enter -> Page 2 */
    push_key(&stk, &g, SDLK_DOWN);
    push_key(&stk, &g, SDLK_RETURN);

    /* Page 2: Civ picker (預設 cursor = 0 = 羅馬) */
    snprintf(path, sizeof path, "%s_p2.ppm", base);
    render_snapshot(&g, &stk, path);

    /* 移到 cursor=12 (slot 14 = 英格蘭 Elizabeth) 然後 Enter -> Page 3 */
    for (int i = 0; i < 12; i++) push_key(&stk, &g, SDLK_RIGHT);
    push_key(&stk, &g, SDLK_RETURN);

    /* Page 3: Name (空名稱狀態 + 輸入 "Anna") */
    const char *name = "Anna";
    for (size_t i = 0; i < strlen(name); i++) {
        SDL_Event ev;
        memset(&ev, 0, sizeof ev);
        ev.type = SDL_TEXTINPUT;
        ev.text.text[0] = name[i];
        civ_dialog_handle_event(&stk, &ev, &g);
    }
    snprintf(path, sizeof path, "%s_p3.ppm", base);
    render_snapshot(&g, &stk, path);

    printf("PASS test_wizard_snapshot\n");

    if (stk.top) civ_dialog_pop(&stk);
    civ_widgets_unregister(&g);
    if (g.font_title) civ_font_close(g.font_title);
    if (g.font_body)  civ_font_close(g.font_body);
    civ_big5_cleanup();
    civ_surface_free(g.framebuffer);
    SDL_Quit();
    return 0;
}
