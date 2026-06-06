/*
 * world/sprite_sheet.h — sprite sheet 切割 + palette LUT cache
 *
 * 對應 spec 03 §3.10 介面契約 + spec 05 §5.6 整合接口。SPR32X32 主
 * sprite sheet (CIVDATA4 id 200, 1472×400) 是 1993 Win port 的主要
 * terrain / unit / building tile 來源。
 */
#ifndef CIV_WORLD_SPRITE_SHEET_H
#define CIV_WORLD_SPRITE_SHEET_H

#include "../gfx/palette.h"
#include "../gfx/surface.h"
#include "../res/rsrcfork.h"

typedef struct civ_sprite_sheet {
    civ_surface_t *sheet;        /* 解碼後的 indexed surface */
    civ_palette_t  pal;          /* sheet 自帶 palette */
    int            tile_w, tile_h;
    int            cols, rows;
    uint8_t        lut[256];     /* sheet palette → game palette 的 LUT */
    bool           lut_built;
} civ_sprite_sheet_t;

/* 從 .RSC 載入 CvPc 切成 tile grid。tile_w × tile_h 切；cols/rows
 * 自動算。失敗回 -1。 */
int civ_sprite_sheet_load(civ_rsrc_t *r, int16_t cvpc_id,
                          int tile_w, int tile_h,
                          civ_sprite_sheet_t *out);

void civ_sprite_sheet_free(civ_sprite_sheet_t *s);

/* 建 LUT 把 sheet palette 映到 base palette（必須在 blit 前呼叫一次） */
void civ_sprite_sheet_build_lut(civ_sprite_sheet_t *s,
                                const civ_palette_t *base);

/* 取得 sprite (col, row) 的 src_rect（在 sheet 內的位置） */
static inline civ_rect_t civ_sprite_rect(const civ_sprite_sheet_t *s,
                                          int col, int row)
{
    civ_rect_t r = {col * s->tile_w, row * s->tile_h, s->tile_w, s->tile_h};
    return r;
}

#endif /* CIV_WORLD_SPRITE_SHEET_H */
