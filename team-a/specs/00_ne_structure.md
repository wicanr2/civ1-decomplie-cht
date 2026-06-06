# Spec 00 — Binary 身分、NE container、外部依賴

> **Team A 規格。** 這份文件是 Team B 對「原版 `CIV.EXE` 是什麼」的唯一資訊來源。Team B 不看 binary、不看 dump（`team-a/dumps/00_ne_structure.txt`）、不開任何 disassembler。

## 0.1  Binary 身分

work-of-record 是 1993《文明帝國 視窗版》經由母專案 Track A 為 Big5 顯示而 patch 過的版本（inline ASCII 字串、`RT_DIALOG` control label、與 non-resident-name table description 改寫為 Big5；code segment 完全沒動）：

| 屬性 | 值 |
|---|---|
| 檔名 | `CIV.EXE` |
| 大小 | 832,512 bytes |
| MD5 | `336FF64650F6391C65A8B804ADFC31C9` |
| SHA-256 | `720C5EA4EFD47FC7069A89C9A366A3A18A561A88E39F963865C6084EEC4DB022` |
| Container | Win16 New Executable（NE），目標 Windows 3.0A 以上 |
| Module name | `WINCIV` |
| Module description | Big5 — bytes `A4 E5 A9 FA AB D2 B0 EA 20 B5 F8 B5 A1 AA A9 00` = `'文明帝國 視窗版'` |

原版 1993 binary（`MODULE_DESC = 'CIVILIZATION for Windows'`、833,024 bytes、MD5 `DCC4399E…`，位置 `D:\03_game_tmp\win31\C\MPS\CIVWIN\CIV.EXE`）結構完全相同：同 linker version（5.10）、同 133-segment layout、同 6 imports、同 11 個導出 callback、同 24 個 RT_DIALOG resource ID。Track A 只 patch 資料槽（inline 字串與 dialog label），沒有任何 code byte 被改。因此兩個 binary 產出的 disassembly、function 集合、與演算法行為都等價。

選 Big5-patched build 作 work-of-record 的理由：

1. 使用者既有的 SFX portable build chain（Track A 的 SFX packager）就是用這份 binary 包的；之後 spec 對 running screenshot 做驗證時，對的是同一個 artifact。
2. 反組譯中看到 string reference 已經指到 Big5 bytes，**Team B 翻譯 catalog 的「使用者可見字串」清單** 直接就是 Track A 選過的那一組 — 不用再做一次翻譯目標選擇。

**Team A 反組譯時要注意**：disassembly 看到 `MOV AX, OFFSET <string>` 或 `PUSH OFFSET <string>` 目標位元為高 byte（`0xA1`–`0xFE` lead byte）時，那個字串是 Big5；對應的原版 1993 英文字串可從母專案的 `data/inline_translations.json` 與 `data/dialog_translations.json` 還原。

## 0.2  記憶體模型與目標

NE header 報告：

- **目標 OS**：Windows（NE exe-type byte = `0x02`）。
- **要求 Windows 版本**：3.0A 以上（欄位值 `0x030A`）。
- **App 類型**：Windows Protected-Mode（WIN_PM）、`MULTIPLEDATA`、`PROTECTED_MODE_ONLY`。所以這是 286 protected mode、segmented 16-bit 應用；per-instance data segment 是 multi-instance `DGROUP` autodata segment。
- **Autodata segment index**：133（最後一個 segment）。
- **初始 heap 保留**：3,072 bytes。
- **初始 stack 保留**：9,216 bytes。
- **Entry point**：`CS:IP = segment 1 : 0x0000`（第一個 code segment 的第一個 byte）。
- **初始 stack pointer**：`SS:SP = segment 133 : 0x0000`（SP 進入時 wrap 到 autodata segment 末端，這是標準 Win16 慣例）。

**對 Team B 的意義**：原版假設 16-bit segmented memory；替代是單一 32/64-bit flat address space。原版用 far pointer（segment + offset）定位 game data 的地方，在 C99 對應就是一個普通 pointer 或一個 global table 的 index。原版用 16-bit `int` 並依賴 wrap-around 語意的地方，C99 對應要明確用 `int16_t`/`uint16_t`；其他情況下 plain `int` 即可。

## 0.3  Segment 結構概述

總共 **133 個 segment**。從 segment table 讀出的模式：

- **Segment 1 到 69**：code。每一個都是 `MOVABLE | PRELOAD | HAS_RELOC | DISCARD`，`flags = 0x1D50`，沒有 DATA bit。大小從幾百 byte 到 ~40 KB；最大單一 code segment 是 **segment 7，40,351 bytes**。
- **Segment 70 到 132**：小 data segment，每個都是 `DATA | MOVABLE | PRELOAD`，`flags = 0x0C51`。大多很小（幾 byte 到幾百 byte），這是 16-bit C 編譯器產出「每個 source file 一個 `_DATA` segment」的痕跡。
- **Segment 133（autodata）**：應用程式合併 `DGROUP` 預設 data segment，46,278 bytes，`DATA | MOVABLE | PRELOAD | HAS_RELOC`。承載程式可寫 globals。

**對 Team B 的意義**：segmentation 是 toolchain artifact，不是設計訊息。Team B 不保留 segment 邊界。所有 globals 歸到一個 flat data area；所有 code 由 C99 實作自己決定怎麼分 module。

dump 內逐 segment 的 relocation-record 數量列為「每 code segment 的 API call 密度代理」。reloc 數最多的 code segment（外部呼叫密度最高）：

| Segment | Reloc 數 | 可能角色 |
|---|---|---|
| 7  | 2,297 | 最大單一 code segment；API 呼叫密度極高。推測是主視窗 procedure / 繪圖 dispatcher。 |
| 63 | 1,010 | 次高的 code segment。 |
| 16 | 954 | 另一個 hot segment。 |
| 45 | 868 | 另一個 hot segment。 |
| 30 | 775 | 另一個 hot segment。 |
| 57 | 762 | 另一個 hot segment。 |

Team A 後續 spec 視內容釐清後再命名這些 segment；目前先以 index 稱呼（`seg_7` 等）。

## 0.4  外部 Win16 依賴（SDL2 替代邊界）

NE header 列出 **恰好 6 個** import module：

| Import module | 提供 | C99 + SDL2 替代 |
|---|---|---|
| `KERNEL` | 記憶體配置、檔案 I/O、系統 metrics、modal loop | C 標準函式庫 + 一個小 platform shim |
| `USER` | 視窗 / 對話框 / 選單 / 訊息佇列 / cursor / 剪貼簿 | SDL2 event loop + 自製 widget/dialog 系統 |
| `GDI` | device context、bitmap、palette、blit、文字、region、brush、pen | SDL2 軟體渲染到 indexed palette framebuffer |
| `WIN87EM` | 8087 浮點 emulation 函式庫 | 原生 FPU（不需替代）— 見 §0.6 |
| `MMSYSTEM` | `sndPlaySound`、MIDI、multimedia timer | SDL2_mixer（或 SDL2 audio + 小 MIDI player） |
| `COMMDLG` | 標準 File-Open / File-Save 對話框 | 自製 file picker 建在 SDL2 widget 上 |

**沒有 DDEML、沒有 SHELL、沒有 DRIVER、沒有 embedded VBX、沒有 winsock、沒有其他 module。** 這是完整外部介面。Team B 實作只要替代這 6 個。

逐 API 呼叫盤點（每個 module 各別 import 哪些 entry、call site 在哪）延後到 spec 01 — 需要 disassembly pass。

## 0.5  導出 callback（唯一以 ordinal 暴露的 entry point）

Resident-name table 導出 module name 與 **11 個**有名 ordinal。11 個都是 Windows callback procedure；沒有任何公開 API 入口。slot 名稱與 ordinal：

| Ordinal | 導出名稱 | 從名稱推測角色 |
|---|---|---|
| 1  | `ENUMFUNC`         | 通用 `EnumXxx` callback（視窗/字型/物件枚舉） |
| 2  | `WDWMAPPROC`       | 主地圖視窗的 `WndProc` |
| 3  | `WDWSMMAPPROC`     | 小地圖 / minimap 視窗的 `WndProc` |
| 4  | `WDWSTATUSPROC`    | 狀態 / 城市資訊視窗的 `WndProc` |
| 5  | `TIMERPROC`        | `SetTimer` callback（每 tick 的 game heartbeat） |
| 6  | `DLGPROC`          | 通用 dialog procedure |
| 7  | `RANDOMRADIOPROC`  | 「Random …」對話框內 radio-button 群組的 subclass / dialog proc |
| 8  | `RANDOMSTATICPROC` | 同對話框靜態區的 subclass |
| 9  | `RANDOMUSERPROC`   | 同對話框 user-input 區的 subclass |
| 10 | `REGIONPROC`       | region-shaped widget 的 subclass / paint proc |
| 11 | `CIVDIALOGPROC`    | 應用程式級對話框 proc（`CivDialog`） |

**對 Team B 的意義**：原版架構是 *多視窗* Win16 應用 — 至少 3 個 top-level 子視窗（主地圖、小地圖、狀態面板），各自有自己的 `WndProc`；加上一個自製「Random …」modeless dialog 帶 3 個 subclassed region；加上一個自製 region 形狀 widget；加上一個應用程式級 dialog proc。

SDL2 替代裡，「主地圖 / 小地圖 / 狀態面板」是一個 SDL2 視窗內的 3 個 widget 區域；各自對應到一個自包含 C module，模組內 event-handler function 扮演原本 `WndProc` 的角色。唯一的 SDL2 event loop 把 input 分派給 cursor / focus 所在的 widget。

## 0.6  浮點慣例（WIN87EM）

import list 出現 `WIN87EM` 表示 binary link 了 Microsoft 的 8087 emulation 函式庫。在沒有 FPU 的 1990s 機器（例：386SX）上，`WIN87EM.DLL` 攔截 x87 指令做軟體 emulation；在 386DX 以上有 FPU 的機器上，同樣指令直接執行。

**對 Team B 的意義**：Team B 的 C99 用原生 `float` 和 `double`（在我們目標所有 host 上都是 IEEE 754 single/double precision）。不需要 emulation。但所有影響可觀察狀態的浮點運算 — 戰鬥隨機骰、科學累積、金錢成長等 — 必須產出與原版相同的數值結果。原版用 `float`（32-bit）還是 `double`（64-bit）、中間運算的精度是多少，**目前未定**，要等 Team A 跑完相關 code 的 disassembly pass。Spec 01+ 會釘下來。

## 0.7  Resource directory 內容

NE resource table 報告下列繫結。這些是 EXE 內僅有的 resource。

### Cursors

16 個 `RT_CURSOR`（編號 `#1` 到 `#16`），每個 disk 上 512 byte；配對 16 個 `RT_GROUP_CURSOR`（`#128`–`#143`）。標準 Win16 cursor grouping（一個 group 對一個 cursor）。Team A 在 spec 02 釐清對話框與 cursor 的繫結後再命名。

### Icon

1 個 `RT_ICON #1`（2,560 byte）配對 1 個 `RT_GROUP_ICON #128`。是應用程式 icon。

### Menu

1 個 `RT_MENU #128`（1,536 byte）。是應用程式主選單列。Team A 在 spec 02 內 dump 結構到 `team-a/dumps/02_menu_128.txt`，對應 spec 條目會列每個 menu item 與其 command ID。

### Dialogs

**24 個** `RT_DIALOG` resource，每個 512-byte slot。ID 是：

```
129, 130, 131, 133, 135, 136, 137, 138, 139, 140, 141,
142, 143, 144, 145, 146, 147, 148, 149, 150, 151,
666, 999, 2000
```

注意：

- ID `132` 與 `134` 在 `131` 與 `135` 之間「缺號」 — 可能原本配發後在開發期間被刪除。
- ID `666`、`999`、`2000` 是 out-of-band 的，幾乎可確定是特殊用途（about box、copy-protection prompt、splash/loading dialog）。Team A 在 spec 02 釐清各自身分。

Team A 在 spec 02 對每個 dialog 寫一份單獨 spec，列每個 control（button / static / edit / listbox）、其 ID、label、tab order、與擁有它的 dialog procedure。

### Accelerator table

1 個 `RT_ACCELERATOR #128`（512 byte）。與 `RT_MENU #128` 配對的鍵盤 shortcut 表。

### String table

**沒有 `RT_STRING` resource。** 所有 user-facing string 都 inline 在 code 或 data segment。1993 慣例。Team B 的 localization 把字串抽到單獨 catalog（每字串一條 entry），不是回填 `RT_STRING`。

### Raw data

**沒有 `RT_RCDATA` resource。** 遊戲的資產檔（`.pic`、`.pal`、`.txt`、`.fnt`）放在 disk 上與 `CIV.EXE` 同目錄；不嵌入 binary 內。載入這些檔的方式記在 spec 03（資產 loader，含 EDILZSS2）。

### Version info

**沒有 `RT_VERSION` resource。** 不嵌入 Win16 version-info block。

## 0.8  編譯器與 runtime — 初步

NE 報的 linker version 是 **5.10**，1990 年代 linker 多家都有（Microsoft LINK 5.10 與 Borland TLINK 5.x 都可能）。`WIN87EM` 作為單獨 import（非 in-binary Borland math library）與 `MULTIPLEDATA` + 明示 `DGROUP` autodata 的用法符合 Microsoft C 7 / C/C++ 7.0 工具鏈（1992–1993 主流 Win16 廠商，MicroProse 也用）。

**最終編譯器辨識在 spec 01 確認（Ghidra 字串表 + prologue pattern 掃過後）。** 為什麼重要：知道 calling convention（`__pascal` vs `__cdecl`）才能正確解讀 disassembly 內參數順序，這直接決定 Team A 在後續 spec 描述 function signature 的方式。

## 0.9  spec 01+ 待解問題

1. 用 in-binary 簽名字串確認編譯器身分（Microsoft C 7.0 vs Borland C++ 3.1）。
2. 找出非導出 function 的主要 calling convention（`__pascal` 是 callback 強制；內部 helper 兩種都可能）。
3. 跟 `CS:IP = 1:0` 入口序列走到 `WinMain`（Win16 標準入口是 `LibMain` / `_WEP` / `WinMain`，在 C runtime setup 之後）。
4. 列每個 import module（KERNEL、USER、GDI、WIN87EM、MMSYSTEM、COMMDLG）逐 import procedure 與其 call site 數。這就是 Team B SDL2 替代必須涵蓋的「API 表面」。
5. 把 11 個導出 callback 對應到實際 `RegisterClass`、`CreateDialog`、`SetTimer` call，讓 Team B 知道各 callback 屬於哪個 window / dialog / timer。
6. 為 code segment 1–69 命名（依內容檢視；目前只用 index）。

## 簽核

- [ ] Team A：這份 spec 準確描述 work-of-record `CIV.EXE`（MD5 `336FF64…`）在 `team-a/dumps/00_ne_structure.txt` 觀察到的結構事實。
- [ ] Team B：這份 spec 可作為 scaffolding（build 設定、外部依賴計畫、callback module 邊界）的實作依據；不需要再回頭看 disassembly。
