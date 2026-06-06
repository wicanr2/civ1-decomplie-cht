#include "glyph_cache.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 小型 open-addressing hash table：key = codepoint，value = civ_glyph_t* */
typedef struct cache_entry {
    uint32_t     key;       /* 0 = empty slot；下界把 0 codepoint 當 sentinel 不存 */
    civ_glyph_t *glyph;
} cache_entry_t;

#define CACHE_CAP 1024      /* 對 M1 demo 字元數足夠 */

typedef struct cache {
    cache_entry_t slots[CACHE_CAP];
    size_t        used;
} cache_t;

/* FT_Library 全 process 共用 */
static FT_Library g_ft_lib = NULL;

static int ft_global_init(void)
{
    if (g_ft_lib) return 0;
    if (FT_Init_FreeType(&g_ft_lib) != 0) {
        fprintf(stderr, "FT_Init_FreeType failed\n");
        return -1;
    }
    return 0;
}

civ_font_t *civ_font_open(const char *path, int size_px)
{
    if (ft_global_init() < 0) return NULL;

    FT_Face face = NULL;
    if (FT_New_Face(g_ft_lib, path, 0, &face) != 0) {
        fprintf(stderr, "FT_New_Face failed: %s\n", path);
        return NULL;
    }

    /* TTC 內若有多個 face，FT_New_Face 預設取第 0 個 */
    if (FT_Set_Pixel_Sizes(face, 0, size_px) != 0) {
        FT_Done_Face(face);
        return NULL;
    }
    if (FT_Select_Charmap(face, FT_ENCODING_UNICODE) != 0) {
        /* 沒有 Unicode charmap，極不尋常 */
        FT_Done_Face(face);
        return NULL;
    }

    civ_font_t *font = calloc(1, sizeof *font);
    font->ft_face_opaque = face;
    font->size_px = size_px;
    font->ascent  = face->size->metrics.ascender  >> 6;
    font->descent = -(face->size->metrics.descender >> 6);

    /* 計算空白 advance */
    if (FT_Load_Char(face, 0x20, FT_LOAD_DEFAULT) == 0) {
        font->space_advance = face->glyph->advance.x >> 6;
    } else {
        font->space_advance = size_px / 2;
    }

    font->cache_opaque = calloc(1, sizeof(cache_t));
    return font;
}

static void glyph_free(civ_glyph_t *g)
{
    if (!g) return;
    free(g->mask);
    free(g);
}

void civ_font_close(civ_font_t *font)
{
    if (!font) return;
    cache_t *c = font->cache_opaque;
    if (c) {
        for (size_t i = 0; i < CACHE_CAP; i++) {
            if (c->slots[i].key != 0) glyph_free(c->slots[i].glyph);
        }
        free(c);
    }
    if (font->ft_face_opaque) FT_Done_Face((FT_Face)font->ft_face_opaque);
    free(font);
}

static civ_glyph_t *build_glyph(FT_Face face, uint32_t cp)
{
    if (FT_Load_Char(face, cp, FT_LOAD_RENDER | FT_LOAD_TARGET_MONO) != 0)
        return NULL;
    FT_GlyphSlot slot = face->glyph;
    FT_Bitmap   *bmp  = &slot->bitmap;

    if (bmp->pixel_mode != FT_PIXEL_MODE_MONO) {
        /* 預期是 mono；保險起見不接受其他模式 */
        return NULL;
    }

    civ_glyph_t *g = calloc(1, sizeof *g);
    if (!g) return NULL;
    g->w         = bmp->width;
    g->h         = bmp->rows;
    g->bearing_x = slot->bitmap_left;
    g->bearing_y = slot->bitmap_top;
    g->advance_x = slot->advance.x >> 6;

    /* FreeType MONO pitch 用 1-byte pad align；我們複製整個 bitmap */
    size_t our_pitch = (size_t)(g->w + 7) / 8;
    g->mask = malloc(our_pitch * (size_t)g->h);
    if (!g->mask) { free(g); return NULL; }
    for (int row = 0; row < g->h; row++) {
        memcpy(g->mask + (size_t)row * our_pitch,
               bmp->buffer + (size_t)row * bmp->pitch,
               our_pitch);
    }
    return g;
}

/* 32-bit FNV-1a-ish hash */
static size_t hash_cp(uint32_t cp)
{
    uint32_t h = cp * 2654435761u;
    return h % CACHE_CAP;
}

const civ_glyph_t *civ_glyph_get(civ_font_t *font, uint32_t codepoint)
{
    if (codepoint == 0) return NULL;
    cache_t *c = font->cache_opaque;

    /* open-addressing linear probe */
    size_t i = hash_cp(codepoint);
    for (size_t step = 0; step < CACHE_CAP; step++) {
        size_t s = (i + step) % CACHE_CAP;
        if (c->slots[s].key == codepoint) return c->slots[s].glyph;
        if (c->slots[s].key == 0) {
            civ_glyph_t *g = build_glyph((FT_Face)font->ft_face_opaque, codepoint);
            if (!g) return NULL;
            c->slots[s].key   = codepoint;
            c->slots[s].glyph = g;
            c->used++;
            return g;
        }
    }
    return NULL;        /* cache full — M2+ 再做 LRU */
}
