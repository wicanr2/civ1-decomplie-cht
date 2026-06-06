#include "civs.h"

#include <string.h>

/* M4-lite hardcoded zh-TW catalog。完整版見 team-b/assets/zh_TW/civs.json
 * （未實作 JSON 載入器 — M4-full 時補）。
 *
 * 實際 STR# 140 順序（test_strings 跑過 Civdata0 驗證）：
 *   slot 1=Caesar 2=Hammurabi 3=Frederick 4=Ramesses 5=Abe Lincoln
 *   6=Alexander 7=M.Gandhi (印度!) 8=(NONE 空 slot) 9=Stalin 10=Shaka
 *   11=Napoleon 12=Montezuma 13=Mao Tse Tung 14=Elizabeth I
 *
 * **重大發現**：1993 Windows 版**沒有蒙古**（Fandom 主頁列 14 文明含
 * Mongol 是錯的，至少對 Win port 而言）。實際 13 文明 + 1 NONE slot。
 * 修正後的 zh-TW catalog 對應 actual order。
 */
typedef struct {
    int         slot;
    const char *leader;
    const char *civ_sing;
    const char *civ_plur;
    const char *adj_sing;
    const char *adj_plur;
} zh_entry_t;

static const zh_entry_t ZH[] = {
    /* slot, 領袖,     文明,     文明複數,   形容詞,   複數形容詞 */
    {  1, "凱撒",       "羅馬",     "羅馬人",   "羅馬",     "羅馬人" },
    {  2, "漢摩拉比",   "巴比倫",   "巴比倫人", "巴比倫",   "巴比倫人" },
    {  3, "腓特烈",     "德意志",   "德意志人", "德意志",   "德意志人" },
    {  4, "拉美西斯",   "埃及",     "埃及人",   "埃及",     "埃及人" },
    {  5, "林肯",       "美利堅",   "美國人",   "美利堅",   "美國人" },
    {  6, "亞歷山大",   "希臘",     "希臘人",   "希臘",     "希臘人" },
    {  7, "甘地",       "印度",     "印度人",   "印度",     "印度人" },
    {  8, "(空)",       "NONE",     "NONE",     "NONE",     "NONE" },
    {  9, "史達林",     "俄羅斯",   "俄羅斯人", "俄羅斯",   "俄羅斯人" },
    { 10, "夏卡",       "祖魯",     "祖魯人",   "祖魯",     "祖魯人" },
    { 11, "拿破崙",     "法蘭西",   "法國人",   "法蘭西",   "法國人" },
    { 12, "蒙特蘇馬",   "阿茲特克", "阿茲特克人","阿茲特克", "阿茲特克人" },
    { 13, "毛澤東",     "中華",     "中國人",   "中華",     "中國人" },
    { 14, "伊莉莎白",   "英格蘭",   "英國人",   "英格蘭",   "英國人" },
};

int civ_civs_build(const civ_strlist_t *leaders,
                   const civ_strlist_t *army_sing,
                   const civ_strlist_t *army_plur,
                   const civ_strlist_t *civ_sing,
                   const civ_strlist_t *civ_plur,
                   civ_civ_entry_t      out[CIV_NUM_CIVS])
{
    if (!out) return -1;
    for (int i = 0; i < CIV_NUM_CIVS; i++) {
        int slot = i + 1;          /* slot 1..14 */
        out[i].slot              = slot;
        out[i].leader_en         = leaders   ? civ_strlist_get(leaders,   slot) : NULL;
        out[i].adj_sing_en       = army_sing ? civ_strlist_get(army_sing, slot) : NULL;
        out[i].adj_plur_en       = army_plur ? civ_strlist_get(army_plur, slot) : NULL;
        out[i].civ_sing_en       = civ_sing  ? civ_strlist_get(civ_sing,  slot) : NULL;
        out[i].civ_plur_en       = civ_plur  ? civ_strlist_get(civ_plur,  slot) : NULL;
        out[i].king_sprite_idx   = i;        /* KING00..13 對應 slot 1..14 */
    }
    return 0;
}

const char *civ_civs_zh(int slot, const char *which)
{
    if (slot < 1 || slot > CIV_NUM_CIVS || !which) return NULL;
    const zh_entry_t *e = &ZH[slot - 1];
    if (strcmp(which, "leader")   == 0) return e->leader;
    if (strcmp(which, "civ_sing") == 0) return e->civ_sing;
    if (strcmp(which, "civ_plur") == 0) return e->civ_plur;
    if (strcmp(which, "adj_sing") == 0) return e->adj_sing;
    if (strcmp(which, "adj_plur") == 0) return e->adj_plur;
    return NULL;
}
