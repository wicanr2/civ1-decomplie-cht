#include "city.h"

#include <string.h>

/*
 * spec 06 §6.2 25 building 完整 cost (shields) 表. 對齊 spec 06 §6.2.1.
 * idx 對齊 OpenCivOne `ImprovementEnum.cs`.
 */
static const struct {
    int cost;
    const char *zh;
} BUILDING_INFO[CIV_BUILDING_COUNT] = {
    [0]  = { 80,  "無" },          /* NONE (placeholder) */
    [1]  = { 200, "宮殿" },        /* Palace */
    [2]  = { 40,  "兵營" },        /* Barracks */
    [3]  = { 60,  "穀倉" },        /* Granary */
    [4]  = { 40,  "神廟" },        /* Temple */
    [5]  = { 80,  "市場" },        /* Marketplace */
    [6]  = { 80,  "圖書館" },      /* Library */
    [7]  = { 80,  "法院" },        /* Courthouse */
    [8]  = { 120, "城牆" },        /* City Walls */
    [9]  = { 120, "水道" },        /* Aqueduct */
    [10] = { 120, "銀行" },        /* Bank */
    [11] = { 160, "大教堂" },      /* Cathedral */
    [12] = { 160, "大學" },        /* University */
    [13] = { 160, "大眾運輸" },    /* Mass Transit */
    [14] = { 100, "競技場" },      /* Colosseum */
    [15] = { 200, "工廠" },        /* Factory */
    [16] = { 320, "製造廠" },      /* Mfg Plant */
    [17] = { 200, "SDI 防禦" },    /* SDI Defense */
    [18] = { 200, "回收中心" },    /* Recycling */
    [19] = { 160, "發電廠" },      /* Power Plant */
    [20] = { 240, "水力發電廠" },  /* Hydro Plant */
    [21] = { 160, "核能電廠" },    /* Nuclear Plant */
    [22] = { 80,  "太空船結構" },  /* SS Structural */
    [23] = { 160, "太空船元件" },  /* SS Component */
    [24] = { 320, "太空船模組" },  /* SS Module */
};

int civ_building_cost(int building_idx)
{
    if (building_idx < 0 || building_idx >= CIV_BUILDING_COUNT) return 0;
    return BUILDING_INFO[building_idx].cost;
}

const char *civ_building_name_zh(int building_idx)
{
    if (building_idx < 0 || building_idx >= CIV_BUILDING_COUNT) return "";
    return BUILDING_INFO[building_idx].zh;
}
