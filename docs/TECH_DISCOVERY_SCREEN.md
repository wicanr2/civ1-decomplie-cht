# Tech Discovery Screen — design spec

> **Milestone target**: M9-tech-screen (R16+ 預定)
> **Reference**: 使用者 2026-06-07 提供截圖 "BRONZE WORKING" tech discovery modal

## 0. 觸發時機

當 player 完成科技研發 (對應 spec 06 §6.5 47 個 tech + 5 future tech), 跳出全螢幕 modal 顯示研發結果. 對應 1991 manual P49 "Science Advisor" + Civilopedia 條目 (STR# 130 tech name + TEXT 條目 = §6.5 文字).

## 1. Layout (從 reference 對位)

```
640 × 480 (or 主視窗 size 整版 modal)

┌────────────────────────────────────────────────────────┐
│  ╔══════════════════════════════════════════════════╗  │  <- 雙層花邊 frame
│  ║                                                  ║  │     綠色 + 紅圓點 + 紅斜紋
│  ║   [圖示]                                         ║  │     (Win16 deco border)
│  ║   ┌─────┐    BRONZE WORKING                      ║  │
│  ║   │圖示 │    Civilization Advance                ║  │  <- 主標題 (黑色, 大 serif)
│  ║   └─────┘    (Taken from French)                 ║  │  <- subtitle (灰)
│  ║                                                  ║  │  <- "從哪文明學來" 灰字
│  ║                                                  ║  │
│  ║   Allows:                                        ║  │  <- 紫/深 紫字
│  ║     Currency                                     ║  │  <- 解鎖科技 (藍)
│  ║     Iron Working                                 ║  │
│  ║                                                  ║  │
│  ║     [unit icon] Phalanx unit                     ║  │  <- 解鎖 unit (紅) + sprite
│  ║     [wonder icon] Colossus Wonder                ║  │  <- 解鎖 wonder (綠) + sprite
│  ║                                                  ║  │
│  ║                              [cursor 劍形]       ║  │
│  ║                                                  ║  │
│  ╚══════════════════════════════════════════════════╝  │
└────────────────────────────────────────────────────────┘
```

## 2. 元素細目

### 2.1 花邊 frame (decorative border)

- **雙層 frame**: outer 綠色 + inner 紅綠 dot pattern
- 圖樣: 綠底 + 紅圓點 + 紅斜紋 (像幾何 wallpaper)
- 推測來自 SPR32X32 sheet 某個裝飾 tile (待確認 col, row)
- **clean-room 設計選擇**: 可直接畫 8×8 紋路 pattern 不取 sprite, 避免 dependency

### 2.2 圖示 (左上)

- 對應 tech 的代表性圖, e.g. BRONZE WORKING = 大鍋火 (cauldron + fire)
- 推測來自 5 個 .RSC 內的 CvPc (spec 03 §3.1: 199 個 CvPc 中部分是 tech illustration)
- 圖示 ~96×96, 黑色背景

### 2.3 標題區 (中上)

- **大標題** (e.g. "BRONZE WORKING"): 黑色 24-32px serif (對齊 manual STR# 130 英文 + civ_dict.c "青銅器")
- **subtitle** "Civilization Advance": 灰色 14-16px
- **"(Taken from XXX)"**: 灰色 14px italic — 顯示這科技是從哪 civ 偷的 / 學的 / 自己研發
  - "Taken from French" = Diplomat 偷的
  - "Discovered by X" = 自家研發
  - "Taught by Y" = 外交交易得到
  - manual P38 "Diplomats / Steal Technology" 對位

### 2.4 Allows 列表 (中央)

紫色標頭 "Allows:" + 4 種解鎖內容:

| 顏色 | 類型 | 來源 | zh-TW |
|---|---|---|---|
| **藍** | 後續可研發的 tech | spec 06 §6.5 prereq DAG | "貨幣 / 鐵器" |
| **藍** | 同上 | 同上 | "民主" 等 |
| **紅** | 可建造的 unit | spec 06 §6.1 prereq tech | "方陣兵 unit" + sprite |
| **綠** | 可建造的 wonder/improvement | spec 06 §6.2 + §6.4 prereq | "巨像 Wonder" + sprite |

每行前面有 16×16 mini sprite 對應 unit/wonder.

### 2.5 Cursor

劍形 cursor (右下) — 對齊 Civ1 標準 cursor. 我們已用 SDL_SetCursor 處理.

## 3. 中文化版需求

| 元素 | 英文 (原版) | zh-TW (我們版本) |
|---|---|---|
| 大標題 | BRONZE WORKING | 青銅器 |
| Civilization Advance | Civilization Advance | 文明進展 (STR# 147 idx 0) |
| (Taken from French) | (Taken from French) | (取自法國) / (法國 Diplomat 偷取) / (自家研發) |
| Allows: | Allows: | 解鎖: |
| Currency | Currency | 貨幣 (STR# 130, civ_dict.c) |
| Iron Working | Iron Working | 鐵器 |
| Phalanx unit | Phalanx unit | 方陣兵 (STR# 132) |
| Colossus Wonder | Colossus Wonder | 巨像 (STR# 131) |

字型: 大標題用 24-32 px serif 中文字 (uming.ttc 16-px lookup w/ 2x scale 或新 32-px font load). subtitle 用 14-16 px (現有 font_body).

## 4. 觸發 logic (R16+ 實作)

```c
/* team-b/src/world/tech.h */
typedef enum {
    CIV_TECH_LEARN_SELF,      /* 自家研發 */
    CIV_TECH_LEARN_DIPLOMAT,  /* Diplomat 偷的 */
    CIV_TECH_LEARN_TRADE,     /* 外交交易 */
    CIV_TECH_LEARN_HUT,       /* 部落小屋 */
    CIV_TECH_LEARN_LIBRARY,   /* Great Library Wonder */
} civ_tech_learn_source_t;

typedef struct civ_tech_discovery_event {
    int   tech_id;            /* spec 06 §6.5 tech enum */
    civ_tech_learn_source_t source;
    int   from_civ_slot;      /* if DIPLOMAT/TRADE, 哪個 civ */
    int   unlocked_techs[8];  /* 後續可研發 (從 prereq DAG 反推) */
    int   unlocked_units[4];  /* spec 06 §6.1 此 tech 為 prereq 的 unit */
    int   unlocked_imp[4];    /* spec 06 §6.2 同 */
    int   unlocked_wonder[4]; /* spec 06 §6.4 同 */
} civ_tech_discovery_event_t;

/* 顯示 modal. 按 ESC / Enter 關. */
void civ_tech_discovery_screen_show(struct civ_game *g,
                                     const civ_tech_discovery_event_t *ev);
void civ_tech_discovery_screen_render(struct civ_game *g, civ_surface_t *fb);
```

對齊既有 `widgets/city_screen.{h,c}` modal 模式 (game->city_screen_open 同模式新 `game->tech_screen_open`).

## 5. R16+ Milestone 拆解

- **R16-1**: tech discovery struct + show/render 框架 (背景 + frame)
- **R16-2**: 花邊 frame 繪製 (clean-room 自畫)
- **R16-3**: 大標題 + subtitle render (zh-TW)
- **R16-4**: Allows 列表 + 4 種類 lookup (從 spec 06 prereq DAG 反推)
- **R16-5**: unit / wonder sprite icon blit
- **R16-6**: 觸發 logic 接 spec 06 §6.5 tech DAG + civ_turn_advance 內 research 進度
- **R16-7**: ctest + snapshot + commit + push

## 6. 與既有 spec 對位

| 來源 | 給本 design |
|---|---|
| spec 06 §6.5 47 tech + prereq | tech_id + unlocked_techs 反推 |
| spec 06 §6.1 28 unit prereq | unlocked_units |
| spec 06 §6.2 25 building prereq | unlocked_imp |
| spec 06 §6.4 22 wonder prereq | unlocked_wonder |
| spec 05 STR# 130/131/132 | 中文翻譯來源 (civ_dict.c 已 ship) |
| spec 09 §9.3 future tech +10 score | tech_id == FutureTech 時 score 累加 |
| widgets/city_screen.{h,c} | modal 框架同模式 |

## 7. Reference image

使用者 2026-06-07 提供 "BRONZE WORKING" 原版截圖 (1991/1993 英文版).
本機路徑需使用者手動存到 `docs/screenshots/reference/civ1_win_tech_discovery.png`.

對比現有 reference:
- `civ1_win_civilopedia_dropdown.png` — 主畫面 + Civilopedia menu
- `civ1_win_city_screen.png` — 城市畫面 (PARIS POP:10,000)
- `civ1_win_main_menu.jpg` — 主選單
- **`civ1_win_tech_discovery.png`** — 科技研發完成 modal (R15 新 reference, 待存)
- **`civ1_win_city_screen_rome.png`** — ROME city w/ embedded tile bg + GREAT LIBRARY panel (R15 新 reference, 待存)
