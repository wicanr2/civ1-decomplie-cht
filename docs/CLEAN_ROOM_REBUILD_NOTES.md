---
name: civ1-clean-room-rewrite
description: 1993 Sid Meier 文明帝國 1 Windows 版 (CIV.EXE Win16 NE) **clean-room SDL2 重寫**路線的技術筆記 (跟 patch 路線 civ1-cht 是分流). 涵蓋 Mac Resource Fork / CvPc / sprite sheet 視覺辨識 / palette 安裝策略 / KING sprite id ≠ STR# 140 slot / GOVT*M sheet 結構 / Civ1 sentinel pixel transparency / R21 RGB scan 走錯路的教訓. 當使用者提到 civ1-decomplie-cht、clean-room civ1、SDL2 重寫文明、diplomat screen、KING portrait、GOVT0M/1M/2M、CIVDATA2/CIVDATA4、palette_nearest LUT、build_skip_mask 透明色、Caesar/Frederick/Elizabeth KING 對應、advisor 切片、CIVTIMES font reuse 時觸發. 跟 civ1-cht skill 互補 (那邊是 hex patch CIV.EXE binary 直接改字串; 這邊是丟掉 binary 從頭重寫 C99 + SDL2). 主動觸發: 即使使用者只說「繼續做 civ1 clean-room」「繼續 RE」也要套用.
---

# Civ1 1993 Windows 版 — Clean-Room SDL2 重寫路線

## 路徑與 repo

| 項目 | 位置 |
|---|---|
| **GitHub repo** | https://github.com/wicanr2/civ1-decomplie-cht |
| **本機路徑** | `D:\03_game_tmp\civ1-decomplie-cht\` (Windows native) |
| **原版資產來源** | `/mnt/d/03_game_tmp/civ1/civ1win/civ1win/` (WSL, 1993 MicroProse 安裝目錄) |
| **Memory** | `~/.claude/projects/.../memory/project_civ1_cht_track_c.md` |

跟 patch 路線 `civ1-cht` 完全分流: 那邊改 `civ1_cht` 底線資料夾, 直接 hex patch CIV.EXE 字串; 這邊是 `civ1-decomplie-cht` 連字號, **從頭重寫 C99 + SDL2**, CIV.EXE 只用來抽 spec, 跟 OpenCivOne MIT 比對 (2026-06-06 起 clean-room 政策放寬讓 Team A 看 OpenCiv1).

## 雙隊 clean-room 制度

| 隊伍 | 可看 | 可寫 | 禁止 |
|---|---|---|---|
| Team A | CIV.EXE Ghidra 反組譯 + OpenCivOne MIT | `team-a/specs/*.md` (00..09) | C 程式碼 |
| Team B | 只看 `team-a/specs/*.md` | `team-b/src/**/*.{c,h}` (C99 + SDL2) | 開 Ghidra、讀原 binary、讀 OpenCivOne |

`team-a/external/OpenCiv1/` 入 .gitignore (codeberg MIT, 不入 repo history).

## 資產容器 — 5 個 .RSC 全清單

每個 .RSC 是 Mac Resource Fork (BE16/BE32), 跨平台 1991 Mac → 1993 Win port 保留.

| 檔 | size | CvPc 數 | 內容摘要 |
|---|---|---|---|
| `CIVDATA0.RSC` | 大 | 4 | NUKE1 640×400 / DOCKER 48×360 / SPY 243×116 / EARTH 320×200 + KDAT(14) GDAT(7) |
| `CIVDATA1.RSC` | 729 KB | 14 | BIRTH01..08 (intro splash, BIRTH01=1024×320) + CIV.GIF title (502×145) + CUSTOM 512×320 + GOVT0A/1A/2A/3A (4 個 939×320 backdrop A 系列) |
| `CIVDATA2.RSC` | 959 KB | **31** | **3× GOVT0M/1M/2M (id 404/405/406, 939×320) — diplomat scene 背景 sheet** + **14× KING00..13 (id 500..513, 427×320) — 14 領袖肖像 sheet** + 14× SPACEST/planet/ARCH (太空船發射相關) |
| `CIVDATA3.RSC` | 1.4 MB | 149 | Civilopedia 科技 icon (ADVFLGHT/ALPHABET/BRIDGE…) 178×132 或 215×100 |
| `CIVDATA4.RSC` | 245 KB | **1** | **SPR32X32.GIF 1472×400** = terrain + unit + city sprite sheet (46 col × 12 row × 32×32) |

工具: `probe_civdata2` (團 B 內 `team-b/tests/`) 列任何 .RSC 的 type/id/name/size + CvPc WxH.

## CvPc 格式快查 (spec 03 §9.1)

```
offset 0..1  : BE16 width
offset 2..3  : BE16 height
offset 4     : LZW min_code_size (直接, 不是 ±1)
offset 5     : palette_count - 1
offset 6+    : RGB triplets (3 bytes 每 entry)
之後         : GIF89a Appendix F LZW (LSB-first bit packing, length-prefixed sub-blocks, CLEAR = 1<<min_code, END = CLEAR+1)
```

185/199 CvPc 已 byte-exact decode (剩 14 個 LZW variant 待解). 工具: `dump_cvpc` CLI 載 1 個 CvPc → PPM (sprite 自身 palette).

`civ_cvpc_decode(payload, len, &out_surf, &out_pal)` 已實作; **caller 拿到 surface + palette 後要自己決定怎麼安裝到 game**.

## 重大踩雷 + 反轉教訓

### 1. KING sprite id **完全 ≠ STR# 140 slot 順序** (R19 → R20 修)

| STR# 140 slot | leader | KING idx (CIVDATA2 id) |
|---|---|---|
| 1 | Caesar (Roman) | KING10 (510) |
| 2 | Hammurabi | KING05 (505) |
| 3 | Frederick (German) | KING12 (512) |
| 4 | Ramesses (Egyptian) | KING01 (501) |
| 5 | Lincoln (American) | KING04 (504) |
| 6 | Alexander (Greek) | KING13 (513) |
| 7 | Gandhi (Indian) | KING02 (502) |
| 8 | (NONE) | KING07 blank — STR# 留空 slot |
| 9 | Stalin (Russian) | KING08 (508) |
| 10 | Shaka (Zulu) | KING03 (503) |
| 11 | Napoleon (French) | KING11 (511) |
| 12 | Montezuma (Aztec) | KING09 (509) |
| 13 | Mao (Chinese) | KING06 (506) |
| 14 | Elizabeth (English) | KING00 (500) |

**這是 1991 Mac CIV 內部編號保留至 1993 Win port, 但 STR# 140 重排顯示順序的後果**. **不能假設 sprite idx = slot - 1**.

證偽方法: dump 14 sprite 視覺辨識 (`dump_kings.sh` + 把 PNG copy 到 `_king_dump/` gitignored 自己看). hardcode `SLOT_TO_KING_IDX[15]` lookup table (見 `team-b/src/world/diplomat.c`).

**1993 Win port 沒有 Mongol** — STR# 140 13 leader + 1 NONE = 14 slot; KING05 是 Hammurabi 不是 Genghis (1991 Mac 可能有的 Genghis sprite 在 KING 編號未保留位置).

### 2. GOVT*M 不只是 backdrop, 是**整套 diplomat scene + advisor sheet** (R21)

每張 GOVT*M (939×320, 3 種政府風格):
- **左半 0..460 × 0..320** = scene backdrop (sky + 宮殿/城牆/議會 + 中央 parchment 對話框 + 兩側 spear/sword ornament)
- **右半 460..939**:
  - 上方 ~0..80 y: 8 個 expression 小頭像 (animation frame grid)
  - 下方 ~80..320 y: **4 個立姿 advisor** (各 ~115×240, x=480/595/710/825)

對應 `civ_world.player_government` (1..6) → `civ_government_to_govt_idx()` collapse 6 政府 → 3 idx:
- Despotism / Monarchy / Anarchy → GOVT0M (古代寶座)
- Communism / Republic → GOVT1M (中世紀宮殿)
- Democracy → GOVT2M (現代議會)

**Cleanroom 自畫 sky+mountain+advisor 是 R18 走錯的路** — 整套就在 sheet 內. R21 接通後刪除 paint_advisor/paint_sky_mountain (留 fallback).

### 3. 透明色 = sentinel pixel **idx 0** (NOT RGB scan) (R21 走錯 → R25 修回)

**正解** (對齊 spec 03 §3.5.1 + OpenCivOne):
```c
static void build_skip_mask(const civ_palette_t *pal, uint8_t skip[256]) {
    (void)pal;
    memset(skip, 0, 256);
    skip[0] = 1;   /* sentinel only */
}
```

**錯誤** (R21 加的 RGB scan):
```c
/* R21 wrong! */
for (int i = 0; i < 256; i++) {
    civ_color_t c = pal->entries[i];
    if (c.r >= 0xE0 && c.g <= 0x40 && c.b >= 0xE0)
        skip[i] = 1;
}
```

副作用: Elizabeth 紅華服深紅 (R≈B0, G≈10, B≈30) 距離 magenta (FF00FF) RGB 不遠, palette 內 (E8, 20, E0) 級 entry 被誤殺 → 紅服飾領袖**缺角**.

R21 之所以加 RGB scan 是因為 R20 前 `g.palette` 還沒切到 KING 自身 palette, idx 0 mapping 走 sprite_sheet palette 跑到亮色 → 看到粉紅大方塊. **真修法是安裝 sprite 自身 palette (R20 已做), 不是改 skip 邏輯**.

### 4. Palette 安裝策略 — 改 `g.palette` > LUT 損失色階 (R20)

每個 CvPc 自帶 palette (256 entry). 直接 `g.palette = sprite_pal` → identity blit, **100% 顏色**. LUT (sprite_pal → game palette nearest) 會 quantize 損失色階, 紅華服變綠等悲劇.

代價: 同一 framebuffer 只能有一張 palette 解讀, 所以 chrome (Win16 灰/藍) + sprite (Mac CvPc) 共存得選一個. diplomat showcase modal 蓋全螢幕時可放心安裝 sprite palette; 主畫面共存場景就只能 LUT.

### 5. MIDI 音樂 **不在 CvPc payload** (R23 證偽)

spec 03 §3 推測「CIVDATA1 payload 內含 MIDI」**錯誤**. probe_civdata2 對 14 個 BIRTH/CIV payload 掃 MThd/MTrk signature 全 0 hit.

下一步: Ghidra walk CIV.EXE 內 `mmsystem.dll!MCISENDCOMMAND` callsites (R13 已 trace 該 API exists). R11 已列 23 個 WAV (sound effect, 不是 music).

可能音源:
- 嵌入 CIV.EXE 自身 data segment
- 用 PC speaker / OPL2 嗶嗶
- 1993 Win port 移除 (1991 Mac 有)

## Pipeline: 視覺辨識 → lookup table

1. `dump_kings.sh` / `dump_govt.sh` — 跑遍 1 個 RSC type 內所有 entry, 出 PNG 到 `team-b/tests/_king_dump/` (gitignored)
2. 開 PNG 一個個看, 識別「哪個 sprite 對應哪個 leader / advisor / icon」
3. hardcode `SLOT_TO_KING_IDX[15]` 或 `TERRAIN_SPRITE[col,row]` lookup table 進 .c
4. 視覺結果固定 → 寫 test_*.c assertion 鎖住 mapping
5. 識別完 dump 目錄可刪 (.gitignore 已 cover)

## 通用 tooling 已 ship

| 工具 | 用途 |
|---|---|
| `probe_civdata2` | 列 .RSC 任何 type 內全 entry (id, name, size, WxH) — 跨 5 個 .RSC |
| `dump_cvpc` | 載 1 個 CvPc id → PPM (sprite 自身 palette) |
| `dump_kings.sh` | 跑 KING00..13 全 dump 進 `_king_dump/` |
| `dump_govt.sh` | 跑 GOVT0M..2M 全 dump |
| `regen_r23.sh` / `regen_snaps.sh` | regen `m11_*` diplomat + main snapshot 一鍵 |

## Render path (廣義)

每個畫面: `widgets/<name>_screen.{h,c}` + state flag in `civ_game` + `widgets_render_all` hook. modal 全螢幕蓋: 安裝該畫面 native palette → identity blit. 共存畫面: LUT.

| modal | 對應 sprite |
|---|---|
| city_screen (R6) | SPR32X32 ocean tile (太花已撤) / fine speckle |
| tech_screen (R16) | clean-room 花邊 frame + KING icon 1 字 |
| diplomat_screen (R18-R25) | GOVT*M backdrop + KING portrait 中央 + advisor 兩側切片 |

## Build / test 標準流程

```bash
# WSL Ubuntu 22.04 內
cd /mnt/d/03_game_tmp/civ1-decomplie-cht/team-b/build
cmake .. -G Ninja >/dev/null 2>&1
ninja 2>&1 | tail -5
ctest 2>&1 | tail -3

# 跑單個 snapshot
SDL_VIDEODRIVER=dummy \
CIV1_DATA_DIR=/mnt/d/03_game_tmp/civ1/civ1win/civ1win \
./tests/test_world_snapshot /tmp/X.ppm <mode>
```

`<mode>` 已支援: 預設 (main) / `city` / `tech` / `diplomat-{elizabeth/frederick/mao/gandhi/id-NN}` / `splash` / `birth-{1..8}`.

## 已 ship Milestone 索引 (R-edition)

| R | 核心成果 |
|---|---|
| R1 | civ_rle_decode (SAV outer layer) |
| R2 | spec 07 SAV layout v0.1 |
| R3b | spec 06 v0.2 (28 unit + 25 building + 22 wonder OpenCivOne ground-truth) |
| R4 | Win16 chrome 對齊 (title + 8 menu + 子視窗) |
| R5 | M7-city 4 城市 spawn + name label |
| R6 | M7-cityscreen modal UI |
| R7 | M7-tick shield/food/pop 推進 |
| R8 | spec 06 §6.7 16 nation 3 軸 personality |
| R9 | spec 09 victory + score 公式 |
| R10 | civ_palette_nearest_rgb public helper + chrome polish |
| R11 | spec 08 24 WAV inventory + audio.h 介面 |
| R12 | Linux portable tar.gz |
| R13 | Ghidra callgraph: data_units 不是 unit init; MMSYSTEM 真 2 API |
| R14 | minimap body 黑底 + terrain base+overlay 雙 layer |
| R15 | TECH_DISCOVERY_SCREEN.md design + 3 ref |
| R16 | M-techscreen modal 完整 ship |
| R17 | 全面繁中化 (chrome + menu + status) + tech showcase 紀念 |
| R18 | M-diplomat 3 段 layout (clean-room 自畫 全錯) |
| R19 | KING sprite 載入 + STR# 140 真 slot 修正 |
| R20 | KING palette 安裝 + SLOT_TO_KING_IDX 視覺辨識 |
| R21 | magenta transparency + GOVT*M backdrop hook (兩個錯改動: RGB scan + sky_mountain 走錯) |
| R22 | status panel 單灰底 + KING 縮圖 + CIVDATA1 inventory |
| R23 | player_civ_slot + splash mode + advisor 切片 + MIDI 證偽 |
| R24 | government 動態 govt_idx + wizard hook + birth-N mode |
| R25 | transparency mask 回到 idx 0 only (R21 RGB scan 撤) |

## R26+ 候選

- **per-leader KING src_rect** (R20 用統一 (250,110,170,205), 部分 KING 偏)
- **tech enum 47 完整** (R16 只 13)
- **wonder enum 22 完整** (R16 hardcode 2)
- **government zh-TW name 表 + wizard 4th page**
- **BIRTH 動畫 timer loop** (R23/R24 只 single frame)
- **音源 Ghidra trace** (R23 證偽 MIDI 在 payload, 改 MCISENDCOMMAND callsite)

## 主動觸發

即使 user 只說以下任一也要套用:
- 「繼續做 civ1 clean-room」
- 「civ1-decomplie-cht 怎麼跑」
- 「KING 對不上 leader」
- 「diplomat 場景怎麼接 sprite」
- 「sentinel pixel transparency」
- 「GOVT0M 是什麼」

跟 `civ1-cht` skill **互補不互斥**: 那邊 patch 路線 (改 binary), 這邊 clean-room (從頭寫). 兩個專案使用相同 .RSC 資產但**架構完全分流**.
