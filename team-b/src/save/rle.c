#include "rle.h"

/*
 * Civ I 1993 Win SAV RLE 算法.
 *
 * Decode (對應 spec 07 §7.1 虛擬碼):
 *
 *   while (src 還有 byte) {
 *       c = src[i++];
 *       if (c < 0x80) {
 *           r = src[i++];
 *           emit r × c times;
 *       } else {
 *           c -= 0x80;
 *           emit src[i..i+c) verbatim;
 *           i += c;
 *       }
 *   }
 *
 * 注意: 若 c == 0 (or c == 0x80), 兩條 path 都 emit 0 byte, 無 effect 但消
 * 一個 input byte. Honza 的 encoder 不該產生這種 packet, 但 decoder 仍要容忍.
 */

int civ_rle_decode(const uint8_t *src, size_t src_len,
                   uint8_t *dst, size_t dst_cap)
{
    size_t in = 0, out = 0;
    while (in < src_len) {
        uint8_t c = src[in++];
        if (c < 0x80) {
            if (in >= src_len) return -1;     /* src 中途斷掉 */
            uint8_t r = src[in++];
            if (out + c > dst_cap) return -1; /* dst overflow */
            for (uint8_t k = 0; k < c; k++) dst[out++] = r;
        } else {
            uint8_t cnt = (uint8_t)(c - 0x80);
            if (in + cnt > src_len) return -1;
            if (out + cnt > dst_cap) return -1;
            for (uint8_t k = 0; k < cnt; k++) dst[out++] = src[in++];
        }
    }
    return (int)out;
}

/*
 * Encode 算法 (greedy):
 *
 *   while (i < src_len) {
 *       1. 算當前 src[i] 後同值 run 長度 r (含 src[i])
 *       2. if r >= 2: 出 packet [r] [src[i]], advance i+=r
 *       3. else (沒 run): 累積 literal 直到下個 run 或 max 127 byte
 *          - 邊掃邊找 next run start; 若 next byte 又跟下下 byte 同
 *            就停 literal (讓 run 接手)
 *          - 出 packet [c|0x80] [c bytes], advance
 *   }
 *
 * 限制: count ∈ [1, 127] (avoid 0, 因為 0x00 / 0x80 packet 是 no-op
 * encoder 不產生).
 */

static size_t count_run(const uint8_t *p, size_t avail)
{
    if (avail == 0) return 0;
    uint8_t v = p[0];
    size_t r = 1;
    while (r < avail && r < 127 && p[r] == v) r++;
    return r;
}

int civ_rle_encode(const uint8_t *src, size_t src_len,
                   uint8_t *dst, size_t dst_cap)
{
    size_t in = 0, out = 0;

    while (in < src_len) {
        size_t r = count_run(&src[in], src_len - in);
        if (r >= 2) {
            /* run mode: [count] [byte] */
            if (out + 2 > dst_cap) return -1;
            dst[out++] = (uint8_t)r;
            dst[out++] = src[in];
            in += r;
        } else {
            /* literal mode: 找下個 run start (>= 2 同) 為止 */
            size_t lit_start = in;
            size_t lit_end   = in + 1;
            while (lit_end < src_len && (lit_end - lit_start) < 127) {
                /* 若 src[lit_end] 之後有 >= 2 同值, 停 literal 讓 run 接手 */
                size_t look = count_run(&src[lit_end], src_len - lit_end);
                if (look >= 2) break;
                lit_end++;
            }
            size_t lit_len = lit_end - lit_start;
            /* lit_len 範圍 1..127. Honza 的 0x80 packet 是 0 byte (no-op),
             * 我們從 1 開始. */
            if (out + 1 + lit_len > dst_cap) return -1;
            dst[out++] = (uint8_t)(0x80 | lit_len);
            for (size_t k = 0; k < lit_len; k++) dst[out++] = src[lit_start + k];
            in = lit_end;
        }
    }
    return (int)out;
}
