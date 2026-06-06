# Team A external research material

> **Clean-room status**: 這個資料夾屬於 **Team A 私人研究材料**, **Team B 不可直接讀**.
> 任何資訊要進到 Team B 必須先萃取進 `../specs/*.md`.

## 來源

使用者 2026-06-06 提供 `user-provided-data/CivWin_File_Format_demonstrator_source.zip`,
作者 **Honza Havlicek <havlicek.honza@gmail.com>** © 2008,
公開研究專案. License 是 "freely redistribute and include this code in any program
as long as this credit is given where due" (見 `Civilization/Readme.txt`).

**這不是原版 Civ I source code** — 是社群 RE 出來的 file format demonstrator,
跟 Track A 的 Ghidra 反組譯一樣屬於外部 research, **可以拿來校驗我們的 spec**.

## 包含什麼

```
Civilization/
  Readme.txt              format documentation (Honza 寫的)
  src/
    common.{h,cpp}        utility
    main.cpp              CLI 入口
    rsc_manager.{h,cpp}   .RSC archive format parse + write
    gif_convert.cpp       Civ1 GIF (非標準 LZW) 解碼
    sav_file.{h,cpp}      SAV file RLE 壓縮 (RLLEncode/RLLDecode)
  release/
    *.exe                 4 個編譯好的 Windows binary (.exe 不入 git)
    convert.bat
civdata4_extracted/
  civdata4.rsc            76 KB CIVDATA4 (含 SPR32X32 sprite sheet) 原版
  readme.txt
```

## 我們已從這份材料萃取到的 spec

| spec | 引用內容 | Honza 的角色 |
|---|---|---|
| [`spec 03 §3.5`](../specs/03_asset_formats_and_tiles.md) | SPR32X32 palette: 64 base + 8 cyclic anim, sentinel pixels @ bottom-left 指定 national color slots | 校驗 + 補完 (我們 RE 出 LZW + sheet size, Honza 補 palette 語意) |
| [`spec 03 §3.6`](../specs/03_asset_formats_and_tiles.md) | RSC file table structure (header / desc / section / file_info) | 校驗 (我們 RE 一致, Honza 補 zero-field 半時 0xFFFF name 意義) |
| [`spec 07`](../specs/07_save_format_and_rle.md) | SAV file RLE 壓縮格式 (byte stream, 0x80 旗標) | **新 spec 主體** (我們之前 spec 01 §1.2 只知 `RLLEncode/Decode` 在 `load.c`, 沒抽到實際算法) |

## 為何 clean-room 安全

1. Honza 的 code 已公開 + 顯式 License 允許 redistribute
2. 我們不直接 copy code 到 `team-b/`
3. 進到 spec 的是 **format description** (= 公開可知的位元欄位排列), 不是 algorithm
4. Team B 看 spec, 不看 `team-a/external/` (跟 Team B 看 Ghidra dump 一樣禁止)

## 還沒萃取的部分

- `gif_convert.cpp` 內 BitReader / LZW 14-bit max 細節 — 可能補我們 14/199 失敗的 CvPc 對位
- Honza 沒研究的部分:
  - SAV file 解壓後內部結構 (city/unit/civ 表 layout)
  - 軍事單位 stats (attack/defense/cost) — Honza Readme.txt L80 明示 "It doesnt contain ... military units stats"
  - 科技樹依存關係 — 同 Readme.txt L81 "discovery tree" 也沒

這些缺口要從 CIV.EXE Ghidra side 抽 (Team A 的 spec 06 工作).

## Credit

如果未來 ship release, README 主檔要 credit Honza Havlicek 的 CivWin File Format
demonstrator 對 spec 03 §3.5 + spec 07 的貢獻.
