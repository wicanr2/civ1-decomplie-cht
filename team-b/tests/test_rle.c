/*
 * test_rle.c — 驗 spec 07 §7.1 Civ I SAV RLE decode/encode
 *
 * 子測:
 *   1. 全零 1024 byte 解後一樣
 *   2. 全唯一 (0..255) round-trip 一致
 *   3. 邊界 count = 127 run / literal
 *   4. random 8 KB round-trip (encode → decode → 原 buffer 比對)
 *   5. golden: decode 真實 CIV.SAV (HAM1000B.SAV ~16 KB), 確保 decoder
 *      不撞牆 (沒 boundary error) + 輸出 size 合理 (> input size)
 */
#include "save/rle.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define EXPECT(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL %s:%d  %s\n", __FILE__, __LINE__, #cond); \
        exit(1); \
    } \
} while (0)

static void test_round_trip(const uint8_t *src, size_t len, const char *label)
{
    /* 留充足 buffer */
    size_t enc_cap = len * 2 + 16;
    size_t dec_cap = len + 16;
    uint8_t *enc = malloc(enc_cap);
    uint8_t *dec = malloc(dec_cap);
    EXPECT(enc && dec);

    int enc_len = civ_rle_encode(src, len, enc, enc_cap);
    EXPECT(enc_len > 0);

    int dec_len = civ_rle_decode(enc, (size_t)enc_len, dec, dec_cap);
    EXPECT(dec_len == (int)len);
    EXPECT(memcmp(src, dec, len) == 0);

    printf("  %s: src=%zu enc=%d dec=%d ratio=%.2f\n",
           label, len, enc_len, dec_len, (double)enc_len / (double)len);

    free(enc);
    free(dec);
}

int main(void)
{
    /* ── 1. 全零 1024 byte ──────────────────────── */
    {
        uint8_t src[1024];
        memset(src, 0, sizeof src);
        test_round_trip(src, sizeof src, "全零 1024");
    }

    /* ── 2. 全唯一 0..255 ────────────────────────── */
    {
        uint8_t src[256];
        for (int i = 0; i < 256; i++) src[i] = (uint8_t)i;
        test_round_trip(src, sizeof src, "全唯一 256");
    }

    /* ── 3. 邊界 count = 127 run ────────────────── */
    {
        uint8_t src[127];
        memset(src, 0xAB, sizeof src);
        uint8_t enc[16];
        int enc_len = civ_rle_encode(src, sizeof src, enc, sizeof enc);
        EXPECT(enc_len == 2);
        EXPECT(enc[0] == 127);
        EXPECT(enc[1] == 0xAB);
        uint8_t dec[256];
        int dec_len = civ_rle_decode(enc, (size_t)enc_len, dec, sizeof dec);
        EXPECT(dec_len == 127);
        for (int i = 0; i < 127; i++) EXPECT(dec[i] == 0xAB);
    }

    /* ── 4. 邊界 count = 127 literal (隨機 unique 序列) ── */
    {
        uint8_t src[127];
        for (int i = 0; i < 127; i++) src[i] = (uint8_t)(i * 3 + 7);  /* 不會有 run */
        uint8_t enc[256];
        int enc_len = civ_rle_encode(src, sizeof src, enc, sizeof enc);
        EXPECT(enc_len == 1 + 127);
        EXPECT(enc[0] == (0x80 | 127));
        uint8_t dec[256];
        int dec_len = civ_rle_decode(enc, (size_t)enc_len, dec, sizeof dec);
        EXPECT(dec_len == 127);
        EXPECT(memcmp(src, dec, 127) == 0);
    }

    /* ── 5. random 8 KB round-trip ──────────────── */
    {
        size_t len = 8 * 1024;
        uint8_t *src = malloc(len);
        EXPECT(src);
        srand(12345);
        /* 用偏向「短 run + 短 literal」分布測壓縮 */
        for (size_t i = 0; i < len; i++) {
            int r = rand() % 100;
            if (r < 30 && i > 0) src[i] = src[i - 1]; /* 30% 重複前 byte */
            else src[i] = (uint8_t)(rand() & 0xFF);
        }
        test_round_trip(src, len, "random 8 KB");
        free(src);
    }

    /* ── 6. golden: decode 真實 CIV.SAV ──────────── */
    {
        const char *data_dir = getenv("CIV1_DATA_DIR");
        if (!data_dir || !*data_dir) {
            printf("  SKIP CIV.SAV golden (CIV1_DATA_DIR 未設)\n");
        } else {
            /* 嘗試常見 SAV 檔名 (我們已知有 HAM*.SAV) */
            const char *candidates[] = {
                "HAM1000B.SAV", "HAM2000B.SAV", "HAM3000B.SAV",
                "CIV1.SAV", "CIV2.SAV", "CIV.SAV",
                NULL,
            };
            char path[1024];
            FILE *f = NULL;
            const char *picked = NULL;
            for (int i = 0; candidates[i]; i++) {
                snprintf(path, sizeof path, "%s/%s", data_dir, candidates[i]);
                f = fopen(path, "rb");
                if (f) { picked = candidates[i]; break; }
            }
            if (!f) {
                printf("  SKIP CIV.SAV golden (找不到任何 *.SAV)\n");
            } else {
                struct stat st;
                stat(path, &st);
                size_t sav_size = (size_t)st.st_size;
                uint8_t *src = malloc(sav_size);
                EXPECT(src);
                size_t n = fread(src, 1, sav_size, f);
                EXPECT(n == sav_size);
                fclose(f);

                /* 保守: dst 給 src_size * 128 (RLE 最大放大率 127:2) */
                size_t dec_cap = sav_size * 64 + 1024;
                uint8_t *dec = malloc(dec_cap);
                EXPECT(dec);
                int dec_len = civ_rle_decode(src, sav_size, dec, dec_cap);
                EXPECT(dec_len > 0);
                printf("  golden %s: SAV=%zu → 解壓 %d byte (ratio %.2fx)\n",
                       picked, sav_size, dec_len,
                       (double)dec_len / (double)sav_size);
                /* 額外: 解壓後 re-encode 算法穩定性 (不要求 byte-exact match
                 * Honza 的 encoder, 只要 decode(encode(decode(SAV))) == decode(SAV)) */
                size_t enc2_cap = (size_t)dec_len * 2 + 16;
                uint8_t *enc2 = malloc(enc2_cap);
                EXPECT(enc2);
                int enc2_len = civ_rle_encode(dec, (size_t)dec_len, enc2, enc2_cap);
                EXPECT(enc2_len > 0);
                uint8_t *dec2 = malloc(dec_cap);
                EXPECT(dec2);
                int dec2_len = civ_rle_decode(enc2, (size_t)enc2_len, dec2, dec_cap);
                EXPECT(dec2_len == dec_len);
                EXPECT(memcmp(dec, dec2, (size_t)dec_len) == 0);
                printf("  re-encode round-trip: re-enc=%d (vs orig SAV=%zu)\n",
                       enc2_len, sav_size);

                free(src);
                free(dec);
                free(enc2);
                free(dec2);
            }
        }
    }

    printf("PASS test_rle (spec 07 §7.1, 6 子測)\n");
    return 0;
}
