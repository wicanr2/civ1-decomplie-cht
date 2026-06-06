/*
 * text/glyph_cache.h — FreeType MONO glyph cache
 *
 * 對應 SDL_IMPLEMENTATION_PLAN §6。
 * 把 Unicode codepoint 換成 1-bit mask，存入小型 hash table；
 * 由 text_out.c 取出後混合到 palette framebuffer。
 *
 * 不用 SDL2_ttf 的原因：要直接寫 palette index，不是 alpha blending。
 */
#ifndef CIV_TEXT_GLYPH_CACHE_H
#define CIV_TEXT_GLYPH_CACHE_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

typedef struct civ_glyph {
    int      w, h;          /* mask 尺寸（mono pitch = (w+7)/8 bytes） */
    int      bearing_x;     /* 左側留白 */
    int      bearing_y;     /* 從 baseline 往上 */
    int      advance_x;     /* 下一個字距 */
    uint8_t *mask;          /* 1 bit per pixel，row-major，pitch = (w+7)/8 */
} civ_glyph_t;

typedef struct civ_font {
    void    *ft_face_opaque;  /* FT_Face — 隱藏在 .c 內 */
    int      size_px;
    int      ascent;
    int      descent;
    int      space_advance;
    void    *cache_opaque;    /* hashmap */
} civ_font_t;

/* 從檔案路徑載入字型。size_px = 16 / 24 / 32 等。失敗回 NULL。 */
civ_font_t *civ_font_open(const char *path, int size_px);
void        civ_font_close(civ_font_t *font);

/* 取得 codepoint 對應 glyph；cache hit 不重 load。空白(0x20) 與
 * 未實作字元回 NULL（caller 應自行處理 space advance / fallback）。 */
const civ_glyph_t *civ_glyph_get(civ_font_t *font, uint32_t codepoint);

#endif /* CIV_TEXT_GLYPH_CACHE_H */
