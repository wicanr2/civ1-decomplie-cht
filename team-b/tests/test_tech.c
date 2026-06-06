/*
 * test_tech.c — R16-7 ctest: tech name + source phrase + prereq DAG 反推
 *
 * 對應 spec 06 §6.5 tech enum + tech.c fill_unlocked 邏輯.
 */
#include "world/tech.h"
#include "world/unit.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

static void test_tech_name_zh(void)
{
    assert(strcmp(civ_tech_name_zh(CIV_TECH_NONE), "") == 0);
    assert(strcmp(civ_tech_name_zh(CIV_TECH_ALPHABET), "字母") == 0);
    assert(strcmp(civ_tech_name_zh(CIV_TECH_BRONZE_WORKING), "青銅器") == 0);
    assert(strcmp(civ_tech_name_zh(CIV_TECH_IRON_WORKING), "鐵器") == 0);
    assert(strcmp(civ_tech_name_zh(CIV_TECH_CURRENCY), "貨幣") == 0);
    /* 越界回空字串 */
    assert(strcmp(civ_tech_name_zh((civ_tech_id_t)999), "") == 0);
    printf("  test_tech_name_zh PASS\n");
}

static void test_subtitle(void)
{
    assert(strcmp(civ_tech_subtitle_zh(), "文明進展") == 0);
    printf("  test_subtitle PASS\n");
}

static void test_source_phrase(void)
{
    assert(strcmp(civ_tech_source_phrase_zh(CIV_TECH_LEARN_SELF, NULL),
                  "(自家研發)") == 0);
    assert(strcmp(civ_tech_source_phrase_zh(CIV_TECH_LEARN_HUT, NULL),
                  "(部落小屋發現)") == 0);
    assert(strcmp(civ_tech_source_phrase_zh(CIV_TECH_LEARN_LIBRARY, NULL),
                  "(大圖書館授予)") == 0);
    /* DIPLOMAT/TRADE 帶 civ name. 用 static buf 取靜態指標複本印出. */
    char p1[64];
    snprintf(p1, sizeof p1, "%s",
             civ_tech_source_phrase_zh(CIV_TECH_LEARN_DIPLOMAT, "法國"));
    if (!strstr(p1, "法國")) { fprintf(stderr, "want 法國 in %s\n", p1); assert(0); }
    if (!strstr(p1, "取自")) { fprintf(stderr, "want 取自 in %s\n", p1); assert(0); }

    char p2[64];
    snprintf(p2, sizeof p2, "%s",
             civ_tech_source_phrase_zh(CIV_TECH_LEARN_TRADE, "德國"));
    if (!strstr(p2, "德國")) { fprintf(stderr, "want 德國 in %s\n", p2); assert(0); }
    if (!strstr(p2, "交易")) { fprintf(stderr, "want 交易 in %s\n", p2); assert(0); }

    /* NULL civ_name 不 crash */
    const char *p3 = civ_tech_source_phrase_zh(CIV_TECH_LEARN_DIPLOMAT, NULL);
    if (!p3) { assert(0); }
    printf("  test_source_phrase PASS\n");
}

static void test_fill_unlocked_bronze_working(void)
{
    civ_tech_discovery_event_t ev = {0};
    ev.tech_id = CIV_TECH_BRONZE_WORKING;
    civ_tech_discovery_fill_unlocked(&ev);
    /* 解鎖 Currency + Iron Working */
    assert(ev.unlocked_techs[0] == CIV_TECH_CURRENCY);
    assert(ev.unlocked_techs[1] == CIV_TECH_IRON_WORKING);
    /* 解鎖 Phalanx 單位 */
    assert(ev.unlocked_units[0] == CIV_UNIT_PHALANX);
    /* 解鎖 Colossus wonder placeholder */
    assert(ev.unlocked_wonder[0] == 1);
    printf("  test_fill_unlocked_bronze_working PASS\n");
}

static void test_fill_unlocked_alphabet(void)
{
    civ_tech_discovery_event_t ev = {0};
    ev.tech_id = CIV_TECH_ALPHABET;
    civ_tech_discovery_fill_unlocked(&ev);
    assert(ev.unlocked_techs[0] == CIV_TECH_WRITING);
    assert(ev.unlocked_techs[1] == CIV_TECH_MAP_MAKING);
    assert(ev.unlocked_techs[2] == CIV_TECH_MATHEMATICS);
    /* alphabet 不解鎖 unit / improvement */
    assert(ev.unlocked_units[0] == 0);
    assert(ev.unlocked_imp[0]   == 0);
    printf("  test_fill_unlocked_alphabet PASS\n");
}

static void test_fill_unlocked_writing(void)
{
    civ_tech_discovery_event_t ev = {0};
    ev.tech_id = CIV_TECH_WRITING;
    civ_tech_discovery_fill_unlocked(&ev);
    assert(ev.unlocked_techs[0] == CIV_TECH_LITERACY);
    /* Library improvement idx 6 (對齊 city.c BUILDING_INFO[6]) */
    assert(ev.unlocked_imp[0] == 6);
    printf("  test_fill_unlocked_writing PASS\n");
}

static void test_fill_unlocked_unknown_tech(void)
{
    /* 沒寫 prereq 的 tech, 解鎖陣列全 0 */
    civ_tech_discovery_event_t ev = {0};
    ev.tech_id = CIV_TECH_HORSEBACK_RIDING;
    civ_tech_discovery_fill_unlocked(&ev);
    assert(ev.unlocked_techs[0] == 0);
    assert(ev.unlocked_units[0] == 0);
    assert(ev.unlocked_imp[0]   == 0);
    assert(ev.unlocked_wonder[0] == 0);
    printf("  test_fill_unlocked_unknown_tech PASS\n");
}

int main(void)
{
    printf("test_tech\n");
    test_tech_name_zh();
    test_subtitle();
    test_source_phrase();
    test_fill_unlocked_bronze_working();
    test_fill_unlocked_alphabet();
    test_fill_unlocked_writing();
    test_fill_unlocked_unknown_tech();
    printf("ALL PASS\n");
    return 0;
}
