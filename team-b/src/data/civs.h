/*
 * data/civs.h — 14 文明 master table
 *
 * 對應 spec 05 §5.3 的 STR# 140/143/144/148/149 + spec 03 § 3.1
 * 的 KING00..13 CvPc sprite。
 *
 * Slot 0 = NONE / Barbarian（依 STR# 不同）；slot 1..14 = 14 文明。
 */
#ifndef CIV_DATA_CIVS_H
#define CIV_DATA_CIVS_H

#include "strings.h"

#include <stdbool.h>

#define CIV_NUM_CIVS 14

typedef struct civ_civ_entry {
    int         slot;          /* 1..14 */
    const char *leader_en;     /* STR# 140 — Caesar / Hammurabi / ... */
    const char *adj_sing_en;   /* STR# 143 — Roman / Babylonian / ... */
    const char *adj_plur_en;   /* STR# 144 — Romans / Babylonians / ... */
    const char *civ_sing_en;   /* STR# 148 — Roman / Babylonian / ... */
    const char *civ_plur_en;   /* STR# 149 — Romans / Babylonians / ... */
    /* 對應領袖肖像 CvPc id。CIVDATA2 內 KING00..13 = 領袖 0..13。
     * civ slot 1..14 用 sprite 0..13；slot 0 (NONE/Barbarian) 用 sprite 13。 */
    int         king_sprite_idx;
} civ_civ_entry_t;

/* 從 STR# resource 建 14 文明 master table。
 * leaders/army_sing/army_plur/civ_sing/civ_plur 必須是已 parse 好的
 * civ_strlist_t。 */
int civ_civs_build(const civ_strlist_t *leaders,
                   const civ_strlist_t *army_sing,
                   const civ_strlist_t *army_plur,
                   const civ_strlist_t *civ_sing,
                   const civ_strlist_t *civ_plur,
                   civ_civ_entry_t      out[CIV_NUM_CIVS]);

/* 取得指定 civ 的中文翻譯（含 領袖名 / 文明形容詞 / 文明名 / 文明複數）。
 * 找不到回 NULL；fallback 給 caller 用英文。 */
const char *civ_civs_zh(int slot, const char *which);
/* which: "leader" | "civ_sing" | "civ_plur" | "adj_sing" | "adj_plur" */

#endif /* CIV_DATA_CIVS_H */
