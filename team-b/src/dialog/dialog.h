/*
 * dialog/dialog.h — modal dialog framework
 *
 * 對應 spec 04 CIVDIALOGPROC (1098:1838) 與 spec 02 §2.2.5。
 * 採 modal 設計 — dialog 出現時 widget 事件被攔截，只送給 dialog；
 * 解除後恢復 widget routing。
 *
 * M4-full 用於：新局精靈 (Difficulty → Civ → Name)、Civilopedia、
 * 存讀檔、外交對話 (spec 04 dialog #999)。
 */
#ifndef CIV_DIALOG_DIALOG_H
#define CIV_DIALOG_DIALOG_H

#include "../gfx/surface.h"

#include <SDL.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct civ_game;
struct civ_dialog;

/* dialog handler 回傳值 */
typedef enum {
    CIV_DLG_CONTINUE = 0,    /* 繼續處理 */
    CIV_DLG_CLOSE,           /* 關閉 dialog */
    CIV_DLG_NEXT,            /* 切換到下一頁 (wizard 用) */
    CIV_DLG_PREV,            /* 回上一頁 */
} civ_dlg_result_t;

typedef civ_dlg_result_t (*civ_dlg_event_fn)(struct civ_dialog *d,
                                              SDL_Event *ev,
                                              struct civ_game *g);
typedef void (*civ_dlg_render_fn)(const struct civ_dialog *d,
                                  civ_surface_t *fb,
                                  struct civ_game *g);
typedef void (*civ_dlg_destroy_fn)(struct civ_dialog *d);

typedef struct civ_dialog {
    const char         *name;
    civ_rect_t          rect;
    civ_dlg_event_fn    event;
    civ_dlg_render_fn   render;
    civ_dlg_destroy_fn  destroy;
    void               *state;
} civ_dialog_t;

/* dialog 模態 stack（M4-full 階段：深度 1，未來可疊多層） */
typedef struct civ_dialog_stack {
    civ_dialog_t *top;       /* 目前 modal dialog，NULL = 無 */
} civ_dialog_stack_t;

/* push 一個 dialog 進 stack */
void civ_dialog_push(civ_dialog_stack_t *s, civ_dialog_t *d);

/* pop 並 destroy 目前 top */
void civ_dialog_pop(civ_dialog_stack_t *s);

/* 若有 dialog active，把 event 送進去，回傳 dialog 是否吃了；
 * 否則 caller 自己路由給 widget。 */
bool civ_dialog_handle_event(civ_dialog_stack_t *s, SDL_Event *ev,
                              struct civ_game *g);

/* 渲染目前 top dialog 到 framebuffer 上方（已經 render 過 widgets 後） */
void civ_dialog_render(const civ_dialog_stack_t *s, civ_surface_t *fb,
                       struct civ_game *g);

#endif /* CIV_DIALOG_DIALOG_H */
