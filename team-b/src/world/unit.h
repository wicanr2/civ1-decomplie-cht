/*
 * world/unit.h — M6-full-lite unit 結構 + 移動 + 攻擊 placeholder
 *
 * 本檔是 M6-full-lite 骨架: unit 類型 / 位置 / owner / hp / 移動點. spec 06
 * (數值表) + spec 07 (combat 公式 + RLL save format) 都還沒寫 Ghidra side,
 * 所以 atk/def/move/cost 全用 placeholder hardcoded value, 真公式待 spec 補.
 *
 * civ_unit_type_t 對齊 spec 05 STR# 132 (28 個 People entry) 但 M6-full-lite
 * 只實作前 5 個常見類型 + Barbarian Camp, 後續類型隨 M7 補.
 */
#ifndef CIV_WORLD_UNIT_H
#define CIV_WORLD_UNIT_H

#include <stdbool.h>
#include <stdint.h>

#define CIV_MAX_UNITS    256   /* M6-lite cap; Civ1 原版 1993 是 unbounded */
#define CIV_NUM_PLAYERS    8   /* 7 文明 + 1 barbarian slot, 對應 FUN_11e8_0337 */

typedef enum {
    CIV_UNIT_NONE       = 0,
    CIV_UNIT_SETTLERS   = 1,   /* STR# 132 idx 0 — 墾荒者 */
    CIV_UNIT_MILITIA    = 2,   /* STR# 132 idx 1 */
    CIV_UNIT_PHALANX    = 3,   /* STR# 132 idx 2 — 方陣兵 */
    CIV_UNIT_LEGION     = 4,
    CIV_UNIT_MUSKETEERS = 5,
    CIV_UNIT_COUNT      = 6,
} civ_unit_type_t;

typedef struct civ_unit {
    civ_unit_type_t type;
    uint8_t         owner;      /* 0..CIV_NUM_PLAYERS-1, 0 = barbarians */
    uint8_t         hp;         /* placeholder 0..20 */
    uint8_t         moves_left; /* per-turn */
    int16_t         x, y;       /* world tile coord */
    bool            alive;
} civ_unit_t;

/* 給 unit 類型查 placeholder attack/defense/move 值。
 * 真值要等 spec 06 從 CIV.EXE code const 陣列抽出. */
void civ_unit_stats(civ_unit_type_t t, int *atk, int *def, int *moves);

/* 取單位顯示名稱 (zh-TW, 對齊 civ_dict.c STR# 132). */
const char *civ_unit_name_zh(civ_unit_type_t t);

/* 攻擊判定 - placeholder 公式 (非原版):
 *   roll = (rand() % (atk+def))
 *   attacker 勝 = roll < atk
 *
 * 真公式待 spec 07. 回 true 表示 attacker 勝 (defender 移除),
 * false 表示 attacker 敗 (attacker 移除). */
bool civ_unit_attack_resolve(civ_unit_t *attacker, civ_unit_t *defender);

#endif /* CIV_WORLD_UNIT_H */
