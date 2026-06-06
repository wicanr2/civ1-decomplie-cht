#include "widget.h"

civ_evt_result_t civ_widget_table_run(civ_widget_t *w,
                                      const civ_widget_msg_entry_t *table,
                                      size_t n,
                                      SDL_Event *ev)
{
    for (size_t i = 0; i < n; i++) {
        if (table[i].sdl_type == ev->type) {
            return table[i].fn(w, ev);
        }
    }
    return 0;
}
