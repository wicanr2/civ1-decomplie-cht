#!/bin/bash
# R12 M8: build portable Linux tarball
#
# 用法 (在 repo root 內):
#   bash tools/build_portable.sh
#
# 產出:
#   dist/civ1-cht-linux-x64.tar.gz
#     ├── civ1                  Linux x64 binary (stripped)
#     ├── civ1.sh               launcher (取 CIV1_DATA_DIR env)
#     ├── README.md             跑法 + 系統需求
#     └── share/
#         └── fonts/
#             └── uming.ttc     CJK 字型 (若系統內找到才複製)
#
# 使用者另需自備:
#   原版 1993 Civ Windows 的 5 個 .RSC + CIV.EXE (放在某 dir 作 CIV1_DATA_DIR)
set -e

REPO_ROOT=$(cd "$(dirname "$0")/.." && pwd)
DIST="$REPO_ROOT/dist"
STAGE="$DIST/civ1-cht-linux-x64"
BUILD="$REPO_ROOT/team-b/build"

echo "==> 1. Stage clean"
rm -rf "$STAGE"
mkdir -p "$STAGE/share/fonts"

echo "==> 2. cmake build (Release)"
cd "$BUILD"
cmake -DCMAKE_BUILD_TYPE=Release . > /dev/null
ninja civ1

echo "==> 3. Copy binary + strip"
cp "$BUILD/civ1" "$STAGE/civ1"
strip --strip-unneeded "$STAGE/civ1" 2>/dev/null || true
size=$(stat -c%s "$STAGE/civ1")
echo "    civ1 stripped: $size byte"

echo "==> 4. Copy CJK font (if present)"
FONT_SRC=/usr/share/fonts/truetype/arphic/uming.ttc
if [ -f "$FONT_SRC" ]; then
    cp "$FONT_SRC" "$STAGE/share/fonts/"
    echo "    copied $FONT_SRC"
else
    echo "    [WARN] $FONT_SRC 不存在, portable 不會自帶 font"
    echo "    [WARN] launcher 會用系統字型 fallback"
fi

echo "==> 5. 寫 launcher civ1.sh"
cat > "$STAGE/civ1.sh" << 'EOF'
#!/bin/bash
# civ1-cht portable launcher
# Usage: ./civ1.sh [CIV1_DATA_DIR]
#   CIV1_DATA_DIR: 含 5 個 .RSC + CIV.EXE 的 dir (使用者自備原版資料)
#   不傳則用 env CIV1_DATA_DIR

SELF=$(cd "$(dirname "$0")" && pwd)
DATA_DIR="${1:-${CIV1_DATA_DIR:-}}"
if [ -z "$DATA_DIR" ] || [ ! -d "$DATA_DIR" ]; then
    echo "用法: ./civ1.sh /path/to/CIVWIN-data-dir"
    echo "或設環境變數: export CIV1_DATA_DIR=/path/to/CIVWIN-data-dir"
    exit 1
fi
export CIV1_DATA_DIR="$DATA_DIR"

# Font (若 portable 自帶 font 則用之)
if [ -f "$SELF/share/fonts/uming.ttc" ]; then
    export CIV_FONT="$SELF/share/fonts/uming.ttc"
fi

exec "$SELF/civ1" "$@"
EOF
chmod +x "$STAGE/civ1.sh"

echo "==> 6. 寫 README.md"
cat > "$STAGE/README.md" << 'EOF'
# Civilization 文明帝國 視窗版 繁中化 (Track C clean-room)

> 從 1993 MicroProse 文明帝國 視窗版 (`CIV.EXE`) 反組譯重寫 + 內建繁體中文化.
> 本 portable 不含原版資料, 須使用者自備合法拷貝.

## 使用方法

```bash
# 1. 你必須有 1993 Civilization Windows 版的 5 個 .RSC + CIV.EXE
#    通常在 \MPS\CIVWIN\ 下, 含:
#      CIVDATA0.RSC ... CIVDATA4.RSC
#      CIV.EXE
#      CIVFONTS.FON (本 Track C 不用; clean-room 自畫字模)

# 2. 解壓本 tarball
tar xzf civ1-cht-linux-x64.tar.gz
cd civ1-cht-linux-x64

# 3. 跑 launcher (傳 CIVWIN data dir 或設 env)
./civ1.sh /path/to/CIVWIN-data-dir

# 或:
export CIV1_DATA_DIR=/path/to/CIVWIN-data-dir
./civ1.sh
```

## 系統需求

- **OS**: Ubuntu 22.04 LTS 或更新 (glibc ≥ 2.35)
- **SDL2**: ≥ 2.0.20 (Ubuntu apt: `sudo apt install libsdl2-2.0-0`)
- **FreeType**: ≥ 2.10 (apt: `sudo apt install libfreetype6`)
- **CJK 字型**: 若 portable 自帶 `share/fonts/uming.ttc` 則用之, 否則用系統字型
  - 系統 fallback: `sudo apt install fonts-arphic-uming` (or `fonts-noto-cjk`)

## 含什麼 / 不含什麼

| 含 | 不含 |
|---|---|
| `civ1` Linux x64 binary (Track C SDL2 重寫) | 原版 `.RSC` 資料 (版權) |
| `civ1.sh` launcher script | `CIVFONTS.FON` (Track C 自畫字模, 不需) |
| `share/fonts/uming.ttc` (可選, 系統有則自帶) | SDL2 / freetype DLL/.so (系統提供) |
| 內建 376 條 zh-TW 翻譯 (spec 05 §5.3 STR# 22 表) | leader 配音 WAV (留 v0.2) |

## 已知功能

- 主選單 (TODO 後續 round)
- 主畫面: 主地圖 + minimap + status panel + Win16 8-menu chrome
- 城市系統: 4 城市 spawn, sprite + name label, status panel cursor hook
- City screen modal: Enter 開 / ESC 關 (8 panel 對齊原版 1993 reference)
- Turn-tick 生產推進: shield 累積, 建造 auto-pick, pop 成長
- Unit 移動 + placeholder 戰鬥
- 14 文明 + 領袖肖像 + zh-TW 翻譯
- 376 條 zh-TW 翻譯 (22/33 STR# 表覆蓋)

## v0.2 待做

- Win64 portable (需 mingw SDL2 SDK 跨編譯, 留下一 round)
- 聲音 (spec 08 已 RE, 24 WAV inventory 完成, Team B `audio.h` 介面待實作)
- 完整 city screen tab interaction (HAPPY/MAP/VIEW 切換)
- 真實 combat 公式 + RNG seed (spec 06 §6.1.1 ground-truth)
- 存讀檔 RLE 整合 (spec 07 §7.1 已 ship Team B `civ_rle_decode/encode`)

## License

- Track C source: MIT
- 翻譯: CC BY-SA 4.0
- 原版 *Sid Meier's Civilization for Windows* © 1993 MicroProse Software, Inc.

## Credits

- **Honza Havlicek 2008** — CivWin File Format demonstrator (RSC parser / LZW / SAV RLE)
- **Rajko Horvat** — [OpenCivOne](https://codeberg.org/rhorvat/OpenCivOne) (MIT) — 28 unit / 25 building / 22 wonder / 47 tech / 24 terrain / 16 nation ground-truth
- **1991 Civilization Manual** — Score 公式 + Winning rules

完整 RE 文件: https://github.com/wicanr2/civ1-decomplie-cht
EOF

echo "==> 7. tar.gz package"
cd "$DIST"
tar czf civ1-cht-linux-x64.tar.gz civ1-cht-linux-x64/
ls -la civ1-cht-linux-x64.tar.gz
tar_size=$(stat -c%s civ1-cht-linux-x64.tar.gz)
echo ""
echo "==> Done. Portable tarball: $DIST/civ1-cht-linux-x64.tar.gz ($tar_size byte)"
echo "==> Stage dir: $STAGE"
echo "==> Try: ./civ1-cht-linux-x64/civ1.sh /path/to/CIVWIN-data-dir"
