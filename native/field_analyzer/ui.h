/* SPDX-License-Identifier: MIT; Copyright (c) 2026 gingerSou1 */
#ifndef WILIPIRATE_FIELD_UI_H
#define WILIPIRATE_FIELD_UI_H
#include "model.h"
/* Backend-independent draw calls: a host sink and the official display adapter
 * execute the same renderer. No transport, storage or hardware instruments. */
struct fa_draw {
 void (*clear)(uint16_t color);
 void (*rect)(int x,int y,int w,int h,uint16_t color);
 void (*text)(int x,int y,int scale,uint16_t fg,uint16_t bg,const char *text);
};
void fa_render(const struct fa_state *s,const struct fa_draw *draw,bool touch);
#endif
