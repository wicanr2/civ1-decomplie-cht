# 階段紀錄

## 2026-06-06 — Phase 0 起步

- Repo 建立：https://github.com/wicanr2/civ1-decomplie-cht
- WSL 環境：Ubuntu 22.04.5 LTS（WSL2 kernel 6.6.114.1），JDK 21.0.11 已預裝。
- 決策：放棄前置 Track B（`openciv1pp/`，衍生自 OpenCiv1） — 本 repo 從 disassembly 起 clean-room 重建。
- 決策：不用 DOSBox / wine / 跑原版 binary 當 oracle。驗證來自 spec 推導 fixture、資產 round-trip、與 Track A 凍結 Big5 build 做純 layout 截圖比對。

## 2026-06-06 — 工具棧 + binary 取得

- Ghidra 12.1.2 安裝到 `/opt/ghidra_12.1.2_PUBLIC`（symlink `/opt/ghidra`）；headless analyzer + JDK 21 驗證通過。
- 輔助工具 apt 安裝：`p7zip-full`、`binwalk`、`xxd`、`file`。（`radare2` / `rizin` 不在 Ubuntu 22.04 預設 repo — 延後；Ghidra 一個就夠用。）
- 發現 `_sfx_build_civ1/game/CIV.EXE` 是 Track A Big5-patched build（832,512 bytes，MD5 `336FF646…`），不是原版 1993。MODULE_DESC 解 Big5 = `'文明帝國 視窗版'`，不是 `'CIVILIZATION for Windows'`。
- 原版 1993 英文 binary 在 `D:\03_game_tmp\win31\C\MPS\CIVWIN\CIV.EXE`（833,024 bytes，MD5 `DCC4399E…`）。
- 兩個 binary 結構完全相同（同 133 segments / 同 6 imports / 同 11 exports）；Track A 只 patch inline 字串槽與 RT_DIALOG label，沒動 code。
- 在原版上跑 spec 00 後，使用者授權改用 Track A Big5-patched binary — spec 00 / README / `team-a/binary/CIV.EXE` 對應更新。選 Big5-patched 是因為 (a) 使用者的 SFX portable 是這份 binary build 的，視覺 layout 驗證能對同一 artifact；(b) Big5 inline string reference 直接餵 Team B 翻譯 catalog。
- 第一輪 Ghidra（原版上）跑了 56 秒；自動偵測 `New Executable (NE)` / `x86:LE:16:Protected Mode`，且自動套用 Win16 export symbol table（KERNEL/USER/GDI/WIN87EM/MMSYSTEM/COMMDLG）。專案刪掉，改在 Big5-patched binary 上重跑。

## 2026-06-06 — Spec 00：NE 結構

- 寫 `team-a/tools/ne_dump.py` — 公開格式（Microsoft NE spec）的 dumper；只讀格式定義允許的欄位，不做任何推論。輸出存 `team-a/dumps/00_ne_structure.txt`。
- 寫第一份簽核 spec `team-a/specs/00_ne_structure.md`：binary 身分、記憶體模型、133 segments（約 69 code + 約 63 data + 1 autodata 在 segment 133）、6 個 Win16 import（KERNEL / USER / GDI / WIN87EM / MMSYSTEM / COMMDLG）、11 個導出 callback（`WDWMAPPROC`、`TIMERPROC`、`CIVDIALOGPROC`…）、resource directory（24 `RT_DIALOG`、1 `RT_MENU`、16 `RT_CURSOR`、1 `RT_ICON`、無 `RT_STRING`、無 `RT_RCDATA`、無 `RT_VERSION`）。
- Spec 00 簽核待 Team A 與 Team B 各別覆核。

## 2026-06-06 — Specs 簽核 + M0 開工（SDL 視窗起來）

使用者代簽 spec 00 / 01 / 02 與 SDL 實作計畫，授權進入實作階段。

### M0 交付
- `team-b/CMakeLists.txt` + `tests/CMakeLists.txt`：CMake 3.16+ / C99 / -Wall -Wextra -Wpedantic / SDL2
- `team-b/src/main.c`：取代 Borland `c0w` stub + WinMain；640×480 視窗、硬體 renderer 失敗 fallback 軟體
- `team-b/src/civ_loop.c`：取代 §2.1.2 H 段 `while (DAT_12d8_24ee == 0) { FUN_1088_0000(); }`；`SDL_PollEvent` 非阻塞 + ~60 Hz present + ESC / 視窗關閉退出
- `team-b/src/civ_game.h`：主結構 skeleton；M0 階段含 quit / timer_counter / tick_count / window / renderer
- `team-b/tests/test_window_lifecycle.c`：headless dummy driver + push SDL_QUIT + 驗證 `civ_loop` 1 秒內結束、`g->quit` 為 true

### 驗證結果
- WSL Ubuntu 22.04 + GCC 11.4 + SDL2 2.0.20：build 6/6 zero warning
- ctest 1/1 PASS（`window_lifecycle` 0.04 秒）
- `civ1` binary headless dummy driver 跑 2 秒被 SIGTERM 結束無 crash

### Ghidra LoadGifPicture 嘗試（部分失敗）
- 寫 `team-a/tools/ghidra_extract_loadgifpicture.py` 想找 5 個 `gr_pic.c` 函式（`GR_PicRead` / `GR_PortDataToBitmap` / `LoadGifPicture` / `InvertBitmap` / `PicDecompress`）的 caller — 假設「ref Borland assert 字串的 function 就是該函式自己」
- Ghidra auto-analysis **沒建立** segment 1420 assert string 的 data reference，5 個 assert string 全部 `getReferencesTo()` 回空
- 改 strategy 待續：(a) byte pattern scan code segment 找 `PUSH 0x2372` 之類 immediate；(b) 重跑 auto-analysis 加 reference walker；(c) 跳過 Ghidra 直接從 CvPc payload bytes 反推 LZW 變體（多試幾種 variant）
- 不擋 M0 / M1 / M2，M3 之前要解

## 2026-06-06 — 平行 fork 兩個 agent：spec 03 資產格式 + Team B SDL 計畫

依使用者指示同時開兩個 agent 推進：

### Agent A（general-purpose）— 資產格式與 tile 抽取（spec 03）

任務：盤點 game directory、反推 .PIC / tile 格式、寫 Python 抽 tile 出 PNG；參考 `D:\03_game_tmp\civ1_cht\` 的 Track A 既有 resource file 反推經驗。

**顛覆性發現**：1993 Windows 版**根本沒有 .PIC 檔**。資產全部打包進 **5 個 `.RSC`（Apple Mac Resource Fork 標準格式）**，呼應 [spec 01 §1.2](../team-a/specs/01_compiler_and_api_surface.md) 對「Win 版是 Mac port、`resmgr.c` 是 Mac Resource Manager port」的論斷 — 這是檔案層的直接證據。

成果：
- `team-a/specs/03_asset_formats_and_tiles.md`（23.2 KB）— 完整資產 spec
  - §3.1 game directory 33 個檔完整盤點（5 RSC + CIV.EXE + CIVFONTS.FON + CIVHELP.HLP + MPLOGO.BMP + 23 WAV）
  - §3.2 Track A 已知 vs 本次新解開的格式對照
  - §3.3 Mac Resource Fork 容器格式（big-endian header + map + type list + ref list + name list + data section）
  - §3.4 CvPc 影像 5-byte header（width:BE16 + height:BE16 + lzw_min_code:u8）
  - §3.6 tile 排列規則（待 LZW 解開後驗證）
  - §3.9 待解問題 5 條（LZW 變體、palette 來源、GDAT/KDAT 語意…）
  - §3.10 給 Team B 的 `civ_load_*` 介面契約
- `team-a/tools/extract_tiles.py`（21.2 KB）— Mac Resource Fork parser + CvPc header parser + GIF89a LZW decoder（Path A）+ Pillow giflib wrapper（Path B）+ tile slicer，CLI `--list / --all / --resource-id / --tile-size / --dump-evidence`
- `assets-extracted/_evidence/`（gitignored）3 個 evidence 檔給後續 Ghidra `LoadGifPicture` 對讀

**5 個 .RSC 內含**：199 個 CvPc 影像（地形/單位/icon/sprite sheet/領袖肖像）+ 33 STR#（string list）+ 399 TEXT（Civilopedia 段落）+ 7 GDAT + 14 KDAT。**推翻** Track A `PROJECT_MEMORY` 「裡面沒有遊戲文字」的結論 — 33 + 399 = 432 條文字資料是 Batch B/C/D/E 翻譯規模的富礦。

**未解的關鍵**：CvPc payload 是 LZW 變體（推測 RLE+LZW 混合），標準 GIF89a decoder 在 17 個 code 後撞牆（code 159 > 字典大小 130）。前 16 個 code 解出**完美正確的灰階 ramp**（`0 0 0 32 32 32 64 64 64 …`）證明 container + root code size 都對。真正解唯有 Ghidra 看 `gr_pic.c::LoadGifPicture`（[spec 01 §1.2](../team-a/specs/01_compiler_and_api_surface.md) 已確認該函式存在）。本次先 ship 已解部分，spec 03 §9.1 留給後續。

### Agent B（Plan）— Team B SDL 實作計畫

任務：吃 spec 00/01/02 + 12 個 callback decompile + WinMain chain + 12 KB LESSONS_LEARNED 規劃 SDL2 重寫架構。Plan agent 為 read-only，內容由主 session 落盤。

成果：`team-b/SDL_IMPLEMENTATION_PLAN.md`（15 章、8 個 milestone M0–M7）：

- §2 完整目錄樹（`team-b/src/` 下 30+ 個 .c/.h 模組），對應原版 `dialogs.c` / `godpal.c` / `gr.c` / `gr_pic.c` / `gr_port.c` / `init.c` / `load.c` / `mac.c` / `resmgr.c` / `shape.c` / `wdwmap.c` / `wdwsmmap.c` / `wdwstat.c` / `windows.c`；Mac shim 壓平到單層
- §3 主迴圈與 event 分派（`civ_loop` + `civ_dispatch_event`），取代原版 `while (DAT_12d8_24ee == 0) { FUN_1088_0000(); }`
- §4 widget dispatch table 模型（22-entry map + 9-entry minimap/status + lockable mode）
- §5 palette framebuffer + drawing-state 結構（取代 552 個 GDI call）
- §6 CJK 字模合成（FreeType MONO + 16×16/24×24 + Big5 byte-pair walker + palette 層合成）
- §7 資源 loader 介面（Mac Resource Fork + CvPc + STR#/TEXT，對齊 spec 03 §3.10）
- §11 i18n 含 spec 03 新發現的 432 條 STR#/TEXT
- **§12 Track A 踩雷對照表 11 條**：otvdm SEGV、CIVFONTS dfCharSet、CityView palette stomp、CJK MONO、hotkey marker、WaitTimer ×12、portable SFX、WSLg input、dialog slot length、shared menu slot、「.RSC 無文字」誤判。Track C 9 條天然避開、2 條（palette stomp、hotkey marker）必須主動處理、1 條（誤判）影響翻譯範圍規劃
- §13 M0–M7 8 個 milestone，各附 ctest 驗證點與 spec 依賴
- §14 17 條「不做的事」清單

依使用者要求 **不 commit、由我集中處理**。

## 2026-06-06 — 整合 Fandom Civ1 wiki 主頁

- 依使用者要求，抓 https://civilization.fandom.com/wiki/Sid_Meier%27s_Civilization 內容。Cloudflare 擋 WebFetch 與一般 curl；改走 Fandom MediaWiki API（`api.php?action=query&prop=revisions&...`）拿 wikitext 成功。
- 整理成 `docs/CIV1_REFERENCE.md`（中文）— 公開遊戲資訊，不屬於 clean-room spec 的一部分（Team B 實作時不直接讀此文件），可作為 Team A 寫 spec 時的 **行為層交叉參考**。
- 主頁明確的 resource format 訊息**只有一處**：開場文字存於 disk text file 可改。其餘為遊戲設計、歷史、評論。
- 整理項目：(1) binary 識別欄位 (2) 平台與版本演進 (3) 介面操作（mouse / 4 個 menu）對應 spec 02 (4) 14 個文明 + 配樂 (5) 開場 .txt 全文 (6) 設計血脈 + 未做出的版本 (Bunten / Daglow) (7) 戰鬥 / AI 隱性加成爭議 (8) 1996 CGW #1 評語 (9) 周邊作品（Master of Magic 同年同公司，技術可雙向流通） (10) IP 歷史 (11) 與本專案 spec 對照表 (12) Fandom 主頁未涵蓋 → 須其他來源補（civfanatics / strategywiki / disassembly）(13) 進一步建議抓的 sub-page。
- 工具：`team-a/notes/extract_wikitext.py` — 從 MediaWiki API JSON 抽 wikitext 的小工具。

## 2026-06-06 — Spec 02：啟動流程、game loop、11 個 callback 職責

- 寫 `team-a/tools/ghidra_extract_spec02.py` Jython post-script — 透過 Ghidra DecompInterface 對 11 個導出 callback + entry stub 各別做 decompile，輸出 `team-a/dumps/02a_<name>.c`；同時從 entry 起點走 call graph，找第一個 call `REGISTERCLASS` / `LOADMENU` / `LOADACCELERATORS` 的 function 作為 WinMain candidate，decompile 到 `02b_winmain_chain.c`。
- WinMain candidate 定位：**`FUN_1008_0000` @ `1008:0000`**（spec 02 §2.1.2）。
- 啟動流程 7 段（C runtime stub → 早期 init → 載入 sub-system → 註冊 TIMERPROC/SETTIMER/ADDFONTRESOURCE → 載 5 個 startup dialog → 載 7 組 game data → 創 3 個視窗 → 鍵盤狀態 hack → 主迴圈 → 清理）全部 walked，每段都對應到 Team B 的 SDL2 替代建議。
- 主迴圈形狀確認：`while (DAT_12d8_24ee == 0) { FUN_1088_0000(); }`，內部用 `PEEKMESSAGE`（spec 01 統計 23 次 call site，遠多於 0 次的 `GETMESSAGE`）做非阻塞 poll；idle path 推進遊戲狀態與動畫。
- 11 個 callback 各別職責整理完，最重要的幾個發現：
  - **`TIMERPROC` 只做 counter decrement**（`DAT_12d8_24f0 -= 1`）— 不 post message、不做任何工作。實際 frame timing 由 idle path 控；timer 只是精準度補丁。
  - **`WDWMAPPROC` / `WDWSMMAPPROC` / `WDWSTATUSPROC` 不用 switch，用 dispatch table**（22 / 9 / 9 個 entry 的平行陣列：`msgs[]` + `handlers[]`）。Team B 對應到 `{sdl_event_type, handler_fn}` 表。
  - `WDWMAPPROC` 有「鎖定 mode」(`DAT_12b0_0000`)：開啟時只接受 `WM_SIZE` 與 `WM_CLOSE`，其他都 DefWindowProc — 推測進入「移動單位」「選城市」這種需要 modal 互動的瞬間。
  - 4 個 RANDOM* / REGIONPROC 都是 subclass procedure — Team B 用 widget event handler 取代。
- 寫 `team-a/specs/02_startup_and_game_loop.md` 涵蓋 §2.1 啟動流程、§2.2 11 個 callback 逐一說明、§2.3 視窗-callback 對應總覽、§2.4 Team B 最小可跑 SDL2 骨架建議。

## 2026-06-06 — Spec 01：Compiler ID + API surface + 原始 source 結構

- 在 Big5-patched binary 上跑 Ghidra auto-analysis：1142 個 function、11 個 Win16 callback 全自動命名（`WDWMAPPROC` 1208:0054 / `CIVDIALOGPROC` 1098:1838 / `TIMERPROC` 1008:0d68 等）、157 個 Win16 import 自動歸到 6 個 module。
- 寫 `team-a/tools/ghidra_extract_spec01.py` — Ghidra Jython post-script，輸出 function 清單、按 segment 1608 thunk reference 計算的 per-API call site 數、entry-point 走路、編譯器簽名字串，到 `team-a/dumps/01[a-d]_*.txt`。
- 編譯器 ID 確認：**Borland C++ 1991**（segment 1420 內有字串 `'Borland C++ - Copyright 1991 Borland Intl.'`）。
- 原始 1993 source 結構從 Borland `assert()` macro 嵌入的 `"func()  :  source.c"` 字串還原：12+ 個 `.c` 檔，包含 `dialogs.c`、`godpal.c`、`gr.c`、`gr_pic.c`（含 **`PicDecompress`** = EDILZSS2 decoder）、`gr_port.c`、`init.c`、`load.c`（含 `RLLDecode`/`RLLEncode` 存檔 RLL 編碼）、`mac.c`（Mac Memory Manager shim）、`resmgr.c`（Mac Resource Manager shim）、`shape.c`、`wdwmap.c` / `wdwsmmap.c` / `wdwstat.c`（三個視窗）、`windows.c`（推測含 `WinMain` / `MenuZ`）。
- 架構發現：**1993 Windows 版本質上是 Mac 移植** — `mac.c` 有 `NewPtr` / `DisposPtr` / `HandToHand` / `DisposeHandle`（Mac Memory Manager API），`resmgr.c` 有 `OpenResFile` / `NewResource` / `AddType` / `RestoMem`（Mac Resource Manager API）。Win16 substrate 用來 emulate Mac primitive，所以 `KERNEL.GLOBALLOCK`/`GLOBALUNLOCK` 佔 KERNEL 全部 667 個 call 的 436 個（206 + 230）— 都在背後撐 Mac Handle/Pointer 抽象。Team B 的 C99 重寫把這兩層 shim 都收掉，直接寫一層。
- API call-site 統計（給 Team B 排 SDL2 取代優先序）：USER 970 / KERNEL 667 / GDI 552 / COMMDLG 4 / MMSYSTEM 4 / WIN87EM 0（runtime 沒用 — link 只是為了 `c0w` 的存在性檢查）。逐個 API 的熱度列在 spec 01。
- 寫 `team-a/specs/01_compiler_and_api_surface.md` 涵蓋編譯器 ID、原始 `.c` source 清單、Mac-port 架構發現、按 module 的 API call-site 完整表、entry-point 啟動走路。
