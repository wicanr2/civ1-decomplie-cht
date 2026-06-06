#include "minimap.h"
#include "../civ_game.h"
#include "../gfx/primitive.h"
#include "../gfx/surface.h"
#include "../text/text_out.h"
#include "../world/world.h"

#include <stdlib.h>

/* D-minimap (2026-06-06): 不能用 VGA 色 idx 9/10/14 直接寫,
 * 因為 test_world_snapshot 用 SPR32X32 sheet palette 當 fb palette,
 * 兩者 idx → RGB 對應完全不同 (sheet 內 idx 9 可能是黃, 不是藍).
 *
 * 解法: 預先 hardcode 每 terrain 的「理想 RGB」, 在 render 時對
 * 當前 g->palette 用 RGB-nearest 找最近的 idx. 這樣不論 fb 用哪份
 * palette 都會挑到對的色.
 *
 * 同樣方法處理 owner 色 (barbarian 紅, p1 黃, p2 藍 等). */

typedef struct { uint8_t r, g, b; } rgb_t;

static const rgb_t TERRAIN_RGB[CIV_TERRAIN_COUNT] = {
    [CIV_TERRAIN_OCEAN]    = {  20,  60, 180 },  /* 深藍 */
    [CIV_TERRAIN_GRASS]    = {  60, 170,  60 },  /* 草綠 */
    [CIV_TERRAIN_PLAINS]   = { 200, 200,  80 },  /* 黃綠平原 */
    [CIV_TERRAIN_FOREST]   = {  20, 100,  20 },  /* 深綠 */
    [CIV_TERRAIN_MOUNTAIN] = { 120, 120, 120 },  /* 灰 */
    [CIV_TERRAIN_HILLS]    = { 120, 130,  80 },  /* 棕綠丘陵 */
    [CIV_TERRAIN_DESERT]   = { 230, 200, 100 },  /* 沙黃 */
    [CIV_TERRAIN_JUNGLE]   = {  40,  80,  40 },  /* 暗綠 */
    [CIV_TERRAIN_SWAMP]    = {  80, 100,  60 },  /* 暗棕綠 */
    [CIV_TERRAIN_TUNDRA]   = { 200, 200, 220 },  /* 淡白藍 */
    [CIV_TERRAIN_ARCTIC]   = { 250, 250, 250 },  /* 白雪 */
    [CIV_TERRAIN_RIVER]    = {  80, 140, 220 },  /* 淡藍 */
};

static const rgb_t OWNER_RGB[CIV_NUM_PLAYERS] = {
    { 255,   0,   0 },  /* 0 barbarian 紅 */
    { 255, 230,   0 },  /* 1 player 黃 */
    {   0, 100, 255 },  /* 2 藍 */
    {   0, 200,   0 },  /* 3 綠 */
    {   0, 200, 200 },  /* 4 青 */
    { 200,   0, 200 },  /* 5 紫 */
    { 180, 180, 180 },  /* 6 灰 */
    { 255, 255, 255 },  /* 7 白 */
};

/* R10: 改用 gfx/palette public civ_palette_nearest_rgb (跨 widget 共用) */
static uint8_t palette_nearest(const civ_palette_t *pal, rgb_t target)
{
    return civ_palette_nearest_rgb(pal, target.r, target.g, target.b);
}

static civ_evt_result_t on_click(civ_widget_t *w, SDL_Event *ev);

/* 9-entry dispatch — spec 02 §2.2.7 推測：WM_PAINT / WM_LBUTTONDOWN
 * / WM_RBUTTONDOWN / WM_MOUSEMOVE / WM_TIMER / WM_CREATE / WM_DESTROY
 * / WM_SIZE / WM_SETFOCUS。M2 stub 只實作可立即測的 3 個。 */
static const civ_widget_msg_entry_t MINIMAP_TABLE[] = {
    { SDL_MOUSEBUTTONDOWN, on_click },
};
#define MINIMAP_TABLE_LEN (sizeof MINIMAP_TABLE / sizeof MINIMAP_TABLE[0])

static civ_evt_result_t on_click(civ_widget_t *w, SDL_Event *ev)
{
    civ_minimap_state_t *s = w->state;
    s->view_x = ev->button.x;
    s->view_y = ev->button.y;
    s->has_view = true;
    return 0;
}

static civ_evt_result_t minimap_dispatch(civ_widget_t *w, SDL_Event *ev)
{
    w->call_count++;
    /* 無鎖定模式 */
    return civ_widget_table_run(w, MINIMAP_TABLE, MINIMAP_TABLE_LEN, ev);
}

static void minimap_render(civ_widget_t *w, civ_surface_t *fb)
{
    /* R4+R10: Win16 子視窗 chrome 用 nearest 解 sheet palette idx 對不上 */
    enum { SUB_TITLE_H = 12 };
    uint8_t c_t_bg = w->game ? civ_palette_nearest_rgb(&w->game->palette, 0x00,0x00,0x80) : 1;
    uint8_t c_t_fg = w->game ? civ_palette_nearest_rgb(&w->game->palette, 0xFF,0xFF,0xFF) : 15;
    civ_fill_rect(fb, (civ_rect_t){w->rect.x, w->rect.y, w->rect.w, SUB_TITLE_H}, c_t_bg);
    if (w->game && w->game->font_body) {
        const char *title = "World Map";
        int tw = civ_text_measure(w->game->font_body, title);
        int tx = w->rect.x + (w->rect.w - tw) / 2;
        civ_text_out(fb, w->game->font_body, tx, w->rect.y + SUB_TITLE_H - 3,
                     title, c_t_fg, c_t_bg, CIV_TEXT_BK_TRANSPARENT);
    }
    /* 縮小 w->rect 給內容區用, 但 widget 接 event 仍用完整 rect */
    civ_rect_t inner = { w->rect.x, w->rect.y + SUB_TITLE_H,
                         w->rect.w, w->rect.h - SUB_TITLE_H };

    /* D-minimap (2026-06-06): 改為真實世界縮圖 (subset of w->rect 後的 inner) */
    civ_rect_t orig_rect = w->rect;
    w->rect = inner;

    /* R14: body 黃底 (sheet idx 9) 改用 palette_nearest 找真黑 */
    uint8_t c_body_bg = w->game ? civ_palette_nearest_rgb(&w->game->palette, 0,0,0) : 0;
    uint8_t c_border  = w->game ? civ_palette_nearest_rgb(&w->game->palette, 0x80,0x80,0x80) : 7;
    civ_fill_rect(fb, w->rect, c_body_bg);
    civ_frame_rect(fb, w->rect, c_border);

    if (!w->game || !w->game->world_ready) {
        /* 沒地圖時退占位 + 文字 */
        if (w->game && w->game->font_body) {
            civ_text_out(fb, w->game->font_body,
                         w->rect.x + 6, w->rect.y + 20,
                         "WDWSMMAP", 15, 0, CIV_TEXT_BK_TRANSPARENT);
            civ_text_out(fb, w->game->font_body,
                         w->rect.x + 6, w->rect.y + 40,
                         "小地圖", 15, 0, CIV_TEXT_BK_TRANSPARENT);
        }
        return;
    }

    const civ_world_t *wd = &w->game->world;

    /* 1. 縮放 60×30 → minimap 內部 (扣 2 邊框 padding) */
    int inner_x = w->rect.x + 2;
    int inner_y = w->rect.y + 2;
    int inner_w = w->rect.w - 4;
    int inner_h = w->rect.h - 4;
    int tw = inner_w / CIV_MAP_W;   /* 60 tile, 156 wide → tw=2 */
    int th = inner_h / CIV_MAP_H;   /* 30 tile, 116 tall → th=3 */
    if (tw < 1) tw = 1;
    if (th < 1) th = 1;

    /* 預先把 12 terrain + 8 owner 的 RGB 透過 RGB-nearest 對到當前 fb
     * palette 的最近 idx, 跨 palette 都對得上 */
    uint8_t terrain_col[CIV_TERRAIN_COUNT];
    for (int k = 0; k < CIV_TERRAIN_COUNT; k++) {
        terrain_col[k] = palette_nearest(&w->game->palette, TERRAIN_RGB[k]);
    }
    uint8_t owner_col[CIV_NUM_PLAYERS];
    for (int o = 0; o < CIV_NUM_PLAYERS; o++) {
        owner_col[o] = palette_nearest(&w->game->palette, OWNER_RGB[o]);
    }
    rgb_t white_rgb = { 255, 255, 255 };
    uint8_t white = palette_nearest(&w->game->palette, white_rgb);

    for (int y = 0; y < CIV_MAP_H; y++) {
        for (int x = 0; x < CIV_MAP_W; x++) {
            int kind = wd->terrain[y][x];
            if (kind < 0 || kind >= CIV_TERRAIN_COUNT) kind = CIV_TERRAIN_OCEAN;
            civ_rect_t r = { inner_x + x * tw, inner_y + y * th, tw, th };
            civ_fill_rect(fb, r, terrain_col[kind]);
        }
    }

    /* 2. unit 點 */
    for (int i = 0; i < wd->units_count; i++) {
        const civ_unit_t *u = &wd->units[i];
        if (!u->alive) continue;
        if (u->x < 0 || u->x >= CIV_MAP_W) continue;
        if (u->y < 0 || u->y >= CIV_MAP_H) continue;
        uint8_t col = owner_col[u->owner < CIV_NUM_PLAYERS ? u->owner : 0];
        int ux = inner_x + u->x * tw;
        int uy = inner_y + u->y * th;
        civ_fill_rect(fb, (civ_rect_t){ux, uy, tw, th}, col);
    }

    /* 3. view rect overlay (白框) */
    {
        int view_cols = 480 / 32;
        int view_rows = (480 - 16) / 32;
        int vx = inner_x + wd->view_x * tw;
        int vy = inner_y + wd->view_y * th;
        int vw = view_cols * tw;
        int vh = view_rows * th;
        if (vx + vw > inner_x + inner_w) vw = inner_x + inner_w - vx;
        if (vy + vh > inner_y + inner_h) vh = inner_y + inner_h - vy;
        civ_frame_rect(fb, (civ_rect_t){vx, vy, vw, vh}, white);
    }

    /* 4. cursor 點 (白色閃亮) */
    int cx = inner_x + wd->cursor_x * tw;
    int cy = inner_y + wd->cursor_y * th;
    civ_fill_rect(fb, (civ_rect_t){cx, cy, tw, th}, white);

    /* 還原 w->rect (event dispatch 仍用原完整 rect) */
    w->rect = orig_rect;
}

static void minimap_destroy(civ_widget_t *w)
{
    free(w->state);
    free(w);
}

static const civ_widget_vtable_t MINIMAP_VTABLE = {
    .name     = "minimap",
    .dispatch = minimap_dispatch,
    .render   = minimap_render,
    .destroy  = minimap_destroy,
};

civ_widget_t *civ_minimap_widget_new(struct civ_game *g, civ_rect_t rect)
{
    civ_widget_t *w = calloc(1, sizeof *w);
    if (!w) return NULL;
    w->vt      = &MINIMAP_VTABLE;
    w->rect    = rect;
    w->visible = true;
    w->game    = g;
    w->state   = calloc(1, sizeof(civ_minimap_state_t));
    if (!w->state) { free(w); return NULL; }
    return w;
}
