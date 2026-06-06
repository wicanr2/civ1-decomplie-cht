/*
 * civ_dict.c — 376 條 zh-TW 翻譯 hard-coded catalog
 *
 * 來源：spec 05 §5.3 + extract_strings.py 對 Civdata0.RSC 的完整 dump。
 * 編碼：UTF-8（spec 02 編碼總則）。
 *
 * 註：部分 STR# 有重複條目（地形 / 君主稱號），翻譯尊重原順序不去重。
 *
 * Round 2026-06-06 D-#3: 補 11 個 STR# 共 135 條 — Tax/Lux rate 格式、Misc
 * Prompts (UI 詞)、Leaders (16) + Army Sing/Plur (8+8) + King Text (8) +
 * Leaders Civ Sing/Plur (16+16) + Input Titles (3) + Orders Menu (19) +
 * Alert Text (10)。
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

/* ── STR# 128 — 11 Tax Rates（稅率格式字串） ──────────────── */
static const char *const TAX_RATES[11] = {
    "    0% 稅率, (100% 科技)",
    "  10% 稅率, (  90% 科技)",
    "  20% 稅率, (  80% 科技)",
    "  30% 稅率, (  70% 科技)",
    "  40% 稅率, (  60% 科技)",
    "  50% 稅率, (  50% 科技)",
    "  60% 稅率, (  40% 科技)",
    "  70% 稅率, (  30% 科技)",
    "  80% 稅率, (  20% 科技)",
    "  90% 稅率, (  10% 科技)",
    "100% 稅率, (    0% 科技)",
};

/* ── STR# 129 — 6 Lux Rates（奢侈率格式字串） ─────────────── */
static const char *const LUX_RATES[6] = {
    "  0% 奢侈品, (50% 科技)",
    "10% 奢侈品, (40% 科技)",
    "20% 奢侈品, (30% 科技)",
    "30% 奢侈品, (20% 科技)",
    "40% 奢侈品, (10% 科技)",
    "50% 奢侈品, (  0% 科技)",
};

/* ── STR# 136 — 14 Misc Prompts（UI 詞彙） ────────────────── */
static const char *const MISC_PROMPTS[14] = {
    "遊戲設定", "請命名這場遊戲:", "存檔 1",
    "顯示", "隱藏",
    "地圖", "世界地圖", "狀態",
    "購買建築", "購買奇蹟", "購買單位",
    "名人堂",
    "隱藏單位", "顯示單位",
};

/* ── STR# 140 — 16 Leaders（領袖名,含 2 個 NONE 空 slot） ── */
static const char *const LEADERS[16] = {
    "",                /* slot 0: empty */
    "凱撒",            /* 1 Caesar */
    "漢摩拉比",        /* 2 Hammurabi */
    "腓特烈",          /* 3 Frederick */
    "拉美西斯",        /* 4 Ramesses */
    "林肯",            /* 5 Abe Lincoln */
    "亞歷山大",        /* 6 Alexander */
    "甘地",            /* 7 M.Gandhi */
    "",                /* 8: empty (no Mongol slot in 1993 Win port) */
    "史達林",          /* 9 Stalin */
    "夏卡",            /* 10 Shaka */
    "拿破崙",          /* 11 Napoleon */
    "蒙特蘇馬",        /* 12 Montezuma */
    "毛澤東",          /* 13 Mao Tse Tung */
    "伊莉莎白一世",    /* 14 Elizabeth I */
    "成吉思汗",        /* 15 Genghis Khan (extra) */
};

/* ── STR# 143 — 8 Army Singular（8 player slot 文明形容詞） ─ */
static const char *const ARMY_SING[8] = {
    "野蠻人", "羅馬", "巴比倫", "德意志",
    "埃及", "美利堅", "希臘", "蒙古",
};

/* ── STR# 144 — 8 Armies Plural ─────────────────────────── */
static const char *const ARMY_PLUR[8] = {
    "野蠻人", "羅馬人", "巴比倫人", "德意志人",
    "埃及人", "美國人", "希臘人", "蒙古人",
};

/* ── STR# 145 — 8 King Text（歷史名君引用） ─────────────── */
static const char *const KING_TEXT[8] = {
    "阿提拉",            /* Attila */
    "凱撒",              /* Caesar */
    "夏卡",              /* Shaka */
    "成吉思汗",          /* Genghis Khan */
    "蒙特蘇馬",          /* Montezuma */
    "薛西斯",            /* Xerxes */
    "薩拉丁",            /* Saladin */
    "蘇皮盧利烏馬斯",    /* Suppiluliumas */
};

/* ── STR# 148 — 16 Leaders Civ Singular（slot 文明名 16 slot 版） ── */
static const char *const LEADERS_CIV_SING[16] = {
    "無",      /* 0 NONE */
    "羅馬",    /* 1 Roman */
    "巴比倫",  /* 2 Babylonian */
    "德意志",  /* 3 German */
    "埃及",    /* 4 Egyptian */
    "美利堅",  /* 5 American */
    "希臘",    /* 6 Greek */
    "印度",    /* 7 Indian */
    "無",      /* 8 NONE */
    "俄羅斯",  /* 9 Russian */
    "祖魯",    /* 10 Zulu */
    "法蘭西",  /* 11 French */
    "阿茲特克",/* 12 Aztec */
    "中華",    /* 13 Chinese */
    "英格蘭",  /* 14 English */
    "蒙古",    /* 15 Mongol */
};

/* ── STR# 149 — 16 Leaders Civ Plural ─────────────────────── */
static const char *const LEADERS_CIV_PLUR[16] = {
    "無", "羅馬人", "巴比倫人", "德意志人",
    "埃及人", "美國人", "希臘人", "印度人",
    "無", "俄羅斯人", "祖魯人", "法國人",
    "阿茲特克人", "中國人", "英國人", "蒙古人",
};

/* ── STR# 151 — 3 Input Titles ──────────────────────────── */
static const char *const INPUT_TITLES[3] = {
    "城市名稱...", "您的名字...", "您的部族名稱...",
};

/* ── STR# 152 — 19 Orders Menu（&X 為 Win16 mnemonic） ────── */
static const char *const ORDERS_MENU[19] = {
    "無命令",
    "加入城市(&A)",
    "建立新城市(&F)",
    "建造道路(&R)",
    "建造鐵路(&R)",
    "建造灌溉(&I)",
    "改變為 (&C)",
    "建造礦坑(&M)",
    "清除污染",
    "建造堡壘(&F)",
    "駐守(&F)",
    "等候",
    "哨兵",
    "前往",
    "掠奪",
    "母城",
    "卸載",
    "解散單位",
    " brimpfwsgPhu ct D",  /* 原 slot 18 為 garbage,保留 raw 不譯 */
};

/* ── STR# 158 — 10 Alert Text（錯誤訊息） ──────────────────── */
static const char *const ALERT_TEXT[10] = {
    "遊戲未存檔。",
    "選擇的磁碟已滿。",
    "選擇的磁碟已鎖定。",
    "發生未預期的 I/O 錯誤。",
    "Civilization™ 只支援 8 位元色彩 (256 色) 與黑白。",
    "您目前處於 4 位元模式 (16 色)。將以黑白顯示。",
    "您目前處於 16 位元模式 (65,536 色)。",
    "您目前處於 24 位元模式 (16,777,216 色)。",
    "您目前處於未知顯示模式。",
    "您目前處於 2 位元模式 (4 色)。將以黑白顯示。",
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
    LOOKUP(CIV_STR_TAX_RATES,         TAX_RATES);
    LOOKUP(CIV_STR_LUX_RATES,         LUX_RATES);
    LOOKUP(CIV_STR_TECH_ADVANCES,     TECH);
    LOOKUP(CIV_STR_IMPROVEMENTS,      IMPROVEMENT);
    LOOKUP(CIV_STR_PEOPLE_UNITS,      UNIT);
    LOOKUP(CIV_STR_TERRAINS,          TERRAIN);
    LOOKUP(CIV_STR_MISC,              MISC);
    LOOKUP(CIV_STR_MISC_PROMPTS,      MISC_PROMPTS);
    LOOKUP(CIV_STR_REPORT_TITLES,     REPORT);
    LOOKUP(CIV_STR_LEADERS,           LEADERS);
    LOOKUP(CIV_STR_GOVERNMENTS,       GOV);
    LOOKUP(CIV_STR_CARAVAN_GOODS,     GOOD);
    LOOKUP(CIV_STR_ARMY_SING,         ARMY_SING);
    LOOKUP(CIV_STR_ARMY_PLUR,         ARMY_PLUR);
    LOOKUP(CIV_STR_KING_TEXT,         KING_TEXT);
    LOOKUP(CIV_STR_KING_TITLE,        KING_TITLE);
    LOOKUP(CIV_STR_CIVPEDIA_SUBTITLE, PEDIA_SUB);
    LOOKUP(CIV_STR_LEADERS_CIV_SING,  LEADERS_CIV_SING);
    LOOKUP(CIV_STR_LEADERS_CIV_PLUR,  LEADERS_CIV_PLUR);
    LOOKUP(CIV_STR_DOCK,              DOCK);
    LOOKUP(CIV_STR_INPUT_TITLES,      INPUT_TITLES);
    LOOKUP(CIV_STR_ORDERS_MENU,       ORDERS_MENU);
    LOOKUP(CIV_STR_ALERT_TEXT,        ALERT_TEXT);
    default:
        return NULL;
    }
#undef LOOKUP
}
