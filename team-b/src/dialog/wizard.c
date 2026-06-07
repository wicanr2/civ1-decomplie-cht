/*
 * wizard.c — 3 頁新局精靈狀態機 + 渲染
 */
#include "wizard.h"

#include "../civ_game.h"
#include "../data/civ_dict.h"
#include "../data/civs.h"
#include "../data/strings.h"
#include "../gfx/primitive.h"
#include "../text/text_out.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ── 內部 state ──────────────────────────────────── */

enum wizard_page {
    PAGE_DIFFICULTY = 0,
    PAGE_CIV        = 1,
    PAGE_NAME       = 2,
    PAGE_GOVERNMENT = 3,   /* R26-C 新 */
    PAGE_DONE       = 4,
};

#define NAME_MAX 16

typedef struct wizard_state {
    int      page;
    int      difficulty;     /* 0..4 */
    int      civ_slot;       /* 1..14（跳過 8 = NONE） */
    char     name[NAME_MAX + 1];
    int      name_len;
    int      government;     /* R26-C: 1..5 (排除 Anarchy 開局), 預設 1=Despotism */
    int      cursor;         /* 目前 highlight 的選項 */
} wizard_state_t;

/* ── 5 個難度 zh ────────────────────────────────── */
static const char *DIFF_ZH[5] = {
    "酋長 (Chieftain)",
    "軍閥 (Warlord)",
    "王子 (Prince)",
    "國王 (King)",
    "皇帝 (Emperor)",
};

/* ── 工具：把 slot 1..14 跳過 slot 8 (NONE) 對映到 0..12 線性 index */
static int slot_to_linear(int slot)
{
    if (slot < 1 || slot > 14) return -1;
    return slot < 8 ? slot - 1 : slot - 2;
}
static int linear_to_slot(int linear)
{
    if (linear < 0 || linear > 12) return -1;
    return linear < 7 ? linear + 1 : linear + 2;
}

/* ── render 三頁 ────────────────────────────────── */

static void render_panel_bg(civ_surface_t *fb, civ_rect_t r)
{
    civ_fill_rect(fb, r, 7);              /* 灰背景 */
    civ_frame_rect(fb, r, 0);             /* 黑邊框 */
    civ_frame_rect(fb,
        (civ_rect_t){r.x + 1, r.y + 1, r.w - 2, r.h - 2}, 8);
}

static void render_title(civ_surface_t *fb, civ_font_t *font_title,
                          civ_rect_t r, const char *txt)
{
    civ_fill_rect(fb, (civ_rect_t){r.x, r.y, r.w, 36}, 9);
    civ_hline(fb, r.x, r.y + 36, r.w, 0);
    if (font_title) {
        int w = civ_text_measure(font_title, txt);
        civ_text_out(fb, font_title,
                     r.x + (r.w - w) / 2, r.y + 26,
                     txt, 15, 9, CIV_TEXT_BK_TRANSPARENT);
    }
}

/* R26-C: 5 個可選政府 (排除 Anarchy idx 0) — 對齊 world/government.h
 * idx 1..5 = Despotism / Monarchy / Communism / Republic / Democracy */
static const char *GOV_ZH[5] = {
    "專制 (Despotism)",
    "君主制 (Monarchy)",
    "共產 (Communism)",
    "共和 (Republic)",
    "民主 (Democracy)",
};

static void render_page_difficulty(const civ_dialog_t *d,
                                   civ_surface_t *fb,
                                   struct civ_game *g)
{
    const wizard_state_t *s = d->state;
    render_panel_bg(fb, d->rect);
    render_title(fb, g->font_title, d->rect, "新局精靈：選擇難度");

    for (int i = 0; i < 5; i++) {
        int y = d->rect.y + 60 + i * 36;
        civ_rect_t row = {d->rect.x + 20, y, d->rect.w - 40, 28};
        uint8_t bg = (i == s->cursor) ? 12 : 7;
        civ_fill_rect(fb, row, bg);
        civ_frame_rect(fb, row, 0);
        if (g->font_body) {
            civ_text_out(fb, g->font_body,
                         row.x + 10, row.y + 19,
                         DIFF_ZH[i],
                         (i == s->cursor) ? 15 : 0, bg,
                         CIV_TEXT_BK_TRANSPARENT);
        }
    }
    if (g->font_body) {
        civ_text_out(fb, g->font_body,
                     d->rect.x + 20, d->rect.y + d->rect.h - 14,
                     "↑↓ 選擇　Enter 下一頁　ESC 取消",
                     0, 7, CIV_TEXT_BK_TRANSPARENT);
    }
}

static void render_page_civ(const civ_dialog_t *d, civ_surface_t *fb,
                            struct civ_game *g)
{
    const wizard_state_t *s = d->state;
    render_panel_bg(fb, d->rect);
    render_title(fb, g->font_title, d->rect, "新局精靈：選擇文明");

    /* 13 個有效文明（跳過 slot 8 = NONE）排成 7×2 grid */
    int cols = 7;
    int rows = 2;
    int gw = (d->rect.w - 40) / cols;
    int gh = 56;
    for (int linear = 0; linear < 13; linear++) {
        int col = linear % cols;
        int row = linear / cols;
        int x = d->rect.x + 20 + col * gw;
        int y = d->rect.y + 60 + row * (gh + 8);
        civ_rect_t cell = {x, y, gw - 4, gh};
        uint8_t bg = (linear == s->cursor) ? 12 : 7;
        civ_fill_rect(fb, cell, bg);
        civ_frame_rect(fb, cell, 0);
        int slot = linear_to_slot(linear);
        if (g->font_body) {
            const char *zh_civ    = civ_civs_zh(slot, "civ_sing");
            const char *zh_leader = civ_civs_zh(slot, "leader");
            char buf[64];
            snprintf(buf, sizeof buf, "%s",   zh_civ    ? zh_civ : "?");
            civ_text_out(fb, g->font_body, x + 6, y + 18, buf,
                         (linear == s->cursor) ? 15 : 0, bg,
                         CIV_TEXT_BK_TRANSPARENT);
            snprintf(buf, sizeof buf, "%s", zh_leader ? zh_leader : "?");
            civ_text_out(fb, g->font_body, x + 6, y + 38, buf,
                         (linear == s->cursor) ? 15 : 1, bg,
                         CIV_TEXT_BK_TRANSPARENT);
        }
    }
    if (g->font_body) {
        civ_text_out(fb, g->font_body,
                     d->rect.x + 20, d->rect.y + d->rect.h - 14,
                     "←↑↓→ 選擇　Enter 下一頁　ESC 回上一頁",
                     0, 7, CIV_TEXT_BK_TRANSPARENT);
    }
}

static void render_page_name(const civ_dialog_t *d, civ_surface_t *fb,
                             struct civ_game *g)
{
    const wizard_state_t *s = d->state;
    render_panel_bg(fb, d->rect);
    render_title(fb, g->font_title, d->rect, "新局精靈：輸入名稱");

    if (g->font_body) {
        char buf[128];
        const char *zh_diff = DIFF_ZH[s->difficulty];
        const char *zh_civ  = civ_civs_zh(s->civ_slot, "civ_sing");
        const char *zh_lead = civ_civs_zh(s->civ_slot, "leader");
        snprintf(buf, sizeof buf, "難度：%s", zh_diff);
        civ_text_out(fb, g->font_body, d->rect.x + 20, d->rect.y + 60,
                     buf, 0, 7, CIV_TEXT_BK_TRANSPARENT);
        snprintf(buf, sizeof buf, "文明：%s", zh_civ ? zh_civ : "?");
        civ_text_out(fb, g->font_body, d->rect.x + 20, d->rect.y + 80,
                     buf, 0, 7, CIV_TEXT_BK_TRANSPARENT);
        snprintf(buf, sizeof buf, "領袖：%s", zh_lead ? zh_lead : "?");
        civ_text_out(fb, g->font_body, d->rect.x + 20, d->rect.y + 100,
                     buf, 0, 7, CIV_TEXT_BK_TRANSPARENT);

        civ_text_out(fb, g->font_body, d->rect.x + 20, d->rect.y + 140,
                     "您的稱號:", 0, 7, CIV_TEXT_BK_TRANSPARENT);
    }

    /* 名稱輸入框 */
    civ_rect_t name_box = {d->rect.x + 20, d->rect.y + 160,
                           d->rect.w - 40, 30};
    civ_fill_rect(fb, name_box, 15);
    civ_frame_rect(fb, name_box, 0);
    if (g->font_body) {
        char buf[NAME_MAX + 2];
        snprintf(buf, sizeof buf, "%s%s", s->name, "_");
        civ_text_out(fb, g->font_body,
                     name_box.x + 8, name_box.y + 20,
                     buf, 0, 15, CIV_TEXT_BK_TRANSPARENT);
    }

    if (g->font_body) {
        civ_text_out(fb, g->font_body,
                     d->rect.x + 20, d->rect.y + d->rect.h - 14,
                     "鍵入名稱　Enter 下一頁　ESC 回上一頁",
                     0, 7, CIV_TEXT_BK_TRANSPARENT);
    }
}

/* R26-C: PAGE_GOVERNMENT render — 對齊 difficulty page 排版.
 * cursor = 1..5 (對應 government enum 1..5, 排除 Anarchy 0). */
static void render_page_government(const civ_dialog_t *d,
                                    civ_surface_t *fb,
                                    struct civ_game *g)
{
    const wizard_state_t *s = d->state;
    render_panel_bg(fb, d->rect);
    render_title(fb, g->font_title, d->rect, "新局精靈：選擇政府");

    for (int i = 0; i < 5; i++) {
        int row_idx = i + 1;   /* 對應 government enum 值 */
        int y = d->rect.y + 60 + i * 36;
        civ_rect_t row = {d->rect.x + 20, y, d->rect.w - 40, 28};
        uint8_t bg = (row_idx == s->cursor) ? 12 : 7;
        civ_fill_rect(fb, row, bg);
        civ_frame_rect(fb, row, 0);
        if (g->font_body) {
            civ_text_out(fb, g->font_body,
                         row.x + 10, row.y + 19,
                         GOV_ZH[i],
                         (row_idx == s->cursor) ? 15 : 0, bg,
                         CIV_TEXT_BK_TRANSPARENT);
        }
    }
    if (g->font_body) {
        civ_text_out(fb, g->font_body,
                     d->rect.x + 20, d->rect.y + d->rect.h - 14,
                     "↑↓ 選擇　Enter 開始遊戲　ESC 回上一頁",
                     0, 7, CIV_TEXT_BK_TRANSPARENT);
    }
}

static void render(const civ_dialog_t *d, civ_surface_t *fb,
                   struct civ_game *g)
{
    const wizard_state_t *s = d->state;
    switch (s->page) {
    case PAGE_DIFFICULTY: render_page_difficulty(d, fb, g); break;
    case PAGE_CIV:        render_page_civ(d, fb, g);        break;
    case PAGE_NAME:       render_page_name(d, fb, g);       break;
    case PAGE_GOVERNMENT: render_page_government(d, fb, g); break;
    default: break;
    }
}

/* ── event 處理 ───────────────────────────────── */

static civ_dlg_result_t handle_difficulty(civ_dialog_t *d, SDL_Event *ev)
{
    wizard_state_t *s = d->state;
    if (ev->type != SDL_KEYDOWN) return CIV_DLG_CONTINUE;
    switch (ev->key.keysym.sym) {
    case SDLK_UP:   if (s->cursor > 0) s->cursor--; break;
    case SDLK_DOWN: if (s->cursor < 4) s->cursor++; break;
    case SDLK_RETURN:
        s->difficulty = s->cursor;
        s->page = PAGE_CIV;
        s->cursor = 0;
        return CIV_DLG_NEXT;
    case SDLK_ESCAPE:
        return CIV_DLG_CLOSE;
    }
    return CIV_DLG_CONTINUE;
}

static civ_dlg_result_t handle_civ(civ_dialog_t *d, SDL_Event *ev)
{
    wizard_state_t *s = d->state;
    if (ev->type != SDL_KEYDOWN) return CIV_DLG_CONTINUE;
    switch (ev->key.keysym.sym) {
    case SDLK_LEFT:  if (s->cursor > 0)  s->cursor--; break;
    case SDLK_RIGHT: if (s->cursor < 12) s->cursor++; break;
    case SDLK_UP:    if (s->cursor >= 7) s->cursor -= 7; break;
    case SDLK_DOWN:  if (s->cursor + 7 <= 12) s->cursor += 7; break;
    case SDLK_RETURN:
        s->civ_slot = linear_to_slot(s->cursor);
        s->page = PAGE_NAME;
        return CIV_DLG_NEXT;
    case SDLK_ESCAPE:
        s->page = PAGE_DIFFICULTY;
        s->cursor = s->difficulty;
        return CIV_DLG_PREV;
    }
    return CIV_DLG_CONTINUE;
}

static civ_dlg_result_t handle_name(civ_dialog_t *d, SDL_Event *ev,
                                    struct civ_game *g)
{
    wizard_state_t *s = d->state;
    if (ev->type == SDL_KEYDOWN) {
        switch (ev->key.keysym.sym) {
        case SDLK_RETURN:
            /* R24: 寫 player 選擇的文明 slot 進 world */
            if (s->civ_slot >= 1 && s->civ_slot <= 14)
                g->world.player_civ_slot = s->civ_slot;
            /* R26-C: 名字輸完去政府選擇 page (非 DONE) */
            s->page = PAGE_GOVERNMENT;
            s->cursor = s->government;   /* 預設 highlight = Despotism */
            return CIV_DLG_NEXT;
        case SDLK_ESCAPE:
            s->page = PAGE_CIV;
            s->cursor = slot_to_linear(s->civ_slot);
            return CIV_DLG_PREV;
        case SDLK_BACKSPACE:
            if (s->name_len > 0) {
                /* UTF-8 安全 backspace：往前找 codepoint 起始 */
                s->name_len--;
                while (s->name_len > 0 &&
                       (s->name[s->name_len] & 0xC0) == 0x80) {
                    s->name_len--;
                }
                s->name[s->name_len] = '\0';
            }
            break;
        }
    }
    if (ev->type == SDL_TEXTINPUT) {
        size_t add = strlen(ev->text.text);
        if (s->name_len + add < NAME_MAX) {
            memcpy(s->name + s->name_len, ev->text.text, add);
            s->name_len += (int)add;
            s->name[s->name_len] = '\0';
        }
    }
    return CIV_DLG_CONTINUE;
}

/* R26-C: PAGE_GOVERNMENT handler. cursor = 1..5 對應 government enum.
 * RETURN 寫 world.player_government → 關閉精靈進入 game. */
static civ_dlg_result_t handle_government(civ_dialog_t *d, SDL_Event *ev,
                                           struct civ_game *g)
{
    wizard_state_t *s = d->state;
    if (ev->type != SDL_KEYDOWN) return CIV_DLG_CONTINUE;
    switch (ev->key.keysym.sym) {
    case SDLK_UP:    if (s->cursor > 1) s->cursor--; break;
    case SDLK_DOWN:  if (s->cursor < 5) s->cursor++; break;
    case SDLK_RETURN:
        s->government = s->cursor;
        if (s->government >= 1 && s->government <= 5)
            g->world.player_government = s->government;
        /* 簡單 sanity 標記：與 M4-full 同公式，方便 test_wizard 對位 */
        g->timer_counter = (uint32_t)(s->difficulty * 100 + s->civ_slot);
        s->page = PAGE_DONE;
        return CIV_DLG_CLOSE;
    case SDLK_ESCAPE:
        s->page = PAGE_NAME;
        return CIV_DLG_PREV;
    }
    return CIV_DLG_CONTINUE;
}

static civ_dlg_result_t event(civ_dialog_t *d, SDL_Event *ev,
                              struct civ_game *g)
{
    wizard_state_t *s = d->state;
    switch (s->page) {
    case PAGE_DIFFICULTY: return handle_difficulty(d, ev);
    case PAGE_CIV:        return handle_civ(d, ev);
    case PAGE_NAME:       return handle_name(d, ev, g);
    case PAGE_GOVERNMENT: return handle_government(d, ev, g);
    default: return CIV_DLG_CLOSE;
    }
}

static void destroy(civ_dialog_t *d)
{
    free(d->state);
    free(d);
}

void civ_wizard_open(civ_dialog_stack_t *stk, struct civ_game *g)
{
    (void)g;
    civ_dialog_t *d = calloc(1, sizeof *d);
    wizard_state_t *st = calloc(1, sizeof *st);
    if (!d || !st) { free(d); free(st); return; }

    st->page       = PAGE_DIFFICULTY;
    st->difficulty = CIV_DIFF_PRINCE;
    st->civ_slot   = 1;
    st->government = 1;   /* R26-C: 1=Despotism, 真實 Civ1 新文明預設 */
    st->cursor     = CIV_DIFF_PRINCE;

    d->name    = "wizard";
    d->rect    = (civ_rect_t){80, 60, 480, 360};
    d->state   = st;
    d->event   = event;
    d->render  = render;
    d->destroy = destroy;

    civ_dialog_push(stk, d);
}
