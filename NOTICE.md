# NOTICE — 第三方資產與著作權聲明

## 1. 本 repo 內含的第三方資產

`assets-extracted/tiles/` 目錄下的 185 個 PNG（與其縮圖 `_thumbs/` 子目錄）是從 1993 MicroProse《文明帝國 視窗版》（*Sid Meier's Civilization for Windows*）的 4 個 `Civdata*.RSC`（Apple Mac Resource Fork 容器）內 CvPc 資源解碼後輸出的點陣圖。

抽取流程完全程式化、可重現，[`team-a/tools/extract_tiles.py`](team-a/tools/extract_tiles.py) + [`team-a/notes/extract_all_cvpc.sh`](team-a/notes/extract_all_cvpc.sh) 由原版遊戲檔案產出。

## 2. 著作權歸屬

- ***Sid Meier's Civilization*** © 1991 MicroProse Software, Inc.
- **Sid Meier's Civilization for Windows** © 1993 MicroProse Software, Inc.
- 美術設計：MicroProse 美術團隊（包含 Mike Reis、Brian Reynolds 等）
- 音樂：Jeffery L. Briggs
- 商標權：目前歸 **Take-Two Interactive** / **2K Games** / **Firaxis Games** 所有
- IP 變更歷史：2004 年底 Atari 將 *Civilization* IP 售予 Take-Two Interactive；2005-01-26 公開消息（詳 [docs/CIV1_REFERENCE.md](docs/CIV1_REFERENCE.md) §10）

## 3. 本 repo 的使用性質

本 repo 為 **clean-room 反組譯與字符工程研究**，目的是：
1. 紀錄 1993 Win16 NE 二進位逆向過程作為遊戲歷史 / 技術考古素材
2. 驗證 CvPc 解碼器（spec 03 §3.4-§3.8 + §9.1）的正確性
3. 規劃繁體中文化翻譯範圍（領袖名 / 科技名 / 單位名 / 建築名等對應）
4. 教學示範：從 disassembly 反推未公開檔案格式的方法論

**本 repo 不**：
- 散布原版遊戲 binary（`CIV.EXE` 必須使用者自備合法拷貝）
- 重打包成可商業執行的衍生作品
- 提供完整可玩遊戲（重寫的 C99 + SDL2 程式碼僅實作部分系統）
- 在任何商業管道販售或牟利

## 4. 法律基礎

本研究依下列原則進行：

- **合理使用 / 學術研究**：對 1993 商業遊戲做反向工程以理解未公開檔案格式、用於技術評論與教學，在多數司法管轄下符合合理使用範圍（fair use / fair dealing / 教育例外）。
- **技術文件創作**：本 repo 大量產出的 spec / 紀錄 / 程式碼是**研究產出物**，採 MIT / CC BY 4.0 / CC BY-SA 4.0 授權（見 [README §License](README.md#license)），與原版 MicroProse 著作獨立。
- **不影響原作品市場**：1993 Windows 版自 1990 年代末已不再販售；Take-Two 於 2005 年將 1991/1993 兩版納入「Civilization Chronicles」合集後又下市；目前無任何商業途徑可購買。本研究不替代任何現行商品。
- **保留標誌**：所有 sprite 文件保留原檔名（含原版 resource ID 與名稱），不混淆其來源。

## 5. 若您是著作權持有人

若 Take-Two Interactive、2K Games、Firaxis Games 或其授權代表認為此 repo 的內容超出合理使用範圍，請透過 [GitHub Issues](https://github.com/wicanr2/civ1-decomplie-cht/issues) 或 GitHub DMCA 流程聯絡 repo 擁有者 `wicanr2`，我們會在合理時間內妥善處理（移除、限制存取或調整內容）。

## 6. 使用者責任

下載或 fork 本 repo 的人請自負以下責任：
1. 自行確認您所在司法管轄區的合理使用範圍。
2. 不得將 `assets-extracted/` 內容用於商業目的、轉散布為遊戲、或重打包銷售。
3. 若要在學術論文 / 部落格文章中引用，請同時引用本 NOTICE 與原版著作權聲明。

---

最後更新：2026-06-06
