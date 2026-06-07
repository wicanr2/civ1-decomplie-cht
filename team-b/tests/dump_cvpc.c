/*
 * R21 dev probe — dump 1 CvPc (id from argv) 用其自身 palette 直接寫 PPM.
 * 用後即丟.
 */
#include <stdio.h>
#include <stdlib.h>
#include "res/rsrcfork.h"
#include "res/loader.h"
#include "gfx/surface.h"
#include "gfx/palette.h"

int main(int argc, char **argv)
{
    if (argc < 4) {
        fprintf(stderr, "usage: %s <RSC path> <CvPc id> <out.ppm>\n", argv[0]);
        return 1;
    }
    int16_t id = (int16_t)atoi(argv[2]);
    civ_rsrc_t *r = civ_rsrc_open(argv[1]);
    if (!r) { fprintf(stderr, "open fail\n"); return 1; }

    civ_surface_t *s = NULL;
    civ_palette_t  p = {0};
    if (civ_load_cvpc_by_id(r, id, &s, &p) != 0) {
        fprintf(stderr, "load CvPc %d fail\n", id);
        civ_rsrc_close(r);
        return 1;
    }

    FILE *f = fopen(argv[3], "wb");
    if (!f) { fprintf(stderr, "write fail\n"); return 1; }
    fprintf(f, "P6\n%d %d\n255\n", s->w, s->h);
    for (int y = 0; y < s->h; y++) {
        for (int x = 0; x < s->w; x++) {
            uint8_t idx = s->pixels[y * s->pitch + x];
            civ_color_t c = p.entries[idx];
            uint8_t rgb[3] = { c.r, c.g, c.b };
            fwrite(rgb, 1, 3, f);
        }
    }
    fclose(f);
    fprintf(stderr, "PASS: %s (%dx%d)\n", argv[3], s->w, s->h);

    civ_surface_free(s);
    civ_rsrc_close(r);
    return 0;
}
