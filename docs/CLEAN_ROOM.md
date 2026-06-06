# Clean-room 制度

## 為什麼要 clean-room

1993 年《文明帝國 視窗版》的原始碼從未公開，根據各方說法已遺失。坊間已有數個從 disassembly 反組譯的 open source 專案，最知名的是 OpenCiv1（1991 DOS 版的 C# 移植），程式以 MIT 釋出，技術上可直接利用。但本專案目標是 **獨立重建** — 一份程式碼的著作脈絡可以追到「**只看 disassembly 觀察寫出來的 spec**」，沒有衍生自任何前人反組譯實作。

這樣做有兩個好處：(1) 釐清 IP 來源毫無疑義；(2) 強迫實作直接面對原版 binary 行為，會浮現衍生 port 通常被掩蓋的細節。

## 雙隊分工

| 隊 | 輸入 | 輸出 | 禁止 |
|---|---|---|---|
| **A — disassembly 側** | `CIV.EXE` 的 Ghidra 專案；Track A 的 EDILZSS2 格式 spec | `team-a/specs/*.md`、`team-a/dumps/*.txt` | 寫 C / C++ / 任何可編譯的程式碼；讀 `team-b/` 下任何檔案；讀 OpenCiv1 / Freeciv / CivOne |
| **B — 實作側** | 只有 `team-a/specs/*.md` | `team-b/src/**`、`team-b/tests/**` | 開 Ghidra；把 `CIV.EXE` 載入任何 disassembler；讀 OpenCiv1 或其他 Civ 重寫；讀 `team-a/dumps/`（只能讀 spec） |

`team-a/dumps/` 存在的目的是 Team A 自己的 audit trail — **不屬於介面**。Team B 永遠不讀。

`team-a/notes/` 是 Team A 私人 scratch，也不屬於介面，PR review 時不必逐行檢查。

## 簽核流程

每份 spec（`team-a/specs/NN_subsystem.md`）末尾都有 footer：

```markdown
## Sign-off

- [ ] Team A：這份 spec 準確描述了該子系統在 CIV.EXE 中觀察到的行為。
- [ ] Team B：這份 spec 可實作；不需要再回頭看 disassembly；不明處已 escalate 給 Team A。
```

兩格都打勾（在 commit 中，由執行該角色的人 / agent 確認）後，Team B 才開始實作該子系統。實作途中若發現含糊，回到 spec PR 提問 — Team A 修 spec；Team B 不會自己「翻 disassembly 看」。

## Agent / 人類角色紀律

當同一操作者（人或 AI agent）在不同 session 擔任兩隊：

1. **同一 session 不戴兩頂帽子。** 一個 session 綁一個隊。換隊要結束目前 session。
2. **Team A session 學到的東西不會默默漏到 Team B session。** Team A → Team B 唯一通道是已 commit 的 spec。
3. **對話 memory 比照辦理。** 如果一條 memory 是 Team A session 寫的，下一個 Team B session 只讀 spec 不讀那條 memory。（實務上：Team A memory 應該記錄 **規範了什麼**，不是 **disassembly 看到什麼**。）

## 可以從 Track A 重用的東西

母專案 `civ1_cht` 的 Track A 做了大量 Win16 NE inline patch 工作。下列 Track A 產出是 **資料**，不是實作，可以乾淨地取用：

- EDILZSS2 解壓 **格式規範**（描述 byte layout，不是 decoder 實作）
- `RT_DIALOG` resource ID 清單與 slot 結構
- 翻譯字串（`inline_translations.json`、`dialog_translations.json`）做為 translation catalog，授權 CC BY-SA 4.0

下列 Track A 產出是 **實作**，不可取用：

- EDILZSS2 的 Python decoder（Team B 從格式 spec 重新實作）
- NE binary patcher script
- Win16 GDI workaround

完整對照表見 `REUSE_FROM_TRACK_A.md`。

## 驗證

沒有行為 oracle（不跑 DOSBox、不用 `wine` 跑原版、不參考任何 port）。驗證來自：

1. **Spec 推導出的單元測試。** Team A 在 spec 內附 worked example（例：「給定輸入 bytes `0x12 0x34 …`，EDILZSS2 decoder 必須產出 bytes `0xAB 0xCD …`」）。Team B 的測試 assert 這個。
2. **資產 round-trip。** 解開的 `.pic`/`.pal` 再 encode 回去要與原檔 byte 等同（驗 loader/saver pair，不驗行為）。
3. **視覺 layout 比對。** 最終 UI 截圖可以對 Track A 已凍結的 Big5-patched binary 截圖比 **layout** 一致，不比 **行為** 一致。

無法用這三種方法測的東西，當作 spec 內未解問題往上 escalate，不是自己跑原版去問答案。
