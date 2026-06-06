# Spec 08 — 音效 / MMSYSTEM / 24 WAV inventory

> **Team A 規格 (v0.1, 2026-06-06 R11)**.
>
> **主要來源**:
> - 1993 Civ Windows 遊戲目錄 24 個 `.WAV` 檔 (使用者本機, `D:\03_game_tmp\win31\C\MPS\CIVWIN\`)
> - spec 01 §1.4 API surface: MMSYSTEM 4 call sites (但未識別具體 API)
> - 1991 manual P54 "Nuclear Weapons" 提到 sound effects

## 8.0  本 spec 解答什麼

- 1993 Civ Win port 用哪 24 個 WAV 檔
- 每個 WAV 對應哪個遊戲事件
- MMSYSTEM 哪 4 個 API call site
- Team B SDL_mixer / SDL_audio 接口契約

## 8.1  24 WAV 檔 inventory

從 `team-a/tools/inventory_wav.sh` dump:

**全部 WAV 一致格式**: PCM (`fmt=0x0001`) / 8-bit mono / **11025 Hz** sample rate (`0x2B11` BE = 11025 LE16)

### 8.1.1  14 領袖配音 (對齊 spec 06 §6.7 14 leaders)

| File | Size | Civ slot | Leader | zh-TW |
|---|---:|---:|---|---|
| `ALEX.WAV` | 60696 | 6 | Alexander | 亞歷山大 |
| `CEAS.WAV` | 58527 | 1 | Caesar | 凱撒 |
| `ELIZ.WAV` | 51814 | 14 | Elizabeth I | 伊莉莎白一世 |
| `FRED.WAV` | 54707 | 3 | Frederick | 腓特烈 |
| `GAND.WAV` | 58651 | 7 | M.Gandhi | 甘地 |
| `GENG.WAV` | 33313 | 15 | Genghis Khan | 成吉思汗 |
| `HAMA.WAV` | 48101 | 2 | Hammurabi | 漢摩拉比 |
| `LINC.WAV` | 43040 | 5 | Abe Lincoln | 林肯 |
| `MAO.WAV` | 38864 | 13 | Mao Tse Tung | 毛澤東 |
| `MONT.WAV` | 30412 | 12 | Montezuma | 蒙特蘇馬 |
| `NAPO.WAV` | 60572 | 11 | Napoleon | 拿破崙 |
| `RAMS.WAV` | 31378 | 4 | Ramesses | 拉美西斯 |
| `SHAK.WAV` | 41478 | 10 | Shaka | 夏卡 |
| `STAL.WAV` | 58790 | 9 | Stalin | 史達林 |

**觸發事件**: 推測在 dialog 開啟與該 civ 接觸時播放 (diplomacy / meet new civ / war declaration).
**Genghis Khan + Stalin + Napoleon 較長** (60s+): 推測是「強硬派」的開場宣戰演說.

### 8.1.2  3 個系統音 (`S_*` 前綴)

| File | Size | 推測用途 |
|---|---:|---|
| `S_BEEP.WAV` | 1408 | UI 警告 beep (短 1.4 KB) |
| `S_LAND.WAV` | 15770 | 單位移動到陸地 / 殖民音 |
| `S_NUKE.WAV` | 28762 | 核爆系統音效 (低頻轟鳴) |

### 8.1.3  3 個戰鬥音

| File | Size | 觸發 |
|---|---:|---|
| `CANNON.WAV` | 25521 | 砲擊 / 一般戰鬥開火 |
| `WE_DIE.WAV` | 19015 | 玩家單位陣亡 |
| `THEY_DIE.WAV` | 22781 | 敵方單位陣亡 |

### 8.1.4  2 個結局音

| File | Size | 觸發 |
|---|---:|---|
| `LOSE2.WAV` | 162270 | game over (失敗) — 對齊 spec 09 CIV_END_DESTROYED + STR# 157 archeologist 結局 |
| `WINTUNE.WAV` | 93334 | 勝利 — 對齊 spec 09 CIV_END_SPACE_WIN / CONQUEST_WIN |

### 8.1.5  特殊

| File | Size | 用途 |
|---|---:|---|
| `OPENING.WAV` | 194595 | 開場 (對齊 STR# 137 Credits + STR# 139 Story 開場文字) |
| `AIRNUKE.WAV` | 188972 | 空射核武 (Nuclear unit 攻擊時動畫音) |

## 8.2  MMSYSTEM API call sites (待 Ghidra walk 精確化)

spec 01 §1.4 統計到 **4 個 MMSYSTEM call sites**. 1993 Win 時代 sound 主要 API:

| Win16 API | 推測用途 | call count |
|---|---|---|
| `sndPlaySound` (MMSYSTEM.2) | 高階單檔播放 (`PlaySound("CANNON.WAV", NULL, SND_FILENAME \| SND_ASYNC)`) | 推測 ~3 |
| `mciSendString` (MMSYSTEM.703) | MCI 串流控制 (背景音樂) | 可能 1 |

**v0.2 R13 預定** Ghidra walk:
1. 在 CIV.EXE imports 表找 MMSYSTEM 4 entry name
2. cross-ref 每個 import 找 caller
3. 對應 §8.1 哪個 WAV 在哪個遊戲事件被叫

## 8.3  Team B 整合介面契約

```c
/* team-b/src/audio/audio.h */
#ifndef CIV_AUDIO_H
#define CIV_AUDIO_H
#include <stdbool.h>

typedef enum {
    /* 14 leader speeches */
    CIV_SND_LEADER_ALEXANDER = 0,
    CIV_SND_LEADER_CAESAR,
    CIV_SND_LEADER_ELIZABETH,
    CIV_SND_LEADER_FREDERICK,
    CIV_SND_LEADER_GANDHI,
    CIV_SND_LEADER_GENGHIS,
    CIV_SND_LEADER_HAMMURABI,
    CIV_SND_LEADER_LINCOLN,
    CIV_SND_LEADER_MAO,
    CIV_SND_LEADER_MONTEZUMA,
    CIV_SND_LEADER_NAPOLEON,
    CIV_SND_LEADER_RAMESSES,
    CIV_SND_LEADER_SHAKA,
    CIV_SND_LEADER_STALIN,
    /* 3 system */
    CIV_SND_BEEP,
    CIV_SND_LAND,
    CIV_SND_NUKE_SYS,
    /* 3 combat */
    CIV_SND_CANNON,
    CIV_SND_WE_DIE,
    CIV_SND_THEY_DIE,
    /* 2 endgame */
    CIV_SND_LOSE,
    CIV_SND_WIN,
    /* 2 special */
    CIV_SND_OPENING,
    CIV_SND_AIR_NUKE,
    CIV_SND_COUNT,
} civ_sound_id_t;

/* 初始化 SDL audio. 失敗回 -1, 無 audio 但程式繼續 (可以靜音玩). */
int civ_audio_init(const char *data_dir);
void civ_audio_shutdown(void);

/* Async play, 不阻塞主迴圈. data_dir 內找 wav, 載入 cache (LRU N) */
void civ_audio_play(civ_sound_id_t snd);

/* leader speech 對齊 spec 06 §6.7 civ slot (1..15) */
void civ_audio_play_leader(int civ_slot);

#endif
```

實作建議:
- 用 SDL_audio (內建 SDL2, 不需 SDL_mixer 額外依賴)
- WAV cache: 24 個 WAV 總 ~1.8 MB, 全 preload 沒問題
- Leader speech 配音 60 KB 量級, 11kHz × 8-bit × 60 s = 660 KB, 合理

## 8.4  與其他 spec 關係

| spec | 來源 | 用途 |
|---|---|---|
| spec 01 §1.4 | MMSYSTEM 4 calls | 本 spec §8.2 待對位 |
| spec 06 §6.7 | 14 leader slots | §8.1.1 對應 |
| spec 09 §9.1 | 勝負結局 | §8.1.4 結局音 trigger |
| spec 02 §2.1.2 | startup 路徑 | OPENING.WAV 在 startup play |

## 8.5  License & credit

23 (24) 個 WAV 檔 © 1993 MicroProse — **不入 repo** (檔案大 + 版權). 使用者自備.
Team B `audio.c` 實作為 MIT.
中文 leader name 沿用 civ_dict.c (CC BY-SA 4.0).

## 8.6  v0.2 (待後 round) 待補

- §8.2 MMSYSTEM 4 call sites 確切 API + caller 識別 (Ghidra walk)
- §8.1 額外: WAV 內 Civilopedia subtitle 文字 (推測無, 但要確認)
- 背景 BGM 是否有 (manual 提 OPENING 是 jingle, 沒提 in-game BGM, 但 WINTUNE 暗示有)
- volume / mute 設定 (1991 manual 提 Preferences 可關音效)
