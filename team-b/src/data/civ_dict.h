/*
 * data/civ_dict.h — 完整 zh-TW 翻譯 catalog（M4-lite 第二波）
 *
 * 對應 spec 05 §5.3 的 STR# 130–150 內全部字串：
 *   STR# 130 (72 科技) / 131 (46 建築+奇蹟) / 132 (28 單位) /
 *   133 (24 地形) / 134 (21 其他) / 138 (7 報告) / 141 (6 政府) /
 *   142 (8 商品) / 146 (13 君主稱號) / 147 (6 Civilopedia 大類) /
 *   150 (10 太空船零件)
 *
 * 共 241 條翻譯，加上 [civs.c](civs.c) 內 14 文明 × 5 欄 = 311 條總計。
 *
 * 使用：
 *   const char *zh = civ_dict_lookup(CIV_STR_TECH_ADVANCES, 0);
 *   // -> "字母"
 */
#ifndef CIV_DATA_CIV_DICT_H
#define CIV_DATA_CIV_DICT_H

#include "strings.h"

/* 對指定 STR# id 與索引取得 zh-TW 翻譯。找不到回 NULL（caller 應
 * fallback 到 STR# 原文）。 */
const char *civ_dict_lookup(int str_id, int index);

#endif /* CIV_DATA_CIV_DICT_H */
