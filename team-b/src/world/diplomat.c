#include "diplomat.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>

/* R19: 對齊 spec 05 STR# 140 真實順序 (civs.c ZH[] table 同步).
 *   slot 1=Caesar 2=Hammurabi 3=Frederick 4=Ramesses 5=Lincoln
 *   6=Alexander 7=Gandhi 8=NONE 9=Stalin 10=Shaka
 *   11=Napoleon 12=Montezuma 13=Mao 14=Elizabeth I  */
static const char *LEADER_NAME_ZH[CIV_LEADER_COUNT + 1] = {
    [CIV_LEADER_NONE]      = "",
    [CIV_LEADER_CAESAR]    = "凱撒",
    [CIV_LEADER_HAMMURABI] = "漢摩拉比",
    [CIV_LEADER_FREDERICK] = "腓特烈大帝",
    [CIV_LEADER_RAMESES]   = "拉美西斯",
    [CIV_LEADER_LINCOLN]   = "林肯",
    [CIV_LEADER_ALEXANDER] = "亞歷山大",
    [CIV_LEADER_GANDHI]    = "甘地",
    [8]                    = "",   /* slot 8 NONE */
    [CIV_LEADER_STALIN]    = "史達林",
    [CIV_LEADER_SHAKA]     = "夏卡",
    [CIV_LEADER_NAPOLEON]  = "拿破崙",
    [CIV_LEADER_MONTEZUMA] = "蒙特蘇馬",
    [CIV_LEADER_MAO]       = "毛澤東",
    [CIV_LEADER_ELIZABETH] = "伊莉莎白一世",
};

static const char *LEADER_CIV_ZH[CIV_LEADER_COUNT + 1] = {
    [CIV_LEADER_NONE]      = "",
    [CIV_LEADER_CAESAR]    = "羅馬",
    [CIV_LEADER_HAMMURABI] = "巴比倫",
    [CIV_LEADER_FREDERICK] = "德意志",
    [CIV_LEADER_RAMESES]   = "埃及",
    [CIV_LEADER_LINCOLN]   = "美利堅",
    [CIV_LEADER_ALEXANDER] = "希臘",
    [CIV_LEADER_GANDHI]    = "印度",
    [8]                    = "",
    [CIV_LEADER_STALIN]    = "俄羅斯",
    [CIV_LEADER_SHAKA]     = "祖魯",
    [CIV_LEADER_NAPOLEON]  = "法蘭西",
    [CIV_LEADER_MONTEZUMA] = "阿茲特克",
    [CIV_LEADER_MAO]       = "中華",
    [CIV_LEADER_ELIZABETH] = "英格蘭",
};

static const char *LEADER_ICON_ZH[CIV_LEADER_COUNT + 1] = {
    [CIV_LEADER_NONE]      = "?",
    [CIV_LEADER_CAESAR]    = "羅",
    [CIV_LEADER_HAMMURABI] = "巴",
    [CIV_LEADER_FREDERICK] = "德",
    [CIV_LEADER_RAMESES]   = "埃",
    [CIV_LEADER_LINCOLN]   = "美",
    [CIV_LEADER_ALEXANDER] = "希",
    [CIV_LEADER_GANDHI]    = "印",
    [8]                    = "?",
    [CIV_LEADER_STALIN]    = "蘇",
    [CIV_LEADER_SHAKA]     = "祖",
    [CIV_LEADER_NAPOLEON]  = "法",
    [CIV_LEADER_MONTEZUMA] = "阿",
    [CIV_LEADER_MAO]       = "華",
    [CIV_LEADER_ELIZABETH] = "英",
};

/* 領袖服裝代表色 — 對齊 reference 圖片觀察 (fallback 用) */
static const struct { uint8_t r, g, b; } LEADER_PAL[CIV_LEADER_COUNT + 1] = {
    [CIV_LEADER_NONE]      = { 0x80, 0x80, 0x80 },
    [CIV_LEADER_CAESAR]    = { 0xC0, 0x00, 0x00 },  /* 羅馬紅 */
    [CIV_LEADER_HAMMURABI] = { 0xC0, 0x80, 0x40 },
    [CIV_LEADER_FREDERICK] = { 0x20, 0x40, 0xA0 },  /* 藍軍服 */
    [CIV_LEADER_RAMESES]   = { 0xE0, 0xC0, 0x00 },  /* 埃及金 */
    [CIV_LEADER_LINCOLN]   = { 0x40, 0x40, 0x40 },
    [CIV_LEADER_ALEXANDER] = { 0xC0, 0x80, 0x00 },
    [CIV_LEADER_GANDHI]    = { 0xE0, 0xE0, 0xE0 },
    [8]                    = { 0x80, 0x80, 0x80 },
    [CIV_LEADER_STALIN]    = { 0x80, 0x20, 0x20 },
    [CIV_LEADER_SHAKA]     = { 0x60, 0x40, 0x20 },
    [CIV_LEADER_NAPOLEON]  = { 0x00, 0x40, 0x80 },
    [CIV_LEADER_MONTEZUMA] = { 0x80, 0xC0, 0x40 },
    [CIV_LEADER_MAO]       = { 0xC0, 0x40, 0x20 },
    [CIV_LEADER_ELIZABETH] = { 0xC0, 0x20, 0x20 },  /* 紅華服 */
};

int civ_leader_king_sprite_id(civ_leader_id_t l)
{
    if ((int)l < 1 || (int)l > CIV_LEADER_COUNT) return -1;
    if ((int)l == 8) return -1;   /* slot 8 NONE 沒有對應 leader, 但 KING07 sprite 存在 */
    /* KING00..13 = CIVDATA2 id 500..513, slot 1..14 對齊 idx 0..13 */
    return 500 + ((int)l - 1);
}

const char *civ_leader_name_zh(civ_leader_id_t l)
{
    if ((int)l < 0 || (int)l > CIV_LEADER_COUNT) return "";
    return LEADER_NAME_ZH[l];
}

const char *civ_leader_civ_name_zh(civ_leader_id_t l)
{
    if ((int)l < 0 || (int)l > CIV_LEADER_COUNT) return "";
    return LEADER_CIV_ZH[l];
}

const char *civ_leader_icon_char_zh(civ_leader_id_t l)
{
    if ((int)l < 0 || (int)l > CIV_LEADER_COUNT) return "?";
    return LEADER_ICON_ZH[l];
}

void civ_leader_palette(civ_leader_id_t l,
                         uint8_t *r, uint8_t *g, uint8_t *b)
{
    if ((int)l < 0 || (int)l > CIV_LEADER_COUNT) l = CIV_LEADER_NONE;
    if (r) *r = LEADER_PAL[l].r;
    if (g) *g = LEADER_PAL[l].g;
    if (b) *b = LEADER_PAL[l].b;
}

const char *civ_diplomat_dialog_zh(const civ_diplomat_event_t *ev)
{
    static char buf[256];
    if (!ev) return "";
    const char *lname = civ_leader_name_zh(ev->leader);
    const char *cname = civ_leader_civ_name_zh(ev->leader);

    switch (ev->mood) {
        case CIV_DIPLOMAT_GREETING:
            if (ev->leader == CIV_LEADER_ELIZABETH) {
                snprintf(buf, sizeof buf,
                         "%s女皇%s向您致意……",
                         cname, lname);
            } else if (ev->leader == CIV_LEADER_FREDERICK) {
                snprintf(buf, sizeof buf,
                         "我等至明智之%s皇帝%s向您致意……",
                         cname, lname);
            } else {
                snprintf(buf, sizeof buf,
                         "%s統治者%s向您致意……",
                         cname, lname);
            }
            break;
        case CIV_DIPLOMAT_DEMAND_TRIBUTE:
            snprintf(buf, sizeof buf,
                     "%s%s要求貢品。否則, 戰爭難免。",
                     cname, lname);
            break;
        case CIV_DIPLOMAT_OFFER_PEACE:
            snprintf(buf, sizeof buf,
                     "%s%s提議和平條約。",
                     cname, lname);
            break;
        case CIV_DIPLOMAT_DECLARE_WAR:
            snprintf(buf, sizeof buf,
                     "%s%s向您宣戰!",
                     cname, lname);
            break;
        case CIV_DIPLOMAT_PROPOSE_TRADE:
            snprintf(buf, sizeof buf,
                     "%s%s欲與您進行科技交易。",
                     cname, lname);
            break;
    }
    return buf;
}
