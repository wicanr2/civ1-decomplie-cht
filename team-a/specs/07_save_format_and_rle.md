# Spec 07 — SAV file 格式 + RLE 壓縮 (RLLEncode / RLLDecode)

> **Team A 規格 (草稿 v0.1, 2026-06-06)**. 接續 [spec 01 §1.2](01_compiler_and_api_surface.md) `load.c` 已識別出的 `CivLoadGame` / `RLLEncode` / `RLLDecode`.
>
> **外部材料**: 本 spec 主體取自 Honza Havlicek 2008 公開研究專案 *CivWin File Format demonstrator* (`team-a/external/Civilization/`), Honza 直接 reverse engineering Civ I 1993 Win port 的 SAV file. License: 自由 redistribute + 須 credit Honza.
>
> **狀態**: §7.1 (RLE 壓縮) 完整 spec 化; §7.2 (解壓後內部結構) 仍 **TODO**, Honza 跟我們都沒抽出.

## 7.0  本 spec 解答什麼

- Civ I 1993 Win SAVE file (`.SAV`) 的外層 RLE 壓縮格式
- `load.c::RLLEncode/Decode` 的演算法等價虛擬碼
- 如何 round-trip 讀寫 SAV (但 **解壓後內容仍未 RE**)

## 7.1  RLE 壓縮格式

SAV file 是純 byte stream RLE — **不是 bitstream**, 不是 LZW. 控制 byte 高位旗標決定模式:

```
SAV file = [ packet ]*
packet:
    case packet_byte < 0x80 (= 0..127):
        packet_byte = count    (注意: 不 +1, 範圍 0..127)
        data_byte  = next byte
        output [data_byte] × count to decompressed stream
    case packet_byte >= 0x80 (= 128..255):
        count = packet_byte - 0x80    (範圍 0..127)
        output next count bytes verbatim
```

**特殊 case**:
- `count == 0` (packet_byte = 0x00 或 0x80): 沒 effect, 但會多消一個 byte (低 0x80 模式) 或不消 byte (高 0x80 模式) — 編碼器不該產生
- packet_byte = 0xFF (max copy 127 bytes verbatim)
- packet_byte = 0x7F (max run 127 same bytes)

### 解壓虛擬碼 (對應 `RLLDecode @ load.c`)

```c
while ((c = read_byte()) != EOF) {
    if (c < 0x80) {
        r = read_byte();
        emit r, c times;
    } else {
        c -= 0x80;
        for (i = 0; i < c; i++) {
            r = read_byte();
            emit r;
        }
    }
}
```

### 壓縮策略 (對應 `RLLEncode @ load.c`)

- 找連續 ≥2 個相同 byte 的 run → 用 `<count> <byte>` 短模式
- 否則找連續不同 bytes 的 block → 用 `<count|0x80> <bytes>` 長模式
- count 上限都是 127

### Round-trip 驗證

Honza 的 demonstrator code 有 RLE_decode + RLE_encode 對稱實作.
Team B 實作後可:
1. 拿真實 `CIV.SAV` decode → temp.bin
2. encode temp.bin → temp.sav
3. byte compare CIV.SAV vs temp.sav (允許 encoder packing 差異)
4. 至少 decode(temp.sav) == decode(CIV.SAV) 必對 (golden 比對)

## 7.2  SAV file 解壓後內部結構 (v0.1 草稿, R2 2026-06-06)

**狀態**: R1 ship 後 (`team-b/src/save/rle.{h,c}` decode pass), R2 對 3 個真實 HAM*.SAV 做 cross-compare 解出主要區塊. **大幅進展但未全 RE**.

### 7.2.1  R2 對位方法

1. 用 `team-a/tools/civ_sav_extract` 把 `HAM1000B.SAV` (1000 BC), `HAM2000B.SAV` (2000 BC), `HAM3000B.SAV` (3000 BC) 解壓成 .bin
2. **3 個解壓後都剛好 107194 byte** → 證明 SAV 內部是 **fixed-size 結構**, 不是 variable-length
3. `team-a/tools/cross_compare_savs.py` 三方 byte 對比:
   - 105926 byte (98.8%) 三 SAV **完全一樣** → 大部分是 fixed format / sparse 0-fill
   - 127 byte (0.1%) 三 SAV **全不一樣** → game state (turn-dependent)
   - 1141 byte (1.1%) 兩同一不同 → 漸進變化 (player stats)
4. Hex dump 識別 ASCII 字串 + 找 LE16/LE32 monotone (turn / year / treasury 候選)

### 7.2.2  已對位 layout map

```
offset  size   content                                    來源
─────────────────────────────────────────────────────────────────────────
0x00000  2     magic: FE FF                               constant 三 SAV
0x00002  2     turn count LE16 (HAM3=50/HAM2=100/HAM1=150) monotone ✓
0x00004  6     可能 (各 LE16): unknown 02 / 04 / 06       constant
0x0000A  2     **西元年 LE16 signed** (HAM3=-3000/HAM2=-2000/HAM1=-1000) monotone ✓
0x0000C  2     LE16 = 0                                   constant
0x0000E  2     LE16 = 15 (= 0x0F, 推測 count of 某資源)   constant
0x00010  2     game-state varying LE16 (HAM3=7/HAM2=2/HAM1=25) V-category
0x00012 256    **8 個 King name records × 32 byte**       ASCII 對齊
              (Attila / Caesar / Hammurabi / Napoleon /
               Montezuma / Xerxes / Saladin / Suppiluliumas)
              對應 STR# 145 但 customized (Hammurabi 取代 Shaka)
0x00112 256    **8 個 civ plural names × 32 byte**         STR# 144 嵌入
              (Barbarians / Romans / Babylonians / French /
               Egyptians / Americans / Greeks / Mongols)
0x00212 256    **8 個 civ singular names × 32 byte**       STR# 143 嵌入
              (Barbarian / Roman / Babylonian / French /
               Egyptian / American / Greek / Mongol)
0x00312  2     padding / unknown
0x00314 ~200  **player stats table 候選** (8 player × ~24 byte 每筆)  V-category 密集
              HAM3=32, HAM2=104, HAM1=286 monotone @ 0x0314 → 推測 treasury/pop
              HAM3=146, HAM2=447, HAM1=875 monotone @ 0x0316
              HAM3=8, HAM2=61, HAM1=189 monotone @ 0x0318
              其他 stats stride pattern 待對位
0x00400 ~2000  待 RE — 可能科技樹 bitmap / civ progress matrix
0x006B0  ~10  monotone HAM3=2/HAM2=5/HAM1=9 @ 0x006B4 (LE16, 城市數?)
              HAM3=4/HAM2=10/HAM1=17 @ 0x006B6 (LE16, 單位數?)
              HAM3=3/HAM2=6/HAM1=10 @ 0x006B8
0x006C8  16   **可能 first 2 city records, 8 byte 各 ×2**
              `a3 00 43 02 00 04 00 00` ×2 = 兩個一樣的 8-byte 結構
              推測: city_x BE16 / city_y BE16 / owner u8 / improvements u8 / ...
0x008A0 ~200  **連續 32-byte stride 數字塊** — 強烈疑似 **city stat 表**
              HAM3=256/HAM2=512/HAM1=768 @ 0x08A3 (LE16, ×256 stride = trade?)
              HAM3=1/HAM2=2/HAM1=3 @ 0x08A4 (LE16, 1 每 turn = food storage?)
              HAM3=768/HAM2=1024/HAM1=1536 @ 0x08A5 (LE16)
              16-byte 內小整數 group 重複, 跨 city ~32 byte 間隔
0x00A72 ~10   ASCII '67:73' (? 比分? 時鐘?)
0x07D4A 8192  **STR# 135 城市名 pool: 256 × 32 byte**     直接從 Civdata0 嵌入
              全 256 個 city name (Rome / Caesarea / Carthage / ... /
              Paris / Tenochtitlan / Peking / London / Samarkand / ...)
              不是「玩家已建的城市」, 是 "engine 隨時可用的 city name 庫"
0x09E2A 602   全 0xFF stretch — **可能 fog of war bitmap** 或 unit slot init marker
0x0A0A3 1477  混合 0x00 + 0xFF — 推測 explored 地圖 bitmap (per-tile visibility)
0x0E000  10   `01 01 01 01 01 01 01 01 01 01` (10 個 0x01) — 推測 fixed marker
0x0E0C5 22811 **大塊全 0** — 未使用 buffer 或 sparse 表 tail
0x156DF 19419 **大塊全 0 + 0x0B at +3** — file 尾巴 padding
```

### 7.2.3  關鍵推斷

1. **SAV file 主體是 master tables 預先嵌入** (king/civ names / city pool) — 約 10 KB 重複內容. 真正 game state 大約 5-10 KB.
2. **Player table 在 0x0314 area**: 8 player slot × ~24 byte each. Treasury / population / known techs / current government 散布其中.
3. **City table 在 0x08A0 area**: 32-byte/city stride. Civ1 最多 ~128 city, 表 size 估 ~4 KB.
4. **Unit table 推測在 0x06C8 area**: 8-byte/unit stride? 待確認.
5. **World map** 沒明顯 1×80×50 = 4000 byte 連續區塊, 可能用 per-tile struct + visibility bitmap 分散儲存. 0x0A0A3 1477 byte 0x00/0xFF 混合是強候選 (60×30=1800 接近, 或 80×50 用 sparse).

### 7.2.4  仍未 RE

- **0x00400-0x006B0**: 可能完整科技樹 / civ progress / wonder ownership bitmap
- **0x006B8-0x008A0**: 中間 ~500 byte 用途
- **0x0A0AB-0x0E000**: ~16 KB 區塊
- **world map terrain id**: 確切 offset 待 Track A `PROJECT_MEMORY` 補充或自家 Ghidra walk `CivLoadGame`
- **player.known_techs bitmap**: 應該 9 byte (72-bit) 在 player record 內
- **city.improvements bitmap**: 應該 6 byte (46-bit) 在 city record 內

### 7.2.5  下個 round 任務 (R3 預訂)

- Ghidra walk `load.c::CivLoadGame` decompile (Team A 用 Track A 工具棧):
  - `fread(buf, size, count, sav)` 系列 call 順序 = SAV byte layout
  - 配對 `Player.treasury` / `Player.known_techs` 等 struct 欄位的 access offset
- 或實驗法: 把 dump 的 0x0314 area 改一個 byte 重 RLE encode 回 SAV, load 進 Civ1 看哪個 player stat 變了
- 補完 §7.2 表後 Team B 可實作 `civ_save_load(SAV) → civ_game_state`

## 7.3  Team B 整合介面契約

```c
/* team-b/src/save/rle.h */
#ifndef CIV_SAVE_RLE_H
#define CIV_SAVE_RLE_H
#include <stddef.h>
#include <stdint.h>

/* 解壓 src 內 src_len bytes 到 dst (caller 配 dst 至少 src_len * 128 大小).
 * 回實際解壓後 byte 數, 失敗回 -1.
 */
int civ_rle_decode(const uint8_t *src, size_t src_len,
                   uint8_t *dst, size_t dst_cap);

/* 壓 src 內 src_len bytes 到 dst (caller 配 dst 至少 src_len + src_len/127 + 16).
 * 回實際輸出 byte 數, 失敗回 -1.
 */
int civ_rle_encode(const uint8_t *src, size_t src_len,
                   uint8_t *dst, size_t dst_cap);

#endif
```

ctest target:
```c
/* tests/test_rle.c */
// 1. Round-trip random data: encode(x) then decode → x
// 2. encode 全 0 (1024 byte) → 8 byte (8 runs of 127+1)
// 3. encode 全唯一 (256 byte 0..255) → 256+2 byte (兩個 0x80+127 block)
// 4. golden 比對: decode 真 CIV.SAV (if CIV1_DATA_DIR set)
```

## 7.4  跟 Track A `PROJECT_MEMORY` 的關係

Track A (Big5-patched binary path) 當時推測 SAV format 但沒抽通. 本 spec 把
Honza 的 work 接進來 — Track A 看的同一個 binary, 我們現在有了參考 algorithm,
RE 阻塞點剩 §7.2 解壓後內部 layout.

## 7.5  其他外部研究參考 (僅資訊)

| 參考 | 來源 | 包含 |
|---|---|---|
| Honza 2008 demonstrator | `team-a/external/Civilization/` | §7.1 完整 |
| Track A `PROJECT_MEMORY` | 母專案 doc | SAV file 結構推測 (未抽通) |
| Civ I source leak (網路傳聞) | 未求證 | 完整 SAV layout (但我們**不查看**, clean-room 風險) |

## 7.6  與其他 spec 的關係

| spec | 給本 spec | 從本 spec |
|---|---|---|
| spec 01 §1.2 | `load.c::RLLEncode/Decode` 已識別 | 本 spec 補完算法等價 |
| spec 02 §2.1.2 D 段 | `Game/Save Game...` menu 觸發 → 跑 `CivLoadGame` | 整合點 |
| spec 06 (TODO) | 數值表 hardcoded 位置 | SAV 內 player.known_techs bitmap 可能對 spec 06 tech id |

## License & credit

本 spec §7.1 的格式描述源自 Honza Havlicek 2008 公開研究. 引用時請 credit:

> *RLE format reverse-engineered by Honza Havlicek (havlicek.honza@gmail.com), 2008, "CivWin File Format demonstrator".*

Honza 的原文 license 是 "freely redistribute and include this code in any program
as long as this credit is given where due" — 我們的 spec 把 "code" 抽象成 "format description" 也適用.
