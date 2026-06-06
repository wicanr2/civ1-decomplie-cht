/*
 * text/utf8.h — UTF-8 → Unicode codepoint 走訪
 *
 * C source 內的中文字面值預設是 UTF-8（modern toolchain），所以
 * text_out 走的是 UTF-8。Track A 翻譯 catalog（JSON）也是 UTF-8。
 *
 * Big5 walker 留在 big5.h 用於：(a) Track A patched binary 內 inline
 * 字串的偵錯，(b) 未來若要直讀原版 .RSC 內 STR# / TEXT 段。
 */
#ifndef CIV_TEXT_UTF8_H
#define CIV_TEXT_UTF8_H

#include <stdint.h>
#include <stddef.h>

/* 取出一個 codepoint。*pos 推進到下一個字元起點。
 * 越界回 0；非法 byte 回 0xFFFD 並前進 1 byte。 */
uint32_t civ_utf8_walk(const uint8_t *str, size_t len, size_t *pos);

#endif /* CIV_TEXT_UTF8_H */
