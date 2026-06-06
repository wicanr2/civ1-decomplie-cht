/*
 * res/loader.h — 高層資源 loader 介面
 *
 * 對應 SDL_IMPLEMENTATION_PLAN §7。把 .RSC 內 CvPc 直接解成
 * civ_surface_t + palette；STR# / TEXT 解成 i18n catalog（M4+）。
 */
#ifndef CIV_RES_LOADER_H
#define CIV_RES_LOADER_H

#include "../gfx/palette.h"
#include "../gfx/surface.h"
#include "rsrcfork.h"

#include <stddef.h>
#include <stdint.h>

/* 從 .RSC 內找指定 (id 或 name) 的 CvPc，解成 surface。
 * opt_pal 非 NULL 時填入 CvPc 內 palette。
 * 失敗回 -1（找不到 / decode 錯誤）。 */
int civ_load_cvpc_by_id(const civ_rsrc_t *r, int16_t id,
                        civ_surface_t **out_surf,
                        civ_palette_t  *opt_pal);

int civ_load_cvpc_by_name(const civ_rsrc_t *r, const char *name,
                          civ_surface_t **out_surf,
                          civ_palette_t  *opt_pal);

#endif /* CIV_RES_LOADER_H */
