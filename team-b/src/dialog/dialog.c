#include "dialog.h"

#include <stdlib.h>

void civ_dialog_push(civ_dialog_stack_t *s, civ_dialog_t *d)
{
    if (!s || !d) return;
    /* M4-full：深度 1，如有舊的先 pop */
    if (s->top) civ_dialog_pop(s);
    s->top = d;
}

void civ_dialog_pop(civ_dialog_stack_t *s)
{
    if (!s || !s->top) return;
    civ_dialog_t *d = s->top;
    s->top = NULL;
    if (d->destroy) d->destroy(d);
    /* 注意：d 本身是否要 free 由 destroy fn 處理 */
}

bool civ_dialog_handle_event(civ_dialog_stack_t *s, SDL_Event *ev,
                              struct civ_game *g)
{
    if (!s || !s->top) return false;
    civ_dialog_t *d = s->top;
    civ_dlg_result_t r = d->event ? d->event(d, ev, g) : CIV_DLG_CONTINUE;
    if (r == CIV_DLG_CLOSE) {
        civ_dialog_pop(s);
    }
    return true;
}

void civ_dialog_render(const civ_dialog_stack_t *s, civ_surface_t *fb,
                       struct civ_game *g)
{
    if (!s || !s->top) return;
    if (s->top->render) s->top->render(s->top, fb, g);
}
