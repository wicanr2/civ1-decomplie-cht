# Team B — SDL2 實作計畫

> 本文件接續 [spec 00](../team-a/specs/00_ne_structure.md) / [spec 01](../team-a/specs/01_compiler_and_api_surface.md) / [spec 02](../team-a/specs/02_startup_and_game_loop.md) / [spec 03](../team-a/specs/03_asset_formats_and_tiles.md) 等 Team A spec，描述 Team B 在 `team-b/src/` 下從零實作 SDL2 版本《文明帝國 視窗版》的完整路徑與分階段交付目標。本文件本身不含可編譯程式碼，只描述介面、檔案組織、與 milestone。實作落地由後續 `team-b/src/**` 個別 PR 完成。

## 1. 總覽

把 1993 Win16 NE `CIV.EXE` 行為（已由 Team A 從 disassembly 反推為 spec）以 **C99 + SDL2 + SDL2_mixer + FreeType** 重新實作，內部 framebuffer 保持原版 **320×200 / 640×480 indexed 8-bit palette**，所有繪圖、混色、文字合成都在 palette 層完成；present 時用 `SDL_RenderCopy` + bicubic 放大到任意視窗大小。預期完成形態：一支可在 Windows 10/11、Linux（X11/Wayland）、macOS 直接執行的單一 binary，外帶一份 `assets/` 樹（含原版 `.RSC` 透過 spec 03 描述的 Mac Resource Fork + CvPc decoder 預先抽取出來的資源）與一份 `zh_TW` 翻譯 catalog；UI 與全部 user-facing 字串為繁體中文，可由設定切回原版英文。

## 2. 目錄結構

`team-b/src/` 下的最終 .c/.h 樹，**壓平 Mac shim**（不保留 `mac.c` + `resmgr.c` 那層 Handle/`OpenResFile` 抽象，全部用 C99 plain pointer + `fopen`/`fread`）。命名沿用 spec 01 §1.2 識別出的原始 `.c` 邊界，便於 Team A 反查 disassembly 函式時對得上：

```
team-b/
├── CMakeLists.txt
├── README.md
├── SDL_IMPLEMENTATION_PLAN.md        ← 本文件
├── assets/
│   ├── zh_TW/
│   │   ├── inline_strings.json        ← 從 Track A data/inline_translations.json 種子
│   │   └── dialog_strings.json        ← 從 Track A data/dialog_translations.json 種子
│   ├── fonts/
│   │   └── NotoSansCJKtc-Regular.otf  ← FreeType 載入
│   └── original/                      ← 使用者放 5 個 .RSC + CIV.EXE + .WAV + CIVFONTS.FON
├── tests/
│   ├── test_rsrc_fork.c               ← Mac Resource Fork parser round-trip
│   ├── test_cvpc_decode.c             ← CvPc 影像解碼
│   ├── test_palette_blit.c
│   ├── test_glyph_cache.c
│   └── test_rll_codec.c
└── src/
    ├── main.c                         ← entry (取代 c0w + WinMain)
    ├── civ_loop.c / .h                ← 主迴圈、idle、frame pacing (取代 FUN_1088_0000)
    ├── civ_game.h                     ← struct civ_game 主狀態 (收歸 1228:* / 12b0:* / 12d8:* / 1420:*)
    ├── civ_event.c / .h               ← civ_dispatch_event：把 SDL_Event 分派到 widget
    │
    ├── gfx/                           ← 對應原版 godpal.c + gr.c + gr_pic.c + gr_port.c + shape.c
    │   ├── surface.c / .h             ← civ_surface_t (8bpp indexed framebuffer)
    │   ├── palette.c / .h             ← civ_palette_t、PushPalette/PopPalette、ANIMATEPALETTE
    │   ├── port.c / .h                ← GR_Port 對應 drawing state: pen/brush/font/cursor
    │   ├── primitive.c / .h           ← LINETO/BITBLT/FILLRECT/FRAMERECT 對應 helper
    │   ├── cvpc.c / .h                ← CvPc 影像 decoder (spec 03 §3.4)
    │   ├── shape.c / .h               ← TileNew/TileDispose/PortTileBlt
    │   └── present.c / .h             ← palette FB → SDL_Texture → bicubic 放大、視窗 present
    │
    ├── text/                          ← CJK 字模 + 文字渲染（spec 01 GDI text out 對應）
    │   ├── glyph_cache.c / .h         ← FreeType MONO load + Big5 → glyph LRU cache
    │   ├── text_out.c / .h            ← TEXTOUT/DRAWTEXT/GETTEXTEXTENT/GETTEXTMETRICS 對應
    │   └── i18n.c / .h                ← 翻譯查表 (catalog → 字串 lookup chokepoint)
    │
    ├── widgets/                       ← 對應原版 wdwmap.c / wdwsmmap.c / wdwstat.c
    │   ├── widget.c / .h              ← widget_t 基底、dispatch table 機制
    │   ├── map.c / .h                 ← WDWMAPPROC + WdwMapOpen/CheckMapGrowLimit
    │   ├── minimap.c / .h             ← WDWSMMAPPROC + WdwSmMapOpen/DrawSmTiles
    │   ├── status.c / .h              ← WDWSTATUSPROC + WdwStatusOpen
    │   └── region.c / .h              ← REGIONPROC (非矩形 hit-test widget)
    │
    ├── dialog/                        ← 對應原版 dialogs.c + CIVDIALOGPROC
    │   ├── dialog.c / .h              ← civ_dialog_t + civ_dialog_open/close/handle_event/render
    │   ├── controls.c / .h            ← button/static/radio/edit/listbox widget
    │   ├── startup.c / .h             ← WinMain D 段 5 個 startup dialog (0xab/b1/be/cb/d8)
    │   ├── civilopedia.c / .h         ← CIVDIALOGPROC 自繪 dialog 路徑
    │   └── messagebox.c / .h          ← 自製 MessageBox 取代 USER.MESSAGEBOX
    │
    ├── res/                           ← 對應原版 resmgr.c + gr.c (template loader)
    │   ├── rsrcfork.c / .h            ← Mac Resource Fork parser (spec 03 §3.3)
    │   ├── loader.c / .h              ← civ_load_cvpc / civ_load_str / civ_load_text 高層 API
    │   └── catalog.c / .h             ← 已載入資源 index
    │
    ├── data/                          ← 對應原版 init.c (LoadAdvisors) + windows.c (MenuZ) 內資料
    │   ├── civs.c / .h                ← 14 文明 + 領袖名
    │   ├── tech.c / .h                ← 科技樹
    │   ├── units.c / .h               ← 單位定義
    │   ├── builds.c / .h              ← 建築 / 奇蹟
    │   ├── govs.c / .h                ← 政府型態
    │   └── advisors.c / .h            ← advisor 文本 (LoadAdvisors)
    │
    ├── world/                         ← 地圖、城市、回合
    │   ├── map.c / .h                 ← terrain grid + scroll + view
    │   ├── city.c / .h                ← 城市狀態
    │   ├── unit.c / .h                ← 單位狀態 + move
    │   ├── ai.c / .h                  ← AI turn step
    │   └── turn.c / .h                ← turn loop
    │
    ├── save/                          ← 對應原版 load.c
    │   └── civ_save.c / .h            ← CivLoadGame/CivSaveGame + RLLDecode/RLLEncode
    │
    ├── audio/                         ← 對應原版 MMSYSTEM bridge
    │   └── audio.c / .h               ← SDL2_mixer wrapper (SNDPLAYSOUND/MCISENDCOMMAND)
    │
    └── util/
        ├── rect.c / .h                ← SETRECT/OFFSETRECT/INFLATERECT/PTINRECT helper
        ├── log.c / .h                 ← 統一 log，可導 stderr / 檔案
        └── platform.c / .h            ← OS 路徑、time、視窗 hint shim
```

Mac shim 壓平的對應規則（詳 [spec 01](../team-a/specs/01_compiler_and_api_surface.md) §1.2 / §1.4.1）：

| 原版 (mac.c + resmgr.c) | Team B |
|---|---|
| `NewPtr` / `DisposPtr` | `malloc` / `free` |
| `Handle` (handle-to-handle 間接) | plain pointer，不做 compaction |
| `GLOBALLOCK` / `GLOBALUNLOCK` (合計 436 call site) | no-op，整個 lock/unlock dance 消失 |
| `OpenResFile` / `NewResource` / `RestoMem` | `res/rsrcfork.c` 直接讀 Mac Resource Fork 結構（spec 03 §3.3） |
| `HandToHand` | `memcpy` 出新 buffer |

## 3. 主迴圈與 event 分派

取代 [spec 02](../team-a/specs/02_startup_and_game_loop.md) §2.1.2 H 段 `while (DAT_12d8_24ee == 0) { FUN_1088_0000(); }` 與 §2.1.3 推測的 PeekMessage idle pump。

```c
/* team-b/src/main.c */
int main(int argc, char **argv) {
    civ_init_paths(argc, argv);              /* 解析 --data-dir / --lang / --vsync */
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) < 0) return 1;

    civ_load_assets(&game);                  /* 取代 WinMain D + E */
    civ_create_window(&game, 640, 480, "文明帝國 視窗版");
    civ_widgets_register(&game);             /* 取代 WinMain F：建立 map / minimap / status */
    civ_open_startup_dialog(&game);          /* 取代 FUN_1200_00a8 (新局精靈) */

    civ_loop(&game);                         /* 取代 WinMain H */

    civ_shutdown(&game);
    SDL_Quit();
    return 0;
}
```

```c
/* team-b/src/civ_loop.c — civ_loop() */
void civ_loop(struct civ_game *g) {
    Uint64 last_present = SDL_GetTicks64();
    Uint64 last_idle    = last_present;
    while (!g->quit) {
        SDL_Event ev;
        /* 非阻塞 poll，對應原版 PEEKMESSAGE 23 次 vs GETMESSAGE 0 次的設計 */
        if (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) { g->quit = true; break; }
            civ_dispatch_event(g, &ev);      /* 取代 TRANSLATE/DISPATCHMESSAGE */
            continue;                         /* 把佇列裡的 event 全吃完再 idle */
        }

        /* idle path — 原版用來推進 AI turn 計算、地圖動畫 tick */
        Uint64 now = SDL_GetTicks64();
        if (now - last_idle >= 1) {           /* 至少 1 ms 一次，呼應原版 5 ms TIMERPROC 解析度 */
            civ_idle_step(g);                 /* 推進 g->timer_counter (=DAT_12d8_24f0) */
            last_idle = now;
        }

        if (now - last_present >= 16) {       /* ~60 Hz cap */
            civ_render(g);
            SDL_RenderPresent(g->renderer);
            last_present = now;
        } else {
            SDL_Delay(1);                     /* 還電給 OS，不忙等 */
        }
    }
}
```

`civ_dispatch_event` 是純 C 分派器，**不模擬 Win16 message**：

```c
LRESULT civ_dispatch_event(struct civ_game *g, SDL_Event *ev) {
    /* 1. 全域熱鍵（對應 LOADACCELERATORS + TRANSLATEACCELERATOR）。 */
    if (civ_accel_try(g, ev)) return 0;

    /* 2. 找出焦點 widget。SDL window 只有一個；widget 是內部分區。 */
    struct widget *w = civ_widget_at(g, ev);
    if (!w) return civ_default_event(g, ev);

    /* 3. 委派 widget 自身的 dispatch table。 */
    return w->vt->dispatch(w, ev);
}
```

## 4. 三視窗 + dialog widget 模型

取代 spec 02 §2.2.6–§2.2.8 的 `WDWMAPPROC` (22-entry table + 鎖定模式) / `WDWSMMAPPROC` (9-entry table) / `WDWSTATUSPROC` (9-entry table) 與 §2.2.4–§2.2.5 的 `DLGPROC` / `CIVDIALOGPROC`。

```c
/* team-b/src/widgets/widget.h */
struct widget;
typedef LRESULT (*widget_handler_fn)(struct widget *w, SDL_Event *ev);

struct widget_msg_entry {
    Uint32           sdl_type;       /* 例如 SDL_MOUSEBUTTONDOWN */
    widget_handler_fn fn;
};

struct widget_vtable {
    LRESULT (*dispatch)(struct widget *w, SDL_Event *ev);
    void    (*render)(struct widget *w, struct civ_port *port);
    void    (*destroy)(struct widget *w);
};

struct widget {
    const struct widget_vtable *vt;
    SDL_Rect                    rect;     /* 取代 GETCLIENTRECT/GETWINDOWRECT */
    bool                        visible;  /* 取代 SHOWWINDOW */
    bool                        focused;
    void                       *state;    /* 各 widget 私有狀態 (取代 SETWINDOWLONG/WORD) */
};
```

主地圖 widget 對應原版 WDWMAPPROC 的 22-entry table + 鎖定模式（[spec 02 §2.2.6](../team-a/specs/02_startup_and_game_loop.md) 已給出 pseudo-code）：

```c
/* team-b/src/widgets/map.c */
static const struct widget_msg_entry map_dispatch[] = {
    { SDL_MOUSEBUTTONDOWN,  on_map_click },
    { SDL_MOUSEBUTTONUP,    on_map_release },
    { SDL_MOUSEMOTION,      on_map_hover },
    { SDL_KEYDOWN,          on_map_key },
    { CIV_EVT_TURN_TICK,    on_map_tick },         /* SDL_RegisterEvents() 註冊 */
    { CIV_EVT_REPAINT,      on_map_repaint },
    /* ... 共 22 條，對齊原版 1420:0634 那張 dispatch table */
};

static LRESULT map_dispatch_fn(struct widget *w, SDL_Event *ev) {
    struct map_state *s = w->state;
    s->call_count++;                              /* 對應 DAT_1420_4860 計數器 */

    if (game.modal_lock) {                        /* 對應 DAT_12b0_0000 */
        /* 鎖定模式：只接受 RESIZE/CLOSE，其他都當預設 */
        if (ev->type == SDL_WINDOWEVENT &&
            ev->window.event == SDL_WINDOWEVENT_RESIZED &&
            game.modal_lock_id == ev->window.windowID) {
            save_modal_geometry(s, ev);           /* 對應 DAT_1420_994c..9952 */
            return 0;
        }
        return civ_default_event(&game, ev);
    }

    for (size_t i = 0; i < ARRAY_LEN(map_dispatch); i++)
        if (map_dispatch[i].sdl_type == ev->type)
            return map_dispatch[i].fn(w, ev);
    return civ_default_event(&game, ev);
}
```

`minimap.c` 與 `status.c` 共用同樣機制，dispatch table 短到 9 entry，**沒有鎖定模式**（spec 02 §2.2.7 / §2.2.8 確認兩個 callback 結構完全相同，77 byte body）。

Dialog 採同樣模式但不走 widget 鏈，而是進入一個 modal-ish 副迴圈：

```c
/* team-b/src/dialog/dialog.h */
struct civ_dialog {
    const struct widget_vtable *vt;
    SDL_Rect          rect;
    struct widget    *controls[CIV_MAX_DLG_CTRL];   /* button/static/radio/edit/listbox */
    size_t            n_controls;
    int               focus;
    int             (*on_command)(struct civ_dialog *, int ctrl_id, int code);
    bool              done;
    int               result;
};

int civ_dialog_run(struct civ_dialog *dlg);   /* 阻塞，回 OK/Cancel；內部仍 PollEvent 不卡住主畫面 timer */
```

`civ_dialog_run` 內部仍呼叫 `civ_idle_step`、`civ_render`、`SDL_RenderPresent`，保持 60 Hz；只是 input 全部走進 dialog 的 controls 而非地圖 widget。

## 5. Palette framebuffer + 繪圖層

取代 `godpal.c` / `gr_port.c` / [spec 01 §1.4.3](../team-a/specs/01_compiler_and_api_surface.md#143-gdi--552-call-site) 的 552 個 GDI call。

```c
/* team-b/src/gfx/surface.h */
typedef struct civ_surface {
    int           w, h;
    int           pitch;
    uint8_t      *pixels;          /* 8 bpp indexed，row-major */
    SDL_Rect      clip;
} civ_surface_t;

civ_surface_t *civ_surface_new(int w, int h);
void           civ_surface_free(civ_surface_t *s);
void           civ_surface_clip_set(civ_surface_t *s, SDL_Rect r);
```

```c
/* team-b/src/gfx/palette.h */
typedef struct civ_palette {
    SDL_Color entries[256];
    int       generation;          /* 每改一次 +1，供 present 偵測是否要重算 SDL_Texture */
} civ_palette_t;

/* 對應 godpal.c：InitGodPalette/FreeGodPalette/PushPalette/PopPalette/ZeroGodPalette */
void civ_palette_init(civ_palette_t *p);
void civ_palette_push(civ_game_t *g);                  /* PUSH 進 stack */
void civ_palette_pop(civ_game_t *g);                   /* POP 回去 */
void civ_palette_animate(civ_palette_t *p,             /* 對應 ANIMATEPALETTE (4 calls) */
                         int start, int count,
                         const SDL_Color *src);
```

Drawing state（對應 GDI 的 `SELECT`/`DELETE`/`MOVETO` 等合計 116 + 72 + 17 = 205 個 call site 全部收進這結構）：

```c
/* team-b/src/gfx/port.h */
typedef struct civ_port {
    civ_surface_t *target;
    civ_palette_t *palette;

    /* 對應原版用 SELECTOBJECT 套用的 4 種 GDI 物件 */
    struct civ_pen   *pen;
    struct civ_brush *brush;
    struct civ_font  *font;

    /* 對應 SETBKMODE / SETTEXTCOLOR / SETBKCOLOR / SETTEXTALIGN */
    uint8_t  text_color;       /* palette index，不是 RGB */
    uint8_t  bk_color;
    uint8_t  bk_mode;          /* TRANSPARENT / OPAQUE */
    uint8_t  text_align;       /* LEFT/RIGHT/CENTER + TOP/BOTTOM/BASELINE */

    /* 對應 MOVETO cursor */
    int      cx, cy;
} civ_port_t;
```

繪圖 helper 一一對應原版 GDI primitive：

| 原版 GDI | Team B helper | 說明 |
|---|---|---|
| `LINETO` (31) | `civ_port_line_to(port, x, y)` | 在 palette FB 上 Bresenham |
| `MOVETO` (17) | `civ_port_move_to(port, x, y)` | 純改 `cx/cy` |
| `BITBLT` (26) | `civ_port_blit(dst, dst_rect, src, src_rect, rop)` | 8 bpp → 8 bpp surface blit |
| `FILLRECT` (63) | `civ_port_fill_rect(port, rect, brush)` | 填 brush 顏色 |
| `FRAMERECT` (24) | `civ_port_frame_rect(port, rect, brush)` | 框線 |
| `TEXTOUT` (15) | `civ_port_text_out(port, x, y, str, len)` | 走 text/text_out.c |
| `SETPIXEL` (2) / `GETPIXEL` (2) | `civ_port_set_pixel` / `civ_port_get_pixel` | 直接 indexed access |
| `BEGINPAINT`/`ENDPAINT`/`GETDC`/`RELEASEDC`/`CREATECOMPATIBLEDC`/`DELETEDC` | **no-op** | palette FB 永遠可寫，不需要 DC lifecycle |
| `ANIMATEPALETTE` (4) | `civ_palette_animate` | 改 palette 後 generation++ |

Present 路徑（`gfx/present.c`）：

1. 主迴圈 60 Hz tick 時，若 `framebuffer->dirty || palette->generation` 變化，把 8 bpp FB + palette 展開到一張 RGBA `SDL_Texture`。
2. `SDL_RenderSetLogicalSize(renderer, 640, 480)` + bicubic filter（`SDL_HINT_RENDER_SCALE_QUALITY=2`）+ `SDL_RenderCopy` 到任意視窗 size。
3. 解析度切換（320×200 ↔ 640×480）只改 framebuffer 大小，不重建 renderer/window。

## 6. CJK 字模合成

取代 `ADDFONTRESOURCE` + `CIVFONTS.FON` 與 GDI text out 路徑。**所有字模在 palette 層合成**，不交給 SDL2_ttf 跑 alpha blending。

設計：

```c
/* team-b/src/text/glyph_cache.h */
struct civ_glyph {
    int       w, h;             /* 內文 16×16 / 標題 24×24，monospaced for CJK */
    int       advance_x;
    int       bearing_x, bearing_y;
    uint8_t  *mask;             /* w*h 個 byte：0 或 1 (FT_LOAD_TARGET_MONO 展開) */
};

struct civ_font {
    int                size_px;          /* 16 或 24 */
    int                ascent, descent;  /* 對應 GETTEXTMETRICS */
    int                space_w;
    FT_Face            ft_face;          /* Noto Sans CJK TC */
    /* LRU cache：key = (codepoint, size)，value = civ_glyph* */
    struct lru_cache  *cache;
};

const struct civ_glyph *civ_glyph_get(struct civ_font *font, uint32_t codepoint);
```

Big5 → glyph mapping：

1. 輸入字串視為 Big5（與翻譯 catalog 編碼一致；catalog 內譯文已是 CC BY-SA Track A `data/inline_translations.json` Big5 encode）。
2. `text_out.c` 內 byte-pair walker：高 byte ∈ 0xA1..0xFE → 與下一 byte 組合查 Big5 → Unicode 表，餘下 ASCII 路徑（與原版 dfCharSet 0x88 byte-pair walking 行為一致）。
3. `civ_glyph_get` 用 Unicode codepoint 透過 FreeType `FT_Load_Char` + `FT_LOAD_TARGET_MONO` 取 1-bit bitmap（**LESSONS_LEARNED 雷區 #6**：CJK 用 mono 不用灰階+門檻）。
4. 合成到 palette FB：mask=1 寫 `port->text_color`，mask=0 視 `bk_mode`：`OPAQUE` → 寫 `bk_color`，`TRANSPARENT` → 跳過。
5. `GETTEXTEXTENT` / `GETTEXTMETRICS` 透過 cache 累計 advance、回傳 16/24 高度。

字型分兩個 `civ_font` 實例：
- **內文 16×16** 對應原版 `CIVTIMES12`（12pt 點陣大致對齊 16 px）
- **標題 24×24** 對應 `CIVTIMES24`

裝飾字（CIVBABYLON / CIVZULU / … 14 個文明命名橫幅）以**獨立資產**處理：M5+ 階段若要重現，做 14 張預渲染 CvPc 取代字型；近期 milestone 內以 24×24 統一中文字型顯示，視覺差異記入 `docs/PARITY.md`。

## 7. 資源 loader 介面

取代 `resmgr.c` + `gr_pic.c` 的 `OpenResFile`/`LoadResource`/`LoadGifPicture` 路徑。Mac Resource Fork 容器與 CvPc header 已由 [spec 03](../team-a/specs/03_asset_formats_and_tiles.md) 完整描述；CvPc 內的 LZW 變體待 spec 03 §9 補完才能完整 decode。

```c
/* team-b/src/res/rsrcfork.h — Mac Resource Fork 解析 (spec 03 §3.3) */
struct rsrc_fork;
struct rsrc_entry {
    uint32_t type;   /* fourCC，例如 'CvPc' / 'STR#' / 'TEXT' / 'GDAT' / 'KDAT' */
    uint16_t id;     /* resource id (1-based) */
    const char *name;/* optional, NULL if no name */
    const uint8_t *data;
    size_t   len;
};
struct rsrc_fork *rsrc_fork_open(const char *path);
const struct rsrc_entry *rsrc_fork_find(struct rsrc_fork *r, uint32_t type, uint16_t id);
size_t rsrc_fork_count(struct rsrc_fork *r, uint32_t type);  /* 例：CvPc 個數 */
void rsrc_fork_close(struct rsrc_fork *r);

/* team-b/src/res/loader.h — 高層 API */
int civ_load_cvpc(const struct rsrc_entry *e,
                  civ_surface_t **out_surf,
                  civ_palette_t  *opt_pal);          /* spec 03 §3.4 + §3.6 */

int civ_load_str_list(const struct rsrc_entry *e,
                      char ***out_strings, size_t *n); /* spec 03 STR# */

int civ_load_text(const struct rsrc_entry *e,
                  char **out_text, size_t *len);      /* spec 03 TEXT */

/* M3 之前 civ_load_cvpc 走 stub：載入 Track A 抽出的預解 raw bytes 跳過 LZW */
```

## 8. 存讀檔（load.c → civ_save.c）

取代 `CivLoadGame` + `RLLDecode`/`RLLEncode`。Track A spec 04 預計描述 RLL（run-length-with-literal）格式。

```c
/* team-b/src/save/civ_save.h — 介面，M0 以 stub 提供 */
int civ_save_game(const struct civ_game *g, const char *path);
int civ_load_game(struct civ_game *g, const char *path);

int civ_rll_decode(const uint8_t *src, size_t src_len, uint8_t **out, size_t *out_len);
int civ_rll_encode(const uint8_t *src, size_t src_len, uint8_t **out, size_t *out_len);
```

M0–M5 期間用簡易自訂二進位格式承載 `struct civ_game` 子集；M6 才落原版 RLL，並在 `tests/test_rll_codec.c` 做 round-trip。

## 9. 音效（SDL2_mixer）

對應 [spec 01 §1.4.5](../team-a/specs/01_compiler_and_api_surface.md#145-mmsystem--4-call-site) 的 4 個 MMSYSTEM call site：

| 原版 | Team B |
|---|---|
| `SNDPLAYSOUND(name, SND_ASYNC)` × 2 | `Mix_PlayChannel(-1, civ_audio_load_wav(name), 0)` |
| `MCISENDCOMMAND(MCI_OPEN/PLAY/STOP, ...)` × 2 | `Mix_PlayMusic(civ_audio_load_music(name), -1)` / `Mix_HaltMusic` |

```c
/* team-b/src/audio/audio.h */
int   civ_audio_init(void);
void  civ_audio_shutdown(void);
void  civ_audio_play_sfx(const char *name);       /* SND_ASYNC 對應 */
void  civ_audio_play_music(const char *name);     /* CD-audio / MIDI 對應；MIDI 用 SDL2_mixer 內建 timidity */
void  civ_audio_stop_music(void);
```

MIDI 路徑：`SDL2_mixer` 內含 `MUS_MID`（Timidity / native MIDI），spec 03 已確認 `CIVDATA1.RSC` 內 CvPc payload 含 MIDI（`MThd/MTrk`）；先 stub，M3+ 補解析。**踩雷**：otvdm v0.9.0 的 `sndPlaySound` SEGV bug（見 §12）— 與 Track C 無關，因為 Track C 完全不經 Win16 MMSYSTEM。

23 個 .WAV 是標準 RIFF WAVE（spec 03 §3.1），直接 `Mix_LoadWAV` 即可。

## 10. State 與 globals 收歸

原版 1228:* / 12b0:* / 12d8:* / 1420:* 上百個 global 收成一個主結構，避免 free-floating globals。

```c
/* team-b/src/civ_game.h — 摘要，完整欄位隨 milestone 擴充 */
struct civ_game {
    /* 主旗標 */
    bool        quit;                /* 原 DAT_12d8_24ee：主迴圈 quit flag */
    uint32_t    timer_counter;       /* 原 DAT_12d8_24f0：TIMERPROC sleep token */
    uint64_t    map_dispatch_count;  /* 原 DAT_1420_4860：metric */

    /* SDL */
    SDL_Window      *window;
    SDL_Renderer    *renderer;
    SDL_Texture     *present_tex;

    /* 繪圖層 */
    civ_surface_t   *framebuffer;    /* 320x200 / 640x480 indexed */
    civ_palette_t    cur_palette;
    civ_palette_t    palette_stack[CIV_PAL_STACK_DEPTH];  /* PushPalette/PopPalette */
    int              palette_stack_top;
    civ_port_t       port;           /* default drawing port */

    /* 視窗 / widget 樹 */
    struct widget   *map_w;
    struct widget   *minimap_w;
    struct widget   *status_w;
    struct widget   *focused_w;

    /* Modal lock 對應 DAT_12b0_0000 / 12b0_0004 */
    bool             modal_lock;
    uint32_t         modal_lock_id;

    /* Dialog stack */
    struct civ_dialog *dlg_stack[CIV_DLG_STACK_DEPTH];
    int                dlg_stack_top;

    /* 翻譯 catalog */
    struct civ_i18n *i18n;

    /* 資料表（M4+） */
    struct civ_civs    *civs;        /* 14 文明 */
    struct civ_tech    *tech_tree;
    struct civ_units   *units;
    struct civ_builds  *builds;
    struct civ_govs    *govs;
    struct civ_advisor *advisors;

    /* 世界狀態（M5+） */
    struct civ_world   *world;
    struct civ_player   players[CIV_MAX_PLAYERS];
    int                 cur_player;
    int                 turn;

    /* 音效 */
    struct civ_audio   *audio;
};
```

Spec 02 §2.5 待解問題 #7 中 Team A 會回頭把 `1228:*` 等 global 對應到原始 source 變數名；屆時 Team B 對齊欄位命名（例：`DAT_12d8_24ee` → `quit`）。

## 11. i18n 介面

```c
/* team-b/src/text/i18n.h */
typedef struct civ_i18n civ_i18n_t;

civ_i18n_t *civ_i18n_open(const char *lang_dir);  /* lang_dir = "assets/zh_TW" */
const char *civ_i18n_lookup(civ_i18n_t *t, const char *key);  /* key 找不到回 NULL */
void        civ_i18n_close(civ_i18n_t *t);

/* chokepoint macro：所有 user-facing 文字一律走 _T() */
#define _T(s)  civ_i18n_or(s, civ_i18n_lookup(game.i18n, (s)))
```

種子 catalog：複製 Track A 的 `data/inline_translations.json` + `data/dialog_translations.json`，授權 CC BY-SA 4.0；在 `assets/zh_TW/inline_strings.json` / `dialog_strings.json` 各為一份 flat `{"English source": "中文翻譯"}` JSON。**chokepoint 在 `text/text_out.c::civ_port_text_out`**：先 lookup 翻譯、找不到回原文（**LESSONS_LEARNED #7** 規則：「無譯回退」）。

**重要新增**：[spec 03 §3.1](../team-a/specs/03_asset_formats_and_tiles.md) 發現 `Civdata0.RSC` 內含 **33 個 STR#（string lists）+ 399 個 TEXT 段落**，是 Civilopedia 內文 + 翻譯 table 的富礦；Batch B/C/D/E 翻譯範圍要把這 432 條納入而非只看 inline ASCII 字串。`i18n` 載入流程：(1) inline_strings.json (2) dialog_strings.json (3) Civdata0 STR#/TEXT key-value 對照（key = "STR#/<id>/<index>" 或 "TEXT/<id>"）。

**hotkey marker**：LESSONS_LEARNED #7 提到原版選單字串會含 `\x8f<ch>` hotkey 標記，需先剝掉再 lookup。`i18n_lookup` 內加同樣的 `civ_strip_hotkey` 預處理。

## 12. Track A 踩雷對照

下表逐條對照 [LESSONS_LEARNED](../../civ1_cht/docs/LESSONS_LEARNED.md) 與 [TRACK_A_README](../../civ1_cht/docs/TRACK_A_README.md) 中累積的雷。Track C 走 SDL 重寫，**多數天然避開**，但避開的原因要明確寫出來，避免新踩雷。

| # | Track A 踩過的雷 | Track C 會不會踩 | 原因 |
|---|---|---|---|
| 1 | **otvdm v0.9.0 `sndPlaySound` SEGV**（Track A Phase 5 BLOCKER，使 Win10 portable build 無法執行；upstream `cd84ae2` 已修但 release 未發） | **否** | Track C 不經 Win16 MMSYSTEM，也不靠 otvdm 載 NE binary。`audio/audio.c` 直接呼叫 SDL2_mixer，原生 Win10/11/Linux/macOS 執行。|
| 2 | **CIVFONTS.FON `dfCharSet=0x88` + FontSubstitutes 雙層 fix**（Phase 2，需在 OS layer 裝 `fonts-arphic-uming` 並改 registry 才能讓 GDI 在 CreateFont 時 rewrite 到 Big5 字型） | **否** | Track C 自己畫字模，FreeType 直接讀 Noto Sans CJK TC，bytes → glyph → palette FB 全程在 process 內，不依賴 OS GDI、不需要 charset code 0x88。|
| 3 | **CityView palette stomp**（LESSONS_LEARNED #5：TER257 把 CBACK 棕地索引蓋掉導致整片發灰；解法 per-pixel RGB-nearest remap LUT） | **會踩**（需主動處理）| Track C 也是多 .PIC / CvPc 各帶自己 palette 的設計（同 1991 DOS 慣例）。`civ_palette_push/pop` API 已準備好，但**只是不蓋的紀律機制**；M5 地圖繪製時若 terrain palette 與 base palette 衝突，仍需 LUT 重映射。寫入 `M5 驗證點`：CBACK + TER257 同時上時要看到雲彩 + 棕地完整。|
| 4 | **CJK glyph `FT_LOAD_TARGET_MONO`**（LESSONS_LEARNED #4：灰階+門檻會產生破碎斜線片）| **否，因為已內建 mono 設計** | §6 已釘死 `FT_LOAD_TARGET_MONO`，glyph cache 存 1-bit mask。|
| 5 | **MenuBox hotkey marker `\x8f<ch>` 黏在 label 後沒剝**（LESSONS_LEARNED #10：splitMenuItems 沒剝） | **會踩** | `i18n_lookup` 必須在查表前剝 hotkey marker；§11 已寫入 `civ_strip_hotkey` 預處理需求。|
| 6 | **WaitTimer 沒乘 12**（LESSONS_LEARNED #10：C# 是 ms × 12 我們忘記乘） | **否** | Track C 不從 OpenCiv1 / C# 衍生（clean-room）；timer 直接用 `SDL_GetTicks64`，沒有任何 platform 倍率。原版 5 ms TIMERPROC 解析度由我們自己選 1 ms idle step + 60 Hz present 重新定義。|
| 7 | **Win10 portable SFX 5.21 MB 打包**（Phase 5 deliverable，需 7z.sfx + otvdm runtime + 遊戲檔 + launcher.bat + 字型 subst registry 注入）| **天然避開** | Track C 是原生可執行檔（C99 + SDL2）+ assets 樹。Windows 端打成 `civ1.exe` + DLL 同捆即可，沒有 16-bit runtime 問題。`docs/PACKAGING.md` 之後補。|
| 8 | **WSLg / winevdm input pipeline 不通**（Phase 3 視覺驗證 BLOCKER：xdotool click/key 被吞）| **否** | Track C 是原生 SDL 應用，input 走 SDL_Event；headless 驗證可在 ctest 直接送 `SDL_PushEvent`，不需要 X11 自動化。|
| 9 | **dialog slot length 約束**（Phase 1 RT_DIALOG patch：Big5 譯文必須 ≤ 原 byte 長度 + ASCII space pad）| **否** | 沒有 NE RT_DIALOG resource；我們的 dialog 全自製，labels 是 `_T("English")` 走 i18n catalog，譯文長度沒有 byte 約束。|
| 10 | **inline string `_Start a New Game_Load a Saved Game_...` 共享 slot**（Phase 3：用 `_` 分隔多條 menu item 包在一條字串，patch 時要對齊原 byte）| **否** | Track C 把每條 menu item 作為獨立 catalog key（已在 Track A `inline_translations.json` 拆好）。|
| 11 | **Track A 誤判「.RSC 裡面沒文字」**（PROJECT_MEMORY 記載）| **影響翻譯範圍** | spec 03 §3.1 已推翻：Civdata0 內 33 STR# + 399 TEXT 是 Civilopedia + 翻譯範圍的富礦。Batch B 翻譯規劃要把這 432 條納入。|

## 13. Milestone 階段

每個 milestone 都附**驗證點**（headless ctest，**不開視窗**；視覺 smoke 走 docker 隔離）。

### M0 — repo skeleton + CMake + SDL 開視窗
**範圍**：`team-b/CMakeLists.txt` + `main.c` + `civ_loop.c` 最薄版本：開 640×480 SDL window、清 palette FB 為 index 0、present 灰底、按 ESC/視窗關閉退出。
**驗證點**：
- `cmake --build` 三平台（win32/MinGW、linux gcc、macOS clang）零警告。
- `ctest` 內含 `test_window_lifecycle.c`：headless 模式下 `SDL_INIT_VIDEO` 用 dummy driver，`civ_loop` 收到 push 的 `SDL_QUIT` 後 1 tick 內結束。

### M1 — palette framebuffer + CJK 字模合成 + ASCII text out
**範圍**：`gfx/surface.c` + `gfx/palette.c` + `gfx/port.c` + `gfx/primitive.c` + `gfx/present.c` + `text/glyph_cache.c` + `text/text_out.c`。一張 640×480 indexed FB，畫 LINETO + FILLRECT + TEXTOUT "文明帝國 Civilization"。
**驗證點**：
- `test_palette_blit.c`：FILLRECT 寫入後 `civ_surface_get_pixel` 回正確 index；ANIMATEPALETTE 改 entry 後 generation++。
- `test_glyph_cache.c`：Big5 byte pair `0xA4 0xE5`（「文」）+ Unicode `0x6587` 回同一 glyph；mask w/h = 16/16；LRU evict 後重 load 還是同 hash。
- smoke：docker 內跑 60 frame，把第 30 frame dump 成 PNG，pixel-perfect 對 `tests/golden/m1_text.png`（含「文明帝國 Civilization」字樣）。

### M2 — 3 個 widget skeleton（map / minimap / status）+ main loop
**範圍**：`widgets/widget.c` + `widgets/map.c` + `widgets/minimap.c` + `widgets/status.c` + `civ_event.c`。3 個 widget 內部分區 layout 640×480（主地圖左 + 小地圖右上 + 狀態欄右下，pixel 座標仿原版）；各畫不同 background colour、響應 mouse hover 改 cursor。
**驗證點**：
- `test_widget_dispatch.c`：push `SDL_MOUSEMOTION` 到主地圖區，`map_w->state->call_count == 1`、`minimap_w->state->call_count == 0`。
- modal_lock=true 後 mouse click 不會進 widget handler。

### M3 — 載入 .RSC + CvPc decode + blit
**範圍**：`res/rsrcfork.c` + `gfx/cvpc.c` 真實 CvPc decoder（從 [spec 03 §3.4 + §3.6](../team-a/specs/03_asset_formats_and_tiles.md) 落地，待 LZW 變體解開）。先以 `Civdata0.RSC` 內較小 CvPc（如 NUKE1 / SPY）為目標。
**驗證點**：
- `test_rsrc_fork.c`：對 `Civdata0.RSC` 解出的 type/id list 與 spec 03 §3.1 對齊（CvPc=4, STR#=33, TEXT=399, GDAT=7, KDAT=14）。
- `test_cvpc_decode.c`：CvPc id 200 (`SPR32X32.GIF`) decode 後 width=1472, height=400, bpp 已知；解出 pixel buffer 對 Track A 已 ground-truth raw 比對 byte-for-byte。
- smoke：M2 視窗 + `civ_load_cvpc(SPR32X32)` blit 到主地圖區，docker frame dump 對 `tests/golden/m3_sprites.png`。

> **依賴**：M3 啟動前 spec 03 §9.1 LZW 變體必須解開。若 LZW 卡關，M3 改用 stub 載 Track A 預解 raw bytes 跳過 decoder，M3-complete 算「rsrc fork + CvPc header parser 通」即可，CvPc 內容 blit 延後到 spec 03 補完。

### M4 — 載入 14 文明資料 + 「新局精靈」dialog
**範圍**：`data/civs.c` + `dialog/dialog.c` + `dialog/controls.c` + `dialog/startup.c`。對應 spec 02 §2.1.2 D 段的 5 個 startup dialog（ID 0xab/b1/be/cb/d8）— 等 Team A spec 02b 確認對應哪 5 個後對齊。M4 先做「Difficulty Level」+「Pick your tribe / 14 文明選擇」+「Player name」三個 dialog 串成 wizard。
**驗證點**：
- `test_dialog_state.c`：模擬 keyboard 上下選擇 radio、按 Enter 推進到下一頁、按 ESC 回上一頁；狀態機進 `game.players[0]`。
- smoke：完成 wizard 後 `game.players[0].civ_id == CIV_BABYLON`、`leader == "智者所羅門"`。

### M5 — 地圖視窗 paint terrain + scroll + cursor
**範圍**：`gfx/shape.c`（TileNew/PortTileBlt）+ `world/map.c` + `widgets/map.c` 真實 paint。先用 spec 03 解出的地形 CvPc（在 `Civdata3.RSC` 內推測）+ 一張預先生成的測試地圖。處理 **#3 palette stomp 雷**：CBACK + terrain CvPc 同時上時做 RGB-nearest LUT。
**驗證點**：
- `test_palette_remap.c`：給定 CBACK palette + terrain CvPc palette，每個 terrain index 找到 CBACK 內最近色，LUT 256 entries 預算。
- smoke：地圖滾動、cursor 移動、選格高亮，frame dump 對 `tests/golden/m5_map.png`；目視確認沒有「整片發灰」。

### M6 — turn loop + AI move + 存讀檔
**範圍**：`world/turn.c` + `world/ai.c` + `world/unit.c` + `save/civ_save.c` 含 `RLLDecode`/`RLLEncode`。
**驗證點**：
- `test_rll_codec.c`：對已知 byte sequence round-trip。
- `test_turn_loop.c`：玩家結束回合 → AI 走子 → 回合計數 +1；存檔 → 重啟 process → 載入 → state 等價。

### M7 — 奇蹟、外交、勝利條件
**範圍**：`data/builds.c`（奇蹟資料）+ 外交 dialog（走 `CIVDIALOGPROC` 對應的 `dialog/civilopedia.c` 自繪路徑）+ 勝利判定。
**驗證點**：
- `test_diplomacy.c`：宣戰 / 和約狀態轉移。
- `test_victory.c`：太空船發射 / 征服勝利條件觸發。

## 14. 不做的事

明確列出延後 / 棄做，避免 milestone 失焦：

| 項目 | 為什麼不做 |
|---|---|
| **Compaction / movable handle** | spec 01 §1.4.1 已論證：原版 436 個 GLOBALLOCK/UNLOCK 是 Mac shim 痕跡，flat address space 直接 no-op。 |
| **Mac Handle 抽象 (`HandToHand` 等)** | spec 01 §1.2 已論證壓平。 |
| **Win16 cooperative scheduler `WAITEVENT`** | 只 1 個 call site，沒有任何遊戲邏輯依賴 yield；移除。 |
| **otvdm 相容性 / Win16 NE 載入** | Track C 是原生 binary，不跑 NE。Track A Phase 5 SEGV 雷天然規避。 |
| **wine font substitution / `dfCharSet=0x88` patch** | Track C 自己畫字模，不走 GDI。 |
| **`CIVFONTS.FON` 21 個 RT_FONT 字模還原** | M1–M7 範圍內統一用 Noto Sans CJK TC + 24×24 標題替代；裝飾字（14 個文明橫幅）以預渲染 CvPc 處理，不還原 .FON 內部 RT_FONT。 |
| **CIVHELP.HLP（Microsoft WinHelp 3.x）** | 1993 .HLP 是 OLE compound file 格式，Windows 10/11 已不支援；以 in-process Civilopedia widget 取代。 |
| **`RT_CURSOR` / `RT_ICON` 還原** | 用 SDL 載 `.cur` / `.ico` 取代；16 個 cursor 用 `SDL_CreateCursor` 從 palette FB 切片，不還原 NE resource。 |
| **`PicDecompress` (EDILZSS2 外包裝)** | spec 03 §3.1 確認：1993 Win 版資產**已經是解過 EDILZSS2 的狀態**直接 ship；除非要重新打包 Track A 的 `.EX$`/`.RM$` 壓縮版（不打算），否則不需要 EDILZSS2 encoder/decoder。 |
| **`PicDecompress` 中 GIF code path** | spec 01 §1.2 提到 `gr_pic.c` 也有 GIF；spec 03 §3.4 也確認 CvPc 用 GIF-LZW 變體。Track C 只實作 CvPc 內的 LZW（spec 03 §3.4），不獨立支援標準 .gif file。 |
| **CD-Audio MCI** | 若資產實際是 CD-DA，先 stub；M7+ 視需求補。MIDI 已內含於 CvPc payload（spec 03）。 |
| **`COMMDLG.GETOPENFILENAME` 樣式 file picker** | 自製 SDL dialog（同其他 UI）。 |
| **多 SDL window** | 單視窗 + 內部 widget 分區（與 `widget.rect` 對齊），不開第二個 OS window。 |

## 15. 與既有 spec 簽核流程銜接

1. **本計畫 PR**：Team B 在本文件落地（PR #N）。使用者 review 後勾選 sign-off footer。
2. **同時 / 之後 Team A 補 spec 03+**：
   - spec 03：✅ 已存在 — 資產 file format（Mac Resource Fork + CvPc + STR#/TEXT/GDAT/KDAT）；待補 CvPc LZW 變體 § 9.1（需 Ghidra 看 `LoadGifPicture`）+ palette 來源 §9.2
   - spec 04：24 個 RT_DIALOG 各 control 表 + dialog procedure 對應 + 5 個 startup dialog ID 對應
   - spec 05：14 文明 / 科技樹 / 單位 / 建築 / 政府資料表 layout
   - spec 06：地圖格式、save file 結構（含 RLLDecode/RLLEncode worked example）
   - spec 07：戰鬥 RNG、外交、勝利條件
3. **Team B milestone 落地**：M0/M1/M2 不需要 spec 04+，可立即動工。M3 待 spec 03 §9.1 LZW 補完。M4 待 spec 04+05。M5 待 spec 06。M6 待 spec 06 補存檔 + spec 07 AI。M7 待 spec 07。
4. **每個 milestone PR**：對應 spec 的 sign-off 勾完才 merge；ctest N/N 全綠（[LESSONS_LEARNED #8](../../civ1_cht/docs/LESSONS_LEARNED.md) 規則）。
5. **跨隊溝通通道**：實作含糊 → 回 Team A spec PR 提問，**Team B 永不開 Ghidra**（[CLEAN_ROOM](../docs/CLEAN_ROOM.md) 規定）。

## Sign-off

- [x] Team B（實作側）：本計畫的目錄結構、widget 模型、繪圖層 API、milestone 排序與 §14「不做的事」一致，可作為 M0 開工依據。
- [x] 使用者（架構審核）：milestone 排序與「不做的事」清單符合預期；接受 M4+ 待 Team A spec 04+ 開展才能啟動。

**使用者簽核 2026-06-06**：認可，M0 開工。
