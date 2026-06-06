/*
 * dialog/wizard.h — 新局精靈（3 頁：Difficulty → Civ → Name）
 *
 * 對應 spec 02 §2.1.2 D 段 5 個 startup dialog（0xAB/B1/BE/CB/D8）
 * 的功能等價物。原版用 Mac 風格 .RSC dialog template；M4-full
 * 走 Team B 自製 modal dialog 框架。
 *
 * Wizard 結束時把選擇寫進 game.player_civ_slot / player_difficulty /
 * player_name，然後關閉。
 */
#ifndef CIV_DIALOG_WIZARD_H
#define CIV_DIALOG_WIZARD_H

#include "dialog.h"

/* 5 個難度 — 對應原版 Chieftain / Warlord / Prince / King / Emperor */
enum {
    CIV_DIFF_CHIEFTAIN = 0,
    CIV_DIFF_WARLORD   = 1,
    CIV_DIFF_PRINCE    = 2,
    CIV_DIFF_KING      = 3,
    CIV_DIFF_EMPEROR   = 4,
};

/* 建立並 push 新局精靈到 dialog stack。 */
void civ_wizard_open(civ_dialog_stack_t *s, struct civ_game *g);

#endif /* CIV_DIALOG_WIZARD_H */
