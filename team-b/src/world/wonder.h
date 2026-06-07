/*
 * world/wonder.h — R26 完整 22 wonder + prereq/obsolete 表
 *
 * 對齊 spec 06 §6.2.4 (OpenCivOne `wonderTypes` ground-truth).
 * 跟 improvement 共用 ImprovementDefinition struct, 但獨立 enum + 表.
 *
 * 3 era × 7 = 21 個 Civ1 1991 wonder, OpenCivOne 加 SETI + Cure for Cancer
 * (v0.3 提案) = 22. R26 接 spec 06 寫 21 + 2 TBD 用標準推測 prereq.
 *
 * 真實 SAV file 用 22-bit bitmap 紀錄哪些 wonder 已建造 (spec 07 §7.2 v0.2).
 */
#ifndef CIV_WORLD_WONDER_H
#define CIV_WORLD_WONDER_H

#include "tech.h"

#include <stdbool.h>
#include <stdint.h>

#define CIV_WONDER_COUNT 22

typedef enum {
    CIV_WONDER_NONE                  = 0,

    /* === Ancient (7) === */
    CIV_WONDER_PYRAMIDS              = 1,
    CIV_WONDER_HANGING_GARDENS       = 2,
    CIV_WONDER_COLOSSUS              = 3,
    CIV_WONDER_LIGHTHOUSE            = 4,
    CIV_WONDER_GREAT_LIBRARY         = 5,
    CIV_WONDER_ORACLE                = 6,
    CIV_WONDER_GREAT_WALL            = 7,

    /* === Medieval (7) === */
    CIV_WONDER_MAGELLANS_EXPEDITION  = 8,
    CIV_WONDER_MICHELANGELOS_CHAPEL  = 9,
    CIV_WONDER_COPERNICUS_OBSERVATORY = 10,
    CIV_WONDER_SHAKESPEARES_THEATRE  = 11,
    CIV_WONDER_ISAAC_NEWTONS_COLLEGE = 12,
    CIV_WONDER_JS_BACHS_CATHEDRAL    = 13,
    CIV_WONDER_DARWINS_VOYAGE        = 14,

    /* === Modern / Industrial (7) === */
    CIV_WONDER_HOOVER_DAM            = 15,
    CIV_WONDER_WOMENS_SUFFRAGE       = 16,
    CIV_WONDER_MANHATTAN_PROJECT     = 17,
    CIV_WONDER_UNITED_NATIONS        = 18,
    CIV_WONDER_APOLLO_PROGRAM        = 19,
    CIV_WONDER_SETI_PROGRAM          = 20,
    CIV_WONDER_CURE_FOR_CANCER       = 21,
} civ_wonder_id_t;

/* zh-TW 名稱 (對齊 spec 05 STR# 131 + civ_dict.c WONDER[]). */
const char *civ_wonder_name_zh(civ_wonder_id_t w);

/* shield cost (spec 06 §6.2.4 ground-truth). */
int civ_wonder_cost(civ_wonder_id_t w);

/* prereq tech (CIV_TECH_NONE = 沒 prereq). */
civ_tech_id_t civ_wonder_prereq_tech(civ_wonder_id_t w);

/* obsolete-by tech — 達到此 tech 後 wonder 失效 (CIV_TECH_NONE = 永不過期). */
civ_tech_id_t civ_wonder_obsolete_by(civ_wonder_id_t w);

#endif /* CIV_WORLD_WONDER_H */
