#include "utf8.h"

uint32_t civ_utf8_walk(const uint8_t *str, size_t len, size_t *pos)
{
    if (*pos >= len) return 0;

    uint8_t b0 = str[*pos];

    /* 0xxxxxxx — ASCII */
    if (b0 < 0x80) { *pos += 1; return b0; }

    /* 10xxxxxx — 出現在第一 byte 表示非法序列 */
    if ((b0 & 0xC0) == 0x80) { *pos += 1; return 0xFFFD; }

    int extra;
    uint32_t cp;
    if ((b0 & 0xE0) == 0xC0) { extra = 1; cp = b0 & 0x1F; }
    else if ((b0 & 0xF0) == 0xE0) { extra = 2; cp = b0 & 0x0F; }
    else if ((b0 & 0xF8) == 0xF0) { extra = 3; cp = b0 & 0x07; }
    else { *pos += 1; return 0xFFFD; }

    if (*pos + extra >= len) { *pos += 1; return 0xFFFD; }

    for (int i = 1; i <= extra; i++) {
        uint8_t b = str[*pos + i];
        if ((b & 0xC0) != 0x80) { *pos += 1; return 0xFFFD; }
        cp = (cp << 6) | (b & 0x3F);
    }
    *pos += extra + 1;

    /* over-long / out-of-range 防護 */
    if (cp > 0x10FFFF || (cp >= 0xD800 && cp <= 0xDFFF)) return 0xFFFD;
    return cp;
}
