#include "tech.h"
#include "unit.h"
#include "wonder.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>

/* R26: zh-TW 名稱 (對齊 spec 05 STR# 130 + civ_dict.c TECH[72]).
 * 順序對齊 tech.h enum (spec 06 §6.5.1 DAG order, 非 civ_dict 字典序). */
static const char *TECH_NAME_ZH[CIV_TECH_COUNT] = {
    [CIV_TECH_NONE]                = "",

    [CIV_TECH_ALPHABET]            = "字母",
    [CIV_TECH_BRONZE_WORKING]      = "青銅器",
    [CIV_TECH_CEREMONIAL_BURIAL]   = "禮葬",
    [CIV_TECH_HORSEBACK_RIDING]    = "騎馬",
    [CIV_TECH_MASONRY]             = "砌築",
    [CIV_TECH_POTTERY]             = "陶器",
    [CIV_TECH_THE_WHEEL]           = "輪",

    [CIV_TECH_CODE_OF_LAWS]        = "法典",
    [CIV_TECH_MAP_MAKING]          = "地圖學",
    [CIV_TECH_WRITING]             = "文字",
    [CIV_TECH_CURRENCY]            = "貨幣",
    [CIV_TECH_IRON_WORKING]        = "鐵器",
    [CIV_TECH_MYSTICISM]           = "神秘學",

    [CIV_TECH_ASTRONOMY]           = "天文學",
    [CIV_TECH_MONARCHY]            = "君主政體",
    [CIV_TECH_ENGINEERING]         = "工程學",
    [CIV_TECH_MATHEMATICS]         = "數學",
    [CIV_TECH_TRADE]               = "貿易",
    [CIV_TECH_CONSTRUCTION]        = "建築工程",
    [CIV_TECH_LITERACY]            = "識字",
    [CIV_TECH_BRIDGE_BUILDING]     = "橋樑建築",
    [CIV_TECH_BANKING]             = "銀行業",
    [CIV_TECH_THE_REPUBLIC]        = "共和政體",
    [CIV_TECH_FEUDALISM]           = "封建制度",
    [CIV_TECH_PHILOSOPHY]          = "哲學",
    [CIV_TECH_RELIGION]            = "宗教",
    [CIV_TECH_MEDICINE]            = "醫學",
    [CIV_TECH_NAVIGATION]          = "航海術",
    [CIV_TECH_MAGNETISM]           = "磁學",
    [CIV_TECH_CHIVALRY]            = "騎士精神",
    [CIV_TECH_UNIVERSITY]          = "大學",
    [CIV_TECH_PHYSICS]             = "物理學",
    [CIV_TECH_INVENTION]           = "發明",
    [CIV_TECH_DEMOCRACY]           = "民主政體",
    [CIV_TECH_THEORY_OF_GRAVITY]   = "萬有引力",
    [CIV_TECH_CHEMISTRY]           = "化學",
    [CIV_TECH_GUNPOWDER]           = "火藥",
    [CIV_TECH_STEAM_ENGINE]        = "蒸汽機",
    [CIV_TECH_RAILROAD]            = "鐵路",
    [CIV_TECH_METALLURGY]          = "冶金學",
    [CIV_TECH_EXPLOSIVES]          = "炸藥",
    [CIV_TECH_INDUSTRIALIZATION]   = "工業化",
    [CIV_TECH_COMMUNISM]           = "共產主義",
    [CIV_TECH_CONSCRIPTION]        = "徵兵制",
    [CIV_TECH_ELECTRICITY]         = "電力",
    [CIV_TECH_ATOMIC_THEORY]       = "原子理論",
    [CIV_TECH_THE_CORPORATION]     = "公司",
    [CIV_TECH_STEEL]               = "鋼鐵",
    [CIV_TECH_REFINING]            = "精煉",
    [CIV_TECH_COMBUSTION]          = "內燃機",
    [CIV_TECH_AUTOMOBILE]          = "汽車",
    [CIV_TECH_MASS_PRODUCTION]     = "大量生產",
    [CIV_TECH_ELECTRONICS]         = "電子學",
    [CIV_TECH_FLIGHT]              = "飛行",
    [CIV_TECH_LABOR_UNION]         = "工會",
    [CIV_TECH_GENETIC_ENGINEERING] = "基因工程",
    [CIV_TECH_PLASTICS]            = "塑膠",
    [CIV_TECH_RECYCLING]           = "資源回收",
    [CIV_TECH_NUCLEAR_FISSION]     = "核分裂",
    [CIV_TECH_COMPUTERS]           = "電腦",
    [CIV_TECH_ADVANCED_FLIGHT]     = "進階飛行",
    [CIV_TECH_ROCKETRY]            = "火箭學",
    [CIV_TECH_NUCLEAR_POWER]       = "核能",
    [CIV_TECH_SPACE_FLIGHT]        = "太空飛行",
    [CIV_TECH_ROBOTICS]            = "機器人學",
    [CIV_TECH_SUPERCONDUCTOR]      = "超導體",
    [CIV_TECH_FUSION_POWER]        = "融合動力",

    [CIV_TECH_FUTURE_1]            = "未來科技 1",
    [CIV_TECH_FUTURE_2]            = "未來科技 2",
    [CIV_TECH_FUTURE_3]            = "未來科技 3",
    [CIV_TECH_FUTURE_4]            = "未來科技 4",
};

/* R26: prereq DAG (spec 06 §6.5.1, OpenCivOne ground-truth).
 * 順序對齊 enum, [tech_id] = { prereq_a, prereq_b }. NONE = 沒 prereq. */
static const civ_tech_id_t PREREQ[CIV_TECH_COUNT][2] = {
    [CIV_TECH_NONE]                = { CIV_TECH_NONE, CIV_TECH_NONE },

    /* No-prereq tier */
    [CIV_TECH_ALPHABET]            = { CIV_TECH_NONE, CIV_TECH_NONE },
    [CIV_TECH_BRONZE_WORKING]      = { CIV_TECH_NONE, CIV_TECH_NONE },
    [CIV_TECH_CEREMONIAL_BURIAL]   = { CIV_TECH_NONE, CIV_TECH_NONE },
    [CIV_TECH_HORSEBACK_RIDING]    = { CIV_TECH_NONE, CIV_TECH_NONE },
    [CIV_TECH_MASONRY]             = { CIV_TECH_NONE, CIV_TECH_NONE },
    [CIV_TECH_POTTERY]             = { CIV_TECH_NONE, CIV_TECH_NONE },
    [CIV_TECH_THE_WHEEL]           = { CIV_TECH_NONE, CIV_TECH_NONE },

    /* 1-prereq tier */
    [CIV_TECH_CODE_OF_LAWS]        = { CIV_TECH_ALPHABET,          CIV_TECH_NONE },
    [CIV_TECH_MAP_MAKING]          = { CIV_TECH_ALPHABET,          CIV_TECH_NONE },
    [CIV_TECH_WRITING]             = { CIV_TECH_ALPHABET,          CIV_TECH_NONE },
    [CIV_TECH_CURRENCY]            = { CIV_TECH_BRONZE_WORKING,    CIV_TECH_NONE },
    [CIV_TECH_IRON_WORKING]        = { CIV_TECH_BRONZE_WORKING,    CIV_TECH_NONE },
    [CIV_TECH_MYSTICISM]           = { CIV_TECH_CEREMONIAL_BURIAL, CIV_TECH_NONE },

    /* 2-prereq tier */
    [CIV_TECH_ASTRONOMY]           = { CIV_TECH_MYSTICISM,         CIV_TECH_MATHEMATICS },
    [CIV_TECH_MONARCHY]            = { CIV_TECH_CEREMONIAL_BURIAL, CIV_TECH_CODE_OF_LAWS },
    [CIV_TECH_ENGINEERING]         = { CIV_TECH_THE_WHEEL,         CIV_TECH_CONSTRUCTION },
    [CIV_TECH_MATHEMATICS]         = { CIV_TECH_ALPHABET,          CIV_TECH_MASONRY },
    [CIV_TECH_TRADE]               = { CIV_TECH_CURRENCY,          CIV_TECH_CODE_OF_LAWS },
    [CIV_TECH_CONSTRUCTION]        = { CIV_TECH_MASONRY,           CIV_TECH_CURRENCY },
    [CIV_TECH_LITERACY]            = { CIV_TECH_WRITING,           CIV_TECH_CODE_OF_LAWS },
    [CIV_TECH_BRIDGE_BUILDING]     = { CIV_TECH_IRON_WORKING,      CIV_TECH_CONSTRUCTION },
    [CIV_TECH_BANKING]             = { CIV_TECH_TRADE,             CIV_TECH_THE_REPUBLIC },
    [CIV_TECH_THE_REPUBLIC]        = { CIV_TECH_CODE_OF_LAWS,      CIV_TECH_LITERACY },
    [CIV_TECH_FEUDALISM]           = { CIV_TECH_MASONRY,           CIV_TECH_MONARCHY },
    [CIV_TECH_PHILOSOPHY]          = { CIV_TECH_MYSTICISM,         CIV_TECH_LITERACY },
    [CIV_TECH_RELIGION]            = { CIV_TECH_PHILOSOPHY,        CIV_TECH_WRITING },
    [CIV_TECH_MEDICINE]            = { CIV_TECH_PHILOSOPHY,        CIV_TECH_TRADE },
    [CIV_TECH_NAVIGATION]          = { CIV_TECH_MAP_MAKING,        CIV_TECH_ASTRONOMY },
    [CIV_TECH_MAGNETISM]           = { CIV_TECH_NAVIGATION,        CIV_TECH_PHYSICS },
    [CIV_TECH_CHIVALRY]            = { CIV_TECH_FEUDALISM,         CIV_TECH_HORSEBACK_RIDING },
    [CIV_TECH_UNIVERSITY]          = { CIV_TECH_MATHEMATICS,       CIV_TECH_PHILOSOPHY },
    [CIV_TECH_PHYSICS]             = { CIV_TECH_MATHEMATICS,       CIV_TECH_NAVIGATION },
    [CIV_TECH_INVENTION]           = { CIV_TECH_ENGINEERING,       CIV_TECH_LITERACY },
    [CIV_TECH_DEMOCRACY]           = { CIV_TECH_PHILOSOPHY,        CIV_TECH_LITERACY },
    [CIV_TECH_THEORY_OF_GRAVITY]   = { CIV_TECH_ASTRONOMY,         CIV_TECH_UNIVERSITY },
    [CIV_TECH_CHEMISTRY]           = { CIV_TECH_UNIVERSITY,        CIV_TECH_MEDICINE },
    [CIV_TECH_GUNPOWDER]           = { CIV_TECH_INVENTION,         CIV_TECH_IRON_WORKING },
    [CIV_TECH_STEAM_ENGINE]        = { CIV_TECH_PHYSICS,           CIV_TECH_INVENTION },
    [CIV_TECH_RAILROAD]            = { CIV_TECH_STEAM_ENGINE,      CIV_TECH_BRIDGE_BUILDING },
    [CIV_TECH_METALLURGY]          = { CIV_TECH_GUNPOWDER,         CIV_TECH_UNIVERSITY },
    [CIV_TECH_EXPLOSIVES]          = { CIV_TECH_GUNPOWDER,         CIV_TECH_CHEMISTRY },
    [CIV_TECH_INDUSTRIALIZATION]   = { CIV_TECH_RAILROAD,          CIV_TECH_BANKING },
    [CIV_TECH_COMMUNISM]           = { CIV_TECH_PHILOSOPHY,        CIV_TECH_INDUSTRIALIZATION },
    [CIV_TECH_CONSCRIPTION]        = { CIV_TECH_THE_REPUBLIC,      CIV_TECH_EXPLOSIVES },
    [CIV_TECH_ELECTRICITY]         = { CIV_TECH_METALLURGY,        CIV_TECH_MAGNETISM },
    [CIV_TECH_ATOMIC_THEORY]       = { CIV_TECH_THEORY_OF_GRAVITY, CIV_TECH_PHYSICS },
    [CIV_TECH_THE_CORPORATION]     = { CIV_TECH_BANKING,           CIV_TECH_INDUSTRIALIZATION },
    [CIV_TECH_STEEL]               = { CIV_TECH_METALLURGY,        CIV_TECH_INDUSTRIALIZATION },
    [CIV_TECH_REFINING]            = { CIV_TECH_CHEMISTRY,         CIV_TECH_THE_CORPORATION },
    [CIV_TECH_COMBUSTION]          = { CIV_TECH_REFINING,          CIV_TECH_EXPLOSIVES },
    [CIV_TECH_AUTOMOBILE]          = { CIV_TECH_COMBUSTION,        CIV_TECH_STEEL },
    [CIV_TECH_MASS_PRODUCTION]     = { CIV_TECH_AUTOMOBILE,        CIV_TECH_THE_CORPORATION },
    [CIV_TECH_ELECTRONICS]         = { CIV_TECH_ENGINEERING,       CIV_TECH_ELECTRICITY },
    [CIV_TECH_FLIGHT]              = { CIV_TECH_COMBUSTION,        CIV_TECH_PHYSICS },
    [CIV_TECH_LABOR_UNION]         = { CIV_TECH_MASS_PRODUCTION,   CIV_TECH_COMMUNISM },
    [CIV_TECH_GENETIC_ENGINEERING] = { CIV_TECH_MEDICINE,          CIV_TECH_THE_CORPORATION },
    [CIV_TECH_PLASTICS]            = { CIV_TECH_REFINING,          CIV_TECH_SPACE_FLIGHT },
    [CIV_TECH_RECYCLING]           = { CIV_TECH_MASS_PRODUCTION,   CIV_TECH_DEMOCRACY },
    [CIV_TECH_NUCLEAR_FISSION]     = { CIV_TECH_MASS_PRODUCTION,   CIV_TECH_ATOMIC_THEORY },
    [CIV_TECH_COMPUTERS]           = { CIV_TECH_MATHEMATICS,       CIV_TECH_ELECTRONICS },
    [CIV_TECH_ADVANCED_FLIGHT]     = { CIV_TECH_FLIGHT,            CIV_TECH_ELECTRICITY },
    [CIV_TECH_ROCKETRY]            = { CIV_TECH_ADVANCED_FLIGHT,   CIV_TECH_ELECTRONICS },
    [CIV_TECH_NUCLEAR_POWER]       = { CIV_TECH_NUCLEAR_FISSION,   CIV_TECH_ELECTRONICS },
    [CIV_TECH_SPACE_FLIGHT]        = { CIV_TECH_COMPUTERS,         CIV_TECH_ROCKETRY },
    [CIV_TECH_ROBOTICS]            = { CIV_TECH_PLASTICS,          CIV_TECH_COMPUTERS },
    [CIV_TECH_SUPERCONDUCTOR]      = { CIV_TECH_PLASTICS,          CIV_TECH_MASS_PRODUCTION },
    [CIV_TECH_FUSION_POWER]        = { CIV_TECH_NUCLEAR_POWER,     CIV_TECH_SUPERCONDUCTOR },

    /* Future tier — chained 1-prereq */
    [CIV_TECH_FUTURE_1]            = { CIV_TECH_FUSION_POWER, CIV_TECH_NONE },
    [CIV_TECH_FUTURE_2]            = { CIV_TECH_FUTURE_1,     CIV_TECH_NONE },
    [CIV_TECH_FUTURE_3]            = { CIV_TECH_FUTURE_2,     CIV_TECH_NONE },
    [CIV_TECH_FUTURE_4]            = { CIV_TECH_FUTURE_3,     CIV_TECH_NONE },
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

bool civ_tech_prereq(civ_tech_id_t t,
                      civ_tech_id_t *out_a, civ_tech_id_t *out_b)
{
    if ((int)t < 0 || (int)t >= CIV_TECH_COUNT) return false;
    if (out_a) *out_a = PREREQ[t][0];
    if (out_b) *out_b = PREREQ[t][1];
    return true;
}

/* R26: 反推 — 找所有 prereq 含 ev->tech_id 的 tech 進 unlocked_techs[].
 * 8 個 slot 上限; 超過則略過 (1991 Civ1 沒有 tech 解鎖超過 8 個後續). */
static void fill_unlocked_techs(civ_tech_discovery_event_t *ev)
{
    int n = 0;
    for (int i = 1; i < CIV_TECH_COUNT && n < 8; i++) {
        if (PREREQ[i][0] == ev->tech_id || PREREQ[i][1] == ev->tech_id) {
            ev->unlocked_techs[n++] = (civ_tech_id_t)i;
        }
    }
}

/* R26-B: wonder reverse-DAG. iterate 22 wonder, 若 prereq_tech == ev->tech_id
 * → 加進 unlocked_wonder[]. 4 slot 上限 (Civ1 1991 沒 tech 解 4+ wonder). */
static void fill_unlocked_wonders(civ_tech_discovery_event_t *ev)
{
    int n = 0;
    for (int w = 1; w < CIV_WONDER_COUNT && n < 4; w++) {
        if (civ_wonder_prereq_tech((civ_wonder_id_t)w) == ev->tech_id) {
            ev->unlocked_wonder[n++] = w;
        }
    }
}

/* R26: hardcoded unit/imp unlock — R16 stub 仍保留 (待 R27+ 接 spec 06
 * §6.1 unit prereq + §6.2 improvement prereq 完整 reverse-DAG). */
static void fill_unlocked_assets_r16(civ_tech_discovery_event_t *ev)
{
    switch (ev->tech_id) {
        case CIV_TECH_BRONZE_WORKING:
            ev->unlocked_units[0] = CIV_UNIT_PHALANX;
            break;
        case CIV_TECH_IRON_WORKING:
            ev->unlocked_units[0] = CIV_UNIT_LEGION;
            break;
        case CIV_TECH_CURRENCY:
            ev->unlocked_imp[0] = 5;       /* Marketplace */
            break;
        case CIV_TECH_WRITING:
            ev->unlocked_imp[0] = 6;       /* Library */
            break;
        case CIV_TECH_MASONRY:
            ev->unlocked_imp[0] = 8;       /* City Walls */
            break;
        case CIV_TECH_POTTERY:
            ev->unlocked_imp[0] = 3;       /* Granary */
            break;
        default:
            break;
    }
}

void civ_tech_discovery_fill_unlocked(civ_tech_discovery_event_t *ev)
{
    if (!ev) return;
    memset(ev->unlocked_techs, 0, sizeof ev->unlocked_techs);
    memset(ev->unlocked_units, 0, sizeof ev->unlocked_units);
    memset(ev->unlocked_imp, 0, sizeof ev->unlocked_imp);
    memset(ev->unlocked_wonder, 0, sizeof ev->unlocked_wonder);

    fill_unlocked_techs(ev);
    fill_unlocked_wonders(ev);
    fill_unlocked_assets_r16(ev);
}
