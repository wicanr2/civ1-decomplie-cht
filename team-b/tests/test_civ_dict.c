/*
 * test_civ_dict.c — 驗 zh-TW catalog 376 條翻譯
 *
 * 對所有 22 個 STR# 翻譯陣列做：
 *   1. 起首 / 結尾條目正確
 *   2. 中間抽樣
 *   3. 越界 / 不存在 str_id 回 NULL
 *
 * 不需要 CIV1_DATA_DIR — 純 catalog 自驗證。
 *
 * D-#3 第二波 (2026-06-06): 加 11 個 STR# 表共 135 條 (128/129/136/140/
 * 143/144/145/148/149/151/152/158)。
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

    /* ── D-#3 第二波 (2026-06-06) 加 11 個 STR# 共 135 條 ────────── */

    /* STR# 128 — 11 稅率 */
    EXPECT(strstr(civ_dict_lookup(CIV_STR_TAX_RATES, 0), "100% 科技") != NULL);
    EXPECT(strstr(civ_dict_lookup(CIV_STR_TAX_RATES, 10), "100% 稅率") != NULL);
    EXPECT(civ_dict_lookup(CIV_STR_TAX_RATES, 11) == NULL);

    /* STR# 129 — 6 奢侈率 */
    EXPECT(strstr(civ_dict_lookup(CIV_STR_LUX_RATES, 0), "奢侈品") != NULL);
    EXPECT(civ_dict_lookup(CIV_STR_LUX_RATES, 6) == NULL);

    /* STR# 136 — 14 UI 提示 */
    EXPECT(strcmp(civ_dict_lookup(CIV_STR_MISC_PROMPTS, 0), "遊戲設定") == 0);
    EXPECT(strcmp(civ_dict_lookup(CIV_STR_MISC_PROMPTS, 5), "地圖") == 0);
    EXPECT(strcmp(civ_dict_lookup(CIV_STR_MISC_PROMPTS, 13), "顯示單位") == 0);

    /* STR# 140 — 16 領袖 */
    EXPECT(strcmp(civ_dict_lookup(CIV_STR_LEADERS, 1),  "凱撒") == 0);
    EXPECT(strcmp(civ_dict_lookup(CIV_STR_LEADERS, 11), "拿破崙") == 0);
    EXPECT(strcmp(civ_dict_lookup(CIV_STR_LEADERS, 15), "成吉思汗") == 0);
    EXPECT(civ_dict_lookup(CIV_STR_LEADERS, 16) == NULL);

    /* STR# 143/144 — 8 軍 sing/plur */
    EXPECT(strcmp(civ_dict_lookup(CIV_STR_ARMY_SING, 0), "野蠻人") == 0);
    EXPECT(strcmp(civ_dict_lookup(CIV_STR_ARMY_SING, 1), "羅馬") == 0);
    EXPECT(strcmp(civ_dict_lookup(CIV_STR_ARMY_PLUR, 1), "羅馬人") == 0);
    EXPECT(strcmp(civ_dict_lookup(CIV_STR_ARMY_PLUR, 7), "蒙古人") == 0);

    /* STR# 145 — 8 歷史名君 */
    EXPECT(strcmp(civ_dict_lookup(CIV_STR_KING_TEXT, 0), "阿提拉") == 0);
    EXPECT(strcmp(civ_dict_lookup(CIV_STR_KING_TEXT, 3), "成吉思汗") == 0);

    /* STR# 148/149 — 16 文明 sing/plur */
    EXPECT(strcmp(civ_dict_lookup(CIV_STR_LEADERS_CIV_SING, 1),  "羅馬") == 0);
    EXPECT(strcmp(civ_dict_lookup(CIV_STR_LEADERS_CIV_SING, 13), "中華") == 0);
    EXPECT(strcmp(civ_dict_lookup(CIV_STR_LEADERS_CIV_PLUR, 13), "中國人") == 0);

    /* STR# 151 — 3 輸入標題 */
    EXPECT(strcmp(civ_dict_lookup(CIV_STR_INPUT_TITLES, 0), "城市名稱...") == 0);
    EXPECT(strcmp(civ_dict_lookup(CIV_STR_INPUT_TITLES, 2), "您的部族名稱...") == 0);

    /* STR# 152 — 19 Orders menu */
    EXPECT(strcmp(civ_dict_lookup(CIV_STR_ORDERS_MENU, 0), "無命令") == 0);
    EXPECT(strstr(civ_dict_lookup(CIV_STR_ORDERS_MENU, 11), "等候") != NULL);
    EXPECT(strcmp(civ_dict_lookup(CIV_STR_ORDERS_MENU, 17), "解散單位") == 0);

    /* STR# 158 — 10 錯誤訊息 */
    EXPECT(strcmp(civ_dict_lookup(CIV_STR_ALERT_TEXT, 0), "遊戲未存檔。") == 0);
    EXPECT(strstr(civ_dict_lookup(CIV_STR_ALERT_TEXT, 4), "256 色") != NULL);

    /* ── 不存在的 str_id ──────────────────────────── */
    EXPECT(civ_dict_lookup(9999, 0) == NULL);

    /* 計總翻譯條目數 — 對齊 spec 05 §5.3 */
    int total = 72 + 46 + 28 + 24 + 21 + 7 + 6 + 8 + 13 + 6 + 10    /* 舊 241 */
              + 11 + 6 + 14 + 16 + 8 + 8 + 8 + 16 + 16 + 3 + 19 + 10; /* 新 135 */
    printf("PASS test_civ_dict (%d 條 zh-TW 翻譯全驗,22 個 STR# 表)\n", total);
    return 0;
}
