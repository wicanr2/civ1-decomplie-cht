/*
 * gfx/present.h — palette framebuffer → SDL renderer
 *
 * 對應 SDL_IMPLEMENTATION_PLAN §5 Present 路徑。
 * 把 8bpp indexed FB + palette 展開成 RGBA SDL_Texture，再用
 * SDL_RenderCopy + bicubic scaling 放到視窗。
 */
#ifndef CIV_GFX_PRESENT_H
#define CIV_GFX_PRESENT_H

#include "palette.h"
#include "surface.h"

#include <SDL.h>

typedef struct civ_present {
    SDL_Renderer *renderer;
    SDL_Texture  *tex;          /* RGBA texture，跟 framebuffer 同 size */
    int           tex_w, tex_h;
    int           last_palette_gen;
    int           last_fb_dirty_seq;  /* internal */
} civ_present_t;

/* 初始化。renderer 必須先存在。 */
int  civ_present_init(civ_present_t *pr, SDL_Renderer *renderer,
                      int fb_w, int fb_h);
void civ_present_destroy(civ_present_t *pr);

/* 提交一個 frame：若 fb dirty 或 palette 變過 → 重建 texture；
 * SDL_RenderCopy 到視窗（caller 自己 SDL_RenderPresent）。 */
void civ_present_frame(civ_present_t *pr,
                       civ_surface_t *fb,
                       const civ_palette_t *pal);

#endif /* CIV_GFX_PRESENT_H */
