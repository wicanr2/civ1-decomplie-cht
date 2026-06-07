/*
 * world/government.h — R26-C 政府型態
 *
 * 對齊 spec 06 §6.3 (manual P43-45 ground-truth) + civ_dict.c GOVERNMENT[].
 * 取代 R24 player_government 用 raw int (1..6) 的做法.
 *
 * 1991 Civ1 6 個 government, 玩家可在 Revolution 後切換 (除 Anarchy 是過渡期).
 */
#ifndef CIV_WORLD_GOVERNMENT_H
#define CIV_WORLD_GOVERNMENT_H

#include <stdbool.h>

#define CIV_GOVERNMENT_COUNT 6

typedef enum {
    CIV_GOVERNMENT_ANARCHY    = 0,   /* 過渡期, 不能正常運作 */
    CIV_GOVERNMENT_DESPOTISM  = 1,   /* 新文明預設 */
    CIV_GOVERNMENT_MONARCHY   = 2,
    CIV_GOVERNMENT_COMMUNISM  = 3,
    CIV_GOVERNMENT_REPUBLIC   = 4,
    CIV_GOVERNMENT_DEMOCRACY  = 5,
} civ_government_t;

const char *civ_government_name_zh(int gov);

/* 是否為新局可選政府 (排除 Anarchy 過渡期). */
bool civ_government_is_starting_choice(int gov);

#endif /* CIV_WORLD_GOVERNMENT_H */
