/*
 * test_wizard.c — M4-full 驗證：新局精靈狀態機
 *
 * 用 SDL_Event 模擬 keyboard 輸入跑完整三頁流程，驗證最後選擇結果。
 * 純邏輯測，不開視窗。
 */
#include "civ_game.h"
#include "dialog/dialog.h"
#include "dialog/wizard.h"

#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EXPECT(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL %s:%d  %s\n", __FILE__, __LINE__, #cond); \
        exit(1); \
    } \
} while (0)

static void push_key(civ_dialog_stack_t *s, struct civ_game *g, SDL_Keycode k)
{
    SDL_Event ev;
    memset(&ev, 0, sizeof ev);
    ev.type = SDL_KEYDOWN;
    ev.key.keysym.sym = k;
    civ_dialog_handle_event(s, &ev, g);
}

int main(void)
{
    SDL_setenv("SDL_VIDEODRIVER", "dummy", 0);
    EXPECT(SDL_Init(SDL_INIT_VIDEO) == 0);

    struct civ_game g = {0};
    civ_dialog_stack_t stk = {0};

    civ_wizard_open(&stk, &g);
    EXPECT(stk.top != NULL);

    /* Page 1 (Difficulty)：cursor 預設 = Prince (2)
     * 按 ↓ → King (3) → 按 Enter 切到 Civ 頁 */
    push_key(&stk, &g, SDLK_DOWN);
    push_key(&stk, &g, SDLK_RETURN);
    EXPECT(stk.top != NULL);   /* 仍 active，切到 Civ 頁 */

    /* Page 2 (Civ)：cursor 預設 = 0 (linear) = slot 1 (羅馬)
     * 按 → → → → → → → 6 次 = cursor 6 = slot 7 (印度 Gandhi)
     * 按 Enter 切到 Name 頁 */
    for (int i = 0; i < 6; i++) push_key(&stk, &g, SDLK_RIGHT);
    push_key(&stk, &g, SDLK_RETURN);
    EXPECT(stk.top != NULL);

    /* Page 3 (Name)：輸入 "Alice" 然後 Enter */
    const char *name = "Alice";
    for (size_t i = 0; i < strlen(name); i++) {
        SDL_Event ev;
        memset(&ev, 0, sizeof ev);
        ev.type = SDL_TEXTINPUT;
        ev.text.text[0] = name[i];
        civ_dialog_handle_event(&stk, &ev, &g);
    }
    push_key(&stk, &g, SDLK_RETURN);

    /* dialog 應該關閉 */
    EXPECT(stk.top == NULL);

    /* timer_counter 用作測試標記：difficulty * 100 + civ_slot
     * Difficulty = King (3) → 3 * 100 = 300
     * Civ = 印度 (slot 7) → +7 = 307 */
    EXPECT(g.timer_counter == 307);

    /* 測試 ESC 取消 */
    civ_wizard_open(&stk, &g);
    EXPECT(stk.top != NULL);
    push_key(&stk, &g, SDLK_ESCAPE);
    EXPECT(stk.top == NULL);

    /* 測試 back navigation：Page 2 ESC → 回 Page 1 */
    civ_wizard_open(&stk, &g);
    push_key(&stk, &g, SDLK_RETURN);     /* Page 1 → Page 2 */
    EXPECT(stk.top != NULL);
    push_key(&stk, &g, SDLK_ESCAPE);     /* Page 2 → Page 1 */
    EXPECT(stk.top != NULL);             /* 不關閉，只切回 */
    push_key(&stk, &g, SDLK_ESCAPE);     /* Page 1 ESC → 關閉 */
    EXPECT(stk.top == NULL);

    SDL_Quit();
    printf("PASS test_wizard (狀態機 + ESC back navigation 全驗)\n");
    return 0;
}
