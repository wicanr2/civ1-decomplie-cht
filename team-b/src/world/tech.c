#include "tech.h"
#include "unit.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>

/* 對齊 spec 05 STR# 130 (tech name) + civ_dict.c 翻譯. R16 階段只 13 個. */
static const char *TECH_NAME_ZH[CIV_TECH_COUNT] = {
    [CIV_TECH_NONE]             = "",
    [CIV_TECH_ALPHABET]         = "字母",
    [CIV_TECH_BRONZE_WORKING]   = "青銅器",
    [CIV_TECH_HORSEBACK_RIDING] = "騎術",
    [CIV_TECH_POTTERY]          = "陶器",
    [CIV_TECH_CURRENCY]         = "貨幣",
    [CIV_TECH_IRON_WORKING]     = "鐵器",
    [CIV_TECH_WRITING]          = "文字",
    [CIV_TECH_MAP_MAKING]       = "地圖學",
    [CIV_TECH_MASONRY]          = "砌石",
    [CIV_TECH_POLYTHEISM]       = "多神教",
    [CIV_TECH_LITERACY]         = "文藝",
    [CIV_TECH_MATHEMATICS]      = "數學",
};

const char *civ_tech_name_zh(civ_tech_id_t t)
{
    if ((int)t < 0 || (int)t >= CIV_TECH_COUNT) return "";
    return TECH_NAME_ZH[t];
}

const char *civ_tech_subtitle_zh(void)
{
    return "文明進展";
}

const char *civ_tech_source_phrase_zh(civ_tech_learn_source_t s,
                                       const char *civ_name)
{
    static char buf[64];
    switch (s) {
        case CIV_TECH_LEARN_SELF:
            return "(自家研發)";
        case CIV_TECH_LEARN_DIPLOMAT:
            snprintf(buf, sizeof buf, "(取自%s)", civ_name ? civ_name : "他國");
            return buf;
        case CIV_TECH_LEARN_TRADE:
            snprintf(buf, sizeof buf, "(向%s交易)", civ_name ? civ_name : "他國");
            return buf;
        case CIV_TECH_LEARN_HUT:
            return "(部落小屋發現)";
        case CIV_TECH_LEARN_LIBRARY:
            return "(大圖書館授予)";
    }
    return "";
}

/* R16-4: hardcoded prereq DAG 反推. 真實 spec 06 §6.5 47 tech 完整表
 * 留 R17. 這裡僅覆蓋 13 個 R16 tech 的常見解鎖. */
static void fill_bronze_working(civ_tech_discovery_event_t *ev)
{
    ev->unlocked_techs[0] = CIV_TECH_CURRENCY;
    ev->unlocked_techs[1] = CIV_TECH_IRON_WORKING;
    ev->unlocked_units[0] = CIV_UNIT_PHALANX;
    ev->unlocked_imp[0]   = 0;       /* (R16 無 wonder hookup) */
    ev->unlocked_wonder[0] = 1;      /* placeholder: Colossus = wonder idx 1 */
}

static void fill_alphabet(civ_tech_discovery_event_t *ev)
{
    ev->unlocked_techs[0] = CIV_TECH_WRITING;
    ev->unlocked_techs[1] = CIV_TECH_MAP_MAKING;
    ev->unlocked_techs[2] = CIV_TECH_MATHEMATICS;
}

static void fill_iron_working(civ_tech_discovery_event_t *ev)
{
    ev->unlocked_units[0] = CIV_UNIT_LEGION;
}

static void fill_currency(civ_tech_discovery_event_t *ev)
{
    ev->unlocked_imp[0] = 5;   /* Marketplace */
}

static void fill_writing(civ_tech_discovery_event_t *ev)
{
    ev->unlocked_techs[0] = CIV_TECH_LITERACY;
    ev->unlocked_imp[0]   = 6; /* Library */
}

static void fill_masonry(civ_tech_discovery_event_t *ev)
{
    ev->unlocked_imp[0] = 8;   /* City Walls */
    ev->unlocked_wonder[0] = 2;  /* placeholder: Pyramids */
}

static void fill_pottery(civ_tech_discovery_event_t *ev)
{
    ev->unlocked_imp[0] = 3;   /* Granary */
}

void civ_tech_discovery_fill_unlocked(civ_tech_discovery_event_t *ev)
{
    if (!ev) return;
    memset(ev->unlocked_techs, 0, sizeof ev->unlocked_techs);
    memset(ev->unlocked_units, 0, sizeof ev->unlocked_units);
    memset(ev->unlocked_imp, 0, sizeof ev->unlocked_imp);
    memset(ev->unlocked_wonder, 0, sizeof ev->unlocked_wonder);

    switch (ev->tech_id) {
        case CIV_TECH_BRONZE_WORKING:   fill_bronze_working(ev); break;
        case CIV_TECH_ALPHABET:         fill_alphabet(ev);       break;
        case CIV_TECH_IRON_WORKING:     fill_iron_working(ev);   break;
        case CIV_TECH_CURRENCY:         fill_currency(ev);       break;
        case CIV_TECH_WRITING:          fill_writing(ev);        break;
        case CIV_TECH_MASONRY:          fill_masonry(ev);        break;
        case CIV_TECH_POTTERY:          fill_pottery(ev);        break;
        default: break;
    }
}
