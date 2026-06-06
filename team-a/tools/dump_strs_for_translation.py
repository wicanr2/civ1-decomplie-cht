#!/usr/bin/env python3
"""列 STR# 內全部字串給翻譯規劃。"""
import json
import sys

data = json.load(open(sys.argv[1]))
str_hash = data["str_hash"]

# 要翻譯的 STR# id
targets = [
    (130, "Civ Advances - 72 科技"),
    (131, "Improvements & Wonders - 46 建築 + 奇蹟"),
    (132, "People - 28 單位"),
    (133, "Terrains - 24 地形"),
    (134, "Miscellaneous - 21 其他"),
    (138, "Report Titles - 7 報告分類"),
    (141, "Governments - 6 政府"),
    (142, "Caravan Goods - 8 商品"),
    (146, "King Title - 13 君主稱號"),
    (147, "CivPedia Subtitle - 6 Civilopedia 大類"),
    (150, "Dock - 10 太空船零件"),
]

for sid, desc in targets:
    s = str_hash.get(str(sid))
    if not s:
        print(f"# missing STR# {sid}")
        continue
    print(f"\n## STR# {sid}  {desc}  count={s['count']}")
    for i, item in enumerate(s["items"]):
        text = item["text"]
        if text == "":
            text = "(empty)"
        print(f"  [{i:3}] {text!r}")
