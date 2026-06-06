/*
 * test_strings.c — M4-lite 驗證
 *
 * 對 Civdata0.RSC 的 STR# 130 (Civ Advances) 驗 72 entries、
 * STR# 132 (People) 驗 28 entries、STR# 140 (Leaders) 驗 16 entries。
 * 對 STR# 137 (Credits) 驗 1993 Win port 開發團隊：SID MEIER /
 * BRUCE SHELLEY / PAUL L. ROWAN / WILLIAM F. DENMAN, JR.。
 *
 * CIV1_DATA_DIR 未設則 SKIP。
 */
#include "data/civs.h"
#include "data/strings.h"
#include "res/rsrcfork.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EXPECT(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL %s:%d  %s\n", __FILE__, __LINE__, #cond); \
        exit(1); \
    } \
} while (0)

#define FOURCC_STR_HASH CIV_FOURCC('S','T','R','#')

static int parse_strid(civ_rsrc_t *r, int16_t id, civ_strlist_t *out)
{
    const civ_rsrc_entry_t *e = civ_rsrc_find(r, FOURCC_STR_HASH, id);
    if (!e) return -1;
    return civ_strlist_parse(e->data, e->len, id, out);
}

int main(void)
{
    const char *data_dir = getenv("CIV1_DATA_DIR");
    if (!data_dir || !*data_dir) {
        printf("SKIP test_strings（CIV1_DATA_DIR 未設）\n");
        return 0;
    }

    char path[1024];
    snprintf(path, sizeof path, "%s/Civdata0.RSC", data_dir);
    civ_rsrc_t *r = civ_rsrc_open(path);
    if (!r) {
        snprintf(path, sizeof path, "%s/CIVDATA0.RSC", data_dir);
        r = civ_rsrc_open(path);
    }
    if (!r) {
        printf("SKIP test_strings（找不到 Civdata0.RSC）\n");
        return 0;
    }

    civ_strlist_t tech = {0};
    EXPECT(parse_strid(r, CIV_STR_TECH_ADVANCES, &tech) == 0);
    EXPECT(tech.count == 72);
    EXPECT(strcmp(civ_strlist_get(&tech, 0), "Alphabet") == 0);
    printf("STR# 130 Civ Advances: %d 條，[0]=%s [4]=%s\n",
           tech.count, civ_strlist_get(&tech, 0), civ_strlist_get(&tech, 4));
    civ_strlist_free(&tech);

    civ_strlist_t units = {0};
    EXPECT(parse_strid(r, CIV_STR_PEOPLE_UNITS, &units) == 0);
    EXPECT(units.count == 28);
    EXPECT(strcmp(civ_strlist_get(&units, 0), "Settlers") == 0);
    printf("STR# 132 People: %d 條，[0]=%s [4]=%s\n",
           units.count, civ_strlist_get(&units, 0), civ_strlist_get(&units, 4));
    civ_strlist_free(&units);

    civ_strlist_t leaders = {0};
    EXPECT(parse_strid(r, CIV_STR_LEADERS, &leaders) == 0);
    EXPECT(leaders.count == 16);
    /* slot 0 = empty; slot 1 = Caesar */
    EXPECT(strcmp(civ_strlist_get(&leaders, 1), "Caesar") == 0);
    EXPECT(strcmp(civ_strlist_get(&leaders, 2), "Hammurabi") == 0);
    /* spec 05 §5.3 揭穿: slot 4 是 Ramesses 不是 Cleopatra */
    EXPECT(strcmp(civ_strlist_get(&leaders, 4), "Ramesses") == 0);
    printf("STR# 140 Leaders: %d 條（slot 1=Caesar / 2=Hammurabi / 4=Ramesses ✓）\n",
           leaders.count);

    civ_strlist_t credits = {0};
    EXPECT(parse_strid(r, CIV_STR_CREDITS, &credits) == 0);
    /* spec 05 §5.3 confirms: SID MEIER + BRUCE SHELLEY + PAUL L. ROWAN
     * + WILLIAM F. DENMAN, JR. */
    EXPECT(strstr(civ_strlist_get(&credits, 1), "SID MEIER") != NULL);
    EXPECT(strstr(civ_strlist_get(&credits, 1), "BRUCE SHELLEY") != NULL);
    EXPECT(strcmp(civ_strlist_get(&credits, 3), "PAUL L. ROWAN") == 0);
    EXPECT(strcmp(civ_strlist_get(&credits, 4), "WILLIAM F. DENMAN, JR.") == 0);
    printf("STR# 137 Credits: 1993 Windows port team verified\n");
    civ_strlist_free(&credits);

    /* 構建 14 文明 master table */
    civ_strlist_t army_sing = {0}, army_plur = {0}, civ_sing = {0}, civ_plur = {0};
    EXPECT(parse_strid(r, CIV_STR_ARMY_SING,        &army_sing) == 0);
    EXPECT(parse_strid(r, CIV_STR_ARMY_PLUR,        &army_plur) == 0);
    EXPECT(parse_strid(r, CIV_STR_LEADERS_CIV_SING, &civ_sing)  == 0);
    EXPECT(parse_strid(r, CIV_STR_LEADERS_CIV_PLUR, &civ_plur)  == 0);

    civ_civ_entry_t civs[CIV_NUM_CIVS] = {0};
    EXPECT(civ_civs_build(&leaders, &army_sing, &army_plur,
                          &civ_sing, &civ_plur, civs) == 0);

    printf("14 文明 master table:\n");
    for (int i = 0; i < CIV_NUM_CIVS; i++) {
        printf("  slot=%2d  %-12s  %-12s  zh=%s/%s\n",
               civs[i].slot,
               civs[i].leader_en   ? civs[i].leader_en   : "?",
               civs[i].civ_sing_en ? civs[i].civ_sing_en : "?",
               civ_civs_zh(civs[i].slot, "leader"),
               civ_civs_zh(civs[i].slot, "civ_sing"));
    }

    /* slot 1 = 凱撒/羅馬 確認 */
    EXPECT(strcmp(civ_civs_zh(1, "leader"),   "凱撒") == 0);
    EXPECT(strcmp(civ_civs_zh(1, "civ_sing"), "羅馬") == 0);
    /* slot 4 = 拉美西斯/埃及（不是 Cleopatra）*/
    EXPECT(strcmp(civ_civs_zh(4, "leader"),   "拉美西斯") == 0);
    EXPECT(strcmp(civ_civs_zh(4, "civ_sing"), "埃及") == 0);

    civ_strlist_free(&leaders);
    civ_strlist_free(&army_sing);
    civ_strlist_free(&army_plur);
    civ_strlist_free(&civ_sing);
    civ_strlist_free(&civ_plur);
    civ_rsrc_close(r);
    printf("PASS test_strings\n");
    return 0;
}
