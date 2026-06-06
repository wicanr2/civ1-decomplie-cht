#include "cvpc.h"

#include <stdlib.h>
#include <string.h>

/* ── 標準 GIF89a LZW decoder (LSB-first) ──────────────── */

#define LZW_MAX_CODE 4096

typedef struct lzw {
    /* 字典：每個 entry 有 (prefix, suffix)；展開時走 prefix 鏈 */
    uint16_t prefix[LZW_MAX_CODE];
    uint8_t  suffix[LZW_MAX_CODE];
    uint8_t  first[LZW_MAX_CODE];   /* 字串首 byte，KwKwK 用 */
    int      next_code;
    int      code_size;
    int      mask;
    int      min_code_size;
    int      clear_code;
    int      end_code;
    int      prev_code;             /* -1 表示剛 CLEAR */
} lzw_t;

static void lzw_reset(lzw_t *l, int min_code_size)
{
    l->min_code_size = min_code_size;
    l->clear_code    = 1 << min_code_size;
    l->end_code      = l->clear_code + 1;
    l->code_size     = min_code_size + 1;
    l->mask          = (1 << l->code_size) - 1;
    l->next_code     = l->end_code + 1;
    l->prev_code     = -1;
    /* leaves 0..clear-1 */
    for (int i = 0; i < l->clear_code; i++) {
        l->prefix[i] = 0xFFFF;     /* leaf sentinel */
        l->suffix[i] = (uint8_t)i;
        l->first[i]  = (uint8_t)i;
    }
}

/* 展開 code 的字串到 out（從尾巴往前寫；caller 知道 out 是 reverse buf）。
 * 回傳寫入的 byte 數，並把字串的「首 byte」寫到 *first_out。 */
static int lzw_expand_reverse(const lzw_t *l, int code, uint8_t *rev_out,
                              uint8_t *first_out)
{
    int wrote = 0;
    while (code != 0xFFFF) {
        rev_out[wrote++] = l->suffix[code];
        if (l->prefix[code] == 0xFFFF) {
            *first_out = l->suffix[code];
            break;
        }
        code = l->prefix[code];
    }
    return wrote;
}

/* GIF89a sub-block defragmenter 同時做 LSB-first bit reader。
 * 模型：caller 持有 cursor (block_left, src 指 next byte)。
 * 取一個 code，回傳 -1 表示流結束。 */
typedef struct bitreader {
    const uint8_t *payload;
    size_t         payload_len;
    size_t         pos;             /* 在 payload 內的位置 */
    int            block_left;      /* 目前 sub-block 還剩幾 byte */
    uint32_t       bit_buf;
    int            bit_count;
} bitreader_t;

static int br_fetch_byte(bitreader_t *br, uint8_t *out)
{
    while (br->block_left == 0) {
        if (br->pos >= br->payload_len) return -1;
        uint8_t len = br->payload[br->pos++];
        if (len == 0) return -1;
        br->block_left = len;
    }
    if (br->pos >= br->payload_len) return -1;
    *out = br->payload[br->pos++];
    br->block_left--;
    return 0;
}

static int br_read_code(bitreader_t *br, int n_bits)
{
    while (br->bit_count < n_bits) {
        uint8_t b;
        if (br_fetch_byte(br, &b) < 0) return -1;
        br->bit_buf  |= (uint32_t)b << br->bit_count;
        br->bit_count += 8;
    }
    int code = (int)(br->bit_buf & (((uint32_t)1 << n_bits) - 1));
    br->bit_buf  >>= n_bits;
    br->bit_count -= n_bits;
    return code;
}

/* ── decode 入口 ────────────────────────────────── */

int civ_cvpc_decode(const uint8_t *payload, size_t payload_len,
                    civ_surface_t **out_surf,
                    civ_palette_t  *opt_pal)
{
    if (!payload || payload_len < 6 || !out_surf) return -1;

    int w = ((int)payload[0] << 8) | payload[1];
    int h = ((int)payload[2] << 8) | payload[3];
    int min_code = payload[4];
    int pal_n    = (int)payload[5] + 1;
    if (w <= 0 || h <= 0 || min_code < 2 || min_code > 11) return -1;

    size_t pal_off = 6;
    size_t pal_end = pal_off + (size_t)pal_n * 3;
    if (pal_end > payload_len) return -1;

    if (opt_pal) {
        for (int i = 0; i < pal_n && i < 256; i++) {
            civ_color_t c = {
                payload[pal_off + i * 3 + 0],
                payload[pal_off + i * 3 + 1],
                payload[pal_off + i * 3 + 2],
                255
            };
            opt_pal->entries[i] = c;
        }
        opt_pal->generation++;
    }

    civ_surface_t *surf = civ_surface_new(w, h);
    if (!surf) return -1;

    bitreader_t br = {
        .payload      = payload,
        .payload_len  = payload_len,
        .pos          = pal_end,
        .block_left   = 0,
        .bit_buf      = 0,
        .bit_count    = 0,
    };

    lzw_t lz;
    lzw_reset(&lz, min_code);

    size_t out_pos = 0;
    size_t total   = (size_t)w * (size_t)h;
    uint8_t *out   = surf->pixels;
    uint8_t rev[LZW_MAX_CODE];

    for (;;) {
        int code = br_read_code(&br, lz.code_size);
        if (code < 0) break;              /* 流結束 */
        if (code == lz.clear_code) {
            lzw_reset(&lz, min_code);
            continue;
        }
        if (code == lz.end_code) break;

        int entry;
        uint8_t first_byte;
        int n;

        if (code < lz.next_code) {
            entry = code;
            n = lzw_expand_reverse(&lz, entry, rev, &first_byte);
        } else if (code == lz.next_code && lz.prev_code != -1) {
            /* KwKwK */
            n = lzw_expand_reverse(&lz, lz.prev_code, rev, &first_byte);
            if (n + 1 > LZW_MAX_CODE) break;
            rev[n++] = first_byte;       /* 把 first_byte 接到尾巴 */
            entry = lz.prev_code;        /* 標記後面的字典更新用前一條 */
            first_byte = first_byte;     /* 維持 */
        } else {
            /* 損壞 */
            break;
        }

        /* 把 rev 反向寫到 out */
        for (int i = n - 1; i >= 0; i--) {
            if (out_pos >= total) goto done;
            out[out_pos++] = rev[i];
        }
        if (out_pos >= total) goto done;

        /* 字典加新條目 */
        if (lz.prev_code != -1 && lz.next_code < LZW_MAX_CODE) {
            lz.prefix[lz.next_code] = (uint16_t)lz.prev_code;
            lz.suffix[lz.next_code] = first_byte;
            lz.first [lz.next_code] = lz.first[lz.prev_code];
            lz.next_code++;
            if (lz.next_code == (1 << lz.code_size) && lz.code_size < 12) {
                lz.code_size++;
                lz.mask = (1 << lz.code_size) - 1;
            }
        }
        lz.prev_code = code;
    }

done:
    surf->dirty = true;
    *out_surf = surf;
    return 0;
}
