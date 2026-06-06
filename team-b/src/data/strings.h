/*
 * data/strings.h — STR# resource 解析（Mac Resource Manager 標準）
 *
 * 對應 spec 05 §5.3：Civdata0.RSC 內 33 個 STR# 是 Civ1 master tables
 * 的 single source of truth。
 *
 * STR# 格式：
 *     BE16 count
 *     count × Pascal string (1 byte len + len bytes)
 */
#ifndef CIV_DATA_STRINGS_H
#define CIV_DATA_STRINGS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* spec 05 §5.6 enum 對應 33 個 STR# id */
enum {
    CIV_STR_TAX_RATES         = 128,
    CIV_STR_LUX_RATES         = 129,
    CIV_STR_TECH_ADVANCES     = 130,
    CIV_STR_IMPROVEMENTS      = 131,
    CIV_STR_PEOPLE_UNITS      = 132,
    CIV_STR_TERRAINS          = 133,
    CIV_STR_MISC              = 134,
    CIV_STR_CITIES            = 135,
    CIV_STR_MISC_PROMPTS      = 136,
    CIV_STR_CREDITS           = 137,
    CIV_STR_REPORT_TITLES     = 138,
    CIV_STR_STORY             = 139,
    CIV_STR_LEADERS           = 140,
    CIV_STR_GOVERNMENTS       = 141,
    CIV_STR_CARAVAN_GOODS     = 142,
    CIV_STR_ARMY_SING         = 143,
    CIV_STR_ARMY_PLUR         = 144,
    CIV_STR_KING_TEXT         = 145,
    CIV_STR_KING_TITLE        = 146,
    CIV_STR_CIVPEDIA_SUBTITLE = 147,
    CIV_STR_LEADERS_CIV_SING  = 148,
    CIV_STR_LEADERS_CIV_PLUR  = 149,
    CIV_STR_DOCK              = 150,
    CIV_STR_INPUT_TITLES      = 151,
    CIV_STR_SPACE_1           = 155,
    CIV_STR_SPACE_2           = 156,
    CIV_STR_ARCHEOLOGIST      = 157,
    CIV_STR_ALERT_TEXT        = 158,
    CIV_STR_DISK_NAMES        = 159,
    CIV_STR_FILE_NAMES        = 160,
};

/* 單一 STR# 結果 */
typedef struct civ_strlist {
    int16_t  id;
    int      count;
    /* count 個 zero-terminated UTF-8 字串連續存放 */
    char    *blob;
    /* count 個 offset 進 blob */
    size_t  *offsets;
} civ_strlist_t;

/* 從 raw STR# payload 解出 civ_strlist；malloc 內部 buffer。
 * 失敗回 -1。 */
int civ_strlist_parse(const uint8_t *payload, size_t len,
                      int16_t id, civ_strlist_t *out);

void civ_strlist_free(civ_strlist_t *s);

const char *civ_strlist_get(const civ_strlist_t *s, int index);

#endif /* CIV_DATA_STRINGS_H */
