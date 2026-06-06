# Spec 01 — 編譯器辨識、原始 source 結構、Win16 API 表面

> **Team A 規格。** 接續 [spec 00](00_ne_structure.md)。
> 配套 dump：`team-a/dumps/01a_functions.txt`、`01b_imports_used.txt`、`01c_entrypoint_walk.txt`、`01d_signature_strings.txt`。

## 1.1  編譯器與 runtime

Binary 由 **Borland C++ 3.x** for Windows 3.x 編譯。證據：

- segment `1420` 內嵌 copyright 字串：`'Borland C++ - Copyright 1991 Borland Intl.'`（dump：`01d_signature_strings.txt`）。
- `1000:0000` 的 entry-point 序列符合 Borland C runtime for Windows 3.x 的標準啟動：先 `CALLF` 到 C startup routine，接著 `MOV [seg], ES` 初始化 per-instance data segment，`STOSB.REP` 把 BSS 區段 `0x5146`–`0xB4BE` 清零，`INT 21h / AH=30h` 取 DOS 環境，`INT 1Ah` 讀 RTC（dump：`01c_entrypoint_walk.txt`）。
- 非 resident name table 帶 `MULTIPLEDATA` 與 per-instance autodata layout，是 Borland `c0w` startup 預期的格式（見 spec 00）。

對 Team B 編譯沒影響 — Team B 的 C99 用任何現代 toolchain 都能編。對 Team A 解讀 disassembly 意義：

- 非 callback 內部 function 的 calling convention **預設 Pascal**（Borland Win16 由左到右 push 參數到 stack，callee 清 stack）。`printf` 系 varargs 例外，明示 `cdecl`。
- 導出給 Win16 的 callback（spec 00 列的 11 個）是 `__pascal __far __export`，window/dialog procedure 的標準 `HWND, UINT, WPARAM, LPARAM`；`ENUMFUNC` 用 Win16 `EnumXxx` 簽名。
- Borland C runtime 的 `assert()` macro 在每個 call site 內嵌「fail 的 function 名 + source file 名」做字串 literal。這把原始 source file 結構免費送給我們（見 §1.2）。

## 1.2  原始 source file 結構（從 `assert()` macro 字串）

Borland C runtime 在每個 `assert()` call site 內嵌 `"FunctionName()  :  source.c"` 字串。從中還原 12 個原始 source file。Team A 對 1993 原版 codebase 組織的工作模型：

| 原始 `.c` file | 看得到的 identifier | 推測角色 |
|---|---|---|
| `dialogs.c`   | `ShowTextByName`、`TextBoxSetup`、`OldTextBoxSetup`、`TextBoxDrawAndClose` | 通用 dialog 建構與文字框內容 |
| `godpal.c`    | `InitGodPalette`、`FreeGodPalette`、`ZeroGodPalette`、`PushPalette`、`PopPalette` | 系統 palette manager — 擁有 256-color logical palette |
| `gr.c`        | `CreateDlgTemplate`、`GR_DialogOpenIndirect` | 圖形 utility（`GR_*` 字首）— 從 `.pic`/`.txt` 資料 runtime 產生 dialog template |
| `gr_pic.c`    | `GR_PicRead`、`GR_PortDataToBitmap`、`LoadGifPicture`、`InvertBitmap`、**`PicDecompress`** | `.pic` 檔 loader。**`PicDecompress` 就是 EDILZSS2 解壓器**（與 Track A 反推格式一致）。也支援 GIF code path。 |
| `gr_port.c`   | `GR_PortOpen`、`GR_PortClose`、`GR_PortChangeColor` | 「graphics port」對 GDI device context 的抽象 — 所有繪圖經過這層 |
| `init.c`      | `LoadAdvisors` | 應用程式 init；載入 advisor entity table |
| `load.c`      | `CivLoadGame`、`RLLDecode`、`RLLEncode` | 存讀檔。存檔用 RLL（run-length-with-literal）編碼；encoder/decoder pair 在此 module |
| **`mac.c`**   | `HandToHand`、`NewPtr`、`DisposPtr`、`DisposeHandle` | Mac Memory Manager emulation。`NewPtr`/`DisposPtr` 是 Apple 的 Handle/Pointer API。在此出現代表 Windows port 保留 Mac 記憶體模型，在 `GlobalAlloc`/`GlobalLock`/`GlobalUnlock` 之上做 emulation。 |
| **`resmgr.c`**| `OpenResFile`、`CDisposeResourceMgr`、`RestoMem`、`ZapRes`、`NewResource`、`AddType`、`AddRFile` | Mac Resource Manager emulation。Windows port 透過 Mac-style `OpenResFile` → `RestoMem` → `NewResource` flow 載入 `.pic`/`.txt`/`.fnt` 檔，不走 Win16 `FindResource`/`LoadResource`。 |
| `shape.c`     | `TileNew`、`TileDispose`、`PortTileBlt` | tile primitive — 地形 tile 建構/釋放、tile-blit 到 graphics port |
| `wdwmap.c`    | `WdwMapOpen`、`CheckMapGrowLimit` | 主地圖視窗的 open / event / paint。擁有 `WDWMAPPROC` window procedure。 |
| `wdwsmmap.c`  | `WdwSmMapOpen`、`DrawSmTiles` | 小地圖視窗。擁有 `WDWSMMAPPROC`。 |
| `wdwstat.c`   | `WdwStatusOpen` | 狀態 / sidebar 視窗。擁有 `WDWSTATUSPROC`。 |
| `windows.c`   | `MenuZ` | top-level 視窗 / 選單 setup。推測擁有 `WinMain` 與 `RegisterClass`。 |

**架構：「Mac 遊戲移植到 Win16」。** `mac.c` + `resmgr.c` 是鐵證 — 原始 1991 *Sid Meier's Civilization* DOS 版之後出 Mac 版；1993 Windows 版本質上是 Mac port 用 Win16 primitive emulate Mac Memory & Resource Manager 而成。Disassembly 上層會看到 Mac 風格 API 名（`NewPtr`、`OpenResFile`、`RestoMem`），底層落在 Win16 call（`GlobalAlloc`、`FindResource`、`LoadResource`）。

**對 Team B 的意義**：C99 重寫 **不** 保留這雙層 Mac-on-Win shim。Team B 寫一層 C99，把 Mac shim + Win16 substrate 做的事直接以 C99 + SDL2 表達。具體：

- `NewPtr` / `DisposPtr` → `malloc` / `free`。
- `Handle`（handle-to-handle 間接性，原本支援記憶體緊縮）→ plain pointer。現代重寫不做 compaction。
- `OpenResFile` / `NewResource` → 一個小的 resource-file reader，直接懂 `.pic` / `.pal` / `.txt` / `.fnt` on-disk 格式。Spec 03 會描述。

## 1.3  Function 盤點

Ghidra 在 `CIV.EXE` 內發現 **1,142 個 function**（dump `01a_functions.txt`）。結構上有興趣、目前已還原的列在下面。

### 1.3.1 11 個導出 callback（沿用 spec 0.5）

| Address | Symbol | Body | 角色 |
|---|---|---|---|
| `1000:0000` | `entry`           | 178 | C runtime startup stub（Borland `c0w`） |
| `1008:0d68` | `TIMERPROC`       |  43 | `SetTimer` callback。body 短 — 推測 post `WM_USER` 後 return |
| `1008:0e12` | `ENUMFUNC`        | 135 | `EnumXxx` callback（2 參數）。字型/物件枚舉用 |
| `1058:06ab` | `RANDOMRADIOPROC` |  79 | 「Random …」對話框 radio-button 群組 subclass |
| `1058:081a` | `RANDOMSTATICPROC`| 263 | 同對話框靜態區 subclass |
| `1058:0921` | `RANDOMUSERPROC`  | 213 | 同對話框 user-input 區 subclass |
| `1058:09f6` | `REGIONPROC`      |  79 | region-shaped custom widget subclass |
| `1098:1838` | `CIVDIALOGPROC`   | 191 | 應用程式級對話框 procedure（`CivDialog`） |
| `1208:0054` | `WDWMAPPROC`      | 203 | 主地圖視窗 procedure（`wdwmap.c`） |
| `1210:0000` | `WDWSMMAPPROC`    |  77 | 小地圖視窗 procedure（`wdwsmmap.c`） |
| `1218:0000` | `WDWSTATUSPROC`   |  77 | 狀態視窗 procedure（`wdwstat.c`） |

`WDWSMMAPPROC` 與 `WDWSTATUSPROC` body size 一樣（都 77 bytes），都位於各 segment 起點。最可能的形狀：薄 dispatcher，`switch(msg) { case WM_PAINT: jump 到內部 painter; … default: return DefWindowProc; }`，內部 painter 是同 segment 內另一個 function。Spec 02 走進去驗證。

### 1.3.2 Import API thunk（segment `1608`）

Segment `1608` 是 thunk segment。每筆都是 4-byte `JMPF` thunk 到一個外部 Win16 import。Ghidra 找到 157 個這種 thunk function（名字在 `01a_functions.txt` 末段看得到）。這就是 `CIV.EXE` 的「API 表面」，與 spec 0.4 的 6 個 import module 完全對應。

## 1.4  Win16 API 表面 — SDL2 替代邊界

每個 import module 之下，下表列呼叫密度最高的 API（按 call-site 計數排序）。資料來源：`01b_imports_used.txt`。

### 1.4.1 KERNEL — 667 call site

| Calls | API | 功能 | C99 替代 |
|---|---|---|---|
| 230 | `GLOBALUNLOCK`   | 釋放 Mac-style movable-memory lock | （no-op — Team B 用 plain pointer） |
| 206 | `GLOBALLOCK`     | 取得 Mac-style movable-memory lock | （no-op） |
|  63 | `GLOBALALLOC`    | 配置 movable heap block | `malloc` |
|  61 | `GLOBALFREE`     | 釋放 heap block | `free` |
|  16 | `FREEPROCINSTANCE` | 釋放 far callback 用的 thunk | （no-op — pointer 不需 thunk） |
|  15 | `MAKEPROCINSTANCE` | 配置 far callback 用的 thunk | （no-op） |
|  15 | `HMEMCPY`        | huge pointer `memcpy` | `memcpy` |
|  10 | `LSTRLEN`        | far pointer `strlen` | `strlen` |
|   8 | `_LCLOSE`        | Win16 file close | `fclose` |
|   5 | `_LLSEEK`        | Win16 file seek | `fseek` |
|   5 | `_LOPEN`         | Win16 file open | `fopen` |
|   5 | `GETFREESPACE`   | 報告剩餘系統記憶體 | （no-op 或固定值） |
|   5 | `GLOBALSIZE`     | block 大小 | 在 `malloc` wrapper 內 track，或移除 |
|   4 | `_HREAD`         | Win16 huge read | `fread` |
|   2 | `_HWRITE`        | Win16 huge write | `fwrite` |
|   2 | `WAITEVENT`      | 讓出 CPU 給其他 task（cooperative scheduler） | （no-op） |
|   1 | `INITTASK`       | Win16 task init（從 `c0w` startup 呼叫） | C runtime entry 取代 |
|   1 | `LOADRESOURCE`   | 載入 `RT_*` resource 到記憶體 | 對 7 種 `.pic`/`.pal`/`.txt`/`.fnt` 格式直接 file read |
|   1 | `FINDRESOURCE`   | 用 ID 找 `RT_*` | 直接 file read |
|   1 | `FREERESOURCE`   | 釋放載入的 resource | `free` |
|   1 | `LSTRCPY`        | far pointer `strcpy` | `strcpy` |
|   1 | `GETMODULEFILENAME` | 載入的 EXE 路徑 | `argv[0]` / `GetModuleFileNameA`（host 相依） |

KERNEL 流量大部分是 `GLOBALLOCK`/`GLOBALUNLOCK` 對（合計 436 call site）— `mac.c`/`resmgr.c` 的 Mac Memory Manager emulation 持續 lock/unlock handle block。C99 替代裡 handle 變 plain pointer；整個 locking 舞蹈消失。

### 1.4.2 USER — 970 call site

| Calls | API | C99 + SDL2 替代 |
|---|---|---|
| 238 | `SETRECT`         | `SDL_Rect` literal 賦值（或 `civ_rect_set`） |
| 111 | `OFFSETRECT`      | helper |
|  63 | `FILLRECT`        | `SDL_FillRect` 在 palette framebuffer 上 |
|  35 | `SENDMESSAGE`     | Team B flat-call 架構直接 function call（沒有 message；每個 `WndProc` case body 變成 C function 直接呼叫） |
|  35 | `GETCLIENTRECT`   | widget 結構內 rect，不必走 system call |
|  33 | `SHOWWINDOW`      | widget 結構上的 visibility flag；SDL 視旗標畫或跳過 |
|  28 | `GETWINDOWRECT`   | widget 結構 |
|  25 | `ENABLEMENUITEM`  | menu 狀態結構 + redraw |
|  24 | `FRAMERECT`       | SDL 線條繪製 |
|  23 | `PEEKMESSAGE`     | 主迴圈內 `SDL_PollEvent` |
|  23 | `INFLATERECT`     | helper |
|  22 | `SETWINDOWLONG`   | widget 結構欄位設值 |
|  18 | `SETWINDOWWORD`   | widget 結構欄位設值 |
|  17 | `MOVEWINDOW`      | 設 widget 結構的 `x`/`y` |
|  15 | `MESSAGEBEEP`     | SDL audio beep |
|  14 | `DISPATCHMESSAGE` | 主迴圈呼對應 widget handler |
|  13 | `TRANSLATEMESSAGE`| （no-op，event model 用不到） |
|  12 | `GETDLGITEM`      | 在 widget 結構查 control |
|  12 | `MODIFYMENU`      | menu 結構 mutate |
|  11 | `GETDC`           | （no-op — 我們直接畫 palette framebuffer） |
|  10 | `CREATEWINDOW`    | 構造 widget 結構 |
|  10 | `DEFDLGPROC`      | 預設 control handler（小 helper） |
|   9 | `MESSAGEBOX`      | 自製 SDL widget |
| 8 each | `BEGINPAINT`/`ENDPAINT` | （no-op — palette framebuffer 一直可畫） |
| 8 each | `GETFREESYSTEMRESOURCES` / `ISDIALOGMESSAGE` / `REGISTERCLASS` / `GETSYSTEMMETRICS` | 各自簡單對應 |
|   7 | `DEFWINDOWPROC` / `GETMENU` | 預設 widget handler / 回傳 menu 結構 |
|  ≤5 | (其餘約 52 個 API) | … |

主流模式是「pixel 座標下的 rect 算術」：238 `SETRECT` + 111 `OFFSETRECT` + 63 `FILLRECT` + 35 `GETCLIENTRECT` + 28 `GETWINDOWRECT` + 23 `INFLATERECT`。原版手動 layout 對 pixel 座標。Team B 替代保留同 approach（`SDL_Rect` widget layout），不引入更高層 layout 系統。

### 1.4.3 GDI — 552 call site

| Calls | API | C99 + SDL2 替代 |
|---|---|---|
| 116 | `SELECTOBJECT`     | 在小 drawing-state 結構內設「目前」pen / brush / font / palette |
|  72 | `DELETEOBJECT`     | 釋放物件結構 |
|  45 | `GETSTOCKOBJECT`   | 回傳 8 個 stock 物件之一（BLACK_PEN、WHITE_BRUSH、SYSTEM_FONT…） |
|  39 | `GETDEVICECAPS`    | 回傳螢幕尺寸 / depth 常數 |
|  31 | `LINETO`           | framebuffer 上的 `SDL_RenderDrawLine` |
|  29 | `SETBKMODE`        | drawing-state 結構欄位 |
|  26 | `BITBLT`           | `SDL_BlitSurface`（palette framebuffer 到 framebuffer） |
|  24 | `SETTEXTCOLOR`     | drawing-state 結構欄位 |
|  20 | `SETBKCOLOR`       | drawing-state 結構欄位 |
|  17 | `CREATEPEN`        | 構造 pen 結構 |
|  17 | `CREATESOLIDBRUSH` | 構造 brush 結構 |
|  17 | `MOVETO`           | drawing-state cursor 位置 |
|  15 | `TEXTOUT`          | 自製文字渲染（palette 層 glyph blit，CJK-aware） |
|  13 | `CREATECOMPATIBLEDC` | （no-op — 我們有一個永久 palette framebuffer） |
|  13 | `DELETEDC`         | （no-op） |
|   9 | `GETTEXTMETRICS`   | 從載入字型回傳 metrics（CIVFONTS 或替代 CJK 字型） |
|   6 | `GETTEXTEXTENT`    | 字型上度量文字 |
|   6 | `SETTEXTALIGN`     | drawing-state 結構欄位 |
|   4 | `ANIMATEPALETTE`   | 更新 palette framebuffer 的 palette entry |
|   3 | 多項 | drawing-state 存取 |
|   2 | `CREATEBITMAP` / `GETPIXEL` / `SETPIXEL` | 直接 framebuffer 存取 |
|   1 | 多項建構/列舉 API | 各自簡單對應 |
|   0 | `POLYGON` / `POLYLINE` / `SETPOLYFILLMODE` / `GETSYSTEMPALETTEENTRIES` | import 但 code 從未呼叫 — 可能由 static lib 自動拉入 |

GDI 由 GDI-物件 lifecycle（`SELECT` / `DELETE` / `GETSTOCK`）加上繪圖 primitive（`LINETO`、`BITBLT`、`TEXTOUT`）主導。未使用 API（POLYGON / POLYLINE / SETPOLYFILLMODE）證實遊戲不畫複雜填充多邊形 — 只有直線渲染。

### 1.4.4 COMMDLG — 4 call site

| Calls | API | 用途 |
|---|---|---|
| 2 | `COMMDLGEXTENDEDERROR` | file-dialog 後錯誤回報 |
| 1 | `GETOPENFILENAME` | open dialog（推測 `Load Game`） |
| 1 | `GETSAVEFILENAME` | save dialog（推測 `Save Game`） |

Team B 用 SDL2 自製 save/load dialog（反正其他 UI 也都自製）。

### 1.4.5 MMSYSTEM — 4 call site

| Calls | API | 用途 |
|---|---|---|
| 2 | `SNDPLAYSOUND`   | 一次性 WAV 播放 |
| 2 | `MCISENDCOMMAND` | MIDI / CD-Audio 命令介面 |

`SDL2_mixer` 包這兩個：`Mix_PlayChannel` 取代 `SNDPLAYSOUND`；`Mix_PlayMusic`（或小 MIDI player）取代 `MCISENDCOMMAND`。各 2 個 call site 表示音訊整合很少 — 推測是片頭音樂 + 一小組 UI / event 音。

### 1.4.6 WIN87EM — 0 call site

`WIN87EM` 在 NE import 中，但 segment `1608` 的 thunk 從未被 code 引用。函式庫被 link 是因為 `c0w` runtime presence-check；程式直接用 FPU。**不需要 C99 替代。**

## 1.5  Entry-point 反組譯走路

`01c_entrypoint_walk.txt` 從 `1000:0000` 開始走，跟著 Borland C runtime startup。重要 landmark：

```
1000:0000  CALLF  1608:005C        ; KERNEL.INITTASK
1000:0005  OR     AX,AX            ; 檢查 task handle
1000:0007  JNZ    1000:000C        ; 非零 → 繼續
1000:0009  JMP    1000:00CF        ; 零 → 退出
1000:0024  MOV    DI,0x5146        ; BSS 起點
1000:0027  MOV    CX,0xB4BE        ; BSS 終點
1000:002A  SUB    CX,DI
1000:002C  CLD
1000:002D  STOSB.REP ES:DI         ; 把 [0x5146, 0xB4BE) BSS 清零
1000:0032  CALLF  1608:0020        ; KERNEL.WAITEVENT（合作式排程）
1000:003B  CALLF  1608:0084        ; USER.INITAPP
1000:0049  INT    1A               ; BIOS 時間 → DX:CX
```

走路在抵達 `WinMain` 前停止（Ghidra auto-analysis 沒傳遞 `WinMain` symbol）。startup stub 最終會 call 進使用者 `WinMain`，那個 function 位於 `windows.c`（依 §1.2 的 source file 盤點），就是 assertion string 內看到的 `MenuZ`。spec 02 走 startup chain 其餘部分，定位 `WinMain` 並標 address。

**對 Team B 的意義**：整段 C runtime startup + `WinMain` 由標準 C99 `main(int argc, char *argv[])` 取代，呼叫 `SDL_Init`、配置 palette framebuffer、註冊 widget、開存檔視窗、進 event loop。§1.5 內操作順序不需保留。

## 1.6  Spec 02+ 待解問題

1. 精確定位 `WinMain`。（從 entry stub 走過 Borland C runtime 到第一個使用者 code function。）
2. 走 `WinMain` body：3 個 top-level 視窗各別 `RegisterClass`、`CreateWindow`、`LoadAccelerators`、`LoadMenu`、`SetTimer`，然後 `GetMessage` / `TranslateMessage` / `DispatchMessage` 迴圈。
3. 辨識 message-pump 結構：合作式多工等待點、idle-time 計算槽、frame timing。
4. 對 3 個導出 window procedure（`WDWMAPPROC`、`WDWSMMAPPROC`、`WDWSTATUSPROC`），列各自 handle 的 message 與 case body。
5. 對 `CIVDIALOGPROC`，列 handle 的 message 與 dispatch 24 個 `RT_DIALOG` resource 的方式。
6. 在 disassembly 內定位 `PicDecompress`（EDILZSS2 decoder）寫 spec 03（資產 file format）。
7. 定位 `RLLDecode` / `RLLEncode` 寫存檔格式 spec。
8. 決定 Team B drawing-state 結構（current pen / brush / font / palette / text color / bg color / bg mode / text align / move-to cursor）— 把讀寫 drawing state 的十幾個 GDI API 收在一個結構。

## 簽核

- [ ] Team A：這份 spec 準確總結 `team-a/dumps/01*` 觀察到的編譯器、source 組織、Win16 API 表面。
- [ ] Team B：這份 spec 可作為 build system / 外部依賴計畫與 API 替代邊界的實作依據，不需要再回頭看 disassembly。
