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
 * 對應 manual P38 Diplomacy + spec 06 §6.7 + spec 03 §3.1 KING00..13 CvPc
 * (CIVDATA2 resource id 500..513) + spec 05 STR# 140 真實順序.
 *
 * **R19 重大發現**: 1993 Win port STR# 140 順序跟 Fandom wiki 不同:
 *   slot 1=Caesar 2=Hammurabi 3=Frederick 4=Ramesses 5=Lincoln
 *   6=Alexander 7=Gandhi 8=NONE 9=Stalin 10=Shaka
 *   11=Napoleon 12=Montezuma 13=Mao 14=Elizabeth I
 * 沒有蒙古, 中國領袖是 Mao 不是 Tzu Hsi.
 * king_sprite_idx = slot - 1 (KING00..13 對應 slot 1..14).
 */
#ifndef CIV_WORLD_DIPLOMAT_H
#define CIV_WORLD_DIPLOMAT_H

#include <stdbool.h>
#include <stdint.h>

#define CIV_LEADER_COUNT 14   /* slot 1..14, slot 8 = NONE */

typedef enum {
    CIV_LEADER_NONE        = 0,
    CIV_LEADER_CAESAR      = 1,   /* 羅馬 — KING00 */
    CIV_LEADER_HAMMURABI   = 2,   /* 巴比倫 — KING01 */
    CIV_LEADER_FREDERICK   = 3,   /* 德意志 — KING02, 銀白假髮藍軍服 */
    CIV_LEADER_RAMESES     = 4,   /* 埃及 — KING03 */
    CIV_LEADER_LINCOLN     = 5,   /* 美利堅 — KING04 */
    CIV_LEADER_ALEXANDER   = 6,   /* 希臘 — KING05 */
    CIV_LEADER_GANDHI      = 7,   /* 印度 — KING06 */
    /* slot 8 = NONE (STR# 140 留空) — KING07 hosted 但未對應 civ */
    CIV_LEADER_STALIN      = 9,   /* 俄羅斯 — KING08 */
    CIV_LEADER_SHAKA       = 10,  /* 祖魯 — KING09 */
    CIV_LEADER_NAPOLEON    = 11,  /* 法蘭西 — KING10 */
    CIV_LEADER_MONTEZUMA   = 12,  /* 阿茲特克 — KING11 */
    CIV_LEADER_MAO         = 13,  /* 中華 — KING12 */
    CIV_LEADER_ELIZABETH   = 14,  /* 英格蘭 — KING13, 紅華服 */
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

/* 取得 KING sprite CvPc id (CIVDATA2 內 500..513). 回 -1 代表 NONE/越界. */
int civ_leader_king_sprite_id(civ_leader_id_t l);

/* zh-TW 領袖名稱 (對齊 spec 05 STR# 140 + civ_dict.c). */
const char *civ_leader_name_zh(civ_leader_id_t l);

/* zh-TW 文明名 (e.g. 英格蘭 / 德意志) — 對話用. */
const char *civ_leader_civ_name_zh(civ_leader_id_t l);

/* 取此 event 應顯示的對話 (zh-TW, ≤ 2 行 64 中文字). 回靜態指標. */
const char *civ_diplomat_dialog_zh(const civ_diplomat_event_t *ev);

/* 1 個字代表 icon (e.g. "英" for Elizabeth, "德" for Frederick) — fallback
 * 用 (sprite 載入失敗時). */
const char *civ_leader_icon_char_zh(civ_leader_id_t l);

/* 領袖代表色 (服裝 RGB) — fallback (sprite 未載入時用). */
void civ_leader_palette(civ_leader_id_t l,
                         uint8_t *r, uint8_t *g, uint8_t *b);

#endif /* CIV_WORLD_DIPLOMAT_H */
