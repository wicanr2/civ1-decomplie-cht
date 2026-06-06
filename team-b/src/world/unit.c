#include "unit.h"

#include <stdlib.h>

/*
 * M6-full-lite 數值表 — 全部 PLACEHOLDER, 真值待 spec 06 從 Ghidra 抽出.
 *
 * 公式對齊 Civ1 wiki 公開資訊:
 *   - Settlers: atk 0 / def 1 / moves 1
 *   - Militia:  atk 1 / def 1 / moves 1
 *   - Phalanx:  atk 1 / def 2 / moves 1
 *   - Legion:   atk 4 / def 2 / moves 1
 *   - Musketeers: atk 3 / def 3 / moves 1
 *
 * 真實 CIV.EXE 內這些值推測 hardcoded 在某個 code segment const 陣列,
 * spec 06 ground-truth pass 會精確抽 (e.g. Musketeers 真值可能略不同).
 */
static const struct {
    int atk, def, moves;
    const char *zh;
} UNIT_STATS[CIV_UNIT_COUNT] = {
    [CIV_UNIT_NONE]       = { 0, 0, 0, ""        },
    [CIV_UNIT_SETTLERS]   = { 0, 1, 1, "墾荒者"  },
    [CIV_UNIT_MILITIA]    = { 1, 1, 1, "民兵"    },
    [CIV_UNIT_PHALANX]    = { 1, 2, 1, "方陣兵"  },
    [CIV_UNIT_LEGION]     = { 4, 2, 1, "軍團兵"  },
    [CIV_UNIT_MUSKETEERS] = { 3, 3, 1, "火槍兵"  },
};

void civ_unit_stats(civ_unit_type_t t, int *atk, int *def, int *moves)
{
    if (t < 0 || t >= CIV_UNIT_COUNT) t = CIV_UNIT_NONE;
    if (atk)   *atk   = UNIT_STATS[t].atk;
    if (def)   *def   = UNIT_STATS[t].def;
    if (moves) *moves = UNIT_STATS[t].moves;
}

const char *civ_unit_name_zh(civ_unit_type_t t)
{
    if (t < 0 || t >= CIV_UNIT_COUNT) return "";
    return UNIT_STATS[t].zh;
}

bool civ_unit_attack_resolve(civ_unit_t *attacker, civ_unit_t *defender)
{
    if (!attacker || !defender) return false;
    int atk = UNIT_STATS[attacker->type].atk;
    int def = UNIT_STATS[defender->type].def;
    if (atk + def == 0) return false;
    int roll = rand() % (atk + def);
    bool win = roll < atk;
    if (win) {
        defender->alive = false;
    } else {
        attacker->alive = false;
    }
    return win;
}
