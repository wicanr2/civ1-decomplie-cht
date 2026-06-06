# Spec 04 — RT_DIALOG 完整盤點與自製控制項家族

> **Team A 規格。** 接續 [spec 00 §0.7](00_ne_structure.md#07-resource-directory-內容)（24 個 RT_DIALOG ID 列表）與 [spec 02 §2.2](02_startup_and_game_loop.md#22-11-個導出-callback-各做什麼)（11 個 callback 職責）。
>
> 配套 dump：[`team-a/dumps/04a_dialogs.txt`](../dumps/04a_dialogs.txt) 人讀格式 + [`team-a/dumps/04b_dialogs.json`](../dumps/04b_dialogs.json) Team B 用。
>
> 工具：[`team-a/tools/ne_dialog_dump.py`](../tools/ne_dialog_dump.py) — Win16 RT_DIALOG parser，從 NE resource table 直接抽出 DLGTEMPLATE 結構。

## 4.1  Win16 DLGTEMPLATE 格式（公開）

格式從 Microsoft Win16 SDK 公開文件 + Track A 已驗證的 spec 取得（屬於格式描述，不是 implementation；clean-room safe）：

```
DLGTEMPLATE_HEADER:
    DWORD style              Windows DS_* + WS_* flags
    BYTE  cdit               控制項數
    WORD  x, y, cx, cy       dialog 在螢幕上的位置與大小（dialog units）
    sz_or_ord menu_name      0x00=無 / 0xFF+WORD=ordinal / 否則 null-term ASCII
    sz_or_ord class_name     同上；CIV.EXE 全部用 "CIVDIALOG" 自製類別
    sz       caption         null-term；CIV.EXE 全部為空（caption 在 paint 時自繪）
    if style & DS_SETFONT (0x40):
        WORD font_size
        sz   font_face       例：'CIVTIMES12'

每個 DLGITEMTEMPLATE (14 bytes header + 變長 data):
    WORD  x, y, cx, cy
    WORD  id
    DWORD style
    item_class   1 byte 0x80-0x85 標準控制項 ordinal，否則 null-term 自製類別名
    item_text    0x00=空 / 0xFF+WORD=ordinal / 否則 null-term ASCII
    BYTE  cb_data
    cb_data bytes...
```

**標準控制項 ordinal**：

| ordinal | 類別 |
|---|---|
| 0x80 | Button |
| 0x81 | Edit |
| 0x82 | Static |
| 0x83 | ListBox |
| 0x84 | ScrollBar |
| 0x85 | ComboBox |

## 4.2  CIV.EXE 24 個 RT_DIALOG 總覽

所有 dialog 都註冊在自製類別 `CIVDIALOG`，由 [`CIVDIALOGPROC @ 1098:1838`](../dumps/02a_civdialogproc.c)（spec 02 §2.2.5）處理。caption 一律空字串，標題由 paint handler 自繪。

| ID | 尺寸 (dlg unit) | 控制項數 | 用途推測 | 備註 |
|---:|---|---:|---|---|
| 129 | 305×120 | 0 | 訊息框（純自繪） | |
| 130 | 512×320 | 2 | 簡單對話 | 兩個按鈕：確 / 結束 |
| 131 | 300×282 | 4 | **城市列表選擇** | '前往城市' / '取消' / ListBox / '選擇要尋找的城市' |
| 133 | 142×52  | 5 | **未存檔提醒** | '您確定要 / 開啟存檔嗎?' + 先存檔 / 開啟 / 取消 |
| 135 | 480×87  | 3 | 自繪內容 + 確定 | 含 RandomUser×2 |
| 136 | 80×84   | 3 | 小型確認 | |
| 137 | 242×142 | 4 | **Civilopedia** | '上一頁' + Edit + Static（內文顯示） |
| 138 | 205×120 | 1 | 小提示框 | 用 `CIVTIMES12` 字型 |
| 139 | 96×47   | 3 | 自繪確認 | |
| 140 | 324×200 | 5 | 含 **說明** 按鈕的大對話框 | |
| 141–143 | 256×~120 | 3-5 | 設定 / 選項類 | |
| 144 | 142×52  | 5 | 確認對話 | |
| 145 | 142×52  | 4 | 簡單訊息 | |
| 146 | 512×320 | 2 | 全螢幕背景 + 兩個 RandomUser | 推測：開場 / 結局畫面 |
| 147 | 512×320 | 2 | 同上 | |
| 148 | 512×320 | 0 | 純背景 paint | |
| 149 | 512×320 | 3 | 全螢幕背景 + 確定 | |
| 150 | 120×36  | 2 | **磁片插入提示** | '請插入磁片 `^0` (含檔案 `^1`)' — sprintf 樣板 |
| 151 | 512×320 | 0 | 全螢幕背景 | style 0x80800000 = 沒 modal frame |
| 666 | 300×150 | 2 | **載入 progress** | '轉換 Civilization 圖檔中' + 中止鈕 |
| 999 | 256×160 | 11 | **外交對話框** | 詳 §4.3 |
| 2000 | 198×117 | 20 | **UI widget 展示測試** | 詳 §4.4，揭穿 RANDOM* family |

**對 Team B 的意義**：
- M4 startup wizard（spec 02 §2.1.2 D 段）載的 5 個 dialog（ID 0xAB/B1/BE/CB/D8 = 171/177/190/203/216）**不在這 24 個 RT_DIALOG 內** — 那些 ID 是 Mac Resource Fork 內的 resource id（`FUN_1190_0000` 走 Mac `OpenResFile` 流程，不走 Win16 `LoadResource`）。M4 dialog 必須從 `.RSC` 載 dialog template（`DLOG`/`DITL` 或類似 Mac 自定 type），spec 04 §4.5 是這個對應的 placeholder。
- Spec 04 真正涵蓋的是 **遊戲中 in-game dialog**（外交、城市選擇、Civilopedia 翻頁、存讀檔、磁片提示、載入 progress），由 Win16 `CreateDialog`/`DialogBox` 從 RT_DIALOG 載入。

## 4.3  Dialog 999 — 外交對話框

11 個控制項，揭穿 1993 Civ1 外交系統的 UI 設計：

| index | 類別 | id | 位置 | 文字 | 推測角色 |
|---:|---|---:|---|---|---|
| 0 | Button | 101 | (220,145) | '確' | 確定 |
| 1 | RandomUser | 800 | (27,106) 200×16 | – | 對方文明的回應文字行 |
| 2 | RandomUser | 801 | (25,130) 10×9 | – | 小指示器（旗幟？） |
| 3 | Button | 106 | (25,145) | '前頁<' | 翻前一頁 |
| 4 | Button | 106 | (55,145) | '後頁>' | 翻後一頁 |
| 5 | Button | 108 | (25,130) | '政府' | 切換到對方政府型態頁 |
| 6 | Button | 107 | (55,130) | '君主' | 切換到對方領袖名頁 |
| 7 | Button | 105 | (100,145) | '威脅' | 對對方發出威脅 |
| 8 | Button | 104 | (130,145) | '害怕' | 表示我方害怕 |
| 9 | Button | 103 | (160,145) | '無趣' | 表示無趣 |
| 10 | Button | 102 | (190,145) | '取消' | 取消對話 |

> **觀察**：按鈕 id 106 出現兩次（前頁 / 後頁）— Win16 dialog 允許多個控制項同 id；CIVDIALOGPROC 根據按鈕的座標或 text 區別。

## 4.4  Dialog 2000 — UI widget 展示測試（揭穿 RANDOM* family）

這個 dialog 不會在正常 gameplay 出現 — 是 1993 開發期間留下的 widget showcase。但它揭穿了 spec 02 §2.2.9-12 的 `RANDOM*PROC` callbacks 對應的完整自製控制項集合：

| 自製類別 | 對應 callback (spec 02) |
|---|---|
| **RandomRadio** | `RANDOMRADIOPROC` @ 1058:06ab |
| **RandomStatic** | `RANDOMSTATICPROC` @ 1058:081a |
| **RandomUser** | `RANDOMUSERPROC` @ 1058:0921（**dialog 999 / 135 / 136 / 139 / 146 / 147 / 149 全用到**） |
| **RandomCheck** | （subclass，可能也屬 RANDOMRADIO 或獨立 sub-proc） |
| **RandomHorzBump** | （水平推桿，可能 spec 02 spec 02 未列的副 callback） |
| **RandomVertBump** | （垂直推桿） |
| **RandomHorzMeter** | （水平進度條） |
| **RandomFrame** | （邊框） |
| **RandomRectangle** | （矩形 hit-test，可能對應 REGIONPROC） |

Dialog 2000 內含可愛的占位文字：'The check / Is in / the mail / ha!ha!' / 'This is a test of the 3d stuff' / 'We will see what it looks like.' — 是英文，**Track A 沒翻譯**（因為非 retail dialog）。

**對 Team B 的意義**：M4+ 寫 dialog widget 時要實作這 9 個自製類別。RandomRadio / RandomStatic / RandomCheck 是基本變體；RandomHorzBump / VertBump 是 scrollbar-like 推桿；RandomMeter 是進度條；RandomFrame / Rectangle 是裝飾。

## 4.5  M4 startup wizard 對應的 5 個 dialog ID — 待確認

spec 02 §2.1.2 D 段顯示 WinMain 載入 5 個 `0xAB / 0xB1 / 0xBE / 0xCB / 0xD8` resource：

| 0xAB = 171 | 0xB1 = 177 | 0xBE = 190 | 0xCB = 203 | 0xD8 = 216 |

這些 ID **不在 RT_DIALOG 範圍** — 推測是 Mac `.RSC` 內的 Mac 風格 dialog template（`DLOG` type 或自定 type）。需要：
- 從 5 個 `.RSC` 用 `civ_rsrc_iter` 掃描看哪些 type tag 內有 171/177/190/203/216 等 ID
- 對找到的 resource 分析其格式（可能是 Mac Resource Manager 的 `DLOG` / `DITL` pair，或自製格式）

待後續 spec 04b 補完。M4 之前必須解。

## 4.6  與 Team B SDL2 整合的接口契約

spec 04 解開後，Team B 的 dialog 系統可以：

```c
/* team-b/src/dialog/dialog.h — 介面契約 */

typedef struct civ_dlg_template civ_dlg_template_t;

/* 從 04b_dialogs.json 載入 24 個 dialog 模板 */
int civ_dlg_load_all(civ_dlg_template_t **out_table, size_t *n);

/* 依 ID 找模板 */
const civ_dlg_template_t *civ_dlg_find(int16_t id);

/* 用模板實例化一個 widget tree */
struct civ_dialog *civ_dlg_instantiate(const civ_dlg_template_t *tmpl,
                                       struct civ_game *g);
```

Team B 直接消化 [`04b_dialogs.json`](../dumps/04b_dialogs.json) 編譯期或載入期建立模板。 字串部分對接 i18n catalog — Big5 字串是 Track A 的翻譯結果，Team B 把它們轉為 UTF-8 進 catalog。

## 4.7  下一步（spec 05+）

1. **Spec 05**：walk WinMain E 段的 7 個 `FUN_*()` init call，找 14 文明 / 科技樹 / 單位 / 建築 / 政府的 in-memory 資料表 layout
2. **Spec 04b**：上面 §4.5 5 個 startup dialog ID 在 `.RSC` 內的找法
3. **Spec 06**：地圖格式、save file 結構（含 RLLDecode/RLLEncode worked example）

## Sign-off

- [x] Team A：本 spec 準確列出 24 個 RT_DIALOG 的結構、自製類別、與 Track A patched Big5 captions。RANDOM* family 揭穿。
- [ ] Team B：本 spec + `04b_dialogs.json` 可作為 dialog 系統實作依據；§4.5 待後續補完才能載 M4 startup wizard。

**使用者代簽 2026-06-06**：spec 04 整體認可，與 spec 00/01/02/03 同等。
