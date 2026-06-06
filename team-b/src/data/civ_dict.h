/*
 * data/civ_dict.h — 完整 zh-TW 翻譯 catalog
 *
 * 對應 spec 05 §5.3 的 STR# 128–158 內主要字串(共 23 個 STR# 表覆蓋):
 *   STR# 128 (11 稅率) / 129 (6 奢侈率) /
 *   STR# 130 (72 科技) / 131 (46 建築+奇蹟) / 132 (28 單位) /
 *   STR# 133 (24 地形) / 134 (21 其他) / 136 (14 UI 提示) /
 *   STR# 138 (7 報告) / 140 (16 領袖) / 141 (6 政府) / 142 (8 商品) /
 *   STR# 143 (8 軍 sing) / 144 (8 軍 plur) / 145 (8 名君) /
 *   STR# 146 (13 君主稱號) / 147 (6 Civilopedia 大類) /
 *   STR# 148 (16 文明 sing) / 149 (16 文明 plur) /
 *   STR# 150 (10 太空船零件) / 151 (3 輸入標題) /
 *   STR# 152 (19 Orders menu) / 158 (10 錯誤訊息)
 *
 * D-#3 第二波 (2026-06-06): 376 條翻譯 + civs.c 14 文明 × 5 欄 = 446 條總計
 *                          佔 Civdata0 約 731 條的 ~52%
 *
 * 仍未翻 (defer 下一輪):
 *   STR# 135 (256 城市名) / 137 (10 Credits, IP 保留英文) /
 *   STR# 139 (44 開場故事) / 153 (16 自訂世界) / 155-160 (太空+磁碟+結局)
 *
 * 使用:
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
