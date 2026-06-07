/*
 * R21 dev probe — enumerate all resources in CIVDATA2.RSC.
 * 列出 id/name/size, 用於識別 advisor / parchment / spear / 等 unnamed CvPc.
 * 用後即丟; 識別完整入 .gitignore.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "res/rsrcfork.h"

static const char *fourcc_str(uint32_t fc, char buf[5])
{
    buf[0] = (fc >> 24) & 0xFF;
    buf[1] = (fc >> 16) & 0xFF;
    buf[2] = (fc >> 8) & 0xFF;
    buf[3] = fc & 0xFF;
    buf[4] = 0;
    return buf;
}

static void list_type(const civ_rsrc_t *r, uint32_t type)
{
    char fc[5];
    fourcc_str(type, fc);
    size_t n = civ_rsrc_count(r, type);
    printf("=== Type '%s': %zu entries ===\n", fc, n);
    for (size_t i = 0; i < n; i++) {
        const civ_rsrc_entry_t *e = civ_rsrc_iter(r, type, i);
        if (!e) continue;
        printf("  id=%5d  name=%-24s  size=%6zu",
               e->id, e->name ? e->name : "(none)", e->len);
        /* For CvPc: dump WxH from header (offset 0..3 = BE16 width, BE16 height) */
        if (type == CIV_FOURCC('C','v','P','c') && e->len >= 4) {
            int w = (e->data[0] << 8) | e->data[1];
            int h = (e->data[2] << 8) | e->data[3];
            printf("  %dx%d", w, h);
        }
        printf("\n");
    }
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "usage: %s <path/to/CIVDATA2.RSC>\n", argv[0]);
        return 1;
    }
    civ_rsrc_t *r = civ_rsrc_open(argv[1]);
    if (!r) { fprintf(stderr, "open fail\n"); return 1; }

    list_type(r, CIV_FOURCC('C','v','P','c'));
    list_type(r, CIV_FOURCC('S','T','R','#'));
    list_type(r, CIV_FOURCC('T','E','X','T'));

    civ_rsrc_close(r);
    return 0;
}
