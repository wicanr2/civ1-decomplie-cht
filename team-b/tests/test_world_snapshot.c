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
#include "widgets/diplomat_screen.h"
#include "widgets/menu_dropdown.h"
#include "widgets/tech_screen.h"
#include "world/diplomat.h"
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
    g.menu_open_idx    = -1;   /* R29: 預設 menu 沒打開 */
    g.menu_cursor_item = -1;
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

    /* R19: 載 CIVDATA2 KING00..13 (id 500..513) → leader_portraits cache.
     * sprite palette 透過 build_lut 翻譯到 g.palette (sprite_sheet.pal). */
    snprintf(path, sizeof path, "%s/CIVDATA2.RSC", data_dir);
    civ_rsrc_t *r2 = civ_rsrc_open(path);
    if (!r2) {
        snprintf(path, sizeof path, "%s/Civdata2.RSC", data_dir);
        r2 = civ_rsrc_open(path);
    }
    if (r2) {
        int loaded = 0;
        for (civ_leader_id_t l = 1; l <= CIV_LEADER_COUNT; l++) {
            int king_id = civ_leader_king_sprite_id(l);
            if (king_id < 0) continue;   /* slot 8 NONE skip */
            civ_surface_t *king = NULL;
            civ_palette_t  king_pal = {0};
            if (civ_load_cvpc_by_id(r2, (int16_t)king_id, &king, &king_pal) == 0) {
                g.leader_portraits[l]      = king;
                g.leader_king_palettes[l]  = king_pal;   /* R20: cache full palette */
                loaded++;
            }
        }
        printf("KING portraits loaded: %d/13\n", loaded);

        /* R21: 載 3 GOVT*M backdrops (id 404/405/406) for diplomat scene */
        int gloaded = 0;
        for (int gi = 0; gi < 3; gi++) {
            civ_surface_t *gb = NULL;
            civ_palette_t  gp = {0};
            int16_t gid = (int16_t)(404 + gi);
            if (civ_load_cvpc_by_id(r2, gid, &gb, &gp) == 0) {
                g.govt_backdrops[gi] = gb;
                g.govt_palettes[gi]  = gp;
                gloaded++;
            }
        }
        printf("GOVT backdrops loaded: %d/3\n", gloaded);

        civ_rsrc_close(r2);
    } else {
        fprintf(stderr, "warning: CIVDATA2.RSC 找不到, leader_portraits 全 fallback\n");
    }

    /* R28-2: 載 Civdata3 discovr1/discovr2 (id 142/143, 各 512×320) — 科技
     * 發現畫面的「科技官員」立像. spec 03 §3 確認在 Civdata3. */
    snprintf(path, sizeof path, "%s/Civdata3.rsc", data_dir);
    civ_rsrc_t *r3 = civ_rsrc_open(path);
    if (!r3) {
        snprintf(path, sizeof path, "%s/CIVDATA3.RSC", data_dir);
        r3 = civ_rsrc_open(path);
    }
    if (r3) {
        int aloaded = 0;
        for (int ai = 0; ai < 2; ai++) {
            civ_surface_t *adv = NULL;
            civ_palette_t  ap = {0};
            int16_t aid = (int16_t)(142 + ai);
            if (civ_load_cvpc_by_id(r3, aid, &adv, &ap) == 0) {
                g.tech_advisor[ai] = adv;
                g.tech_advisor_palettes[ai] = ap;
                aloaded++;
            }
        }
        printf("tech advisors loaded: %d/2\n", aloaded);
        civ_rsrc_close(r3);
    } else {
        fprintf(stderr, "warning: Civdata3.rsc 找不到, tech_advisor fallback\n");
    }

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

    /* R18 demo: 若 argv 帶 "diplomat-XXX" 則打開 diplomat visit modal.
     * R20 重大: 安裝 KING palette 為 g.palette → sprite blit identity, 顏色 100%. */
    if (argc > 2 && strncmp(argv[2], "diplomat-", 9) == 0) {
        if (file_exists(CIV_DEFAULT_FONT_PATH)) {
            if (g.font_title) civ_font_close(g.font_title);
            g.font_title = civ_font_open(CIV_DEFAULT_FONT_PATH, 36);
        }
        civ_diplomat_event_t *dev = &g.diplomat_screen_event;
        memset(dev, 0, sizeof *dev);
        if (strcmp(argv[2], "diplomat-elizabeth") == 0)
            dev->leader = CIV_LEADER_ELIZABETH;
        else if (strcmp(argv[2], "diplomat-frederick") == 0)
            dev->leader = CIV_LEADER_FREDERICK;
        else if (strcmp(argv[2], "diplomat-mao") == 0)
            dev->leader = CIV_LEADER_MAO;
        else if (strcmp(argv[2], "diplomat-gandhi") == 0)
            dev->leader = CIV_LEADER_GANDHI;
        else if (strncmp(argv[2], "diplomat-id-", 12) == 0) {
            /* R20 dump mode: diplomat-id-NN where NN = 0..13 直接取 KING sprite
             * 不問 enum, 用 leader=NN+1 (因為 sprite idx = leader - 1) */
            int id = atoi(argv[2] + 12);
            if (id < 0) id = 0;
            if (id > 13) id = 13;
            dev->leader = (civ_leader_id_t)(id + 1);
        }
        else
            dev->leader = CIV_LEADER_CAESAR;
        dev->mood = CIV_DIPLOMAT_GREETING;
        g.diplomat_screen_open = true;
        g.modal_lock           = true;

        /* R20: 安裝該領袖 KING palette 為 game palette → sprite 100% 顏色 */
        if (g.leader_portraits[dev->leader]) {
            g.palette = g.leader_king_palettes[dev->leader];
        }
    }

    /* R23: splash mode — 載 CIVDATA1 CIV.GIF (id 136, 502×145) 居中 blit.
     * R24: birth-N mode (N=1..8) — 載 BIRTH0N (id 127+N) 縮放居中.
     *      BIRTH01 = id 128 (1024×320), BIRTH2..8 = id 129..135 (512×320 each).
     * 對應 1991/1993 原版開機 splash + intro sequence. */
    int is_splash = (argc > 2 && strcmp(argv[2], "splash") == 0);
    int birth_n = -1;
    if (argc > 2 && strncmp(argv[2], "birth-", 6) == 0) {
        birth_n = atoi(argv[2] + 6);
        if (birth_n < 1 || birth_n > 8) birth_n = 1;
    }

    /* R29 (C1 gap): menu-NAME 模式 — 主畫面 + 打開指定 menu dropdown.
     * NAME ∈ {file/edit/orders/advisors/world/civilopedia/city/help}.
     * 對應 menu_open_idx 0..7. cursor_item 預設 0 (反白第一項). */
    if (argc > 2 && strncmp(argv[2], "menu-", 5) == 0) {
        const char *name = argv[2] + 5;
        int idx = -1;
        if      (strcmp(name, "file")        == 0) idx = 0;
        else if (strcmp(name, "edit")        == 0) idx = 1;
        else if (strcmp(name, "orders")      == 0) idx = 2;
        else if (strcmp(name, "advisors")    == 0) idx = 3;
        else if (strcmp(name, "world")       == 0) idx = 4;
        else if (strcmp(name, "civilopedia") == 0) idx = 5;
        else if (strcmp(name, "city")        == 0) idx = 6;
        else if (strcmp(name, "help")        == 0) idx = 7;
        if (idx >= 0) {
            g.menu_open_idx    = idx;
            g.menu_cursor_item = 0;   /* 反白第一項, 對齊 reference */
        }
    }
    if (is_splash || birth_n >= 1) {
        snprintf(path, sizeof path, "%s/CIVDATA1.RSC", data_dir);
        civ_rsrc_t *r1 = civ_rsrc_open(path);
        if (!r1) {
            snprintf(path, sizeof path, "%s/Civdata1.RSC", data_dir);
            r1 = civ_rsrc_open(path);
        }
        int16_t target_id = is_splash ? 136 : (int16_t)(127 + birth_n);
        civ_surface_t *art = NULL;
        civ_palette_t  apal = {0};
        if (r1 && civ_load_cvpc_by_id(r1, target_id, &art, &apal) == 0) {
            g.palette = apal;
            uint8_t c_black = civ_palette_nearest_rgb(&g.palette, 0, 0, 0);
            civ_fill_rect(g.framebuffer, (civ_rect_t){0, 0, FB_W, FB_H}, c_black);
            /* 居中 + 若過寬則 nearest-neighbor 縮 (BIRTH01 1024 太寬) */
            int dst_w = art->w;
            int dst_h = art->h;
            if (dst_w > FB_W) {
                dst_h = dst_h * FB_W / dst_w;
                dst_w = FB_W;
            }
            int dx = (FB_W - dst_w) / 2;
            int dy = (FB_H - dst_h) / 2;
            if (dst_w == art->w && dst_h == art->h) {
                civ_rect_t src = { 0, 0, art->w, art->h };
                civ_surface_blit(g.framebuffer, dx, dy, art, &src);
            } else {
                /* 簡單 nearest scale */
                for (int yy = 0; yy < dst_h; yy++) {
                    int sy = yy * art->h / dst_h;
                    for (int xx = 0; xx < dst_w; xx++) {
                        int sx = xx * art->w / dst_w;
                        if (sx < 0 || sx >= art->w || sy < 0 || sy >= art->h) continue;
                        int px = dx + xx, py = dy + yy;
                        if (px < 0 || px >= g.framebuffer->w ||
                            py < 0 || py >= g.framebuffer->h) continue;
                        g.framebuffer->pixels[py * g.framebuffer->pitch + px] =
                            art->pixels[sy * art->pitch + sx];
                    }
                }
            }
            civ_surface_free(art);
            printf("%s id=%d rendered (%dx%d → %dx%d)\n",
                   is_splash ? "splash" : "birth",
                   target_id, art ? 0 : 0, 0, dst_w, dst_h);
        } else {
            fprintf(stderr, "splash/birth: id=%d load fail\n", target_id);
        }
        if (r1) civ_rsrc_close(r1);
    } else {
        paint_background(&g);
        civ_widgets_render_all(&g);
        civ_city_screen_render(&g, g.framebuffer);
        civ_tech_screen_render(&g, g.framebuffer);
        civ_diplomat_screen_render(&g, g.framebuffer);
        /* R29 (C1): menu dropdown 在最上層 (蓋過 modal screens 都 OK,
         * 因 menu 一打開, 其餘 widget 都靜止. 對齊 1993 Win16 行為). */
        if (g.menu_open_idx >= 0) {
            civ_menu_bar_highlight(g.framebuffer, &g, g.menu_open_idx);
            int dd_x = civ_menu_bar_x(&g, g.menu_open_idx) - 4;
            civ_menu_dropdown_render(g.framebuffer, &g,
                                      g.menu_open_idx, g.menu_cursor_item,
                                      dd_x, 32);
        }
    }

    const char *out_path = argc > 1 ? argv[1] : "m5_world.ppm";
    write_ppm(out_path, g.framebuffer, &g.palette);
    printf("PASS test_world_snapshot → %s\n", out_path);

    /* R19: 釋放 leader portraits */
    for (int l = 1; l <= CIV_LEADER_COUNT; l++) {
        if (g.leader_portraits[l]) {
            civ_surface_free(g.leader_portraits[l]);
            g.leader_portraits[l] = NULL;
        }
    }
    /* R21: 釋放 GOVT backdrops */
    for (int gi = 0; gi < 3; gi++) {
        if (g.govt_backdrops[gi]) {
            civ_surface_free(g.govt_backdrops[gi]);
            g.govt_backdrops[gi] = NULL;
        }
    }
    /* R28-2: 釋放 tech_advisor sprites */
    for (int ai = 0; ai < 2; ai++) {
        if (g.tech_advisor[ai]) {
            civ_surface_free(g.tech_advisor[ai]);
            g.tech_advisor[ai] = NULL;
        }
    }

    civ_sprite_sheet_free(&g.sprite_sheet);
    civ_widgets_unregister(&g);
    if (g.font_title) civ_font_close(g.font_title);
    if (g.font_body)  civ_font_close(g.font_body);
    civ_big5_cleanup();
    civ_surface_free(g.framebuffer);
    SDL_Quit();
    return 0;
}
