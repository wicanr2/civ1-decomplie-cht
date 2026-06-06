/*
 * world/tech.h — R16 M-techscreen 科技 + discovery event
 *
 * 對齊 spec 06 §6.5 (47 tech) + spec 05 STR# 130 (tech name).
 * 1991 manual P49 "Science Advisor" 對應. 1993 Win 內 tech 進度
 * 推測佔 47-byte bitmap (對應 SAV §7.2 0x????). 真實 offset 待 spec 07 v0.3.
 *
 * R16 階段: 只實作前 13 個 tech (常用一階) + discovery event struct.
 * 後續 R17+ 補滿 47 + 5 future tech.
 */
#ifndef CIV_WORLD_TECH_H
#define CIV_WORLD_TECH_H

#include <stdbool.h>
#include <stdint.h>

#define CIV_TECH_COUNT   13  /* R16 階段; 真值 47 (spec 06 §6.5) */

typedef enum {
    CIV_TECH_NONE             = 0,
    CIV_TECH_ALPHABET         = 1,   /* 字母 — 無 prereq */
    CIV_TECH_BRONZE_WORKING   = 2,   /* 青銅器 — 無 prereq */
    CIV_TECH_HORSEBACK_RIDING = 3,   /* 騎術 — 無 prereq */
    CIV_TECH_POTTERY          = 4,   /* 陶器 — 無 prereq */
    CIV_TECH_CURRENCY         = 5,   /* 貨幣 — Bronze Working */
    CIV_TECH_IRON_WORKING     = 6,   /* 鐵器 — Bronze Working + Warrior Code */
    CIV_TECH_WRITING          = 7,   /* 文字 — Alphabet */
    CIV_TECH_MAP_MAKING       = 8,   /* 地圖學 — Alphabet */
    CIV_TECH_MASONRY          = 9,   /* 砌石 — 無 prereq */
    CIV_TECH_POLYTHEISM       = 10,  /* 多神教 — Horseback Riding + Ceremonial Burial */
    CIV_TECH_LITERACY         = 11,  /* 文藝 — Writing + Code of Laws */
    CIV_TECH_MATHEMATICS      = 12,  /* 數學 — Alphabet + Masonry */
} civ_tech_id_t;

typedef enum {
    CIV_TECH_LEARN_SELF       = 0,   /* 自家研發 */
    CIV_TECH_LEARN_DIPLOMAT   = 1,   /* Diplomat 偷取 */
    CIV_TECH_LEARN_TRADE      = 2,   /* 外交交易 */
    CIV_TECH_LEARN_HUT        = 3,   /* 部落小屋 */
    CIV_TECH_LEARN_LIBRARY    = 4,   /* Great Library Wonder */
} civ_tech_learn_source_t;

/* discovery event payload. R16-1 起 widgets/tech_screen 顯示時讀此值. */
typedef struct civ_tech_discovery_event {
    civ_tech_id_t           tech_id;
    civ_tech_learn_source_t source;
    int                     from_civ_slot;   /* DIPLOMAT/TRADE 才有效, 0..7 */

    /* 解鎖內容 (spec 06 §6.5 prereq DAG 反推, R16-4 填). 0 = 結束. */
    civ_tech_id_t           unlocked_techs[8];
    int                     unlocked_units[4];   /* CIV_UNIT_* */
    int                     unlocked_imp[4];     /* building idx, 對齊 city.c BUILDING_INFO */
    int                     unlocked_wonder[4];  /* wonder idx, 待 R17 接 wonder 表 */
} civ_tech_discovery_event_t;

/* zh-TW 名稱 (對齊 spec 05 STR# 130 + civ_dict.c). */
const char *civ_tech_name_zh(civ_tech_id_t t);

/* "Civilization Advance" 對應 zh subtitle. */
const char *civ_tech_subtitle_zh(void);

/* "(取自法國)" / "(自家研發)" 等 source 對應 zh 短語. civ_name 在 source ==
 * DIPLOMAT/TRADE 才 dereferenced, 其餘可傳 NULL. 回靜態 buffer (非執行緒安全,
 * 用於單 frame render). */
const char *civ_tech_source_phrase_zh(civ_tech_learn_source_t s,
                                       const char *civ_name);

/* R16-4: 從 spec 06 prereq DAG 反推, 填 event->unlocked_*.
 * 不會修改 tech_id / source / from_civ_slot. */
void civ_tech_discovery_fill_unlocked(civ_tech_discovery_event_t *ev);

#endif /* CIV_WORLD_TECH_H */
