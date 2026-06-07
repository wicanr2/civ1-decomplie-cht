#include "government.h"

/* R26-C: 6 政府 zh-TW (對齊 spec 06 §6.3 + civ_dict.c GOVERNMENT[]).
 * idx = enum 值 (0..5). */
static const char *const NAME_ZH[CIV_GOVERNMENT_COUNT] = {
    [CIV_GOVERNMENT_ANARCHY]   = "無政府",
    [CIV_GOVERNMENT_DESPOTISM] = "專制",
    [CIV_GOVERNMENT_MONARCHY]  = "君主制",
    [CIV_GOVERNMENT_COMMUNISM] = "共產",
    [CIV_GOVERNMENT_REPUBLIC]  = "共和",
    [CIV_GOVERNMENT_DEMOCRACY] = "民主",
};

const char *civ_government_name_zh(int gov)
{
    if (gov < 0 || gov >= CIV_GOVERNMENT_COUNT) return "";
    return NAME_ZH[gov];
}

bool civ_government_is_starting_choice(int gov)
{
    /* 過渡期 Anarchy 不算正式政府, 新局精靈排除 */
    return gov >= CIV_GOVERNMENT_DESPOTISM && gov <= CIV_GOVERNMENT_DEMOCRACY;
}
