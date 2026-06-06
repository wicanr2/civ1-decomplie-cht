/*
 * test_glyph_cache.c — M1 驗證：Big5 walker + FreeType glyph cache。
 *
 * 不假設特定字型；若預設 font 不存在則直接 PASS（skip）— 因為
 * font 可能因平台而異，但 byte-pair walker 仍應在 host 無字型時可用。
 */
#include "text/big5.h"
#include "text/glyph_cache.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define EXPECT(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL %s:%d  %s\n", __FILE__, __LINE__, #cond); \
        exit(1); \
    } \
} while (0)

#ifndef CIV_DEFAULT_FONT_PATH
#define CIV_DEFAULT_FONT_PATH "/usr/share/fonts/truetype/arphic/uming.ttc"
#endif

/* ─── Big5 walker ───────────────────────────────────────────────── */

static void test_big5_lead_trail(void)
{
    EXPECT(civ_big5_is_lead(0xA4));
    EXPECT(civ_big5_is_lead(0xFE));
    EXPECT(!civ_big5_is_lead(0xA0));
    EXPECT(!civ_big5_is_lead(0x41));

    EXPECT(civ_big5_is_trail(0x40));
    EXPECT(civ_big5_is_trail(0xA1));
    EXPECT(!civ_big5_is_trail(0x3F));
    EXPECT(!civ_big5_is_trail(0x80));
}

static void test_big5_walk_ascii(void)
{
    const char *s = "abc";
    size_t pos = 0;
    EXPECT(civ_big5_walk((const uint8_t*)s, 3, &pos) == 'a');
    EXPECT(pos == 1);
    EXPECT(civ_big5_walk((const uint8_t*)s, 3, &pos) == 'b');
    EXPECT(pos == 2);
    EXPECT(civ_big5_walk((const uint8_t*)s, 3, &pos) == 'c');
    EXPECT(pos == 3);
}

static void test_big5_walk_zh(void)
{
    /* "文明" 在 Big5 = A4 E5 A9 FA */
    uint8_t s[] = {0xA4, 0xE5, 0xA9, 0xFA};
    size_t  pos = 0;
    uint32_t cp1 = civ_big5_walk(s, 4, &pos);
    EXPECT(pos == 2);
    EXPECT(cp1 == 0x6587);    /* U+6587 「文」 */
    uint32_t cp2 = civ_big5_walk(s, 4, &pos);
    EXPECT(pos == 4);
    EXPECT(cp2 == 0x660E);    /* U+660E 「明」 */
}

/* ─── glyph cache ──────────────────────────────────────────────── */

static int file_exists(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0;
}

static void test_font_open_close(void)
{
    if (!file_exists(CIV_DEFAULT_FONT_PATH)) {
        printf("SKIP test_font_open_close（找不到 %s）\n", CIV_DEFAULT_FONT_PATH);
        return;
    }
    civ_font_t *f = civ_font_open(CIV_DEFAULT_FONT_PATH, 16);
    EXPECT(f);
    EXPECT(f->size_px == 16);
    EXPECT(f->ascent > 0);
    civ_font_close(f);
}

static void test_glyph_ascii(void)
{
    if (!file_exists(CIV_DEFAULT_FONT_PATH)) {
        printf("SKIP test_glyph_ascii\n");
        return;
    }
    civ_font_t *f = civ_font_open(CIV_DEFAULT_FONT_PATH, 16);
    EXPECT(f);
    const civ_glyph_t *g = civ_glyph_get(f, 'A');
    EXPECT(g);
    EXPECT(g->w > 0 && g->h > 0);
    EXPECT(g->mask);
    EXPECT(g->advance_x > 0);
    /* 二次取應 hit cache，回同 pointer */
    const civ_glyph_t *g2 = civ_glyph_get(f, 'A');
    EXPECT(g2 == g);
    civ_font_close(f);
}

static void test_glyph_cjk(void)
{
    if (!file_exists(CIV_DEFAULT_FONT_PATH)) {
        printf("SKIP test_glyph_cjk\n");
        return;
    }
    civ_font_t *f = civ_font_open(CIV_DEFAULT_FONT_PATH, 16);
    EXPECT(f);
    /* U+6587 「文」 */
    const civ_glyph_t *g = civ_glyph_get(f, 0x6587);
    EXPECT(g);
    EXPECT(g->w > 0 && g->h > 0);
    /* CJK 字應該寬度接近 size_px，allow ± */
    EXPECT(g->advance_x > 8 && g->advance_x < 24);
    civ_font_close(f);
}

int main(void)
{
    test_big5_lead_trail();
    test_big5_walk_ascii();
    test_big5_walk_zh();
    test_font_open_close();
    test_glyph_ascii();
    test_glyph_cjk();
    civ_big5_cleanup();
    printf("PASS test_glyph_cache\n");
    return 0;
}
