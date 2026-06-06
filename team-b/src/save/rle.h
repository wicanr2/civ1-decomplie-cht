/*
 * save/rle.h — Civ I 1993 Win SAV file 內層 RLE 壓縮/解壓
 *
 * 對應 spec 07 §7.1, spec 01 §1.2 `load.c::RLLEncode/Decode`.
 *
 * RLE 格式 (byte stream, NOT bitstream):
 *   packet_byte < 0x80 (= 0..127):
 *     count = packet_byte (0..127); data_byte = next byte
 *     output [data_byte] × count times to decompressed stream
 *
 *   packet_byte >= 0x80 (= 128..255):
 *     count = packet_byte - 0x80 (0..127)
 *     output next count bytes verbatim
 *
 * 來源: Honza Havlicek 2008 CivWin File Format demonstrator (spec 07).
 */
#ifndef CIV_SAVE_RLE_H
#define CIV_SAVE_RLE_H

#include <stddef.h>
#include <stdint.h>

/* 解壓 src[0..src_len) 到 dst[0..dst_cap). 回實際輸出 byte 數;
 * dst overflow 回 -1; src 中途斷掉回 -1. */
int civ_rle_decode(const uint8_t *src, size_t src_len,
                   uint8_t *dst, size_t dst_cap);

/* 壓 src[0..src_len) 到 dst[0..dst_cap). 回實際輸出 byte 數;
 * dst overflow 回 -1.
 *
 * 算法: greedy run + literal:
 *   1. 看當前 byte 之後相同 byte 數 r (含當前)
 *   2. 若 r >= 2: 出 [r] [byte] (run mode), advance r
 *   3. 否則: 累積 literal block 直到下個 run 或 max 127, 出
 *      [count|0x80] [count bytes] */
int civ_rle_encode(const uint8_t *src, size_t src_len,
                   uint8_t *dst, size_t dst_cap);

#endif /* CIV_SAVE_RLE_H */
