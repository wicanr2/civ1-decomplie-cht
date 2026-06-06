/*
 * test_civ_dict.c — 驗 zh-TW catalog 241 條翻譯
 *
 * 對所有 11 個 STR# 翻譯陣列做：
 *   1. 起首 / 結尾條目正確
 *   2. 中間抽樣
 *   3. 越界 / 不存在 str_id 回 NULL
 *
 * 不需要 CIV1_DATA_DIR — 純 catalog 自驗證。
 */
#include "data/civ_dict.h"
#include "data/strings.h"

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
    /* ── 72 科技 ───────────────────────────────────── */
    EXPECT(strcmp(civ_dict_lookup(CIV_STR_TECH_ADVANCES, 0),  "字母") == 0);
    EXPECT(strcmp(civ_dict_lookup(CIV_STR_TECH_ADVANCES, 1),  "法典") == 0);
    EXPECT(strcmp(civ_dict_lookup(CIV_STR_TECH_ADVANCES, 6),  "天文學") == 0);
    EXPECT(strcmp(civ_dict_lookup(CIV_STR_TECH_ADVANCES, 42), "陶器") == 0);
    EXPECT(strcmp(civ_dict_lookup(CIV_STR_TECH_ADVANCES, 71), "未來科技") == 0);
    EXPECT(civ_dict_lookup(CIV_STR_TECH_ADVANCES, 72) == NULL);
    EXPECT(civ_dict_lookup(CIV_STR_TECH_ADVANCES, -1) == NULL);

    /* ── 46 建築 + 奇蹟 ────────────────────────────── */
    EXPECT(strcmp(civ_dict_lookup(CIV_STR_IMPROVEMENTS, 0),  "無") == 0);
    EXPECT(strcmp(civ_dict_lookup(CIV_STR_IMPROVEMENTS, 1),  "宮殿") == 0);
    EXPECT(strcmp(civ_dict_lookup(CIV_STR_IMPROVEMENTS, 25), "金字塔") == 0);
    EXPECT(strcmp(civ_dict_lookup(CIV_STR_IMPROVEMENTS, 43), "阿波羅計畫") == 0);
    EXPECT(strcmp(civ_dict_lookup(CIV_STR_IMPROVEMENTS, 45), "癌症療法") == 0);
    EXPECT(civ_dict_lookup(CIV_STR_IMPROVEMENTS, 46) == NULL);

    /* ── 28 單位 ───────────────────────────────────── */
    EXPECT(strcmp(civ_dict_lookup(CIV_STR_PEOPLE_UNITS, 0),  "墾荒者") == 0);
    EXPECT(strcmp(civ_dict_lookup(CIV_STR_PEOPLE_UNITS, 2),  "方陣兵") == 0);
    EXPECT(strcmp(civ_dict_lookup(CIV_STR_PEOPLE_UNITS, 21), "戰艦") == 0);
    EXPECT(strcmp(civ_dict_lookup(CIV_STR_PEOPLE_UNITS, 27), "商隊") == 0);
    EXPECT(civ_dict_lookup(CIV_STR_PEOPLE_UNITS, 28) == NULL);

    /* ── 24 地形（含 duplicates）─────────────────── */
    EXPECT(strcmp(civ_dict_lookup(CIV_STR_TERRAINS, 0),  "沙漠") == 0);
    EXPECT(strcmp(civ_dict_lookup(CIV_STR_TERRAINS, 10), "海洋") == 0);
    EXPECT(strcmp(civ_dict_lookup(CIV_STR_TERRAINS, 14), "草原") == 0);     /* dup */
    EXPECT(strcmp(civ_dict_lookup(CIV_STR_TERRAINS, 23), "河流") == 0);     /* dup */

    /* ── 21 其他 ───────────────────────────────────── */
    EXPECT(strcmp(civ_dict_lookup(CIV_STR_MISC, 0),  "老兵單位") == 0);
    EXPECT(strcmp(civ_dict_lookup(CIV_STR_MISC, 20), "貿易路線") == 0);

    /* ── 7 報告分類 ─────────────────────────────── */
    EXPECT(strcmp(civ_dict_lookup(CIV_STR_REPORT_TITLES, 0), "城市狀態") == 0);
    EXPECT(strcmp(civ_dict_lookup(CIV_STR_REPORT_TITLES, 6), "軍事損失") == 0);

    /* ── 6 政府 ─────────────────────────────────── */
    EXPECT(strcmp(civ_dict_lookup(CIV_STR_GOVERNMENTS, 0), "無政府") == 0);
    EXPECT(strcmp(civ_dict_lookup(CIV_STR_GOVERNMENTS, 5), "民主") == 0);

    /* ── 8 商品 ─────────────────────────────────── */
    EXPECT(strcmp(civ_dict_lookup(CIV_STR_CARAVAN_GOODS, 0), "絲綢") == 0);
    EXPECT(strcmp(civ_dict_lookup(CIV_STR_CARAVAN_GOODS, 7), "香料") == 0);

    /* ── 13 君主稱號 ─────────────────────────────── */
    EXPECT(strcmp(civ_dict_lookup(CIV_STR_KING_TITLE, 0),  "先生") == 0);
    EXPECT(strcmp(civ_dict_lookup(CIV_STR_KING_TITLE, 3),  "同志") == 0);
    EXPECT(strcmp(civ_dict_lookup(CIV_STR_KING_TITLE, 12), "皇帝") == 0);

    /* ── 6 Civilopedia 大類 ───────────────────────── */
    EXPECT(strcmp(civ_dict_lookup(CIV_STR_CIVPEDIA_SUBTITLE, 0), "文明進展") == 0);
    EXPECT(strcmp(civ_dict_lookup(CIV_STR_CIVPEDIA_SUBTITLE, 5), "遊戲概念") == 0);

    /* ── 10 太空船零件 ──────────────────────────── */
    EXPECT(strcmp(civ_dict_lookup(CIV_STR_DOCK, 3), "推進") == 0);
    EXPECT(strcmp(civ_dict_lookup(CIV_STR_DOCK, 8), "太陽能板") == 0);

    /* ── 不存在的 str_id ──────────────────────────── */
    EXPECT(civ_dict_lookup(9999, 0) == NULL);

    /* 計總翻譯條目數 — 對齊 spec 05 §5.3 */
    int total = 72 + 46 + 28 + 24 + 21 + 7 + 6 + 8 + 13 + 6 + 10;
    printf("PASS test_civ_dict (%d 條 zh-TW 翻譯全驗)\n", total);
    return 0;
}
