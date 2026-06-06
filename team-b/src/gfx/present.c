#include "present.h"

#include <stdlib.h>

int civ_present_init(civ_present_t *pr, SDL_Renderer *renderer, int fb_w, int fb_h)
{
    pr->renderer = renderer;
    pr->tex_w = fb_w;
    pr->tex_h = fb_h;
    pr->last_palette_gen = -1;
    pr->last_fb_dirty_seq = -1;

    /* bicubic 等價於 best linear SDL filter；M1 階段先用 linear */
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

    pr->tex = SDL_CreateTexture(renderer,
                                SDL_PIXELFORMAT_RGBA32,
                                SDL_TEXTUREACCESS_STREAMING,
                                fb_w, fb_h);
    if (!pr->tex) return -1;
    SDL_SetTextureBlendMode(pr->tex, SDL_BLENDMODE_NONE);

    /* logical size 讓 SDL 自動處理視窗 resize 時的縮放 */
    SDL_RenderSetLogicalSize(renderer, fb_w, fb_h);
    return 0;
}

void civ_present_destroy(civ_present_t *pr)
{
    if (pr->tex) {
        SDL_DestroyTexture(pr->tex);
        pr->tex = NULL;
    }
    pr->renderer = NULL;
}

void civ_present_frame(civ_present_t *pr,
                       civ_surface_t *fb,
                       const civ_palette_t *pal)
{
    /* fb 或 palette 有變動才重建 texture */
    bool need_upload = fb->dirty || (pal->generation != pr->last_palette_gen);
    if (need_upload) {
        void *tex_pixels = NULL;
        int pitch = 0;
        if (SDL_LockTexture(pr->tex, NULL, &tex_pixels, &pitch) == 0) {
            uint8_t *src = fb->pixels;
            uint8_t *dst = (uint8_t *)tex_pixels;
            for (int y = 0; y < fb->h; y++) {
                uint8_t       *drow = dst + (size_t)y * pitch;
                const uint8_t *srow = src + (size_t)y * fb->pitch;
                for (int x = 0; x < fb->w; x++) {
                    civ_color_t c = pal->entries[srow[x]];
                    drow[x * 4 + 0] = c.r;
                    drow[x * 4 + 1] = c.g;
                    drow[x * 4 + 2] = c.b;
                    drow[x * 4 + 3] = c.a;
                }
            }
            SDL_UnlockTexture(pr->tex);
        }
        fb->dirty = false;
        pr->last_palette_gen = pal->generation;
    }

    SDL_SetRenderDrawColor(pr->renderer, 0, 0, 0, 255);
    SDL_RenderClear(pr->renderer);
    SDL_RenderCopy(pr->renderer, pr->tex, NULL, NULL);
}
