#include "diplomat.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>

static const char *LEADER_NAME_ZH[CIV_LEADER_COUNT + 1] = {
    [CIV_LEADER_NONE]      = "",
    [CIV_LEADER_ELIZABETH] = "伊莉莎白一世",
    [CIV_LEADER_FREDERICK] = "腓特烈大帝",
    [CIV_LEADER_CAESAR]    = "凱撒",
    [CIV_LEADER_HAMMURABI] = "漢摩拉比",
    [CIV_LEADER_NAPOLEON]  = "拿破崙",
    [CIV_LEADER_RAMSES]    = "拉美西斯",
    [CIV_LEADER_LINCOLN]   = "林肯",
    [CIV_LEADER_ALEXANDER] = "亞歷山大",
    [CIV_LEADER_GANDHI]    = "甘地",
    [CIV_LEADER_GENGHIS]   = "成吉思汗",
    [CIV_LEADER_TZU_HSI]   = "慈禧太后",
    [CIV_LEADER_SHAKA]     = "夏卡",
    [CIV_LEADER_MONTEZUMA] = "蒙特祖瑪",
    [CIV_LEADER_STALIN]    = "史達林",
};

static const char *LEADER_CIV_ZH[CIV_LEADER_COUNT + 1] = {
    [CIV_LEADER_NONE]      = "",
    [CIV_LEADER_ELIZABETH] = "英格蘭",
    [CIV_LEADER_FREDERICK] = "日耳曼",
    [CIV_LEADER_CAESAR]    = "羅馬",
    [CIV_LEADER_HAMMURABI] = "巴比倫",
    [CIV_LEADER_NAPOLEON]  = "法蘭西",
    [CIV_LEADER_RAMSES]    = "埃及",
    [CIV_LEADER_LINCOLN]   = "美利堅",
    [CIV_LEADER_ALEXANDER] = "希臘",
    [CIV_LEADER_GANDHI]    = "印度",
    [CIV_LEADER_GENGHIS]   = "蒙古",
    [CIV_LEADER_TZU_HSI]   = "中華",
    [CIV_LEADER_SHAKA]     = "祖魯",
    [CIV_LEADER_MONTEZUMA] = "阿茲特克",
    [CIV_LEADER_STALIN]    = "蘇聯",
};

static const char *LEADER_ICON_ZH[CIV_LEADER_COUNT + 1] = {
    [CIV_LEADER_NONE]      = "?",
    [CIV_LEADER_ELIZABETH] = "英",
    [CIV_LEADER_FREDERICK] = "德",
    [CIV_LEADER_CAESAR]    = "羅",
    [CIV_LEADER_HAMMURABI] = "巴",
    [CIV_LEADER_NAPOLEON]  = "法",
    [CIV_LEADER_RAMSES]    = "埃",
    [CIV_LEADER_LINCOLN]   = "美",
    [CIV_LEADER_ALEXANDER] = "希",
    [CIV_LEADER_GANDHI]    = "印",
    [CIV_LEADER_GENGHIS]   = "蒙",
    [CIV_LEADER_TZU_HSI]   = "華",
    [CIV_LEADER_SHAKA]     = "祖",
    [CIV_LEADER_MONTEZUMA] = "阿",
    [CIV_LEADER_STALIN]    = "蘇",
};

/* 領袖服裝代表色 — 對齊 reference 圖片觀察 */
static const struct { uint8_t r, g, b; } LEADER_PAL[CIV_LEADER_COUNT + 1] = {
    [CIV_LEADER_NONE]      = { 0x80, 0x80, 0x80 },
    [CIV_LEADER_ELIZABETH] = { 0xC0, 0x20, 0x20 },  /* 紅華服 */
    [CIV_LEADER_FREDERICK] = { 0x20, 0x40, 0xA0 },  /* 藍軍服 */
    [CIV_LEADER_CAESAR]    = { 0xC0, 0x00, 0x00 },  /* 羅馬紅 */
    [CIV_LEADER_HAMMURABI] = { 0xC0, 0x80, 0x40 },
    [CIV_LEADER_NAPOLEON]  = { 0x00, 0x40, 0x80 },
    [CIV_LEADER_RAMSES]    = { 0xE0, 0xC0, 0x00 },  /* 埃及金 */
    [CIV_LEADER_LINCOLN]   = { 0x40, 0x40, 0x40 },
    [CIV_LEADER_ALEXANDER] = { 0xC0, 0x80, 0x00 },
    [CIV_LEADER_GANDHI]    = { 0xE0, 0xE0, 0xE0 },
    [CIV_LEADER_GENGHIS]   = { 0x80, 0x40, 0x20 },
    [CIV_LEADER_TZU_HSI]   = { 0xE0, 0xC0, 0x00 },  /* 中華金黃 */
    [CIV_LEADER_SHAKA]     = { 0x60, 0x40, 0x20 },
    [CIV_LEADER_MONTEZUMA] = { 0x80, 0xC0, 0x40 },
    [CIV_LEADER_STALIN]    = { 0x80, 0x20, 0x20 },
};

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

/* R18-1 對話文字 — 對齊 reference 圖片內英文原句直譯
 *   Elizabeth: "Greetings from Elizabeth I, ruler and Empress of the English..."
 *              → "英格蘭女皇伊莉莎白一世向您致意..."
 *   Frederick: "Greetings from our most wise Emperor Frederick of the Germans..."
 *              → "我等至明智之日耳曼皇帝腓特烈向您致意..."
 *
 * 句末 "..." 模擬待玩家點擊 continue 的進度感. */
const char *civ_diplomat_dialog_zh(const civ_diplomat_event_t *ev)
{
    static char buf[256];
    if (!ev) return "";
    const char *lname = civ_leader_name_zh(ev->leader);
    const char *cname = civ_leader_civ_name_zh(ev->leader);

    switch (ev->mood) {
        case CIV_DIPLOMAT_GREETING:
            /* 為了 reference 兩位特別客製化句式 */
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
