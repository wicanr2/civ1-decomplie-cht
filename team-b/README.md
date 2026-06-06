# team-b — 實作側

C99 + SDL2 重寫《文明帝國 視窗版》。實作依據是
[`SDL_IMPLEMENTATION_PLAN.md`](SDL_IMPLEMENTATION_PLAN.md) 與
`../team-a/specs/*.md`。

## 依賴

- C99 編譯器（GCC / Clang / MSVC）
- CMake ≥ 3.16
- SDL2（M0）；後續 milestone 會加 SDL2_mixer + FreeType
- 跨平台目標：Windows 10/11、Linux X11/Wayland、macOS

### Linux / WSL

```bash
sudo apt-get install -y libsdl2-dev cmake ninja-build
```

### Windows

裝 [SDL2-devel](https://github.com/libsdl-org/SDL/releases) MSVC 或 MinGW 包，
或用 vcpkg / Scoop。

## 建置

```bash
cd team-b
cmake -S . -B build -G Ninja
cmake --build build
```

## 跑

```bash
./build/civ1                 # M0：開 640×480 視窗、灰底、ESC 退出
```

## 測試

```bash
cd build && ctest --output-on-failure
```

## Milestone 進度

| Milestone | 範圍 | 狀態 |
|---|---|---|
| **M0** | CMake + SDL 開視窗 + 主迴圈 + ESC 退出 + ctest skeleton | ✅ |
| **M1** | palette framebuffer + CJK 字模 + 中文 text out | ✅ |
| **M2** | 3 個 widget skeleton + dispatch table + modal lock | ✅ |
| M3 | Mac Resource Fork + CvPc decode | ⏳（依賴 spec 03 §9.1 LZW 變體） |
| M4 | 14 文明資料 + 新局精靈 dialog | ⏳ |
| M5 | 地圖視窗 paint terrain | ⏳ |
| M6 | turn loop + AI + 存讀檔 | ⏳ |
| M7 | 奇蹟 / 外交 / 勝利條件 | ⏳ |

詳見 [`SDL_IMPLEMENTATION_PLAN.md`](SDL_IMPLEMENTATION_PLAN.md) §13。

## clean-room 紀律

Team B 寫程式只看本目錄 + `../team-a/specs/*.md`。**不開** Ghidra、
**不看** `../team-a/dumps/`、**不看** `../team-a/notes/`、**不看**
OpenCiv1 / Freeciv / 其他 Civ 重寫。詳見
[`../docs/CLEAN_ROOM.md`](../docs/CLEAN_ROOM.md)。
