/*
 * world/tech.h — R26 完整 67 core + 5 future tech + DAG
 *
 * 對齊 spec 06 §6.5.1 (OpenCivOne ground-truth) + civ_dict.c STR# 130.
 * R16 stub 13 tech → R26 完整 72.
 *
 * Enum 排序: spec 06 DAG 順序 (no-prereq → 1-prereq → 2-prereq → future).
 * 同一 prereq 級內依 spec 06 文件出現順序.
 *
 * 真實 SAV file 用 47-byte bitmap (`spec 07 §7.2`), 對應 R27+ tech-state field.
 */
#ifndef CIV_WORLD_TECH_H
#define CIV_WORLD_TECH_H

#include <stdbool.h>
#include <stdint.h>

#define CIV_TECH_COUNT   72   /* 67 core + 5 future */

typedef enum {
    CIV_TECH_NONE                = 0,

    /* === No-prereq tier (7) === */
    CIV_TECH_ALPHABET            = 1,
    CIV_TECH_BRONZE_WORKING      = 2,
    CIV_TECH_CEREMONIAL_BURIAL   = 3,
    CIV_TECH_HORSEBACK_RIDING    = 4,
    CIV_TECH_MASONRY             = 5,
    CIV_TECH_POTTERY             = 6,
    CIV_TECH_THE_WHEEL           = 7,

    /* === 1-prereq tier (6) === */
    CIV_TECH_CODE_OF_LAWS        = 8,   /* Alphabet */
    CIV_TECH_MAP_MAKING          = 9,   /* Alphabet */
    CIV_TECH_WRITING             = 10,  /* Alphabet */
    CIV_TECH_CURRENCY            = 11,  /* Bronze Working */
    CIV_TECH_IRON_WORKING        = 12,  /* Bronze Working */
    CIV_TECH_MYSTICISM           = 13,  /* Ceremonial Burial */

    /* === 2-prereq tier (54), spec 06 §6.5.1 order === */
    CIV_TECH_ASTRONOMY           = 14,  /* Mysticism + Mathematics */
    CIV_TECH_MONARCHY            = 15,  /* Ceremonial Burial + Code of Laws */
    CIV_TECH_ENGINEERING         = 16,  /* Wheel + Construction */
    CIV_TECH_MATHEMATICS         = 17,  /* Alphabet + Masonry */
    CIV_TECH_TRADE               = 18,  /* Currency + Code of Laws */
    CIV_TECH_CONSTRUCTION        = 19,  /* Masonry + Currency */
    CIV_TECH_LITERACY            = 20,  /* Writing + Code of Laws */
    CIV_TECH_BRIDGE_BUILDING     = 21,  /* Iron Working + Construction */
    CIV_TECH_BANKING             = 22,  /* Trade + The Republic */
    CIV_TECH_THE_REPUBLIC        = 23,  /* Code of Laws + Literacy */
    CIV_TECH_FEUDALISM           = 24,  /* Masonry + Monarchy */
    CIV_TECH_PHILOSOPHY          = 25,  /* Mysticism + Literacy */
    CIV_TECH_RELIGION            = 26,  /* Philosophy + Writing */
    CIV_TECH_MEDICINE            = 27,  /* Philosophy + Trade */
    CIV_TECH_NAVIGATION          = 28,  /* Map Making + Astronomy */
    CIV_TECH_MAGNETISM           = 29,  /* Navigation + Physics */
    CIV_TECH_CHIVALRY            = 30,  /* Feudalism + Horseback Riding */
    CIV_TECH_UNIVERSITY          = 31,  /* Mathematics + Philosophy */
    CIV_TECH_PHYSICS             = 32,  /* Mathematics + Navigation */
    CIV_TECH_INVENTION           = 33,  /* Engineering + Literacy */
    CIV_TECH_DEMOCRACY           = 34,  /* Philosophy + Literacy */
    CIV_TECH_THEORY_OF_GRAVITY   = 35,  /* Astronomy + University */
    CIV_TECH_CHEMISTRY           = 36,  /* University + Medicine */
    CIV_TECH_GUNPOWDER           = 37,  /* Invention + Iron Working */
    CIV_TECH_STEAM_ENGINE        = 38,  /* Physics + Invention */
    CIV_TECH_RAILROAD            = 39,  /* Steam Engine + Bridge Building */
    CIV_TECH_METALLURGY          = 40,  /* Gunpowder + University */
    CIV_TECH_EXPLOSIVES          = 41,  /* Gunpowder + Chemistry */
    CIV_TECH_INDUSTRIALIZATION   = 42,  /* Railroad + Banking */
    CIV_TECH_COMMUNISM           = 43,  /* Philosophy + Industrialization */
    CIV_TECH_CONSCRIPTION        = 44,  /* The Republic + Explosives */
    CIV_TECH_ELECTRICITY         = 45,  /* Metallurgy + Magnetism */
    CIV_TECH_ATOMIC_THEORY       = 46,  /* Theory of Gravity + Physics */
    CIV_TECH_THE_CORPORATION     = 47,  /* Banking + Industrialization */
    CIV_TECH_STEEL               = 48,  /* Metallurgy + Industrialization */
    CIV_TECH_REFINING            = 49,  /* Chemistry + The Corporation */
    CIV_TECH_COMBUSTION          = 50,  /* Refining + Explosives */
    CIV_TECH_AUTOMOBILE          = 51,  /* Combustion + Steel */
    CIV_TECH_MASS_PRODUCTION     = 52,  /* Automobile + The Corporation */
    CIV_TECH_ELECTRONICS         = 53,  /* Engineering + Electricity */
    CIV_TECH_FLIGHT              = 54,  /* Combustion + Physics */
    CIV_TECH_LABOR_UNION         = 55,  /* Mass Production + Communism */
    CIV_TECH_GENETIC_ENGINEERING = 56,  /* Medicine + The Corporation */
    CIV_TECH_PLASTICS            = 57,  /* Refining + Space Flight */
    CIV_TECH_RECYCLING           = 58,  /* Mass Production + Democracy */
    CIV_TECH_NUCLEAR_FISSION     = 59,  /* Mass Production + Atomic Theory */
    CIV_TECH_COMPUTERS           = 60,  /* Mathematics + Electronics */
    CIV_TECH_ADVANCED_FLIGHT     = 61,  /* Flight + Electricity */
    CIV_TECH_ROCKETRY            = 62,  /* Advanced Flight + Electronics */
    CIV_TECH_NUCLEAR_POWER       = 63,  /* Nuclear Fission + Electronics */
    CIV_TECH_SPACE_FLIGHT        = 64,  /* Computers + Rocketry */
    CIV_TECH_ROBOTICS            = 65,  /* Plastics + Computers */
    CIV_TECH_SUPERCONDUCTOR      = 66,  /* Plastics + Mass Production */
    CIV_TECH_FUSION_POWER        = 67,  /* Nuclear Power + Superconductor */

    /* === Future tier (5) === */
    CIV_TECH_FUTURE_1            = 68,
    CIV_TECH_FUTURE_2            = 69,
    CIV_TECH_FUTURE_3            = 70,
    CIV_TECH_FUTURE_4            = 71,
} civ_tech_id_t;

typedef enum {
    CIV_TECH_LEARN_SELF       = 0,
    CIV_TECH_LEARN_DIPLOMAT   = 1,
    CIV_TECH_LEARN_TRADE      = 2,
    CIV_TECH_LEARN_HUT        = 3,
    CIV_TECH_LEARN_LIBRARY    = 4,
} civ_tech_learn_source_t;

typedef struct civ_tech_discovery_event {
    civ_tech_id_t           tech_id;
    civ_tech_learn_source_t source;
    int                     from_civ_slot;
    civ_tech_id_t           unlocked_techs[8];
    int                     unlocked_units[4];
    int                     unlocked_imp[4];
    int                     unlocked_wonder[4];
} civ_tech_discovery_event_t;

/* zh-TW 名稱 (對齊 spec 05 STR# 130 + civ_dict.c). */
const char *civ_tech_name_zh(civ_tech_id_t t);

const char *civ_tech_subtitle_zh(void);

const char *civ_tech_source_phrase_zh(civ_tech_learn_source_t s,
                                       const char *civ_name);

/* R26: 從完整 prereq DAG (spec 06 §6.5.1) 反推 — 找所有
 * 「2 個 prereq 之一 = ev->tech_id」的 tech 進 unlocked_techs.
 * 不會修 source / from_civ_slot. */
void civ_tech_discovery_fill_unlocked(civ_tech_discovery_event_t *ev);

/* R26: 取本 tech 的 2 個 prereq (寫進 out_a/out_b; 沒有 prereq 寫 NONE).
 * 越界回 false. */
bool civ_tech_prereq(civ_tech_id_t t,
                      civ_tech_id_t *out_a, civ_tech_id_t *out_b);

#endif /* CIV_WORLD_TECH_H */
