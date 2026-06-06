/*
 * test_diplomat.c — R18 M-diplomat ctest:
 *   leader name lookup / civ name / icon char / dialog 串接 / palette range
 */
#include "world/diplomat.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

static void test_leader_name_zh(void)
{
    assert(strcmp(civ_leader_name_zh(CIV_LEADER_NONE), "") == 0);
    assert(strcmp(civ_leader_name_zh(CIV_LEADER_ELIZABETH), "伊莉莎白一世") == 0);
    assert(strcmp(civ_leader_name_zh(CIV_LEADER_FREDERICK), "腓特烈大帝") == 0);
    assert(strcmp(civ_leader_name_zh(CIV_LEADER_CAESAR),    "凱撒") == 0);
    /* 越界 */
    assert(strcmp(civ_leader_name_zh((civ_leader_id_t)999), "") == 0);
    printf("  test_leader_name_zh PASS\n");
}

static void test_leader_civ_name(void)
{
    /* R19: 對齊 STR# 140 真實 civ name */
    assert(strcmp(civ_leader_civ_name_zh(CIV_LEADER_ELIZABETH), "英格蘭") == 0);
    assert(strcmp(civ_leader_civ_name_zh(CIV_LEADER_FREDERICK), "德意志") == 0);
    assert(strcmp(civ_leader_civ_name_zh(CIV_LEADER_MAO),       "中華") == 0);
    printf("  test_leader_civ_name PASS\n");
}

static void test_icon_char(void)
{
    assert(strcmp(civ_leader_icon_char_zh(CIV_LEADER_ELIZABETH), "英") == 0);
    assert(strcmp(civ_leader_icon_char_zh(CIV_LEADER_FREDERICK), "德") == 0);
    assert(strcmp(civ_leader_icon_char_zh(CIV_LEADER_MAO),       "華") == 0);
    /* 越界回 "?" */
    assert(strcmp(civ_leader_icon_char_zh((civ_leader_id_t)999), "?") == 0);
    printf("  test_icon_char PASS\n");
}

static void test_king_sprite_id(void)
{
    /* R19: 對齊 spec 03 §3.1 CIVDATA2 KING00..13 = id 500..513 */
    assert(civ_leader_king_sprite_id(CIV_LEADER_CAESAR)    == 500);
    assert(civ_leader_king_sprite_id(CIV_LEADER_FREDERICK) == 502);
    assert(civ_leader_king_sprite_id(CIV_LEADER_ELIZABETH) == 513);
    /* slot 8 NONE 沒對應 leader → -1 */
    assert(civ_leader_king_sprite_id((civ_leader_id_t)8) == -1);
    assert(civ_leader_king_sprite_id(CIV_LEADER_NONE)    == -1);
    /* 越界 → -1 */
    assert(civ_leader_king_sprite_id((civ_leader_id_t)999) == -1);
    printf("  test_king_sprite_id PASS\n");
}

static void test_palette_rgb(void)
{
    uint8_t r, g, b;
    civ_leader_palette(CIV_LEADER_ELIZABETH, &r, &g, &b);
    /* Elizabeth 紅華服: R > G && R > B */
    if (r <= g || r <= b) {
        fprintf(stderr, "Elizabeth RGB (%d,%d,%d) not red-dominant\n", r,g,b);
        assert(0);
    }
    civ_leader_palette(CIV_LEADER_FREDERICK, &r, &g, &b);
    /* Frederick 藍軍服: B > R && B > G */
    if (b <= r || b <= g) {
        fprintf(stderr, "Frederick RGB (%d,%d,%d) not blue-dominant\n", r,g,b);
        assert(0);
    }
    /* NULL 安全 */
    civ_leader_palette(CIV_LEADER_NONE, NULL, NULL, NULL);
    printf("  test_palette_rgb PASS\n");
}

static void test_dialog_greeting(void)
{
    civ_diplomat_event_t ev = {0};
    char buf[256];

    ev.leader = CIV_LEADER_ELIZABETH;
    ev.mood   = CIV_DIPLOMAT_GREETING;
    snprintf(buf, sizeof buf, "%s", civ_diplomat_dialog_zh(&ev));
    if (!strstr(buf, "伊莉莎白")) { fprintf(stderr, "want 伊莉莎白 in %s\n", buf); assert(0); }
    if (!strstr(buf, "英格蘭"))   { fprintf(stderr, "want 英格蘭 in %s\n", buf); assert(0); }
    if (!strstr(buf, "致意"))     { fprintf(stderr, "want 致意 in %s\n", buf); assert(0); }

    ev.leader = CIV_LEADER_FREDERICK;
    snprintf(buf, sizeof buf, "%s", civ_diplomat_dialog_zh(&ev));
    if (!strstr(buf, "腓特烈")) { fprintf(stderr, "want 腓特烈 in %s\n", buf); assert(0); }
    if (!strstr(buf, "德意志")) { fprintf(stderr, "want 德意志 in %s\n", buf); assert(0); }

    /* generic (其他領袖 fallback) */
    ev.leader = CIV_LEADER_CAESAR;
    snprintf(buf, sizeof buf, "%s", civ_diplomat_dialog_zh(&ev));
    if (!strstr(buf, "凱撒")) { fprintf(stderr, "want 凱撒 in %s\n", buf); assert(0); }
    if (!strstr(buf, "羅馬")) { fprintf(stderr, "want 羅馬 in %s\n", buf); assert(0); }
    printf("  test_dialog_greeting PASS\n");
}

static void test_dialog_moods(void)
{
    civ_diplomat_event_t ev = {0};
    ev.leader = CIV_LEADER_ELIZABETH;

    char buf[256];
    ev.mood = CIV_DIPLOMAT_DECLARE_WAR;
    snprintf(buf, sizeof buf, "%s", civ_diplomat_dialog_zh(&ev));
    if (!strstr(buf, "宣戰")) { fprintf(stderr, "want 宣戰 in %s\n", buf); assert(0); }

    ev.mood = CIV_DIPLOMAT_OFFER_PEACE;
    snprintf(buf, sizeof buf, "%s", civ_diplomat_dialog_zh(&ev));
    if (!strstr(buf, "和平")) { fprintf(stderr, "want 和平 in %s\n", buf); assert(0); }

    ev.mood = CIV_DIPLOMAT_DEMAND_TRIBUTE;
    snprintf(buf, sizeof buf, "%s", civ_diplomat_dialog_zh(&ev));
    if (!strstr(buf, "貢品")) { fprintf(stderr, "want 貢品 in %s\n", buf); assert(0); }

    ev.mood = CIV_DIPLOMAT_PROPOSE_TRADE;
    snprintf(buf, sizeof buf, "%s", civ_diplomat_dialog_zh(&ev));
    if (!strstr(buf, "交易")) { fprintf(stderr, "want 交易 in %s\n", buf); assert(0); }
    printf("  test_dialog_moods PASS\n");
}

static void test_null_event(void)
{
    /* NULL ev 不 crash, 回空串 */
    const char *d = civ_diplomat_dialog_zh(NULL);
    if (!d || d[0] != '\0') {
        fprintf(stderr, "expected empty string for NULL ev, got %p\n", (void*)d);
        assert(0);
    }
    printf("  test_null_event PASS\n");
}

int main(void)
{
    printf("test_diplomat\n");
    test_leader_name_zh();
    test_leader_civ_name();
    test_icon_char();
    test_king_sprite_id();
    test_palette_rgb();
    test_dialog_greeting();
    test_dialog_moods();
    test_null_event();
    printf("ALL PASS\n");
    return 0;
}
