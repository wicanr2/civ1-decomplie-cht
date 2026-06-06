/*
 * gfx/cvpc.h — CvPc 影像 decoder
 *
 * CvPc 是 1991 Mac CIV 設計的點陣圖容器，1993 Windows 版直接沿用
 * (spec 01 §1.2 的 `gr_pic.c::LoadGifPicture` + `PicDecompress`)。
 *
 * 格式（spec 03 §3.4 + §9.1，已解開）：
 *   offset 0-1  : width  (BE16)
 *   offset 2-3  : height (BE16)
 *   offset 4    : LZW min_code_size  (直接！不是 ±1，例：7 → CLEAR=128)
 *   offset 5    : palette_count - 1
 *   offset 6+   : RGB triplets (3 bytes each)
 *   之後         : 標準 GIF89a Appendix F LZW（LSB-first bit packing，
 *                  length-prefixed sub-blocks，CLEAR = 1<<min_code，
 *                  END = CLEAR+1）
 */
#ifndef CIV_GFX_CVPC_H
#define CIV_GFX_CVPC_H

#include "palette.h"
#include "surface.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* 解一個 CvPc payload。
 *   payload / payload_len：來自 civ_rsrc_entry_t::data / .len
 *   out_surf：成功時填一個 new 出來的 civ_surface_t（caller 負責 free）
 *   opt_pal：非 NULL 時把 CvPc 內 palette 寫入該 palette_t；palette_count
 *           不到 256 的部分保留 caller 原值（不清零）
 *
 * 失敗 (header 損壞 / LZW 撞牆) 回 -1，out_surf 不變。 */
int civ_cvpc_decode(const uint8_t *payload, size_t payload_len,
                    civ_surface_t **out_surf,
                    civ_palette_t  *opt_pal);

#endif /* CIV_GFX_CVPC_H */
