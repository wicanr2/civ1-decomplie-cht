#include "sprite_sheet.h"

#include "../gfx/cvpc.h"

#include <stdlib.h>
#include <string.h>

#define FOURCC_CvPc CIV_FOURCC('C','v','P','c')

int civ_sprite_sheet_load(civ_rsrc_t *r, int16_t cvpc_id,
                          int tile_w, int tile_h,
                          civ_sprite_sheet_t *out)
{
    if (!r || !out || tile_w <= 0 || tile_h <= 0) return -1;
    memset(out, 0, sizeof *out);

    const civ_rsrc_entry_t *e = civ_rsrc_find(r, FOURCC_CvPc, cvpc_id);
    if (!e) return -1;

    civ_palette_zero(&out->pal);
    if (civ_cvpc_decode(e->data, e->len, &out->sheet, &out->pal) != 0)
        return -1;

    out->tile_w = tile_w;
    out->tile_h = tile_h;
    out->cols   = out->sheet->w / tile_w;
    out->rows   = out->sheet->h / tile_h;
    return 0;
}

void civ_sprite_sheet_free(civ_sprite_sheet_t *s)
{
    if (!s) return;
    if (s->sheet) civ_surface_free(s->sheet);
    s->sheet = NULL;
}

void civ_sprite_sheet_build_lut(civ_sprite_sheet_t *s,
                                const civ_palette_t *base)
{
    if (!s || !base) return;
    civ_palette_build_lut(s->pal.entries, 256, base, s->lut);
    s->lut_built = true;
}
