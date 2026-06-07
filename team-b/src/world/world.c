#include "world.h"

#include <stdio.h>
#include <string.h>

/*
 * world.c — terrain enum → SPR32X32 (col, row) lookup + PoC demo 地圖
 *
 * M5-真落地 (2026-06-06 第二輪)：
 *   terrain[][] 從原本「row*46+col 的 sprite linear index」改為存
 *   civ_terrain_kind_t enum 值，map.c 透過 civ_terrain_sprite_coord()
 *   查 SPR32X32 內代表 tile (col, row)。
 *
 * SPR32X32 layout (1472×400 = 46 cols × 12 rows, 視覺辨認):
 *   cols  0..13 = rivers / forests / units / icons (個別 sprite)
 *   cols 14..29 = 純 terrain band, 每 row 一種地形 16 變體
 *   cols 30..45 = 城市 / wonder / 領袖肖像 / 軍事單位
 *
 * 中央代表 col 取 22 (相當於 band 中段「正常 tile」非邊界)。
 * 確切座標表是 best-guess，spec 06 ground-truth pass 會修。
 */

/* Ground-truth 對位 (D-spec06, 2026-06-06):
 *
 * 工具: team-a/tools/crop_terrain_rows.sh + crop_full_rows.sh
 * 證據: docs/screenshots/terrain_rows/row{0..11}_col22_zoom4x.png
 *
 * SPR32X32 cols 14-29 是 terrain band (每 row 一種地形 16 個 neighbor blend
 * 變體)。col 22 (band 中段) 是「無鄰接」的代表 tile,M5 用這當每地形 default。
 * cols 0-13 含 rivers + forests + units + 特殊資源 sprite。
 *
 * 12 row 視覺辨識結果 (見 [[project_civ1_cht_track_c]]):
 */
static const struct {
    int col, row;
} TERRAIN_SPRITE[CIV_TERRAIN_COUNT] = {
    [CIV_TERRAIN_DESERT]   = { 22, 0 },  /* row 0: 乾黃綠 + 仙人掌 */
    [CIV_TERRAIN_PLAINS]   = { 22, 1 },  /* row 1: 密集橘綠平原 */
    [CIV_TERRAIN_GRASS]    = { 22, 2 },  /* row 2: 淡黃綠草原 */
    [CIV_TERRAIN_FOREST]   = { 22, 3 },  /* row 3: 深綠樹叢 */
    [CIV_TERRAIN_JUNGLE]   = { 22, 4 },  /* row 4: 深綠斜紋雨林 */
    [CIV_TERRAIN_MOUNTAIN] = { 22, 5 },  /* row 5: 灰白岩石尖峰 */
    [CIV_TERRAIN_HILLS]    = { 22, 5 },  /* (暫共用 mountain band, 真 hills
                                            可能在 cols 0-13 某處,待 ground-truth 二輪) */
    [CIV_TERRAIN_TUNDRA]   = { 22, 6 },  /* row 6: 冰冷雜色 */
    [CIV_TERRAIN_ARCTIC]   = { 22, 7 },  /* row 7: 白雪藍混 glacier */
    [CIV_TERRAIN_SWAMP]    = { 22, 8 },  /* row 8: 綠底藍水塊 */
    [CIV_TERRAIN_OCEAN]    = { 22, 9 },  /* row 9: 深藍 ocean */
    [CIV_TERRAIN_RIVER]    = {  4, 4 },  /* row 4 cols 0-13: 藍蛇形 river 變體 */
};

void civ_terrain_sprite_coord(civ_terrain_kind_t kind, int *out_col, int *out_row)
{
    if (kind < 0 || kind >= CIV_TERRAIN_COUNT) kind = CIV_TERRAIN_OCEAN;
    if (out_col) *out_col = TERRAIN_SPRITE[kind].col;
    if (out_row) *out_row = TERRAIN_SPRITE[kind].row;
}

/* 60×30 PoC 地圖:
 *   外圈 2 格 OCEAN 包圍
 *   中央大陸 GRASS 為主
 *   散布若干 hardcoded blob (mountain / forest / hills / desert / river)
 *   不需要 Perlin/雜訊,讓視覺辨認得出原版各 terrain 即可
 */

static void fill_blob(civ_world_t *w, int cx, int cy, int rx, int ry,
                      civ_terrain_kind_t kind)
{
    for (int y = cy - ry; y <= cy + ry; y++) {
        for (int x = cx - rx; x <= cx + rx; x++) {
            if (x < 0 || x >= CIV_MAP_W || y < 0 || y >= CIV_MAP_H) continue;
            /* 橢圓判定 */
            int dx = x - cx, dy = y - cy;
            if (rx == 0 || ry == 0) {
                w->terrain[y][x] = (uint8_t)kind;
            } else if (dx*dx*ry*ry + dy*dy*rx*rx <= rx*rx*ry*ry) {
                w->terrain[y][x] = (uint8_t)kind;
            }
        }
    }
}

void civ_world_init_demo(civ_world_t *w)
{
    if (!w) return;
    memset(w, 0, sizeof *w);

    /* 1. 全 OCEAN base */
    for (int y = 0; y < CIV_MAP_H; y++)
        for (int x = 0; x < CIV_MAP_W; x++)
            w->terrain[y][x] = (uint8_t)CIV_TERRAIN_OCEAN;

    /* 2. 中央大陸 GRASS (排除外圍 ~3 格海岸) */
    for (int y = 3; y < CIV_MAP_H - 3; y++) {
        for (int x = 4; x < CIV_MAP_W - 4; x++) {
            /* 橢圓大陸 — 簡單裁形,讓海岸線不齊整 */
            int dx = x - CIV_MAP_W / 2;
            int dy = y - CIV_MAP_H / 2;
            int Rx = CIV_MAP_W / 2 - 4;
            int Ry = CIV_MAP_H / 2 - 3;
            if (dx*dx*Ry*Ry + dy*dy*Rx*Rx <= Rx*Rx*Ry*Ry)
                w->terrain[y][x] = (uint8_t)CIV_TERRAIN_GRASS;
        }
    }

    /* 3. 散布 terrain blob */
    fill_blob(w, 12,  8, 3, 2, CIV_TERRAIN_FOREST);
    fill_blob(w, 20, 12, 2, 2, CIV_TERRAIN_MOUNTAIN);
    fill_blob(w, 32, 10, 3, 2, CIV_TERRAIN_HILLS);
    fill_blob(w, 45, 14, 3, 2, CIV_TERRAIN_DESERT);
    fill_blob(w, 18, 20, 4, 2, CIV_TERRAIN_PLAINS);
    fill_blob(w, 38, 22, 3, 2, CIV_TERRAIN_JUNGLE);
    fill_blob(w, 28, 18, 2, 1, CIV_TERRAIN_SWAMP);

    /* 4. 北極帶 — 最上一橫排陸地改 ARCTIC */
    for (int x = 0; x < CIV_MAP_W; x++) {
        if (w->terrain[3][x] != (uint8_t)CIV_TERRAIN_OCEAN)
            w->terrain[3][x] = (uint8_t)CIV_TERRAIN_ARCTIC;
        if (CIV_MAP_H - 4 >= 0 &&
            w->terrain[CIV_MAP_H - 4][x] != (uint8_t)CIV_TERRAIN_OCEAN)
            w->terrain[CIV_MAP_H - 4][x] = (uint8_t)CIV_TERRAIN_TUNDRA;
    }

    /* 5. 一條從北至南河流 (column 25 大陸範圍內) */
    for (int y = 4; y < CIV_MAP_H - 4; y++) {
        int rx = 25 + ((y % 4 == 0) ? 1 : 0);
        if (rx >= 0 && rx < CIV_MAP_W &&
            w->terrain[y][rx] != (uint8_t)CIV_TERRAIN_OCEAN)
            w->terrain[y][rx] = (uint8_t)CIV_TERRAIN_RIVER;
    }

    /* 視窗 view 起始 (M6-full-lite 後改):
     *   map widget 15 cols × 14 rows visible @ 32×32 tile
     *   shift 到 (20, 10) 讓 player 1 (30..31, 15..16) + 1 barbarian
     *   (21, 12) + player 4 (20, 22) 都能進視野, 展示多 player 場景. */
    w->view_x   = 20;
    w->view_y   = 10;
    w->cursor_x = CIV_MAP_W / 2;
    w->cursor_y = CIV_MAP_H / 2;

    /* M6-full-lite: spawn 8 player 的 starting unit
     *   player 1 (Roman 凱撒) 取地圖中央: 2 settler + 1 phalanx
     *   player 2..7 各放 1 unit 在大陸不同象限
     *   player 0 = barbarian: 放 2 個 militia 在 jungle / mountain 邊
     */
    w->units_count   = 0;
    w->selected_unit = -1;
    w->last_combat_msg[0] = '\0';
    w->player_civ_slot = 1;       /* R23: 預設 player = Roman (slot 1, Caesar) */
    w->player_government = 1;     /* R24: 預設 Despotism (新文明開局) */

    civ_world_spawn_unit(w, CIV_UNIT_SETTLERS, 1, 30, 15);
    civ_world_spawn_unit(w, CIV_UNIT_SETTLERS, 1, 31, 15);
    civ_world_spawn_unit(w, CIV_UNIT_PHALANX,  1, 30, 16);

    civ_world_spawn_unit(w, CIV_UNIT_SETTLERS, 2, 15, 10);
    civ_world_spawn_unit(w, CIV_UNIT_LEGION,   2, 16, 10);

    civ_world_spawn_unit(w, CIV_UNIT_SETTLERS, 3, 45, 12);
    civ_world_spawn_unit(w, CIV_UNIT_LEGION,   3, 46, 12);

    civ_world_spawn_unit(w, CIV_UNIT_SETTLERS, 4, 20, 22);

    civ_world_spawn_unit(w, CIV_UNIT_SETTLERS, 5, 42, 22);

    /* barbarian camp - 在 jungle / mountain 區放敵兵 */
    civ_world_spawn_unit(w, CIV_UNIT_MILITIA, 0, 38, 22);
    civ_world_spawn_unit(w, CIV_UNIT_MILITIA, 0, 21, 12);

    /* R5 M6-full: spawn 4 cities (一個 per major player slot) ===== */
    w->cities_count = 0;
    civ_world_spawn_city(w, "羅馬",       1, 28, 14, 3);   /* Player 1 Caesar */
    civ_world_spawn_city(w, "巴比倫",     2, 17, 11, 2);   /* Player 2 Hammurabi */
    civ_world_spawn_city(w, "底比斯",     3, 44, 13, 2);   /* Player 3 Ramesses */
    civ_world_spawn_city(w, "雅典",       6, 22, 20, 1);   /* Player 6 Alexander */

    /* 預設選 player 1 第一個 settler */
    w->selected_unit = 0;
    if (w->units_count > 0) {
        w->cursor_x = w->units[0].x;
        w->cursor_y = w->units[0].y;
    }
}

int civ_world_spawn_city(civ_world_t *w, const char *name_zh, uint8_t owner,
                         int x, int y, int initial_pop)
{
    if (!w || w->cities_count >= CIV_MAX_CITIES) return -1;
    if (x < 0 || x >= CIV_MAP_W || y < 0 || y >= CIV_MAP_H) return -1;
    civ_city_t *c = &w->cities[w->cities_count];
    memset(c, 0, sizeof *c);
    if (name_zh) {
        strncpy(c->name, name_zh, sizeof c->name - 1);
        c->name[sizeof c->name - 1] = '\0';
    }
    c->x = (int16_t)x;
    c->y = (int16_t)y;
    c->owner = owner;
    c->population = (uint8_t)initial_pop;
    c->building_target = -1;
    c->shield_stock = 0;
    c->food_stock = 0;
    c->buildings_bitmap = (1u << 1);  /* Palace built (Civ1 capitol 默認) */
    c->alive = true;
    return w->cities_count++;
}

int civ_world_city_at(const civ_world_t *w, int x, int y)
{
    if (!w) return -1;
    for (int i = 0; i < w->cities_count; i++) {
        if (w->cities[i].alive &&
            w->cities[i].x == x && w->cities[i].y == y)
            return i;
    }
    return -1;
}

int civ_world_spawn_unit(civ_world_t *w, civ_unit_type_t t,
                         uint8_t owner, int x, int y)
{
    if (!w || w->units_count >= CIV_MAX_UNITS) return -1;
    if (x < 0 || x >= CIV_MAP_W || y < 0 || y >= CIV_MAP_H) return -1;
    int atk, def, moves;
    civ_unit_stats(t, &atk, &def, &moves);
    civ_unit_t *u = &w->units[w->units_count];
    u->type        = t;
    u->owner       = owner;
    u->hp          = 20;
    u->moves_left  = (uint8_t)moves;
    u->x           = (int16_t)x;
    u->y           = (int16_t)y;
    u->alive       = true;
    return w->units_count++;
}

int civ_world_unit_at(const civ_world_t *w, int x, int y)
{
    if (!w) return -1;
    for (int i = 0; i < w->units_count; i++) {
        if (w->units[i].alive && w->units[i].x == x && w->units[i].y == y)
            return i;
    }
    return -1;
}

bool civ_world_move_selected(civ_world_t *w, int dx, int dy,
                             int view_cols, int view_rows)
{
    if (!w || w->selected_unit < 0 || w->selected_unit >= w->units_count)
        return false;
    civ_unit_t *u = &w->units[w->selected_unit];
    if (!u->alive || u->moves_left == 0) return false;

    int nx = u->x + dx;
    int ny = u->y + dy;
    if (nx < 0 || nx >= CIV_MAP_W || ny < 0 || ny >= CIV_MAP_H) return false;

    /* OCEAN 拒絕 (M6-lite: 沒處理 transport ship) */
    if (w->terrain[ny][nx] == (uint8_t)CIV_TERRAIN_OCEAN) return false;

    int target = civ_world_unit_at(w, nx, ny);
    if (target >= 0) {
        civ_unit_t *d = &w->units[target];
        if (d->owner == u->owner) return false;  /* 自家 unit, 拒絕 */
        /* 戰鬥 */
        const char *atk_name = civ_unit_name_zh(u->type);
        const char *def_name = civ_unit_name_zh(d->type);
        bool win = civ_unit_attack_resolve(u, d);
        if (win) {
            snprintf(w->last_combat_msg, sizeof w->last_combat_msg,
                     "%s 擊敗 %s", atk_name, def_name);
            u->x = (int16_t)nx;
            u->y = (int16_t)ny;
        } else {
            snprintf(w->last_combat_msg, sizeof w->last_combat_msg,
                     "%s 被 %s 擊敗", atk_name, def_name);
            w->selected_unit = -1;
        }
        u->moves_left = 0;
    } else {
        u->x = (int16_t)nx;
        u->y = (int16_t)ny;
        u->moves_left--;
    }

    /* 同步 cursor + view scroll */
    if (w->selected_unit >= 0 && w->units[w->selected_unit].alive) {
        w->cursor_x = w->units[w->selected_unit].x;
        w->cursor_y = w->units[w->selected_unit].y;
    } else {
        w->cursor_x = nx;
        w->cursor_y = ny;
    }
    int margin = 2;
    if (w->cursor_x - w->view_x < margin && w->view_x > 0) w->view_x--;
    if (w->cursor_x - w->view_x >= view_cols - margin &&
        w->view_x + view_cols < CIV_MAP_W) w->view_x++;
    if (w->cursor_y - w->view_y < margin && w->view_y > 0) w->view_y--;
    if (w->cursor_y - w->view_y >= view_rows - margin &&
        w->view_y + view_rows < CIV_MAP_H) w->view_y++;
    return true;
}

void civ_world_cycle_selection(civ_world_t *w, int player_slot)
{
    if (!w || w->units_count == 0) return;
    int start = w->selected_unit < 0 ? 0 : (w->selected_unit + 1);
    for (int probe = 0; probe < w->units_count; probe++) {
        int i = (start + probe) % w->units_count;
        civ_unit_t *u = &w->units[i];
        if (u->alive && u->owner == player_slot && u->moves_left > 0) {
            w->selected_unit = i;
            w->cursor_x = u->x;
            w->cursor_y = u->y;
            return;
        }
    }
}

void civ_world_move_cursor(civ_world_t *w, int dx, int dy,
                           int cols, int rows)
{
    if (!w) return;
    int nx = w->cursor_x + dx;
    int ny = w->cursor_y + dy;
    if (nx < 0)            nx = 0;
    if (ny < 0)            ny = 0;
    if (nx >= CIV_MAP_W)   nx = CIV_MAP_W - 1;
    if (ny >= CIV_MAP_H)   ny = CIV_MAP_H - 1;
    w->cursor_x = nx;
    w->cursor_y = ny;

    int margin = 2;
    if (nx - w->view_x < margin && w->view_x > 0) w->view_x--;
    if (nx - w->view_x >= cols - margin && w->view_x + cols < CIV_MAP_W) w->view_x++;
    if (ny - w->view_y < margin && w->view_y > 0) w->view_y--;
    if (ny - w->view_y >= rows - margin && w->view_y + rows < CIV_MAP_H) w->view_y++;
}

/* R24: 政府型態 (1..6) → GOVT*M sheet idx (0..2).
 * CIVDATA2 內只有 3 種 backdrop, 故 6 政府要 collapse 進 3 styles. */
int civ_government_to_govt_idx(int government)
{
    switch (government) {
        case 1: /* Despotism */
        case 2: /* Monarchy  */
        case 6: /* Anarchy   */
            return 0;   /* GOVT0M — 古代寶座 */
        case 3: /* Communism */
        case 4: /* Republic  */
            return 1;   /* GOVT1M — 中世紀 */
        case 5: /* Democracy */
            return 2;   /* GOVT2M — 現代議會 */
        default:
            return 1;   /* fallback monarchy */
    }
}
