/*
 * civ_widgets.h — 建立 / 拆除三個內建 widget（map / minimap / status）
 *
 * 對應 spec 02 §2.1.2 WinMain F 段「創 3 個視窗」與 SDL_PLAN §4。
 */
#ifndef CIV_WIDGETS_H
#define CIV_WIDGETS_H

struct civ_game;

/* 在 game 內配置三個 widget（rect 依 640×480 layout 固定）並 register
 * 成 game->map_w / minimap_w / status_w。成功回 0；失敗回 -1。 */
int  civ_widgets_register(struct civ_game *g);

/* 反向，destroy 並 NULL 化 game 內三個 pointer。 */
void civ_widgets_unregister(struct civ_game *g);

/* 把所有 visible widget 畫到 framebuffer 上（map → minimap → status
 * 順序，後畫覆蓋前畫；不重疊則順序無關）。 */
void civ_widgets_render_all(struct civ_game *g);

#endif /* CIV_WIDGETS_H */
