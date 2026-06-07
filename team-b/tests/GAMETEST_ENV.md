# Game Test 環境 setup 紀錄

日期: 2026-06-07
作業環境: Windows 10 + WSL2 (Ubuntu-22.04)

## 一、Linux 端 (WSL Ubuntu-22.04)

可用工具 (全部 pre-installed):

| 工具 | 版本 / 路徑 | 用途 |
|------|------------|------|
| Xvfb | /usr/bin/Xvfb | headless X server |
| ImageMagick `convert` `import` `identify` | /usr/bin | PPM->PNG + X 截圖 |
| `xwd` `xdotool` | /usr/bin | X 視窗截圖 + key inject |
| dosbox | 0.74-3-4 | DOS 模擬器 (本次未使用) |
| wine-stable | 11.0.0 | /opt/wine-stable/bin/wine |
| libsdl2-2.0-0 | 2.0.20 amd64 + i386 | SDL2 runtime |
| uming.ttc | /usr/share/fonts/truetype/arphic/ | CJK 字型 (預設) |

啟動 Xvfb:

```bash
nohup Xvfb :99 -screen 0 1024x768x24 >/tmp/xvfb.log 2>&1 & disown
export DISPLAY=:99
```

## 二、我們 SDL2 重寫版

| 項目 | 路徑 |
|------|------|
| Binary | `/mnt/d/03_game_tmp/civ1-decomplie-cht/team-b/build/civ1` |
| Snapshot harness | `tests/test_world_snapshot <out.ppm> <mode>` |
| Mode 清單 | `main` / `city` / `tech` / `splash` / `diplomat-{elizabeth,frederick,mao,gandhi}` / `birth-{1..8}` |
| 資產 | `CIV1_DATA_DIR=/mnt/d/03_game_tmp/civ1/civ1win/civ1win` |
| 字型 | env `CIV_FONT` 可 override，預設 uming.ttc |

最簡單跑法:

```bash
cd /mnt/d/03_game_tmp/civ1-decomplie-cht/team-b/build
ninja                                  # rebuild (test_world_snapshot 不在原 build set)
export SDL_VIDEODRIVER=dummy
export CIV1_DATA_DIR=/mnt/d/03_game_tmp/civ1/civ1win/civ1win
./tests/test_world_snapshot /tmp/out.ppm main
convert /tmp/out.ppm /tmp/out.png
```

本次 batch script: `team-b/tests/gametest_run_us.sh` (產 16 張 PNG, 一氣呵成 < 30 秒)

## 三、1993 原版 (Win16 NE) — wine 嘗試結果

備註: **本次最終未跑成功**。狀況記錄:

1. Binary 在 `/mnt/d/03_game_tmp/civ1/civ1win/civ1win/CIV.EX$` (檔名 `$` 是 Big5 patched 慣例)
2. wine 11 已裝在 `/opt/wine-stable/`，但 system PATH 沒含它，需用絕對路徑 `/opt/wine-stable/bin/wine`
3. 嘗試 `WINEPREFIX=/tmp/wineprefix_civ wine ./CIV.EXE`，wineboot --init 超過 90 秒仍未完成 (rundll32 setupapi InstallHinfSection 階段卡住)
4. 推測 WSL2 內第一次 wine prefix 初始化耗時太長 (~3-5 分鐘)，後續 round 應該 reuse prefix 就會快
5. **預先 init prefix 的命令** (供下次使用):
   ```bash
   export WINEPREFIX=/tmp/wineprefix_civ
   export DISPLAY=:99
   export WINEDEBUG=-all
   /opt/wine-stable/bin/wineboot --init   # 第一次 ~5 分鐘
   /opt/wine-stable/bin/wine /tmp/CIV.EXE
   ```
6. **替代方案**: Win16 NE 用 `otvdm` (winevdm) 跑可能比 wine native 更穩，但本機沒裝

## 四、Reference 截圖路徑

`docs/screenshots/reference/`:

| 檔案 | 尺寸 | 用途 |
|------|------|------|
| civ1_win_main_menu.jpg | 640×480 | Title splash + New Game 對話框 |
| civ1_win_civilopedia_dropdown.png | 640×480 | 主畫面 + Civilopedia 下拉選單 |
| civ1_win_tech_discovery.png | 640×400 | 科技發現 modal (BRONZE WORKING) |
| civ1_win_city_screen.png | 861×541 | 城市 (PARIS) — scaled / cropped |
| civ1_win_city_screen_rome.png | 400×250 | 城市 (Rome) |
| civ1_diplomat_elizabeth_pc29.webp | 639×399 | 外交 Elizabeth |
| civ1_diplomat_frederick_pc29.webp | 640×400 | 外交 Frederick |

## 五、截圖輸出位置

- 我們 SDL2: `/tmp/civ1_diff_us/{main,city,tech,splash,diplomat-*,birth-*}.png`
- 原版 reference 已 copy 到: `/tmp/civ1_diff_orig/`
- 存底 (給 main agent + 後續 round): `/mnt/d/03_game_tmp/civ1-decomplie-cht/docs/screenshots/gametest_2026-06-07/`
