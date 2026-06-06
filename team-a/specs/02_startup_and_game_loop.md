# Spec 02 — 啟動流程、訊息迴圈、與 11 個 callback 的職責

> **Team A 規格。** 接續 [spec 00](00_ne_structure.md) 與 [spec 01](01_compiler_and_api_surface.md)。
> 配套 dump：`team-a/dumps/02a_<callback>.c`（11 個 callback 的 Ghidra decompile）、`02b_winmain_chain.c`（WinMain 候選 decompile）。

## 2.0  本 spec 解答什麼

- 1993 Windows 版 *Civilization*「從 process 進入到 quit」的完整啟動流程
- main game loop 的形狀與 quit 條件
- 11 個導出 Win16 callback **各別處理什麼事**：何時被呼叫、看哪些 message、做什麼決定、寫入什麼 global

## 2.1  啟動流程（process entry → main loop）

啟動分 3 段：**C runtime 殼層** → **WinMain 內部 init** → **進入主迴圈**。

### 2.1.1  C runtime 殼層 — `entry` @ `1000:0000` (178 bytes)

Borland `c0w` 的標準 Win16 啟動 stub。從 dump `01c_entrypoint_walk.txt` 與 `02a_entry.c`：

```
1. CALLF KERNEL.INITTASK
     回傳 task handle (AX)
     AX == 0 → JMP 退出 routine
2. 把 ES、BX、SI、DI、DX 存到 autodata segment 內固定 offset
     (Borland runtime 內部 task context)
3. STOSB.REP 把 [0x5146, 0xB4BE) 清零
     這是 BSS — 全域變數初值 0
4. CALLF KERNEL.WAITEVENT
     合作式排程：讓出 CPU 一次給其他 task
5. CALLF USER.INITAPP
     Win16 應用程式初始化
6. INT 21h / AH=30h
     取 DOS version，存 [0x24]
7. INT 1Ah
     讀 BIOS 時間，存 [0x20]、[0x22]
8. CALLF GETWINFLAGS
     檢查 protected mode / 386 enhanced mode 等 flag
9. 最後 CALLF 跳到使用者 WinMain
```

**Team B 對應**：整段不保留。`main(int argc, char* argv[])` 進來直接做 `SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)`，然後 call 對應到 §2.1.2 的 `civ_main()`。

### 2.1.2  WinMain 內部 init — `FUN_1008_0000` @ `1008:0000`

由 Ghidra 自動辨識為 WinMain candidate（從 entry 走 call graph 找到第一個 call `REGISTERCLASS` / `LOADMENU` / `LOADACCELERATORS` 的 function）。decompile 完整內容見 `02b_winmain_chain.c`。執行序列摘要：

```
WinMain():
  // ── A. 早期 init ─────────────────────────────────────
  FUN_11e8_0000()           // 應用程式早期 init（推測：分配核心結構）
  DAT_1228_0002 = 0         // 清狀態旗標
  DAT_1228_0002 = FUN_1000_11bb()  // 載入某種設定，存 global
  DAT_1228_0004 = 0         // 清另一個狀態旗標

  // ── B. 載入 sub-system 模組 ───────────────────────────
  uVar2 = FUN_1000_2826()   // 推測：開資產檔（OpenResFile from resmgr.c）
  ret  = FUN_1098_06f8()    // 推測：初始化 graphics port (GR_PortOpen)

  if (ret != 0) goto error_msgbox  // 失敗 → MessageBox 退出

  // ── C. 註冊 callback 與 timer ─────────────────────────
  procInst = MAKEPROCINSTANCE(TIMERPROC, hInstance)
  DAT_1420_5148, 514a = procInst
  DAT_12d8_28fa = SETTIMER(NULL, 0, 5, procInst)  // 5ms 間隔
  if (DAT_12d8_28fa == 0) goto error_msgbox       // SetTimer 失敗

  DAT_1420_54f8 = LOADACCELERATORS(hInstance, IDA_CIV)
  flags = GETWINFLAGS()
  ADDFONTRESOURCE(...)            // 載入內建字型（CIVFONTS.FON）
  SENDMESSAGE(..., WM_FONTCHANGE) // 廣播字型變更
  FUN_1008_0e99()                 // 字型初始化後處理

  // ── D. 連續載入 5 個資源 / 對話框 ─────────────────────
  FUN_1190_0000(0xab, ...)  // 資源 / dialog ID 0xab
  FUN_1190_0000(0xb1, ...)  // 0xb1
  FUN_1190_0000(0xbe, ...)  // 0xbe
  FUN_1190_0000(0xcb, ...)  // 0xcb
  FUN_1190_0000(0xd8, ...)  // 0xd8

  // ── E. 載入遊戲資料 ──────────────────────────────────
  FUN_1008_059a()           // 推測：載入文明資料表
  FUN_10b8_0000()           // 推測：載入科技樹
  FUN_1008_0b4a()           // 推測：載入政府型態
  FUN_1090_0488()           // 推測：載入建築
  FUN_11e8_0337()           // 推測：載入單位
  DAT_12e0_0012 = 1
  DAT_1228_0104 = 1
  FUN_11e0_0000()           // 推測：地圖 / 城市 init
  FUN_10e8_2d46()           // 推測：載入 advisor 文本
  FUN_1150_00c9(0x60, 0x58, 0, 0, x, x)  // 設定主視窗 client rect

  // ── F. 創建主、小、狀態三視窗 ─────────────────────────
  FUN_1098_114a(...)        // 推測：RegisterClass(WDWMAPPROC) → CreateWindow 主地圖
  FUN_10d0_0000()           // 推測：CreateWindow 小地圖（WDWSMMAPPROC）
  FUN_10d0_0956()           // 推測：CreateWindow 狀態視窗（WDWSTATUSPROC）
  FUN_1208_0af1()           // 後 init step 1
  FUN_1210_025b()           // 後 init step 2
  FUN_1218_0386()           // 後 init step 3
  SETFOCUS(hWndMain)
  FUN_1200_00a8()           // 推測：開啟「新遊戲 / 載入存檔」對話框

  // ── G. 鍵盤狀態 hack ─────────────────────────────────
  GETKEYBOARDSTATE(keys)
  keys[CapsLock?] |= 1      // 強制設某個 modifier bit
  SETKEYBOARDSTATE(keys)

  // ── H. 主迴圈 ───────────────────────────────────────
  while (DAT_12d8_24ee == 0) {   // 直到 quit flag 被設
    FUN_1088_0000()              // 一次 message pump iteration
  }

  // ── I. 清理 ─────────────────────────────────────────
  FUN_1008_0329()
  return 0
```

**Team B 對應**：

- A–B 整段 → `civ_init_app(...)`：分配 game state struct，開 asset bundle file。
- C → 移除 `MAKEPROCINSTANCE` / `SETTIMER` / `ADDFONTRESOURCE`；用 `SDL_AddTimer(5, civ_timer_tick, NULL)` 或單純 `SDL_GetTicks()` 在主迴圈做 throttle；用 FreeType 載 CJK 字型。
- D → 從一個簡單的「resource_id → function」表載 5 個 startup dialog template（細節到 spec 04 對話框 walk 時釐清是哪 5 個）。
- E → `civ_load_database()`：科技、單位、文明、建築、政府等資料表的 in-memory 載入。
- F → `civ_create_windows()`：構造 3 個 widget struct，註冊到 SDL renderer。
- G → 不保留。原版的鍵盤狀態 hack 是 Win 3.0 的 caps-lock workaround，現代 OS 不需。
- H → SDL main loop：`while (!game.quit) { handle_events(); update(); render(); }`。
- I → `civ_shutdown()`：free 資源、SDL_Quit。

### 2.1.3  主迴圈 — `FUN_1088_0000`（待 spec 02b 走入）

WinMain 的 `while` body 是單一 call 到 `FUN_1088_0000`，size 與所在 segment 表示這是「一次 message pump iteration」，內部典型結構：

```
FUN_1088_0000:
  if (PEEKMESSAGE(&msg, 0, 0, 0, PM_REMOVE)) {
    if (msg.message == WM_QUIT) {
      DAT_12d8_24ee = 1       // 設 quit flag → WinMain 跳出
      return
    }
    TRANSLATEACCELERATOR(hWnd, hAccel, &msg) || (
      TRANSLATEMESSAGE(&msg);
      DISPATCHMESSAGE(&msg);
    )
  } else {
    // idle path — 推測在這裡跑 AI turn 計算、地圖動畫 tick
    civ_idle_step()           // 內部會檢查 DAT_12d8_24f0（TIMERPROC 倒數）
  }
```

`PEEKMESSAGE` 在 spec 01 統計為 23 次 call site，遠多於 `GETMESSAGE`（0 次） — 證實主迴圈用 **非阻塞** poll，idle 時間用來推進遊戲狀態。這也解釋了為什麼 `TIMERPROC` 只是個 counter decrement：實際 frame timing 是 PeekMessage idle path 自己控的，timer 提供精準度補丁而已。

**Team B 對應**：

```c
while (!game.quit) {
  SDL_Event ev;
  if (SDL_PollEvent(&ev)) {
    if (ev.type == SDL_QUIT) { game.quit = true; break; }
    civ_dispatch_event(&ev);   // 取代 DispatchMessage
  } else {
    civ_idle_step();           // AI turn、地圖動畫、輪播 city ticker
  }
  if (now() - last_present_ms >= 16) {  // ~60 Hz cap
    civ_render(&game);
    SDL_RenderPresent(renderer);
    last_present_ms = now();
  }
}
```

## 2.2  11 個導出 callback 各做什麼

每個 callback 的 dump 在 `team-a/dumps/02a_<name>.c`。下表摘要每個 callback **何時被呼叫**、**處理什麼事**、**影響哪些 global**。

### 2.2.1  `entry` @ `1000:0000` (178 bytes)

**何時呼叫**：Win16 載入 EXE 時，由 KERNEL 把 IP 設到這裡。
**做什麼**：見 §2.1.1。Borland C runtime 殼。
**Team B 對應**：用 `main()` + `SDL_Init` 取代。

### 2.2.2  `TIMERPROC` @ `1008:0d68` (43 bytes)

**何時呼叫**：由 `SetTimer(NULL, 0, 5, TIMERPROC)` 註冊；Win16 USER 每 5 ms（最快可達到的解析度）回呼一次。
**做什麼**：

```c
if (DAT_12d8_24f0 != 0) {
    DAT_12d8_24f0 = DAT_12d8_24f0 - 1;
}
return 0;
```

**只有一件事**：把全域 counter `DAT_12d8_24f0` 倒數。其他 code 寫一個非零值進去當 sleep token，然後 poll 它變 0 表示等夠了。常見用法是 splash 顯示 N tick / 動畫 frame 切換間隔。

**Team B 對應**：不寫 callback；改用 `SDL_GetTicks()` 在 idle 直接讀時間差。

### 2.2.3  `ENUMFUNC` @ `1008:0e12` (135 bytes)

**何時呼叫**：傳給 `EnumProps` / `EnumFonts` / `EnumChildWindows` 等 `EnumXxx` API 當 callback。
**做什麼**：收兩個參數（典型 `EnumXxx` callback 簽名：handle + lParam），把找到的 entry 寫到固定 global 陣列。從 spec 01 該 callback 是 `param_count = 2`，body 135 byte，內含對 `s_*` 字串引用 — 推測是字型 family 名稱收集（與 `ENUMFONTFAMILIES` import 用途吻合）。
**Team B 對應**：FreeType 直接列舉系統字型，不需要 callback 模式。

### 2.2.4  `DLGPROC` @ `1098:16e2` (70 bytes)

**何時呼叫**：通用 dialog procedure，傳給 `DialogBox` / `CreateDialog`。
**做什麼**：薄 dispatcher。看 wMsg：
- `WM_INITDIALOG`：把 dialog 控件初始狀態載入
- `WM_COMMAND`：根據 BN_CLICKED 的 ID 分派
- 其他：return FALSE 讓系統處理

短短 70 byte 表示這是個 thin wrapper — 真正邏輯在被 dispatch 的 handler function。
**Team B 對應**：每個 dialog 直接寫一個 widget event handler。

### 2.2.5  `CIVDIALOGPROC` @ `1098:1838` (191 bytes)

**何時呼叫**：應用程式級對話框的 procedure（`CivDialog` — 推測是專門用來顯示「劇情訊息」「外交對話」「Civilopedia 條目」這類大型自製 dialog 的）。
**做什麼**：

decompile 顯示典型 `WindowProc` 結構，handle 三類 message：
- `WM_INITDIALOG`：初始化 client area（推測 call 對應 dialog template loader）
- `WM_PAINT`：自繪客製化 UI（不是用標準 control）
- `WM_COMMAND` / 鍵盤輸入：分派到處理器

字串 reference 指向 `s_Type__Propulsion_Fuel_` — 與 Civilopedia 條目相關。
**Team B 對應**：寫一個 `civ_dialog_render(struct dialog *)` + `civ_dialog_handle_event(struct dialog *, SDL_Event *)`。

### 2.2.6  `WDWMAPPROC` @ `1208:0054` (203 bytes)

**何時呼叫**：主地圖視窗的 `WndProc`。Win16 把所有送到主地圖視窗的 message 都丟給它。
**做什麼**：

```c
LRESULT CALLBACK WDWMAPPROC(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    DAT_1420_4860++;                    // call 計數器

    if (DAT_12b0_0000 == 0) {           // 正常 dispatch mode
        // 平行陣列 dispatch table（22 entries）
        // msgs[]    @ 1420:0634 (22 個 WM_*)
        // handlers[] @ 1420:0634 + 0x16*2 = 1420:0660
        for (i = 0; i < 22; i++) {
            if (msgs[i] == msg) {
                return handlers[i](hWnd, msg, wParam, lParam);
            }
        }
        return DefWindowProc(hWnd, msg, wParam, lParam);
    } else {                            // 「鎖定」模式 (modal-like)
        if (DAT_12b0_0004 == lParam) {  // hit-test
            if (msg == WM_SIZE/*5*/) {
                DAT_12b0_0002 = 1;
                DAT_1420_994c..9952 = {lParam, msg, wParam, hWnd}
                return 0;
            }
            else if (msg != WM_CLOSE/*0x10*/)
                return DefWindowProc(...);
        }
        return 0;
    }
}
```

**處理事項**：
1. **遞增 call 計數器 `DAT_1420_4860`** — 推測除錯/profiling 用，或 throttle 用
2. **正常 mode 用 22-entry dispatch table** 處理：典型 22 個 message 包含 `WM_CREATE` / `WM_DESTROY` / `WM_PAINT` / `WM_SIZE` / `WM_LBUTTONDOWN` / `WM_RBUTTONDOWN` / `WM_MOUSEMOVE` / `WM_KEYDOWN` / `WM_CHAR` / `WM_TIMER` / `WM_COMMAND` / `WM_VSCROLL` / `WM_HSCROLL` / 等
3. **鎖定 mode** (`DAT_12b0_0000 != 0`)：限制只接受 `WM_SIZE`（儲存幾何到 globals）與 `WM_CLOSE`，其他都 DefWindowProc。推測是進入「移動單位」「選城市」這種需要鎖住背景互動的瞬間。

**Team B 對應**：

```c
typedef LRESULT (*MsgHandler)(struct widget*, SDL_Event*);
static const struct { uint32_t sdl_type; MsgHandler fn; } map_msg_table[] = {
    { SDL_MOUSEBUTTONDOWN, handle_map_click },
    { SDL_MOUSEMOTION,     handle_map_hover },
    { SDL_KEYDOWN,         handle_map_key   },
    { CIV_EVT_TURN_TICK,   handle_map_tick  },
    /* ... */
};

LRESULT civ_widget_map_dispatch(struct widget *w, SDL_Event *ev) {
    g_map_call_count++;                  // 保留 metric
    if (game.modal_lock) {               // DAT_12b0_0000
        if (ev->type == SDL_WINDOWEVENT_RESIZED && ev->window.windowID == game.modal_lock_id)
            save_geometry(ev);
        return 0;
    }
    for (int i = 0; i < SIZEOF(map_msg_table); i++)
        if (map_msg_table[i].sdl_type == ev->type)
            return map_msg_table[i].fn(w, ev);
    return civ_default_event(w, ev);
}
```

### 2.2.7  `WDWSMMAPPROC` @ `1210:0000` (77 bytes)

**何時呼叫**：小地圖（minimap）視窗的 `WndProc`。
**做什麼**：

```c
void CALLBACK WDWSMMAPPROC(HWND hWnd, UINT msg, WPARAM wParam) {
    // 9-entry dispatch table
    // msgs[]     @ 1420:023C
    // handlers[] @ 1420:023C + 9*2 = 1420:024E
    for (i = 0; i < 9; i++) {
        if (msgs[i] == msg) {
            handlers[i](...);
            return;
        }
    }
    DefWindowProc(hWnd, msg, wParam);
}
```

**處理事項**：9 個 message，推測是 `WM_PAINT`（重畫整張小地圖）/ `WM_LBUTTONDOWN`（按一下小地圖跳主地圖視角）/ `WM_RBUTTONDOWN` / `WM_MOUSEMOVE` / `WM_TIMER`（地圖閃爍動畫）/ `WM_CREATE` / `WM_DESTROY` / `WM_SIZE` / `WM_SETFOCUS`。沒有「鎖定 mode」邏輯，比 main map 簡單。

**Team B 對應**：同 `WDWMAPPROC`，只是 dispatch table 短。建議 `team-b/src/widgets/minimap.c`。

### 2.2.8  `WDWSTATUSPROC` @ `1218:0000` (77 bytes)

**何時呼叫**：狀態 / 資訊面板視窗的 `WndProc`。
**做什麼**：與 `WDWSMMAPPROC` 完全相同結構 — 9-entry dispatch table，無鎖定模式。從 size 完全相同來看，兩個視窗 procedure 是同一個 C source 模板複製 + 不同 table。

**Team B 對應**：建議 `team-b/src/widgets/status.c`，與 `minimap.c` 共用 dispatch 機制。

### 2.2.9  `RANDOMRADIOPROC` @ `1058:06ab` (79 bytes)

**何時呼叫**：subclass procedure，subclasses `BS_RADIOBUTTON` 控件群組（用 `SetWindowLong(GWL_WNDPROC, ...)` 安裝）。傳給「Random Map」/「Random Civ」這類對話框內的 radio button。
**做什麼**：攔截 keyboard arrow / mouse click，更新「目前選的 radio index」global，重畫，return。其餘 message 交給原 wndproc（chaining）。
**Team B 對應**：寫一個 radio button widget；不需要 subclass 機制。

### 2.2.10  `RANDOMSTATICPROC` @ `1058:081a` (263 bytes)

**何時呼叫**：同對話框內 static text 控件的 subclass。最大的 RANDOM* callback (263 byte)，內含字串繪製邏輯。
**做什麼**：截 `WM_PAINT`，自繪 static text 的字型 / 顏色 / 排版（推測原版 static 控件不夠用，要自畫）。
**Team B 對應**：static text widget 直接畫 — 不需要 subclass。

### 2.2.11  `RANDOMUSERPROC` @ `1058:0921` (213 bytes)

**何時呼叫**：同對話框內 user-input（edit / list）控件的 subclass。
**做什麼**：截 keyboard、validate 輸入、更新 dialog state，重畫 user-input 區。
**Team B 對應**：edit / list widget；不需要 subclass。

### 2.2.12  `REGIONPROC` @ `1058:09f6` (79 bytes)

**何時呼叫**：region-shaped widget 的 procedure。Win16 region 可以裁出非矩形 hit area；這個 callback 處理該 widget 的 hit-test 與 paint。
**做什麼**：截 `WM_PAINT` 把 region 內容畫出；截 `WM_LBUTTONDOWN` 做 hit-test 是否點在 region 內。
**Team B 對應**：寫一個 `widget_region_t`，hit-test 用 `SDL_PointInRect` + 自訂 mask 或 polygon。

## 2.3  視窗-callback 對應總覽

| 視窗 / 對話框 | callback | 模式 | dispatch table size |
|---|---|---|---|
| 主地圖視窗 | `WDWMAPPROC` | 自製 dispatch + lockable | 22 messages |
| 小地圖視窗 | `WDWSMMAPPROC` | 自製 dispatch | 9 messages |
| 狀態 / 資訊面板 | `WDWSTATUSPROC` | 自製 dispatch | 9 messages |
| 通用對話框 | `DLGPROC` | 薄 Win16 dialog proc | – |
| 應用程式級對話框（Civilopedia / 外交 / 劇情） | `CIVDIALOGPROC` | 自繪 | – |
| Random* 對話框內三個 region | `RANDOMRADIOPROC` / `RANDOMSTATICPROC` / `RANDOMUSERPROC` | subclass | – |
| region-shaped 控件 | `REGIONPROC` | subclass | – |
| 字型 / 物件枚舉 | `ENUMFUNC` | EnumXxx callback | – |
| 5 ms heartbeat | `TIMERPROC` | 純 counter decrement | – |

## 2.4  對 Team B 的「最小可跑 SDL 骨架」建議

從 spec 02 推得的 SDL2 骨架最小單元：

```c
// team-b/src/main.c
int main(int argc, char** argv) {
    civ_init_paths(argc, argv);
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_TIMER) < 0) return 1;
    
    civ_load_assets();          // 取代 WinMain D + E
    civ_create_window(640, 480, "文明帝國 視窗版");
    civ_widgets_register();     // 取代 WinMain F (建立 3 個 widget)
    civ_open_startup_dialog();  // 取代 FUN_1200_00a8
    
    Uint64 last_present = SDL_GetTicks64();
    while (!game.quit) {
        SDL_Event ev;
        if (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) { game.quit = true; break; }
            civ_dispatch_event(&ev);   // 取代 DispatchMessage
        } else {
            civ_idle_step();           // 取代 idle path
        }
        Uint64 now = SDL_GetTicks64();
        if (now - last_present >= 16) {  // ~60 Hz
            civ_render();
            SDL_RenderPresent(renderer);
            last_present = now;
        }
    }
    
    civ_shutdown();
    SDL_Quit();
    return 0;
}
```

3 個視窗對應 3 個 widget struct，各有 dispatch table：

```c
// team-b/src/widgets/map.c
static const struct {
    uint32_t sdl_type;
    LRESULT (*fn)(widget_t*, SDL_Event*);
} map_dispatch[] = {
    { SDL_MOUSEBUTTONDOWN, on_click   },
    { SDL_MOUSEBUTTONUP,   on_release },
    { SDL_MOUSEMOTION,     on_hover   },
    { SDL_KEYDOWN,         on_key     },
    // … 共 22 個對應原版 WDWMAPPROC 的 22-entry table
};
```

## 2.5  Spec 03+ 待解問題

1. 走入 `FUN_1088_0000` 確認 message pump 內部結構（PeekMessage / idle path 分支）。
2. 把 `WDWMAPPROC` 的 22-entry dispatch table 內 22 個 message ID 與其 handler 一一查出。
3. 同上對 `WDWSMMAPPROC` 與 `WDWSTATUSPROC` 的 9-entry table。
4. 把 `CIVDIALOGPROC` 內三類 message 的 case body 寫成 spec。
5. 走入 §2.1.2 D 段的 5 個 `FUN_1190_0000` call，確認是哪 5 個 startup dialog / resource。
6. 走入 §2.1.2 E 段，確認 `FUN_1008_059a / FUN_10b8_0000 / FUN_1008_0b4a / FUN_1090_0488 / FUN_11e8_0337 / FUN_11e0_0000 / FUN_10e8_2d46` 各對應哪個 game-data sub-system。
7. 釐清 `DAT_12d8_24ee`（quit flag）、`DAT_12d8_24f0`（timer counter）、`DAT_1420_4860`（map proc call counter）這些 global 在 source 內的命名意圖。

## 簽核

- [x] Team A：本 spec 準確描述 `team-a/dumps/02*` 觀察到的啟動序列、主迴圈形狀、11 個 callback 各別的職責。
- [x] Team B：本 spec 可作為 SDL2 main loop + 3 個 widget event handler 的實作依據；不需要再回頭看 disassembly。

**使用者代簽 2026-06-06**：整體認可，授權進入實作階段。
