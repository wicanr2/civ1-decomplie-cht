/*
 * text/big5.h — Big5 → Unicode 轉換
 *
 * 對應 spec 02 dfCharSet 0x88 byte-pair walking 行為。
 * 內部走 POSIX iconv，handle 在第一次呼叫時 lazy 開啟。
 */
#ifndef CIV_TEXT_BIG5_H
#define CIV_TEXT_BIG5_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

/* 判斷 byte 是否是 Big5 第一 byte（lead byte）。 */
static inline bool civ_big5_is_lead(uint8_t b)
{
    return b >= 0xA1 && b <= 0xFE;
}

/* 判斷 byte 是否可作為 Big5 第二 byte（trail byte）。 */
static inline bool civ_big5_is_trail(uint8_t b)
{
    return (b >= 0x40 && b <= 0x7E) || (b >= 0xA1 && b <= 0xFE);
}

/* 把 2-byte Big5 序列轉成 Unicode codepoint。失敗回 0xFFFD。 */
uint32_t civ_big5_to_unicode(uint8_t lead, uint8_t trail);

/* 在 string 內走一個字元，回傳該字元的 Unicode codepoint，並把 *pos
 * 推進到下一個字元。len 是 string 總長度。
 * 如果 *pos >= len 時呼叫，回 0。 */
uint32_t civ_big5_walk(const uint8_t *str, size_t len, size_t *pos);

/* 釋放 iconv handle（process 結束前呼叫；可省略，反正 process 死了）。 */
void civ_big5_cleanup(void);

#endif /* CIV_TEXT_BIG5_H */
