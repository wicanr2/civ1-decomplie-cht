# dist/ — portable build 產出 (gitignored)

> R12 M8 portable SFX. 產出真實 portable 透過 GitHub Releases 發布, 不入 git history.

## 跑法

```bash
# 從 repo root
bash tools/build_portable.sh
```

產出:
- `dist/civ1-cht-linux-x64/` — stage dir
- `dist/civ1-cht-linux-x64.tar.gz` — 完整 portable (~10 MB)

詳細跑法 + 系統需求見 `civ1-cht-linux-x64/README.md` (extract 後)

## 平台支援

| Platform | 狀態 | 備註 |
|---|---|---|
| Linux x64 | ✅ ship (R12) | `build_portable.sh` 內建 |
| Win64 | 🟡 v0.2 | 需 mingw SDL2 SDK + freetype SDK (apt 無, 需手動下載或 vcpkg) |
| macOS | 🟡 v0.3 | 需 brew SDL2 + framework bundle |

## Win64 cross-compile 設置筆記 (給未來)

```bash
# 1. apt 安裝 mingw 工具鏈
sudo apt install gcc-mingw-w64-x86-64 g++-mingw-w64-x86-64

# 2. 下載 SDL2-devel mingw 版
wget https://www.libsdl.org/release/SDL2-devel-2.28.5-mingw.tar.gz
tar xzf SDL2-devel-2.28.5-mingw.tar.gz
sudo cp -r SDL2-2.28.5/x86_64-w64-mingw32/* /usr/x86_64-w64-mingw32/

# 3. FreeType-mingw 待手動 build 或下載 prebuilt
# 4. cmake 用 toolchain file (CMAKE_TOOLCHAIN_FILE=mingw-w64.cmake)
# 5. Bundle SDL2.dll + freetype6.dll + civ1.exe + assets
# 6. 7z.sfx 自解 (對齊 PG-cht wing-portable-sfx skill)
```

不在本 round scope.
