/*
 * civ_loop.h — 主迴圈
 *
 * 對應 spec 02 §2.1.2 H 段 `while (DAT_12d8_24ee == 0) { FUN_1088_0000(); }`
 * 與 §2.1.3 推測的 PEEKMESSAGE idle pump。
 */
#ifndef CIV_LOOP_H
#define CIV_LOOP_H

struct civ_game;

/* 進入主迴圈。從 g->quit 變 true 之後 1 tick 內返回。 */
void civ_loop(struct civ_game *g);

#endif /* CIV_LOOP_H */
