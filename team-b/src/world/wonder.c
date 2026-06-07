#include "wonder.h"

#include <stddef.h>

/* R26: 22 wonder 完整資料 (spec 06 §6.2.4 OpenCivOne ground-truth).
 * cost/prereq/obsolete 對齊原版 1991 Civ1 + OpenCivOne v0.3. */
static const struct {
    const char    *zh;
    int            cost;
    civ_tech_id_t  prereq;
    civ_tech_id_t  obsolete;
} WONDER_INFO[CIV_WONDER_COUNT] = {
    [CIV_WONDER_NONE]                   = { "",            0,   CIV_TECH_NONE,             CIV_TECH_NONE },

    /* Ancient */
    [CIV_WONDER_PYRAMIDS]               = { "金字塔",      300, CIV_TECH_MASONRY,          CIV_TECH_COMMUNISM },
    [CIV_WONDER_HANGING_GARDENS]        = { "空中花園",    300, CIV_TECH_POTTERY,          CIV_TECH_INVENTION },
    [CIV_WONDER_COLOSSUS]               = { "巨像",        200, CIV_TECH_BRONZE_WORKING,   CIV_TECH_ELECTRICITY },
    [CIV_WONDER_LIGHTHOUSE]             = { "燈塔",        200, CIV_TECH_MAP_MAKING,       CIV_TECH_MAGNETISM },
    [CIV_WONDER_GREAT_LIBRARY]          = { "大圖書館",    300, CIV_TECH_LITERACY,         CIV_TECH_UNIVERSITY },
    [CIV_WONDER_ORACLE]                 = { "神諭",        300, CIV_TECH_MYSTICISM,        CIV_TECH_RELIGION },
    [CIV_WONDER_GREAT_WALL]             = { "萬里長城",    300, CIV_TECH_MASONRY,          CIV_TECH_GUNPOWDER },

    /* Medieval */
    [CIV_WONDER_MAGELLANS_EXPEDITION]   = { "麥哲倫的遠征", 400, CIV_TECH_NAVIGATION,       CIV_TECH_NONE },
    [CIV_WONDER_MICHELANGELOS_CHAPEL]   = { "米開朗基羅教堂", 300, CIV_TECH_RELIGION,       CIV_TECH_COMMUNISM },
    [CIV_WONDER_COPERNICUS_OBSERVATORY] = { "哥白尼天文台", 300, CIV_TECH_ASTRONOMY,        CIV_TECH_AUTOMOBILE },
    [CIV_WONDER_SHAKESPEARES_THEATRE]   = { "莎士比亞劇場", 400, CIV_TECH_MEDICINE,         CIV_TECH_ELECTRONICS },
    [CIV_WONDER_ISAAC_NEWTONS_COLLEGE]  = { "牛頓學院",     400, CIV_TECH_THEORY_OF_GRAVITY,CIV_TECH_NUCLEAR_FISSION },
    [CIV_WONDER_JS_BACHS_CATHEDRAL]     = { "巴哈大教堂",   400, CIV_TECH_RELIGION,         CIV_TECH_NONE },
    [CIV_WONDER_DARWINS_VOYAGE]         = { "達爾文之旅",   300, CIV_TECH_RAILROAD,         CIV_TECH_NONE },

    /* Modern / Industrial */
    [CIV_WONDER_HOOVER_DAM]             = { "胡佛水壩",     600, CIV_TECH_ELECTRONICS,      CIV_TECH_NONE },
    [CIV_WONDER_WOMENS_SUFFRAGE]        = { "女性投票權",   600, CIV_TECH_INDUSTRIALIZATION,CIV_TECH_NONE },
    [CIV_WONDER_MANHATTAN_PROJECT]      = { "曼哈頓計畫",   600, CIV_TECH_NUCLEAR_FISSION,  CIV_TECH_NONE },
    [CIV_WONDER_UNITED_NATIONS]         = { "聯合國",       600, CIV_TECH_COMMUNISM,        CIV_TECH_NONE },
    [CIV_WONDER_APOLLO_PROGRAM]         = { "阿波羅計畫",   600, CIV_TECH_SPACE_FLIGHT,     CIV_TECH_NONE },

    /* spec 06 v0.3 補 (TBD), 推測 Civ1 標準: SETI ← Computers, Cure ← Genetic Engineering */
    [CIV_WONDER_SETI_PROGRAM]           = { "SETI 計畫",    600, CIV_TECH_COMPUTERS,            CIV_TECH_NONE },
    [CIV_WONDER_CURE_FOR_CANCER]        = { "癌症療法",     600, CIV_TECH_GENETIC_ENGINEERING,  CIV_TECH_NONE },
};

const char *civ_wonder_name_zh(civ_wonder_id_t w)
{
    if ((int)w < 0 || (int)w >= CIV_WONDER_COUNT) return "";
    return WONDER_INFO[w].zh;
}

int civ_wonder_cost(civ_wonder_id_t w)
{
    if ((int)w < 0 || (int)w >= CIV_WONDER_COUNT) return 0;
    return WONDER_INFO[w].cost;
}

civ_tech_id_t civ_wonder_prereq_tech(civ_wonder_id_t w)
{
    if ((int)w < 0 || (int)w >= CIV_WONDER_COUNT) return CIV_TECH_NONE;
    return WONDER_INFO[w].prereq;
}

civ_tech_id_t civ_wonder_obsolete_by(civ_wonder_id_t w)
{
    if ((int)w < 0 || (int)w >= CIV_WONDER_COUNT) return CIV_TECH_NONE;
    return WONDER_INFO[w].obsolete;
}
