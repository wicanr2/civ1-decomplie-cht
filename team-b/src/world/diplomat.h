/*
 * world/diplomat.h — R18 M-diplomat 外交訪問事件
 *
 * 對齊 reference: docs/screenshots/reference/civ1_diplomat_elizabeth_pc29.webp
 *                docs/screenshots/reference/civ1_diplomat_frederick_pc29.webp
 *
 * 1991/1993 Civ1 中, 外交訪問畫面在以下時機觸發:
 *   - 對方領袖第一次見面 ("Greetings from XXX, ruler of YYY...")
 *   - 對方派 Diplomat 過來談判
 *   - 戰爭/和平/貢品要求
 *
 * 對應 manual P38 Diplomacy + spec 06 §6.7 16 nation 3-axis AI personality.
 * 本 R18 階段只實作 first-greeting 訊息 (PC_29 圖片對應), 後續 R-diplomacy
 * 再補完整 4 種 stance + 5 種 tone.
 */
#ifndef CIV_WORLD_DIPLOMAT_H
#define CIV_WORLD_DIPLOMAT_H

#include <stdbool.h>
#include <stdint.h>

#define CIV_LEADER_COUNT 14   /* 對齊 spec 06 §6.7 + civ_dict.c STR# 140 */

typedef enum {
    CIV_LEADER_NONE        = 0,
    CIV_LEADER_ELIZABETH   = 1,   /* 英國 — Greeting 紅色華服 */
    CIV_LEADER_FREDERICK   = 2,   /* 德國 — Greeting 藍色軍服 */
    CIV_LEADER_CAESAR      = 3,   /* 羅馬 */
    CIV_LEADER_HAMMURABI   = 4,   /* 巴比倫 */
    CIV_LEADER_NAPOLEON    = 5,   /* 法國 */
    CIV_LEADER_RAMSES      = 6,   /* 埃及 */
    CIV_LEADER_LINCOLN     = 7,   /* 美國 */
    CIV_LEADER_ALEXANDER   = 8,   /* 希臘 */
    CIV_LEADER_GANDHI      = 9,   /* 印度 */
    CIV_LEADER_GENGHIS     = 10,  /* 蒙古 */
    CIV_LEADER_TZU_HSI     = 11,  /* 中國 */
    CIV_LEADER_SHAKA       = 12,  /* 祖魯 */
    CIV_LEADER_MONTEZUMA   = 13,  /* 阿茲特克 */
    CIV_LEADER_STALIN      = 14,  /* 蘇聯 */
} civ_leader_id_t;

typedef enum {
    CIV_DIPLOMAT_GREETING        = 0,   /* 首次見面 — "Greetings from..." */
    CIV_DIPLOMAT_DEMAND_TRIBUTE  = 1,   /* 要求進貢 */
    CIV_DIPLOMAT_OFFER_PEACE     = 2,   /* 提議和平 */
    CIV_DIPLOMAT_DECLARE_WAR     = 3,   /* 宣戰 */
    CIV_DIPLOMAT_PROPOSE_TRADE   = 4,   /* 提議交易 */
} civ_diplomat_mood_t;

typedef struct civ_diplomat_event {
    civ_leader_id_t      leader;
    civ_diplomat_mood_t  mood;
} civ_diplomat_event_t;

/* zh-TW 領袖名稱 (對齊 spec 05 STR# 140 + civ_dict.c). */
const char *civ_leader_name_zh(civ_leader_id_t l);

/* zh-TW 文明名 (e.g. 英國 / 德國) — 對話用. */
const char *civ_leader_civ_name_zh(civ_leader_id_t l);

/* 取此 event 應顯示的對話 (zh-TW, ≤ 2 行 64 中文字). 回靜態指標. */
const char *civ_diplomat_dialog_zh(const civ_diplomat_event_t *ev);

/* 1 個字代表 icon (e.g. "英" for Elizabeth, "德" for Frederick) — 對齊 R17
 * tech_icon_char_zh 設計, 顯示在領袖頭像占位中央. */
const char *civ_leader_icon_char_zh(civ_leader_id_t l);

/* 領袖代表色 (服裝 RGB) — 用來畫頭像占位 + spear ornament. */
void civ_leader_palette(civ_leader_id_t l,
                         uint8_t *r, uint8_t *g, uint8_t *b);

#endif /* CIV_WORLD_DIPLOMAT_H */
