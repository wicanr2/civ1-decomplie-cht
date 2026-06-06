# Spec 03 — 資產檔格式與 tile 抽取

> **Team A 規格。** 接續 [spec 01](01_compiler_and_api_surface.md)（§1.2 的 source file 對應表）與 [spec 02](02_startup_and_game_loop.md)。
> 配套工具：[`team-a/tools/extract_tiles.py`](../tools/extract_tiles.py)。
> 配套 evidence dump：`assets-extracted/tiles/_evidence/`（gitignored）。
>
> **狀態**：Mac Resource Fork 容器與 CvPc 5-byte 影像 header **已解開**；CvPc 內 LZW 變體 **未完全解開**，本 spec §9 列為待解問題。

---

## 3.1  game directory 完整資產盤點

1993 MicroProse Civilization for Windows 安裝後的遊戲檔（位置：`D:\03_game_tmp\_sfx_build_civ1\game\`，已解過 EDILZSS2 外包裝）：

| 檔名 | 大小 (B) | 推測用途 | Track A 已知格式？ | 本 spec 補上 |
|---|---:|---|---|---|
| `Civdata3.rsc` | 1,433,655 | 主資產包：149 個 CvPc（Civilopedia 科技 icon、單位 sprite、奇蹟圖等） | ✗（PROJECT_MEMORY 推測「裡面沒有遊戲文字，不用 RE」） | **Mac Resource Fork** + CvPc 容器 |
| `CIVDATA2.RSC` | 959,728 | 31 個 CvPc（GOVT*M、KING* 領袖肖像、SPACEST 等大型 set-piece） | ✗ | 同上 |
| `CIV.EXE` | 832,512 | Win16 NE 主程式 | ✓（spec 00/01/02 涵蓋；Track A Phase 1/3 patch） | — |
| `CIVDATA1.RSC` | 729,188 | 14 個 CvPc（BIRTH01..8、CUSTOM、CIV 標題等 intro 序列；含 MIDI 在 payload 內） | ✗ | **同上 + payload header `MThd/MTrk` = MIDI 信號** |
| `CIVFONTS.FON` | 244,224 | Win16 NE bitmap font library（21 RT_FONT：14 文明裝飾字 + 7 CIVTIMES UI 字） | ✓（Track A Phase 2 dfCharSet patch） | — |
| `Opening.wav` | 194,595 | 開場音樂 PCM | 標準 RIFF WAVE | — |
| `Airnuke.wav` | 188,972 | 核武音效 | 同上 | — |
| `Civdata0.RSC` | 181,519 | **遊戲文字 + 4 個 CvPc**：STR# (33 條 string list) / TEXT (399 條 Civilopedia 段落) / GDAT×7 / KDAT×14 / CvPc×4 (NUKE1、DOCKER、SPY、EARTH) | ✗ | **完整 type list + STR#/TEXT 用 Mac Resource Manager 標準格式** |
| `Lose2.wav` | 162,270 | 落敗音效 | RIFF WAVE | — |
| `CIVDATA4.RSC` | 144,102 | 1 個 CvPc：**`SPR32X32.GIF` 1472×400 = 城市/單位 sprite sheet** | ✗ | 同上 |
| `Wintune.wav` | 93,334 | 勝利音樂 | RIFF WAVE | — |
| `CIVHELP.HLP` | 72,787 | Microsoft WinHelp 3.x | 公開格式 | — |
| `Alex.wav`…`Mont.wav` | 30k–60k 各 | 14 領袖語音檔（Alexander, Caesar, Elizabeth, …） | RIFF WAVE | — |
| `S_nuke.wav`、`Cannon.wav`、`They_die.wav`、`We_die.wav`、`S_land.wav`、`S_beep.wav` | 1k–28k | 戰鬥/系統音效 | RIFF WAVE | — |
| `MPLOGO.BMP` | 4,718 | MicroProse logo（1-bit Windows DIB；BMP magic `42 4D` 已驗） | 公開 BMP 格式 | — |

**沒有 `*.PIC` 檔。** 任務說明預設「TERRAIN1.PIC」存在實際上不存在於 1993 Windows 版。**真正的 tileset 在 `CIVDATA4.RSC` 內單一 CvPc `SPR32X32.GIF` (1472×400)**；地形/單位/wonder/icon 圖則分散在 5 個 .RSC 共 **199 個 CvPc**。

> 旁注：spec 01 §1.2 提到 `gr_pic.c` 含 `GR_PicRead` / `PicDecompress` / **`LoadGifPicture`** — `PicDecompress` 就是 EDILZSS2 解壓器（已被 Track A 解開），`LoadGifPicture` 看名字應該是處理 .RSC 內 CvPc 的 GIF-LZW pipeline。即「.PIC 概念上存在，但實際 ship 的 1993 Windows 版本把它們全打包進 .RSC 並改用 LZW 而非 EDILZSS2」。

---

## 3.2  Track A 已搞懂 vs 本 spec 新解開

| 格式 | Track A | 本 spec |
|---|---|---|
| EDILZSS2 外包裝 (.EX$/.FO$/.HL$/.RS$/.RM$) | ✅ 完整解開（`tools/edilzss2_decode.py`） | — 沿用 |
| CIV.EXE Win16 NE 結構 | ✅ 24 RT_DIALOG + 0 RT_STRING + 133 segments | — 已在 spec 00/01/02 |
| CIVFONTS.FON 內 21 RT_FONT | ✅ dfCharSet 0x88 patch | — |
| Win16 RT_DIALOG walker | ✅ ASCII-space padding | — |
| inline data segment ASCII 字串 | ✅ NULL padding | — |
| **Mac Resource Fork 容器 (.RSC)** | ❌（記為「裡面沒有遊戲文字」） | ✅ **完整 layout 解開** |
| **STR# / TEXT 內 Mac Pascal-string 表** | ❌ | ✅ Civdata0 內 33 STR# + 399 TEXT（**推翻** Track A 「裡面沒文字」結論；翻譯 Batch B-E 可從這 432 條 + Track A 已掃描的 inline 字串雙線並進） |
| **GDAT / KDAT** | ❌ | 結構辨識（24 / 28 byte fixed-size records，看起來是 6-bit DAC RGB 8-tuple + colour-cycling LUT），語意尚未確認 — §9 待解 |
| **CvPc 影像容器 5-byte header** | ❌ | ✅ width(BE16) + height(BE16) + bpp(u8) |
| **CvPc tail 的 LZW 變體解碼** | ❌ | ⚠️ **未解開** — §9 待解；evidence 與 partial decoder 已 ship |
| **256-color VGA palette 來源** | ❌ | ⚠️ **未找到** — §9 待解（推測 `godpal.c::InitGodPalette` hardcoded 在 CIV.EXE，或自 GDAT/KDAT 組裝） |

---

## 3.3  Mac Resource Fork (.RSC) 容器格式

1993 Windows 版 5 個 `Civdata*.RSC` 全部是 **Apple Mac Resource Fork 標準格式**（big-endian），原本是 1991 Civ Mac 版的資產容器，Windows port 把它整段端進來（spec 01 §1.2 的 `resmgr.c` 內 `OpenResFile` 名字 = Mac Resource Manager API direct port）。

### 3.3.1  File header（offset 0x00..0x0FF）

| Offset | Size | 欄位 | 說明 |
|---:|---:|---|---|
| `0x00` | u32 BE | `dataOffset` | data section 起點（一律 `0x00000100`） |
| `0x04` | u32 BE | `mapOffset` | resource map 起點 |
| `0x08` | u32 BE | `dataLength` | data section 長度 |
| `0x0C` | u32 BE | `mapLength` | map 長度 |
| `0x10` | 240 B | reserved | 通常含 `"NewCivN.rsrc"` 之類的編譯時 system reservation，不影響解析 |

### 3.3.2  Resource map（mapOffset 起）

| Offset (rel) | Size | 欄位 |
|---:|---:|---|
| `0x00` | 16 | file header 副本（in-memory 用，忽略） |
| `0x10` | u32 | next map handle（in-memory，0） |
| `0x14` | u16 | file ref |
| `0x16` | u16 | map attributes |
| `0x18` | u16 | `typeListOffset`（**相對 mapOffset**） |
| `0x1A` | u16 | `nameListOffset`（相對 mapOffset） |
| `0x1C` | u16 | `numTypes − 1`（`0xFFFF` = 0） |

### 3.3.3  Type list（mapOffset + typeListOffset 起；前 2 byte 重複 numTypes-1）

每 entry 8 byte：

| Size | 欄位 |
|---:|---|
| 4 | type tag（4 ASCII char）|
| u16 BE | `numResources − 1` |
| u16 BE | `refListOffset`（相對 typeListOffset） |

### 3.3.4  Reference list（per-type；每 entry 12 byte）

| Size | 欄位 |
|---:|---|
| s16 BE | resource id |
| u16 BE | `nameOffset`（相對 nameListOffset；`0xFFFF` 無名） |
| u8 | attributes |
| u24 BE | `dataOffset`（**相對 file `dataOffset`**；指向 `u32 length + payload bytes`） |
| u32 | reserved（in-memory handle，0） |

### 3.3.5  Name list（mapOffset + nameListOffset 起）

Pascal string：1 byte length + N bytes ASCII。

### 3.3.6  Data section（file `dataOffset` 起）

每 resource entry 對應一段 `u32_BE length` + `length` bytes payload。

### 3.3.7  全 5 個 .RSC 的 type 表

| RSC | type tag | count | 內容 |
|---|---|---:|---|
| Civdata0 | `STR#` | 33 | Mac Pascal-string lists：Tax Rates / Civ Advances / Improvements & Wonders / People / Terrains / Cities / Story / Leaders / Governments / Caravan Goods / Customize World / About Text / Space 1 / Space 2 / Archeologist / Civ File Names / Civ Disk Names / Alert Text … |
| Civdata0 | `TEXT` | 399 | Civilopedia 內文段落（**推翻** Track A 「裡面沒文字」結論） |
| Civdata0 | `GDAT` | 7 | 24-byte fixed records（看起來 8 RGB triples × 6-bit DAC，疑似 micro-palette） |
| Civdata0 | `KDAT` | 14 | 28-byte fixed records（疑似 colour-cycling lookup） |
| Civdata0 | `CvPc` | 4 | NUKE1 640×400、DOCKER 48×360、SPY 243×116、EARTH 320×200 |
| CIVDATA1 | `CvPc` | 14 | BIRTH01 1024×320、BIRTH2..8 512×320、CIV 502×145、CUSTOM 512×320、GOVT2A 939×320 |
| CIVDATA2 | `CvPc` | 31 | GOVT0M/1M/2M 939×320、KING00..N 427×320、SPACEST 512×320 |
| Civdata3 | `CvPc` | **149** | Civilopedia 科技 icon（ADVFLGHT.GIF、ALPHABET.GIF、BRIDGE.GIF、…）多 178×132 或 215×100 |
| CIVDATA4 | `CvPc` | 1 | **`SPR32X32.GIF` 1472×400 = sprite sheet** |

`STR#` 與 `TEXT` 用標準 Mac Resource Manager Pascal-string 格式，可直接重用既有 ResEdit / mac-resfork 工具解出。**這是 Batch B-E 翻譯的高優先級新來源** — 比 Track A 從 CIV.EXE inline 掃出來的 484 條 prose 多出一整路 Civilopedia 內文。

---

## 3.4  CvPc 影像容器格式

每個 `CvPc` resource 的 payload 開頭是 5-byte 影像 header，後面接 GIF-style 變寬 LZW 編碼的 image data：

| Offset | Size | 欄位 | 觀察 |
|---:|---:|---|---|
| `0x00` | u16 BE | `width` | pixel 寬，例：640 / 512 / 1472 / 178 / 215 / 320 |
| `0x02` | u16 BE | `height` | pixel 高 |
| `0x04` | u8 | `lzwMinCodeSize` | 觀察值 `{6, 7, 8}`；對應 64/128/256-entry palette 模型；對齊 GIF89a Image Data 的 "LZW Minimum Code Size" 欄位的位置與語意 |
| `0x05` | … | LZW image data tail | GIF89a Appendix F 變寬 LZW；root code 寬度 = `lzwMinCodeSize + 1` bits（與 GIF 一致）；**完整解碼方法未解** — §9.1 |

**Header 完整證據**（從 9 個樣本實測）：

| .RSC | resource | W×H | byte 4 | byte 5..14 |
|---|---|---|:---:|---|
| Civdata0 | NUKE1.gif | 640×400 | `07` | `7F FF FF FF EF EF EF FF FF CE` |
| Civdata0 | DOCKER.GIF | 48×360 | `07` | `7F FF FF FF EB EB EB C6 EF E7` |
| Civdata0 | EARTH.GIF | 320×200 | `08` | `FF 00 00 00 00 00 AB 02 23 02` |
| CIVDATA1 | BIRTH01.gif | 1024×320 | `08` | `98 FF FF FF 29 52 C6 6B 6B B5` |
| CIVDATA1 | BIRTH2.gif | 512×320 | `08` | `DF FF FF FF BD C6 FF AD B5 F7` |
| CIVDATA1 | BIRTH3.gif | 512×320 | `08` | `DF FF FF FF FF FF DE AD AD DE` |
| CIVDATA2 | GOVT0M.GIF | 939×320 | `07` | `7F FF FF FF 0E DB FF 8A 73 FF` |
| CIVDATA2 | GOVT1M.GIF | 939×320 | `06` | `3F FF FF FF FD FF FF FA FF FF` |
| Civdata3 | discovr1.gif | 512×320 | `06` | `3F FF FF FF 57 FF FF C3 E3 FF` |
| **CIVDATA4** | **SPR32X32.GIF** | **1472×400** | **`07`** | **`7F 00 00 00 20 20 20 40 40 40`** |

**byte 5 是第一個 GIF sub-block 的大小**（GIF89a §22 「Image Data」格式），bpp=6/7/8 對應 `3F`/`7F`/`98..FF` — `98..FF` 不固定，視 LZW encoder 第一個 buffer flush 的大小而定。CIVDATA4 / SPR32X32 第一個 sub-block size = 127，接著前 17 個解出的 code 是 `127, 0, 0, 0, 32, 32, 32, 64, 64, 64, 96, 96, 96, 128, 128, 128, 159` — 前 16 個是「CLEAR + 5 個灰階 ramp 三連」完美匹配，**第 17 個 code 159 超出當時字典大小（130）→ LZW 變體與標準 GIF89a 在 code-size 升級時機或字典 seed 上有出入**（詳 §9.1）。

---

## 3.5  Palette（.PAL）格式

**1993 Windows 版的 256-color palette 不存在獨立 `.PAL` 檔。** game directory 內沒有任何 .PAL 副檔名檔案。Palette 必然存在這三處之一（**未確認，§9.2 待解**）：

1. **CIV.EXE 內 hardcoded** — spec 01 §1.2 的 `godpal.c::InitGodPalette` 強烈暗示 palette 是程式內常數。需要 disassemble `InitGodPalette` 找到 256 entry × 3 byte 的 `static const`。
2. **Civdata0.RSC 內 `GDAT` × 7**：7 × 24 byte = 7 個 8-RGB-triple micro palettes。值 ≤ 0x40 看起來是 6-bit VGA DAC 寫入值。**但 7×8 = 56 顏色不夠 256**，可能是「palette section animation」用的 colour-cycling slot。
3. **Civdata0.RSC 內 `KDAT` × 14**：14 × 28 byte，看起來是 colour-cycling LUT（每 record 開頭重複 byte，類似 palette index 動畫表）。

無 palette 不影響 LZW 解碼本身（解出的是 1-byte-per-pixel palette **索引** stream），只影響最後出 PNG 的視覺。`extract_tiles.py` 用內建 gradient palette 作 sanity check fallback；走通 LZW 後可用 `--palette` 餵 256×3 RGB binary。

---

## 3.6  Tile 排列規則

唯一明確的 tile 容器是 `CIVDATA4.RSC` 的 `SPR32X32.GIF` (1472 × 400)：

- 命名直接指出 **32 × 32 cell**。
- 1472 / 32 = **46 columns**。
- 400 / 32 = 12.5 → **不整除**，最後一列只有 16 px 高。需驗證是不是
  - (a) header 解錯（W/H byte order 對調 → 應為 400×1472），或
  - (b) sheet 設計上就是 12 個完整 32px row + 1 個 16px row 用來放半高的 banner 元素，或
  - (c) 32 × 16 的精靈也存在（單位 sprite 在某些 1990s 遊戲是 32×16 用兩個 stack 起來）。

第一個可能性已部分排除：所有其他 CvPc 在 byte 0..3 的 WxH 解讀都對應到 1990s 標準 VGA 解析度（640×400、320×200、512×320 — 都是 W 先 H 後）。

地形類型對應（從 Track A `data/inline_translations.json` Batch A + Civdata0 STR# 133 'Terrains' 預期內容）：

| 推測 tile index | 地形 | 來源 |
|---:|---|---|
| 0 | Desert 沙漠 | STR# 133 byte 3 起 `'\x06Desert'` |
| 1 | Plains 平原 | `'\x06Plains'` |
| 2 | Grassland 草原 | `'\x09Grassla'`（截斷） |
| 3+ | Forest / Mountain / Tundra / Glacier / Swamp / Jungle / Hill / Ocean / River | STR# 133 完整解開後對齊 |

正確的 tile-index ↔ terrain-type 映射需要 LZW 解通 + 視覺比對 sheet 才能定案。

---

## 3.7  Tile 抽取演算法（`extract_tiles.py`）

工具放在 `team-a/tools/extract_tiles.py`。Pipeline：

1. **讀 .RSC** → `parse_resfork(buf)` 回傳所有 `ResourceEntry`（含 type/id/name/payload）。
2. **挑選 `type_tag == "CvPc"`** 的 entries（用 `--all` 或 `--resource-id N`）。
3. **解 5-byte header** → `parse_cvpc_header(payload)` → `(width, height, lzwMinCodeSize)`。
4. **嘗試解 LZW**（兩條 path 並排）：
   - **Path A** — 自家 `gif_lzw_decode()`，從 GIF89a Appendix F 重新推導；目前撞到 §9.1 的格式 wedge。
   - **Path B** — 把 CvPc body 重新封裝成完整 GIF89a 檔（人造 6-byte magic + LSD + Image Descriptor + grayscale Local Color Table + Image Data tail + trailer）餵給 Pillow；Pillow 內部用 giflib decode。今天兩條都 fail（Pillow 回 "broken data stream"），但 Path B 留下的 `.reconstructed.gif` 是給離線 RE 工具（ImageMagick / IrfanView / Ghidra `LoadGifPicture` 對讀）的 evidence。
5. **render PNG**：若任一條 path 解出 ≥ 1/8 expected pixels，用 `Image.new("P")` + `putpalette()` 出 PNG。
6. **slice tiles**：`--tile-size N` 開啟時，把整張 image 用左上→右下 grid 切 N×N，每片各存一個 PNG。

CLI：

```bash
python extract_tiles.py CIVDATA4.RSC --resource-id 200 \
    --tile-size 32 --output-dir assets-extracted/tiles \
    --dump-evidence
```

`--dump-evidence` 寫出三個 debug artifact 到 `<output-dir>/_evidence/`：

| 檔名 | 內容 | 用途 |
|---|---|---|
| `<base>.lzwstream.bin` | sub-block defragment 後的 LZW byte stream | 給離線 LZW 偵錯（hex / 位元 trace） |
| `<base>.pathA_partial.bin` | Path A decoder 在卡住前解出的 byte（通常 1-20 byte） | 確認哪些 code 解出正確、從哪個 code 起變壞 |
| `<base>.reconstructed.gif` | 假裝成 GIF89a 的完整檔 | 給其他 GIF decoder 試開 |

---

## 3.8  如何驗證

### 3.8.1  Mac Resource Fork parser

- `--list` 子命令印出每個 .RSC 的 type 摘要 + 完整 resource list。Cross-check 對照本 spec §3.3.7 的表。
- 5 個 .RSC 共解出 **199 個 CvPc + 33 STR# + 399 TEXT + 7 GDAT + 14 KDAT = 652 個 resource**，無 parser error，無 mis-aligned offset。

### 3.8.2  CvPc 5-byte header

- 對 9 個樣本（§3.4 表）逐一 dump byte 0..14，確認 W×H 為合理 VGA 解析度。
- bpp byte ∈ `{6, 7, 8}` 對齊 GIF89a LZW min code size 合法範圍。

### 3.8.3  LZW 解碼

- **目前不通過**。每個 CvPc Path A 都在 1..20 byte 處撞到「code 超出當時字典」。Path B 給 Pillow 也都回 broken data。
- **回歸測試 baseline**：將來改寫的 decoder 一次 spike → 9 個樣本 header 全對齊 + SPR32X32 解出 588800 byte + 視覺辨認出 32×32 sprite cell + 至少 8 個 cell 跟單位形狀（trireme、phalanx、settlers、…）對得起來 = 通過。

---

## 3.9  待解問題

### 9.1  CvPc LZW 變體 — ✅ 已解開 (2026-06-06)

**結論**：CvPc LZW **就是標準 GIF89a Appendix F**，沒任何變體。

之前撞牆是**初始 decoder bug**：所有 199 個 CvPc 用「標準 GIF」decoder 都在 1..20 byte 處撞「code 159 > dict size 130」abort。但 Track A `extract_tiles.py` 初版把 CvPc header byte at offset 4 當成某種「mode byte」而非 LZW `min_code_size`。**正確解讀**：

- offset 4 byte = LZW `min_code_size` **直接**（不是 ±1，不是 mode byte）
- offset 5 byte = `palette_count - 1`
- offset 6 起 `palette_count * 3` bytes = palette (RGB triples)
- 剩餘 = 標準 GIF89a sub-block 串流（length-prefixed）+ 標準 LSB-first LZW (CLEAR = `1 << min_code`, END = CLEAR + 1)

**反推路徑**：透過 `team-a/tools/ghidra_byte_pattern_scan.py` 掃 `PUSH 0x2372` (LoadGifPicture assert string offset) 找到 `LoadGifPicture @ 10b8:11bc`。decompile (`team-a/dumps/03a_loadgifpicture.c`) 顯示 byte at offset 4 (`uVar1`) 直接被當參數傳給 `PicDecompress @ 10b8:158c`（即 LZW decoder 本人）。`PicDecompress` 線 118 `CLEAR = 2 << (param & 0x1f)` 看起來像 `min+1`，但實際呼叫 site 的 calling convention 偏移分析顯示 `param` 點到的是 `param_4` (y-offset) 而非 byte4 — Ghidra 對 cdecl16far 的 stack frame 解讀有誤導性。最終以 SPR32X32 樣本回歸測試確認：`min_code_size = byte4 = 7` → CLEAR = 128 → 第一個 9-bit code = 128 = CLEAR ✓。

**完整解開驗證**（2026-06-06）：
- 4 個 .RSC 內 **185 個 CvPc 全部 decode 成功，0 fail**
- SPR32X32（最大；1472×400 = 588800 pixel）byte-exact 全解
- 視覺：地形 sprite sheet、Pyramids/Mausoleum 奇蹟、14 個 KING* 領袖肖像、單位 sprite（trireme/cavalry/tank/battleship）全部肉眼可辨
- 截圖見 `docs/screenshots/cvpc_spr32x32_decoded.png` 與 `docs/screenshots/cvpc_king00_elizabeth.png`

**Production tool**：`team-a/tools/extract_tiles.py`（同檔案；CLI `--list / --id / --out-dir`）。輸出 PNG（PIL）或 PPM（PIL 缺時 fallback）。

### 9.2  256-color VGA palette 在哪？

CIV.EXE 800 KB 內必有一段 256 × 3 = 768 byte 的 RGB palette（或 6-bit-DAC 等價）。**證據**：spec 01 §1.2 的 `godpal.c::InitGodPalette` / `PushPalette` / `PopPalette`。**待做**：grep CIV.EXE data segments 找 256 連續 (R, G, B) triple；第一條應為 `(0, 0, 0)` 黑、最後一條多半是 `(255, 255, 255)` 白或 `(255, 0, 255)` colour-key 紫紅。

`Civdata0` 的 7 × `GDAT` + 14 × `KDAT` 的角色（micro palette? cycling table?）一併在這階段確認。

### 9.3  SPR32X32 高度 400 不被 32 整除

§3.6 列了三個可能（W/H byte order、半高 row 設計、32×16 sprite）。解 LZW 出整圖後肉眼即可區分。

### 9.4  Civdata1 的「`MThd` / `MTrk` payload 在 CvPc resource 內」

實測 Civdata1 的某些 resource payload 開頭含 MIDI magic `MThd`/`MTrk`，但 resource type tag 仍是 `CvPc`。可能：
- (a) 1991 Mac 原版用一個獨立 `Midi` type tag，1993 Windows port 為省事都重用 `CvPc`；
- (b) `MThd` 是該 CvPc resource 之**前一個** resource 的 data tail 黏到 view（resource map offset 對齊問題）。

需要從 resource id 對應到 `gr_pic.c` 內呼叫 site 才能判斷。

### 9.5  STR# Pascal-string 表 → 翻譯 catalog 整合

Civdata0 的 33 STR# + 399 TEXT 是 Track A 完全沒掃過的翻譯來源。需要：
- 從 Resource Manager 風格 layout (`u16 count; { u8 length; bytes }... `) parse 出所有條目。
- 對齊 Track A 既有 `data/inline_translations.json` 的譯名（Senate / Wonder / Despotism / …），避免術語不一致。

這屬於翻譯 backlog 而非格式問題，放在這裡備忘。

---

## 3.10  與 Team B SDL2 整合的接口契約

下列 C 介面是 Team A 把 1993 raw asset 喂給 Team B C99/SDL2 重寫所需的最小契約。**只訂介面，不訂實作**。實作必然依賴 §9.1 + §9.2 解開，但 Team B 可以先用 stub 寫好呼叫 site。

```c
// civ_assets.h
//
// All paths are UTF-8.

typedef struct civ_rsc civ_rsc;

// Open a .RSC. Returns NULL on parse error.
civ_rsc *civ_rsc_open(const char *path);
void     civ_rsc_close(civ_rsc *r);

// Iterate resources by type tag. Returns id list; caller frees.
size_t   civ_rsc_list_ids(const civ_rsc *r,
                          const char tag[4], int16_t *ids_out, size_t cap);

// Raw payload accessor.
const uint8_t *civ_rsc_payload(const civ_rsc *r,
                               const char tag[4], int16_t id,
                               size_t *size_out);

// Decode a CvPc resource. *pixels_out is owned by the civ_rsc handle
// (lifetime tied to civ_rsc_close). 1 byte per pixel = palette index.
typedef struct civ_pic {
    int      width;
    int      height;
    int      bpp;          // 6, 7, or 8
    const uint8_t *pixels; // width * height bytes
} civ_pic;

int civ_load_pic(civ_rsc *r, int16_t id, civ_pic *out);

// Convenience: blit a sub-tile out of a decoded picture.
int civ_load_tile(const civ_pic *src,
                  int cell_x, int cell_y,
                  int tile_w, int tile_h,
                  uint8_t *tile_out /* size = tile_w * tile_h */);

// Install the 1993 base palette into SDL2. NULL = use built-in
// fallback (godpal.c-derived once §9.2 is solved).
typedef struct { uint8_t r, g, b; } civ_rgb;
int civ_set_palette(SDL_Renderer *ren, const civ_rgb pal[256]);
```

語意註：
- `civ_load_pic` 必須是 **idempotent** — Team B render loop 可能每 frame 呼一次。內部對 decoded pixel buffer cache 即可。
- `pixels` 為 palette 索引，不是 RGB；Team B 自己用 `civ_rgb` 表轉換或交 SDL2 `SDL_PIXELFORMAT_INDEX8` surface。
- `SPR32X32.GIF` 經 `civ_load_pic` 後 Team B 端配合 `civ_load_tile(src, cx, cy, 32, 32, ...)` 取單格 sprite。
- `civ_set_palette` 在 §9.2 找到 256×3 RGB 表之前用一個內建 placeholder 即可；解開後換成從 CIV.EXE `godpal.c::InitGodPalette` const data section dump 出的 768 byte 表。

---

## 附錄 A — 實測 evidence 路徑

| 檔案 | 內容 |
|---|---|
| `team-a/tools/extract_tiles.py` | 唯一新增 tool；無 Track A code 重用 |
| `assets-extracted/tiles/_evidence/SPR32X32.GIF_id200.lzwstream.bin` | sub-block defragment 後 LZW byte stream（382 byte） |
| `assets-extracted/tiles/_evidence/SPR32X32.GIF_id200.pathA_partial.bin` | Path A 解出的 13 byte（CLEAR + 4 個灰階三連） |
| `assets-extracted/tiles/_evidence/SPR32X32.GIF_id200.reconstructed.gif` | 假 GIF89a 144184 byte，給離線 GIF tool 試開 |

`assets-extracted/` 全目錄 gitignored — 不入 repo，僅本機 build 產出。

---

## 附錄 B — 跟 Track A repo 的關係

本 spec 完全沒有 import 或 copy Track A `D:\03_game_tmp\civ1_cht\tools\*.py` 的任何 source line。Track A 的 EDILZSS2 / NE font / RT_DIALOG 工具與本 spec 涵蓋的 .RSC + CvPc 格式毫無重疊（不同層）。

唯一從 Track A docs 收回的事實知識：
1. Track A README §EDILZSS2 — 確認 .RSC 不需先解 EDILZSS2（5 個 .RSC 已是「解壓後」狀態）。
2. Track A LESSONS_LEARNED §「CityView 修復案例研究」 — 確認 1990s Civ 系列 **每個 .PIC 自帶 palette**，這對齊本 spec §9.2「palette 必然存在於 CIV.EXE 或 .RSC 內某處」的方向；同時 LESSONS_LEARNED 第 219 行（PROJECT_MEMORY mirror）寫 1991 DOS Civ1 `.pic` codec 是 `RLE + LZW + 18-bit palette`，這啟發 §9.1 第 3 條「RLE + LZW 混合」假設。

無 source-code 抄襲，clean-room 原則保留完整。
