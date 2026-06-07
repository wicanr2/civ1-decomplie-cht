/*
 * world/world.h — 世界地圖（terrain grid + view + cursor）
 *
 * Civ1 原版地圖 80×50 wrap-around；M5-lite 走較小尺寸 60×30 簡化。
 * terrain[] 每格存 civ_terrain_kind_t 語意 (0..CIV_TERRAIN_COUNT-1)，
 * 渲染時 widgets/map.c 透過 civ_terrain_sprite_coord() 查 SPR32X32
 * 內的 (col, row) 真實貼圖位置。
 */
#ifndef CIV_WORLD_WORLD_H
#define CIV_WORLD_WORLD_H

#include "city.h"
#include "unit.h"

#include <stdbool.h>
#include <stdint.h>

#define CIV_MAP_W 60
#define CIV_MAP_H 30

typedef enum {
    CIV_TERRAIN_OCEAN    = 0,
    CIV_TERRAIN_GRASS    = 1,
    CIV_TERRAIN_PLAINS   = 2,
    CIV_TERRAIN_FOREST   = 3,
    CIV_TERRAIN_MOUNTAIN = 4,
    CIV_TERRAIN_HILLS    = 5,
    CIV_TERRAIN_DESERT   = 6,
    CIV_TERRAIN_JUNGLE   = 7,
    CIV_TERRAIN_SWAMP    = 8,
    CIV_TERRAIN_TUNDRA   = 9,
    CIV_TERRAIN_ARCTIC   = 10,
    CIV_TERRAIN_RIVER    = 11,
    CIV_TERRAIN_COUNT    = 12,
} civ_terrain_kind_t;

typedef struct civ_world {
    uint8_t terrain[CIV_MAP_H][CIV_MAP_W];  /* civ_terrain_kind_t 值 */
    int     view_x, view_y;       /* top-left tile shown in widget */
    int     cursor_x, cursor_y;   /* world coords of selected tile */

    /* M6-full-lite: unit roster ------------------------------ */
    civ_unit_t units[CIV_MAX_UNITS];
    int        units_count;
    int        selected_unit;     /* index into units[]; -1 = no selection */

    /* M6-full-lite: 最近一次戰鬥結果 (給 status panel 顯示) ----- */
    char       last_combat_msg[96];

    /* R5 M6-full city: city roster (spec 06 §6.2) -------- */
    civ_city_t cities[CIV_MAX_CITIES];
    int        cities_count;

    /* R23: 玩家所屬文明 slot (1..14, 對齊 STR# 140 真實 slot).
     * 用於 status panel 國家狀態縮圖 + 對外發言 / diplomat dialog.
     * R24: wizard 完成寫此值 (取代 R23 hardcode = 1). */
    int        player_civ_slot;

    /* R24: 玩家政府型態 (對齊 spec 06 §6.3 6 種 government).
     *   1=Despotism, 2=Monarchy, 3=Communism, 4=Republic, 5=Democracy, 6=Anarchy
     * 預設 1=Despotism (新文明開局). diplomat scene + status panel 顯示此值.
     * R25+ 接 government revolution flow. */
    int        player_government;

    /* R27-A: 玩家已研發 tech bitmap. bit N (N=0..71) = CIV_TECH_NONE..FUTURE_5.
     * 對齊 world/tech.h CIV_TECH_COUNT=72. spec 09 §9.3 future_tech 因子算
     * tech_id >= CIV_TECH_FUTURE_1 (=68) 的 bit count. tech 0 = NONE 不算. */
    uint64_t   tech_acquired[2];   /* 2×64 = 128 bit, 用 72 個; future-proof. */
} civ_world_t;

/* R24: 政府型態 → GOVT*M sheet idx (CIVDATA2 內只有 3 種 backdrop):
 *   Despotism/Monarchy/Anarchy → 0 (GOVT0M, 古代寶座)
 *   Communism/Republic         → 1 (GOVT1M, 中世紀宮殿)
 *   Democracy                  → 2 (GOVT2M, 現代議會)
 * 對齊 1991/1993 原版選圖規則 (Despotism/Monarchy 共用古代 backdrop). */
int civ_government_to_govt_idx(int government);

/* 把 terrain enum 對應到 SPR32X32 內的 (col, row)。
 *
 * 對位來源: docs/screenshots/cvpc_spr32x32_decoded.png 視覺辨認。
 * SPR32X32 cols 14..29 是 terrain band，每 row 一種地形 (含 16 個變體
 * for blending — M5-lite 只取每帶中央代表 col=22)。cols 0..13 含 forest
 * / river 等獨立 sprite。座標表為近似；spec 06 ground-truth pass 再對齊。 */
void civ_terrain_sprite_coord(civ_terrain_kind_t kind, int *out_col, int *out_row);

/* 建簡易 demo 地圖：外圈海洋包圍 + 中央大陸 + 隨機 forest/mountain/hills/desert blob。 */
void civ_world_init_demo(civ_world_t *w);

/* 鍵盤移動 cursor，視 cursor 是否觸碰視窗邊緣自動 scroll。
 * cols/rows 是當前可顯示的 tile 數（widget 內）。 */
void civ_world_move_cursor(civ_world_t *w, int dx, int dy,
                           int cols, int rows);

/* M6-full-lite ──────────────────────────────────────────────── */

/* 在 (x, y) 放一個 unit, 回 index, 失敗回 -1。 */
int civ_world_spawn_unit(civ_world_t *w, civ_unit_type_t t,
                         uint8_t owner, int x, int y);

/* 找 (x, y) 上的第一個 alive unit, 沒找到回 -1。 */
int civ_world_unit_at(const civ_world_t *w, int x, int y);

/* 對 selected_unit (若 alive) 做 (dx, dy) 一格移動。
 *   - 目標格越界 / OCEAN: 拒絕
 *   - 目標格上有 enemy unit: 觸發 civ_unit_attack_resolve, 寫
 *     last_combat_msg, 勝者佔據格子
 *   - 目標格上有自己 unit: 拒絕
 *   - 空格: 直接 move
 *
 * 同步把 cursor 移到該 unit 位置, 並消耗 1 moves_left。
 * 回 true 表示有移動或戰鬥, false 表示拒絕 (caller 可重試方向). */
bool civ_world_move_selected(civ_world_t *w, int dx, int dy,
                             int view_cols, int view_rows);

/* TAB 鍵循環 player 1 (slot 0) 還有 moves_left > 0 的 unit. */
void civ_world_cycle_selection(civ_world_t *w, int player_slot);

/* R5: 在 (x, y) 放城市. 回 index, 失敗回 -1. */
int civ_world_spawn_city(civ_world_t *w, const char *name_zh, uint8_t owner,
                         int x, int y, int initial_pop);

/* 找 (x, y) 上 alive city, 沒找到回 -1. */
int civ_world_city_at(const civ_world_t *w, int x, int y);

#endif /* CIV_WORLD_WORLD_H */
