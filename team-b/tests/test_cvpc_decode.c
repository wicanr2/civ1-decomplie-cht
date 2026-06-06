/*
 * test_cvpc_decode.c — M3 驗證：CvPc decoder 對齊 spec 03 §3.4 + §9.1
 *
 * 若 CIV1_DATA_DIR env 未設就 SKIP。
 * 設定時對 Civdata0.RSC 內的 EARTH (id 137, 320×200) 做：
 *   1. surface 尺寸 = 320×200
 *   2. 解出 pixel 數 = 320 * 200
 *   3. palette[0] = 黑（CIV1 慣例第 0 個 entry 是黑色）
 *   4. 至少一個非 0 的 palette index 出現在 surface 內（不是全黑）
 */
#include "gfx/cvpc.h"
#include "gfx/palette.h"
#include "gfx/surface.h"
#include "res/loader.h"
#include "res/rsrcfork.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EXPECT(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL %s:%d  %s\n", __FILE__, __LINE__, #cond); \
        exit(1); \
    } \
} while (0)

int main(void)
{
    const char *data_dir = getenv("CIV1_DATA_DIR");
    if (!data_dir || !*data_dir) {
        printf("SKIP test_cvpc_decode（CIV1_DATA_DIR 未設）\n");
        return 0;
    }

    char path[1024];
    snprintf(path, sizeof path, "%s/Civdata0.RSC", data_dir);
    civ_rsrc_t *r = civ_rsrc_open(path);
    if (!r) {
        snprintf(path, sizeof path, "%s/CIVDATA0.RSC", data_dir);
        r = civ_rsrc_open(path);
    }
    if (!r) {
        printf("SKIP test_cvpc_decode（找不到 Civdata0.RSC）\n");
        return 0;
    }

    /* 先列出所有 CvPc + 它們的 name，方便診斷 */
    size_t n_cvpc = civ_rsrc_count(r, CIV_FOURCC('C','v','P','c'));
    printf("Civdata0 CvPc 清單:\n");
    for (size_t i = 0; i < n_cvpc; i++) {
        const civ_rsrc_entry_t *e = civ_rsrc_iter(r, CIV_FOURCC('C','v','P','c'), i);
        if (!e) continue;
        printf("  [%zu] id=%d  name=%s  size=%zu\n",
               i, e->id, e->name ? e->name : "(none)", e->len);
    }

    civ_surface_t *surf = NULL;
    civ_palette_t  pal  = {0};

    /* by_id 137 = EARTH（從 extract_tiles.py 輸出已確認） */
    int rc = civ_load_cvpc_by_id(r, 137, &surf, &pal);
    EXPECT(rc == 0);

    /* 驗 by_name 對副檔名敏感 — Mac Resource Fork 內 name 帶 .GIF */
    civ_surface_t *surf2 = NULL;
    EXPECT(civ_load_cvpc_by_name(r, "EARTH", &surf2, NULL) == -1);
    EXPECT(civ_load_cvpc_by_name(r, "EARTH.GIF", &surf2, NULL) == 0);
    EXPECT(surf2 != NULL);
    EXPECT(surf2->w == 320 && surf2->h == 200);
    civ_surface_free(surf2);
    EXPECT(surf != NULL);
    EXPECT(surf->w == 320);
    EXPECT(surf->h == 200);
    EXPECT(surf->pixels != NULL);

    /* palette[0] 為黑 */
    EXPECT(pal.entries[0].r == 0);
    EXPECT(pal.entries[0].g == 0);
    EXPECT(pal.entries[0].b == 0);

    /* surface 不能全 0 */
    int total = surf->w * surf->h;
    int nonzero = 0;
    int distinct[256] = {0};
    for (int i = 0; i < total; i++) {
        uint8_t v = surf->pixels[i];
        if (v != 0) nonzero++;
        distinct[v] = 1;
    }
    int n_distinct = 0;
    for (int i = 0; i < 256; i++) n_distinct += distinct[i];

    printf("EARTH 320x200: nonzero=%d / total=%d, distinct indices=%d\n",
           nonzero, total, n_distinct);
    EXPECT(nonzero > 100);
    EXPECT(n_distinct > 4);

    civ_surface_free(surf);
    civ_rsrc_close(r);
    printf("PASS test_cvpc_decode\n");
    return 0;
}
