#include "big5.h"

#include <iconv.h>
#include <stdint.h>
#include <string.h>

/* iconv handle 全 process 共用一份，第一次呼叫時開啟。
 * 不 thread-safe — M1+ 若要多執行緒得改成 thread-local。 */
static iconv_t g_iconv_big5_to_u32 = (iconv_t)-1;

static void big5_init(void)
{
    if (g_iconv_big5_to_u32 == (iconv_t)-1) {
        /* glibc 用 "BIG5" 或 "BIG-5"；UTF-32LE 是 4-byte little-endian Unicode */
        g_iconv_big5_to_u32 = iconv_open("UTF-32LE", "BIG5");
        if (g_iconv_big5_to_u32 == (iconv_t)-1) {
            /* fallback：CP950 是 Windows 的 Big5 變體 */
            g_iconv_big5_to_u32 = iconv_open("UTF-32LE", "CP950");
        }
    }
}

void civ_big5_cleanup(void)
{
    if (g_iconv_big5_to_u32 != (iconv_t)-1) {
        iconv_close(g_iconv_big5_to_u32);
        g_iconv_big5_to_u32 = (iconv_t)-1;
    }
}

uint32_t civ_big5_to_unicode(uint8_t lead, uint8_t trail)
{
    big5_init();
    if (g_iconv_big5_to_u32 == (iconv_t)-1) return 0xFFFD;

    char    inbuf[2]  = { (char)lead, (char)trail };
    char    outbuf[8] = {0};
    char   *in_p      = inbuf;
    char   *out_p     = outbuf;
    size_t  in_left   = 2;
    size_t  out_left  = 4;   /* 一個 UTF-32 codepoint */

    size_t n = iconv(g_iconv_big5_to_u32, &in_p, &in_left, &out_p, &out_left);
    if (n == (size_t)-1 || out_left == 4) {
        /* 重置 iconv 狀態以免下次呼叫帶髒 state（in 與 out 用兩組變數
         * 避免 -Wrestrict warning） */
        char   *r_in = NULL,  *r_out = NULL;
        size_t  r_in_left = 0, r_out_left = 0;
        iconv(g_iconv_big5_to_u32, &r_in, &r_in_left, &r_out, &r_out_left);
        return 0xFFFD;
    }

    uint32_t cp;
    memcpy(&cp, outbuf, 4);
    return cp;
}

uint32_t civ_big5_walk(const uint8_t *str, size_t len, size_t *pos)
{
    if (*pos >= len) return 0;
    uint8_t c = str[*pos];
    if (civ_big5_is_lead(c) && *pos + 1 < len && civ_big5_is_trail(str[*pos + 1])) {
        uint32_t cp = civ_big5_to_unicode(c, str[*pos + 1]);
        *pos += 2;
        return cp;
    }
    *pos += 1;
    return c;        /* ASCII / Latin1，直接當 codepoint */
}
