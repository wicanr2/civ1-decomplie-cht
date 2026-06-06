# civ1-decomplie-cht

> 對 **1993 MicroProse《文明帝國 視窗版》**（Win16 NE `CIV.EXE`）做 clean-room 反組譯，重寫成可攜的 **C99 + SDL2**，並內建繁體中文化。

## 專案目的

從 1993 Windows 版 *Sid Meier's Civilization* 的二進位檔從頭反組譯並重寫。不參考任何 upstream 的 open source 程式庫（特別是不看 [OpenCiv1](https://codeberg.org/rhorvat/OpenCiv1) 或其衍生分支）。不 patch 原始 `CIV.EXE`。也不依賴任何 Win16 API（GDI / USER / MMSYSTEM 全部由 SDL2 取代）。

輸出目標是一份單一的 C99 程式碼，透過 SDL2 跨平台編譯；內部維持 palette framebuffer（320×200 / 640×480 原生），present 時用 bicubic 放大到任意視窗大小；中文字模（內文 16×16、標題 24×24）在 palette 層合成。

## Clean-room 雙隊制度

| 隊伍 | 可看 | 可寫 | 禁止 |
|---|---|---|---|
| **Team A** | `CIV.EXE` 的 Ghidra 反組譯與 decompile | `team-a/specs/*.md`（規格文件：散文、虛擬碼、常數表、資料結構描述） | 寫 C 程式碼；讀 `team-b/` |
| **Team B** | 只看 `team-a/specs/*.md` | `team-b/src/**/*.{c,h}`（C99 + SDL2 實作） | 開 Ghidra；讀原始 binary；讀 OpenCiv1 / 其他 Civ clone |

每份 spec 在實作前要簽核兩次：
- Team A 簽：「這份 spec 準確描述原版行為」
- Team B 簽：「這份 spec 可實作，不需要再回去看 disassembly」

同一個操作者（人或 AI agent）可以在不同 session 輪流擔任兩隊，但同一 session 內只戴一頂帽子，也不會把 Ghidra 來的知識帶進 Team B session — 唯一的橋樑是已簽核的 spec。

## 目錄結構

```
team-a/                       Team A 工作區 — disassembly 側
  specs/                       Markdown 規格（clean-room 介面）
  dumps/                       Ghidra decompile 文字輸出（記錄用）
  notes/                       Team A 私人筆記（不屬於 spec）
  binary/                      原始 CIV.EXE（gitignore，使用者自備）

team-b/                       Team B 工作區 — 實作側
  src/                         C99 + SDL2 source
  build/                       CMake 輸出（gitignore）
  tests/                       單元測試（對 spec 萃取出的 fixture）
  assets/zh_TW/                翻譯資料（CC BY-SA 4.0）

docs/
  CLEAN_ROOM.md                雙隊制度與簽核流程細節
  PHASE_LOG.md                  逐階段進度紀錄
  REUSE_FROM_TRACK_A.md        從前置 Track A 可乾淨取用什麼
  CIV1_REFERENCE.md            Fandom 公開資料對照（行為層參考，非 spec）

assets-extracted/             從 CIV.EXE 抽出的 .pic / .pal / .txt（gitignore）
tools/                         共用資產抽取工具（EDILZSS2 等，MIT）
```

## 進度

| 階段 | 內容 | 狀態 |
|---|---|---|
| 0 | WSL 工具棧（Ghidra 12 / 7z / binwalk）、CIV.EXE 取得、clean-room repo 骨架 | ✅ 完成 |
| 1 | 資產抽取（重用 Track A EDILZSS2 格式 spec） | ⏳ |
| 2 | WinMain / 訊息迴圈 / API 邊界盤點（已寫 spec 00、spec 01） | 🟡 推進中 |
| 3 | 各子系統 RE → C（loader → renderer → input → audio → game state → AI → 存讀檔） | ⏳ |
| 4 | SDL2 骨架 + CJK 16/24px 字模 cache | ⏳ |
| 5 | Ghidra-only 驗證 oracle | ⏳ |

## 工作對象（work-of-record）

`CIV.EXE` 參考資訊（使用者自備，不入 repo）：
- 大小：832,512 bytes（Track A Big5-patched 版 1993《文明帝國 視窗版》；code segments 與原版完全相同）
- MD5：`336FF64650F6391C65A8B804ADFC31C9`
- SHA-256：`720C5EA4EFD47FC7069A89C9A366A3A18A561A88E39F963865C6084EEC4DB022`
- NE 簽名位於 offset `0x60`；module name `WINCIV`；description `'文明帝國 視窗版'`（Big5）

原版 1993 英文 binary（`MODULE_DESC = 'CIVILIZATION for Windows'`，833,024 bytes，MD5 `DCC4399E…`）位於 `D:\03_game_tmp\win31\C\MPS\CIVWIN\CIV.EXE`，結構完全相同（同 133 segments / 同 6 imports / 同 11 exports）。Track A 只 patch 字串槽與 RT_DIALOG label，沒動 code byte。選 Big5-patched 版作 work-of-record 是因為使用者的 SFX portable 是同一份 binary build 的，視覺 layout 驗證時可對到同一 artifact。

## 本專案**不**做的事

- 不散布原版 binary 或資產的任何部分。
- 不把反組譯結果（function listing、disassembly bytes、decompiler 輸出）嵌入到實作 source tree。
- 不修改、patch、或重打包 `CIV.EXE`（那是母專案 Track A 的路線，已封存）。

## 反組譯資產（研究用途）

- [`docs/ASSETS_INDEX.md`](docs/ASSETS_INDEX.md) — **185 個 sprite 完整分類索引**（領袖 14 / 科技 ~65 / 單位 ~28 / 奇蹟 / 政府 / 太空 / 動畫 / UI），含縮圖
- [`docs/screenshots/cvpc_spr32x32_decoded.png`](docs/screenshots/cvpc_spr32x32_decoded.png) — 主 sprite sheet 1472×400
- [`docs/screenshots/cvpc_king00_elizabeth.png`](docs/screenshots/cvpc_king00_elizabeth.png) — Queen Elizabeth I 領袖肖像示意
- 完整版權聲明：[`NOTICE.md`](NOTICE.md)

> 所有 sprite © 1993 MicroProse Software / 現屬 Take-Two Interactive / Firaxis Games。**本 repo 為 clean-room 反組譯研究**，純技術考古 + 翻譯範圍規劃，不商用、不重打包、不替代原版商品。

## License

- 程式碼（`team-b/src/`、`tools/`）：**MIT**
- 翻譯文字（`team-b/assets/zh_TW/`）：**CC BY-SA 4.0**
- 規格文件（`team-a/specs/`）：**CC BY 4.0**
- 原版 *Sid Meier's Civilization for Windows* © 1993 MicroProse Software, Inc. — 使用者必須自備合法拷貝。
