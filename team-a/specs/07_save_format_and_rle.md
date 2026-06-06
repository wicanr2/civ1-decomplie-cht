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

## 7.2  SAV file 解壓後內部結構 (TODO)

**完全未 RE**. Honza 在 `sav_file.cpp` 註解寫 *"The file is basically completely undocumented yet"*.

推測內含 (從 spec 01 §1.2 `load.c::CivLoadGame` 行為):
- header: 版本 / world seed / turn 數
- world: 80×50 terrain grid (但 spec 03 我們已知 Win port 用 60×30? 待 binary 對照)
- player slots × 8: civ ID / 領袖 / 政府 / treasury / tax/lux/sci rate / known techs bitmap
- city list: name / pos / pop / production / improvements bitmap / shield store
- unit list: type / pos / owner / hp / moves / home city / orders
- diplomatic relations matrix: 7×7
- score table

**取得方法**:
- 先 ship Team B `civ_rle_decode/encode` (spec 07 §7.1 基礎)
- 拿真實 SAV decode 後手動分析 byte layout (跟 Track A `PROJECT_MEMORY` 第二輪可協作)
- 或 Ghidra walk `load.c::CivLoadGame` decompile 找 `fread()` size / 結構 layout

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
