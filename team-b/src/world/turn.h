/*
 * world/turn.h — 回合迴圈（M6 lite）
 *
 * 對應 spec 02 §2.1.2 H 段的 idle path + 原版 turn-based 機制。
 * M6-lite 只實作最薄骨架：turn counter + AI 簡易行動模擬。
 * 真實單位 / 城市 / 外交留 M6-full（依 spec 06+07）。
 */
#ifndef CIV_WORLD_TURN_H
#define CIV_WORLD_TURN_H

#include <stdint.h>

struct civ_game;

/* 進入下一回合：turn++、跑 AI mock action、隨機選一個格子推進
 * "AI move" counter（純 demo，無實際遊戲邏輯）。 */
void civ_turn_advance(struct civ_game *g);

#endif /* CIV_WORLD_TURN_H */
