#!/usr/bin/env python3
"""
生成 docs/ASSETS_INDEX.md：把 assets-extracted/tiles/ 內 185 個 CvPc PNG
分類整理成可視化 markdown 表格。

順便產出 docs/screenshots/asset_thumbs/ 內 128×128 縮圖（保持 aspect）。
"""
import re
import sys
from pathlib import Path

try:
    from PIL import Image
except ImportError:
    print("PIL/Pillow 必要", file=sys.stderr)
    sys.exit(1)


# ---------------------------------------------------------------------------
# 路徑
# ---------------------------------------------------------------------------
ROOT       = Path(__file__).resolve().parent.parent.parent
ASSETS_DIR = ROOT / "assets-extracted" / "tiles"
THUMBS_DIR = ROOT / "assets-extracted" / "tiles" / "_thumbs"
INDEX_PATH = ROOT / "docs" / "ASSETS_INDEX.md"

THUMB_MAX = 96   # 縮圖最大邊長 (px)


# ---------------------------------------------------------------------------
# 分類規則
# ---------------------------------------------------------------------------
# (順序：先試 exact match，再 prefix，再 fallback)

LEADERS = [f"KING{i:02d}" for i in range(14)]

TECH_ICONS = set("""
ADVFLGHT ALPHABET ASTRONOM ATOMIC AUTO BANKING BRIDGE BRONZE BURIAL
CHEMISTR CHIVALRY CODELAW COMBUSTI COMPUTER CONSCRIP CONSTRUC CORPORAT
CURRENCY DEMOCRAC ELECTRIC ELECTRON ENGINEER EXPLOSIV FEUDAL FISSION
FLIGHT FUSION GENETIC GRAVITY GUNPOWDE INDUSTRI INVENTIO IRON LITERACY
MAGNET MASONRY MASSPROD MATH MEDICINE METALLUR MONARCHY MYSTIC NAVIGATI
NUCLEAR PHILOSOP PHYSICS PLASTIC POP RAILROAD RECYCLE REFINING RELIGION
REPUBLIC ROBOT ROCKETRY SAIL SPACE STEAM STEEL SUPERCON TRADE UNION
UNIVERSI WHEEL WRITING COMMIE
""".split())

UNITS = set("""
ARMOR ARTILLER BATTLESH BOMBER CANNON CARAVAN CARRIER CATAPULT CAVALRY
CHARIOT CRUISER DIPLOMAT FIGHTER FRIGATE HORSE INVADER2 INVADER3 INVADERS
IRONCLAD KNIGHT LEGION MECHINF MILITIA MUSKETEE NUKE1 PHALANX RIFLEMEN
SETTLERS SUBMARIN TRANSPOR TRIREME
""".split())

WONDERS_BUILDINGS = set("""
ARCH WONDERS HOUSE HILL CITYPIX2 CITYPIX3
""".split())

GOVERNMENTS = set("""
GOVT0M GOVT1M GOVT2M
""".split())

SPACE = set("""
SPACE SPACEST
""".split())

PLANETS = set(f"planet{i}" for i in range(10))

CASTLES = set(f"castle{r}{c}" for r in range(5) for c in "abc")

ROADS = {"ROAD", "road0", "road1", "road2", "road3", "BRIDGE_ROAD"}

DECORATIONS = set("""
leftbush lefttree leftstatue rightbush righttree rightstatue
sky0 sky1 starrynight basestrip pottery
""".split())

INTRO_ANIM = set("""
Small_Birth_2 Small_Birth_3 Small_Birth_4 Small_Birth_5
Small_Birth_6 Small_Birth_7 Small_Birth_8
""".split())

UI_SPECIAL = set("""
DIFFS LOVE1 LOVE2 RIOT RIOT2 ICONPGT1 ICONPGT2 fame slamall MAP
""".split())

SPECIAL = set("""
EARTH SPY DOCKER SPR32X32 discovr1 discovr2
""".split())


def classify(name):
    """回傳 (大分類, 子分類)"""
    if name in LEADERS:
        return ("Leaders", "領袖肖像 (14 文明)")
    if name in LEADERS:
        return ("Leaders", "領袖肖像")
    if name in TECH_ICONS:
        return ("Technology", "科技 icon")
    if name in UNITS:
        return ("Units", "單位 sprite")
    if name in WONDERS_BUILDINGS:
        return ("Wonders & Buildings", "建築 / 奇蹟")
    if name in GOVERNMENTS:
        return ("Government", "政府型態")
    if name in PLANETS:
        return ("Space race", "Planet sprites")
    if name in SPACE:
        return ("Space race", "太空")
    if name in CASTLES:
        return ("Animations", "City castle 動畫 (5×3)")
    if name in ROADS:
        return ("World tiles", "Road / 道路")
    if name in DECORATIONS:
        return ("World tiles", "裝飾元素 (城牆 / 樹 / 雕像 / 天空)")
    if name in INTRO_ANIM:
        return ("Animations", "文明誕生開場 (Small_Birth)")
    if name in UI_SPECIAL:
        return ("UI", "對話 / 選單 / 狀態")
    if name in SPECIAL:
        return ("Special", "特殊 (主 sprite sheet / 工具 / 截圖)")
    return ("Other", "未分類")


def make_thumb(src_path, dst_path):
    """生成縮圖，回傳 (thumb_w, thumb_h)"""
    if dst_path.exists() and dst_path.stat().st_mtime >= src_path.stat().st_mtime:
        with Image.open(dst_path) as im:
            return im.size
    im = Image.open(src_path).convert("RGBA")
    # 把預設 magenta (255,0,255) 變透明，方便觀察 sprite 形狀
    px = im.load()
    for y in range(im.size[1]):
        for x in range(im.size[0]):
            r, g, b, a = px[x, y]
            if r == 255 and g == 0 and b == 255:
                px[x, y] = (255, 0, 255, 0)
    im.thumbnail((THUMB_MAX, THUMB_MAX), Image.NEAREST)
    dst_path.parent.mkdir(parents=True, exist_ok=True)
    im.save(dst_path)
    return im.size


CATEGORY_ORDER = [
    "Leaders",
    "Units",
    "Technology",
    "Wonders & Buildings",
    "Government",
    "Space race",
    "World tiles",
    "Animations",
    "UI",
    "Special",
    "Other",
]


def main():
    if not ASSETS_DIR.exists():
        print(f"找不到 {ASSETS_DIR}", file=sys.stderr)
        return 1

    png_files = sorted(ASSETS_DIR.glob("*.png"))
    print(f"找到 {len(png_files)} 個 PNG")

    # name -> [(rsc_stem, rid, png_path, w, h)]
    items_by_name = {}
    for p in png_files:
        # 檔名格式：<RSC_stem>_<id>_<name>.png
        m = re.match(r"^(\w+)_(\d+)_(.+)$", p.stem)
        if not m:
            print(f"  skip {p.name}")
            continue
        rsc, rid, name = m.group(1), int(m.group(2)), m.group(3)
        with Image.open(p) as im:
            w, h = im.size
        items_by_name.setdefault(name, []).append((rsc, rid, p, w, h))

    # 縮圖
    THUMBS_DIR.mkdir(parents=True, exist_ok=True)
    print(f"生成縮圖到 {THUMBS_DIR.relative_to(ROOT)}")
    for name, items in items_by_name.items():
        for rsc, rid, p, w, h in items:
            thumb = THUMBS_DIR / p.name
            make_thumb(p, thumb)

    # 分類
    bucket = {cat: [] for cat in CATEGORY_ORDER}
    for name, items in sorted(items_by_name.items()):
        cat, sub = classify(name)
        bucket.setdefault(cat, []).append((name, sub, items))

    # 生成 markdown
    lines = []
    lines.append("# CIV.EXE 內建 sprite assets — 完整索引")
    lines.append("")
    lines.append("> **研究用途**。所有 sprites 著作權屬 © 1993 MicroProse Software, Inc.，現由 Take-Two Interactive / Firaxis Games 持有。本目錄為**逆向工程研究紀錄**，用於驗證 CvPc decoder 與翻譯範圍規劃；不作商業發行。詳見 [NOTICE.md](NOTICE.md) 與 [README §License](../README.md#license)。")
    lines.append("")
    lines.append(f"來源：1993 MicroProse《文明帝國 視窗版》遊戲目錄內 4 個 `Civdata*.RSC`（Apple Mac Resource Fork 格式）；解碼工具 [`team-a/tools/extract_tiles.py`](../team-a/tools/extract_tiles.py)；CvPc 格式描述 [`team-a/specs/03_asset_formats_and_tiles.md`](../team-a/specs/03_asset_formats_and_tiles.md)。")
    lines.append("")
    lines.append(f"共 **{len(png_files)} 個 sprite**，2.4 MB。縮圖在表內顯示（最大邊 {THUMB_MAX}px，magenta 背景已轉透明），點 ID 直連原圖。")
    lines.append("")

    # 統計表
    lines.append("## 分類統計")
    lines.append("")
    lines.append("| 分類 | 數量 |")
    lines.append("|---|---:|")
    total = 0
    for cat in CATEGORY_ORDER:
        n = sum(len(items) for _, _, items in bucket.get(cat, []))
        if n > 0:
            lines.append(f"| {cat} | {n} |")
            total += n
    lines.append(f"| **合計** | **{total}** |")
    lines.append("")

    # 每分類細表
    for cat in CATEGORY_ORDER:
        rows = bucket.get(cat, [])
        if not rows:
            continue
        n_total = sum(len(items) for _, _, items in rows)
        lines.append(f"## {cat} ({n_total})")
        lines.append("")

        # 按 sub 進一步分組
        sub_groups = {}
        for name, sub, items in rows:
            sub_groups.setdefault(sub, []).append((name, items))

        for sub in sorted(sub_groups.keys()):
            group = sub_groups[sub]
            n_sub = sum(len(items) for _, items in group)
            lines.append(f"### {sub} ({n_sub})")
            lines.append("")
            lines.append("| Sprite | Name | Resource | Size |")
            lines.append("|---|---|---|---|")
            for name, items in sorted(group):
                for rsc, rid, p, w, h in items:
                    rel_full  = f"../assets-extracted/tiles/{p.name}"
                    rel_thumb = f"../assets-extracted/tiles/_thumbs/{p.name}"
                    lines.append(
                        f"| ![]({rel_thumb}) | `{name}` | [{rsc} #{rid}]({rel_full}) | {w}×{h} |"
                    )
            lines.append("")

    INDEX_PATH.parent.mkdir(parents=True, exist_ok=True)
    INDEX_PATH.write_text("\n".join(lines), encoding="utf-8")
    print(f"已寫 {INDEX_PATH.relative_to(ROOT)} ({len(lines)} lines)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
