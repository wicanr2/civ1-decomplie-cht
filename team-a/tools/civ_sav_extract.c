/*
 * civ_sav_extract.c — Team A 工具
 *
 * 把 Civ I 1993 Win *.SAV 經 civ_rle_decode 解壓成 raw .bin
 * 給 spec 07 §7.2 內部 layout 分析用.
 *
 * 用法:
 *   civ_sav_extract input.SAV output.bin
 *
 * 編譯 (standalone, 連 civ_core static lib):
 *   gcc -I team-b/src civ_sav_extract.c team-b/build/libciv_core.a -lSDL2 -lfreetype -lm
 */
#include "save/rle.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

int main(int argc, char **argv)
{
    if (argc != 3) {
        fprintf(stderr, "usage: %s input.SAV output.bin\n", argv[0]);
        return 2;
    }
    FILE *fi = fopen(argv[1], "rb");
    if (!fi) { perror(argv[1]); return 1; }
    struct stat st;
    stat(argv[1], &st);
    size_t in_size = (size_t)st.st_size;
    uint8_t *in = malloc(in_size);
    fread(in, 1, in_size, fi);
    fclose(fi);

    /* 保守 dst cap: in_size × 128 上限 (RLE 最大膨脹比 127:2) */
    size_t cap = in_size * 64 + 1024;
    uint8_t *out = malloc(cap);
    int out_len = civ_rle_decode(in, in_size, out, cap);
    if (out_len < 0) {
        fprintf(stderr, "decode 失敗\n");
        return 1;
    }

    FILE *fo = fopen(argv[2], "wb");
    if (!fo) { perror(argv[2]); return 1; }
    fwrite(out, 1, (size_t)out_len, fo);
    fclose(fo);
    printf("%s (%zu byte) → %s (%d byte, ratio %.2fx)\n",
           argv[1], in_size, argv[2], out_len,
           (double)out_len / (double)in_size);

    free(in);
    free(out);
    return 0;
}
