/*
 * gfx/palette.h — 256-color RGB palette + push/pop stack
 *
 * 對應 godpal.c (InitGodPalette / PushPalette / PopPalette /
 * ZeroGodPalette / FreeGodPalette) 與 GDI ANIMATEPALETTE。
 */
#ifndef CIV_GFX_PALETTE_H
#define CIV_GFX_PALETTE_H

#include <stdint.h>

typedef struct civ_color {
    uint8_t r, g, b, a;     /* a 留作未來 alpha 用，目前一律 255 */
} civ_color_t;

typedef struct civ_palette {
    civ_color_t entries[256];
    int         generation;  /* 每改一次 +1；present.c 監視這個決定是否要重建 SDL_Texture */
} civ_palette_t;

/* 預設 VGA 風格 256 色 palette：
 *   0-15：標準 VGA 16 色
 *   16-31：深藍漸層
 *   32-47：深紅漸層
 *   48-63：灰階漸層
 *   64-255：6×6×6 RGB cube + 多餘填色
 */
void civ_palette_default(civ_palette_t *p);

/* 全清為 0,0,0 */
void civ_palette_zero(civ_palette_t *p);

/* 單一 entry 設值 + generation++ */
void civ_palette_set(civ_palette_t *p, int index, civ_color_t c);

/* 範圍動畫式 set — 對應 GDI ANIMATEPALETTE */
void civ_palette_animate(civ_palette_t *p, int start, int count, const civ_color_t *src);

#endif /* CIV_GFX_PALETTE_H */
