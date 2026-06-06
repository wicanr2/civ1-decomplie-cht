docs: README 大改 — 反映 M0..M5-B ship + 加 RE 覆蓋盤點 + Roadmap

使用者要求每 round ship 都要更新 GitHub README. 之前 README 進度表停留在 M0 級別,
跟實際 ship 13 個 milestone 嚴重脫節. 一併加入「RE 覆蓋盤點」段落,讓任何人 (含
未來自己) 一眼能看出: (1) 哪些 spec 寫完了 (2) 哪些 spec 還沒寫 (3) 每個子系統
RE 程度多少 (4) 192 個資產解碼到哪.

# 改動內容

  ## 目前 ship 狀態 (新段落)
    13 個 milestone 表 (M0..M5-B + M6-lite) + 狀態 + 證據截圖
    最後 ctest 14/14 PASS + build 33/33 zero warning

  ## RE 覆蓋盤點 (新段落)
    已 spec ✅:  spec 00 / 01 / 02 / 03 / 04 / 05 + 每份關鍵發現摘要
    待寫 spec ❌: spec 06 (數值表) / 07 (combat+AI+save) / 08 (sound)
                  / 09 (victory) + 為什麼 deferred

    子系統 RE 狀態表 (12 項):
      Renderer 80% / Loader 70% / 存讀檔 0% (阻 M6-full)
      Dialog 100% (spec 04) / 視窗 proc 60% (dispatch 表骨架)
      AI/外交 5% / 音效 0% / Game state 30% / Combat 0%
      城市生產 0% / 太空船 文字側完 / 演算法未碰

    資產解碼狀態表 (7 類):
      CvPc 185/199 ✅ + 14 個 LZW variant ❌
      STR# 33 ✅ / TEXT 399 文字未全 dump
      GDAT 7 / KDAT 14 / WAV 23 全部未碰

  ## Roadmap (改寫)
    Phase 0..8 完整路線圖,Phase 5 = RE 第二輪 (spec 06/07/08/09)
    Phase 6 = M6-full (阻 spec 06/07)
    Phase 7 = M7 / Phase 8 = polish

  ## 目錄結構 (更新)
    team-b/src/ 加入 gfx/res/text/widgets/world/dialog/data 子目錄
    docs/ 加 ASSETS_INDEX.md + screenshots/ 12 張

# 為何寫 RE 盤點

使用者明示「一併檢查 那些部分還沒有反組譯完成」. 之前散在 spec/PHASE_LOG
裡面要翻很久才看得到全貌. 改成 README 內單一表格 一頁看完.

下一輪 ship 時這份表格的「狀態欄」要跟著更新.
