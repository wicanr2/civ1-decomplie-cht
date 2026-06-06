/*
 * civ_dict.c — 241 條 zh-TW 翻譯 hard-coded catalog
 *
 * 來源：spec 05 §5.3 + extract_strings.py 對 Civdata0.RSC 的完整 dump。
 * 編碼：UTF-8（spec 02 編碼總則）。
 *
 * 註：部分 STR# 有重複條目（地形 / 君主稱號），翻譯尊重原順序不去重。
 */
#include "civ_dict.h"

#include <string.h>

/* ── STR# 130 — 72 Civ Advances（科技樹） ─────────────────── */
static const char *const TECH[72] = {
    "字母", "法典", "貨幣", "原子理論", "民主政體", "君主政體",
    "天文學", "地圖學", "航海術", "數學", "醫學", "物理學",
    "工程學", "大學", "磁學", "電子學", "砌築", "青銅器",
    "鐵器", "橋樑建築", "發明", "電腦", "文字", "蒸汽機",
    "貿易", "禮葬", "神秘學", "核分裂", "哲學", "宗教",
    "識字", "騎馬", "封建制度", "輪", "火藥", "工業化",
    "化學", "內燃機", "飛行", "進階飛行", "太空飛行", "大量生產",
    "陶器", "共產主義", "共和政體", "建築工程", "火箭學", "公司",
    "冶金學", "鐵路", "核能", "萬有引力", "鋼鐵", "銀行業",
    "電力", "精煉", "炸藥", "超導體", "汽車", "基因工程",
    "塑膠", "資源回收", "騎士精神", "機器人學", "徵兵制", "工會",
    "融合動力", "1", "2", "3", "4", "未來科技",
};

/* ── STR# 131 — 46 Improvements & Wonders（建築 + 奇蹟） ─────── */
static const char *const IMPROVEMENT[46] = {
    "無", "宮殿", "兵營", "穀倉", "神廟", "市場",
    "圖書館", "法院", "城牆", "水道", "銀行", "大教堂",
    "大學", "大眾運輸", "競技場", "工廠", "製造廠", "SDI 防禦",
    "回收中心", "發電廠", "水力發電廠", "核能電廠",
    "太空船結構", "太空船元件", "太空船模組",
    "金字塔", "空中花園", "巨像", "燈塔", "大圖書館", "神諭",
    "萬里長城", "麥哲倫的遠征", "米開朗基羅教堂", "哥白尼天文台",
    "莎士比亞劇場", "牛頓學院", "巴哈大教堂", "達爾文之旅",
    "胡佛水壩", "女性投票權", "曼哈頓計畫", "聯合國",
    "阿波羅計畫", "SETI 計畫", "癌症療法",
};

/* ── STR# 132 — 28 People（單位） ──────────────────────────── */
static const char *const UNIT[28] = {
    "墾荒者", "民兵", "方陣兵", "軍團兵", "火槍兵", "步兵",
    "騎兵", "騎士", "投石機", "加農炮", "戰車", "裝甲車",
    "機械化步兵", "炮兵", "戰鬥機", "轟炸機",
    "三層槳座戰船", "帆船", "巡防艦", "鐵甲艦",
    "巡洋艦", "戰艦", "潛艇", "航空母艦", "運輸船",
    "核彈", "外交官", "商隊",
};

/* ── STR# 133 — 24 Terrains（地形 + 特殊資源） ─────────────── */
static const char *const TERRAIN[24] = {
    "沙漠", "平原", "草原", "森林", "丘陵", "山脈",
    "凍原", "北極", "沼澤", "叢林", "海洋", "河流",
    "綠洲", "馬", "草原",         /* 14 = grassland 變體 */
    "獵物", "煤", "黃金",
    "獵物",                        /* 18 = duplicate */
    "海豹", "石油", "寶石", "魚",
    "河流",                        /* 23 = duplicate */
};

/* ── STR# 134 — 21 Miscellaneous（其他 UI 詞彙） ──────────── */
static const char *const MISC[21] = {
    "老兵單位", "駐守", "灌溉", "採礦", "道路", "鐵路",
    "無政府", "專制", "君主制", "共產政府", "共和制", "民主制",
    "堡壘", "掠奪", "解散", "污染", "哨兵",
    "奢侈品", "稅收", "科學", "貿易路線",
};

/* ── STR# 138 — 7 Report Titles（報告分類） ────────────────── */
static const char *const REPORT[7] = {
    "城市狀態", "軍事狀態", "情報報告", "態度調查",
    "貿易報告", "科學報告", "軍事損失",
};

/* ── STR# 141 — 6 Governments（政府型態） ──────────────────── */
static const char *const GOV[6] = {
    "無政府", "專制", "君主", "共產", "共和", "民主",
};

/* ── STR# 142 — 8 Caravan Goods（商品） ────────────────────── */
static const char *const GOOD[8] = {
    "絲綢", "銀", "葡萄酒", "銅", "寶石", "染料", "鹽", "香料",
};

/* ── STR# 146 — 13 King Title（君主稱號） ─────────────────── */
static const char *const KING_TITLE[13] = {
    "先生", "皇帝", "國王", "同志", "總統", "總統",
    "女皇", "女王", "酋長", "主公", "王子", "國王", "皇帝",
};

/* ── STR# 147 — 6 CivPedia Subtitle ─────────────────────── */
static const char *const PEDIA_SUB[6] = {
    "文明進展", "城市建築", "世界奇蹟", "軍事單位", "地形種類", "遊戲概念",
};

/* ── STR# 150 — 10 Dock（太空船零件，Apollo Program） ─────── */
static const char *const DOCK[10] = {
    "結構 1", "結構 2", "結構 3", "推進", "燃料",
    "無", "居住", "維生", "太陽能板", "支援",
};

/* ── 主 lookup ──────────────────────────────────────── */

const char *civ_dict_lookup(int str_id, int index)
{
    if (index < 0) return NULL;

#define LOOKUP(id, arr) \
    case id: \
        if (index < (int)(sizeof(arr) / sizeof((arr)[0]))) return (arr)[index]; \
        return NULL;

    switch (str_id) {
    LOOKUP(CIV_STR_TECH_ADVANCES,     TECH);
    LOOKUP(CIV_STR_IMPROVEMENTS,      IMPROVEMENT);
    LOOKUP(CIV_STR_PEOPLE_UNITS,      UNIT);
    LOOKUP(CIV_STR_TERRAINS,          TERRAIN);
    LOOKUP(CIV_STR_MISC,              MISC);
    LOOKUP(CIV_STR_REPORT_TITLES,     REPORT);
    LOOKUP(CIV_STR_GOVERNMENTS,       GOV);
    LOOKUP(CIV_STR_CARAVAN_GOODS,     GOOD);
    LOOKUP(CIV_STR_KING_TITLE,        KING_TITLE);
    LOOKUP(CIV_STR_CIVPEDIA_SUBTITLE, PEDIA_SUB);
    LOOKUP(CIV_STR_DOCK,              DOCK);
    default:
        return NULL;
    }
#undef LOOKUP
}
