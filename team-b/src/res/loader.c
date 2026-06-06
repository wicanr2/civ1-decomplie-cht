#include "loader.h"
#include "../gfx/cvpc.h"

#include <string.h>

#define FOURCC_CvPc CIV_FOURCC('C','v','P','c')

int civ_load_cvpc_by_id(const civ_rsrc_t *r, int16_t id,
                        civ_surface_t **out_surf,
                        civ_palette_t  *opt_pal)
{
    if (!r) return -1;
    const civ_rsrc_entry_t *e = civ_rsrc_find(r, FOURCC_CvPc, id);
    if (!e) return -1;
    return civ_cvpc_decode(e->data, e->len, out_surf, opt_pal);
}

int civ_load_cvpc_by_name(const civ_rsrc_t *r, const char *name,
                          civ_surface_t **out_surf,
                          civ_palette_t  *opt_pal)
{
    if (!r || !name) return -1;
    size_t n = civ_rsrc_count(r, FOURCC_CvPc);
    for (size_t i = 0; i < n; i++) {
        const civ_rsrc_entry_t *e = civ_rsrc_iter(r, FOURCC_CvPc, i);
        if (e && e->name && strcmp(e->name, name) == 0) {
            return civ_cvpc_decode(e->data, e->len, out_surf, opt_pal);
        }
    }
    return -1;
}
