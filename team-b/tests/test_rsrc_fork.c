/*
 * test_rsrc_fork.c — M3 驗證：Mac Resource Fork parser 對齊 spec 03 §3.1
 *
 * 若 CIV1_DATA_DIR env var 未設定就 SKIP（避免 CI 強制要求原版資產）。
 * 設定時測 Civdata0.RSC 內 type 計數對 spec 03 §3.1（4 CvPc / 33 STR# /
 * 399 TEXT / 7 GDAT / 14 KDAT）。
 */
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
        printf("SKIP test_rsrc_fork（CIV1_DATA_DIR 未設）\n");
        return 0;
    }

    char path[1024];
    /* CIVDATA / Civdata 大小寫雜亂；先試 Civdata0.RSC */
    snprintf(path, sizeof path, "%s/Civdata0.RSC", data_dir);
    civ_rsrc_t *r = civ_rsrc_open(path);
    if (!r) {
        snprintf(path, sizeof path, "%s/CIVDATA0.RSC", data_dir);
        r = civ_rsrc_open(path);
    }
    if (!r) {
        printf("SKIP test_rsrc_fork（%s/Civdata0.RSC 找不到）\n", data_dir);
        return 0;
    }

    /* spec 03 §3.1：Civdata0 內 4 CvPc + 33 STR# + 399 TEXT + 7 GDAT
     *               + 14 KDAT */
    size_t n_cvpc = civ_rsrc_count(r, CIV_FOURCC('C','v','P','c'));
    size_t n_str  = civ_rsrc_count(r, CIV_FOURCC('S','T','R',' '));    /* STR# 的 # 實際是 0x23 */
    size_t n_str_hash = civ_rsrc_count(r, CIV_FOURCC('S','T','R','#'));
    size_t n_text = civ_rsrc_count(r, CIV_FOURCC('T','E','X','T'));
    size_t n_gdat = civ_rsrc_count(r, CIV_FOURCC('G','D','A','T'));
    size_t n_kdat = civ_rsrc_count(r, CIV_FOURCC('K','D','A','T'));

    printf("Civdata0: CvPc=%zu STR#=%zu STR-space=%zu TEXT=%zu GDAT=%zu KDAT=%zu\n",
           n_cvpc, n_str_hash, n_str, n_text, n_gdat, n_kdat);

    EXPECT(n_cvpc == 4);
    EXPECT(n_str_hash == 33);
    EXPECT(n_text == 399);
    EXPECT(n_gdat == 7);
    EXPECT(n_kdat == 14);

    /* 抽一個 CvPc payload 大概看一下 */
    const civ_rsrc_entry_t *e = civ_rsrc_iter(r, CIV_FOURCC('C','v','P','c'), 0);
    EXPECT(e != NULL);
    EXPECT(e->data != NULL);
    EXPECT(e->len > 6);
    /* width / height BE16 */
    int w = ((int)e->data[0] << 8) | e->data[1];
    int h = ((int)e->data[2] << 8) | e->data[3];
    EXPECT(w > 0 && w < 4096);
    EXPECT(h > 0 && h < 4096);
    printf("  first CvPc id=%d  %dx%d  (name=%s)\n",
           e->id, w, h, e->name ? e->name : "(none)");

    civ_rsrc_close(r);
    printf("PASS test_rsrc_fork\n");
    return 0;
}
