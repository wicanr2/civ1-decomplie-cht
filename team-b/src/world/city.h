/*
 * world/city.h — M6-full 城市結構
 *
 * 對應 spec 06 §6.2 (city improvement) + spec 07 §7.2 (SAV city table).
 * spec 06 §6.2.1 25 building cost / maint / prereq 全 ground-truth.
 *
 * 1993 Win port 內 city 推測佔 32-byte record (對 SAV §7.2 0x08A0 area).
 * 我們 in-memory 用更寬鬆 layout (含 zh name + 完整 building bitmap).
 */
#ifndef CIV_WORLD_CITY_H
#define CIV_WORLD_CITY_H

#include <stdbool.h>
#include <stdint.h>

#define CIV_MAX_CITIES 64           /* per-game; 原版 1993 Win 上限不確 */
#define CIV_BUILDING_COUNT 25       /* spec 06 §6.2 完整 25 building */

typedef struct civ_city {
    char     name[24];              /* UTF-8 zh-TW name (e.g. "羅馬") */
    int16_t  x, y;                  /* world tile coords */
    uint8_t  owner;                 /* 0=barbarian, 1..7=civ slot */
    uint8_t  population;            /* 城市規模 (Civ1 1..10+) */
    int8_t   building_target;       /* -1 = idle, else building idx (per spec 06 §6.2) */
    int16_t  shield_stock;          /* 累積生產進度 */
    int16_t  food_stock;            /* 食物 storage */
    uint32_t buildings_bitmap;      /* 25-bit bitmap, bit N = built building N */
    uint32_t wonders_bitmap;        /* R27-A: 22-bit bitmap (CIV_WONDER_PYRAMIDS=1..21
                                       + 2 OpenCivOne SETI/Cure). bit N = 已建造
                                       wonder enum N (對齊 world/wonder.h). */
    bool     alive;
} civ_city_t;

/* 中央製造廠取個 building idx → cost (spec 06 §6.2). */
int civ_building_cost(int building_idx);

/* zh-TW 名稱 (對齊 spec 05 civ_dict.c STR# 131 翻譯). */
const char *civ_building_name_zh(int building_idx);

#endif /* CIV_WORLD_CITY_H */
