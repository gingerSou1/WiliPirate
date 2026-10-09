/* SPDX-License-Identifier: MIT; Copyright (c) 2026 gingerSou1 */
#include "fw2.h"
#include "ui.h"
#ifndef FIELD_STANDARD_STARTUP_APPROVED
#error "Standard BSP startup needs explicit approval; validate the UI on host instead."
#endif
static struct fa_state state;
static bool touch_available;
static uint16_t wire(uint16_t c){return (uint16_t)((c>>8)|(c<<8));}
static void clear(uint16_t c){st7796_fill_screen(wire(c));}
static void rect(int x,int y,int w,int h,uint16_t c){st7796_fill_rect(x,y,w,h,wire(c));}
static void text(int x,int y,int scale,uint16_t fg,uint16_t bg,const char *s){st7796_draw_text(x,y,scale,wire(fg),wire(bg),s);}
static const struct fa_draw draw={clear,rect,text};
static void redraw(void){fa_render(&state,&draw,touch_available);}
int main(void){
 fa_init(&state);
 board_init();
 fw2_app_recovery_init();
 st7796_init();
 touch_available=ft6336_init();
 fw2_app_about_use_lcd_restore(redraw);
 redraw();board_backlight_set(1);
 /* No OneWili link, instrument operations, power/VREF setters, storage,
  * output or release_unused policy. Only approved standard BSP startup. */
 for(;;){
  fw2_app_recovery_task();uartkbd_event_t event;
  while(uartkbd_next_event(&event))if(event.pressed&&event.btn==UARTKBD_BTN_CANCEL&&fa_back(&state))redraw();
  uint16_t x=0,y=0;bool down=touch_available&&ft6336_poll(&x,&y);
  if(fa_touch(&state,down,x,y))redraw();
  fw2_app_recovery_sleep_ms(10);
 }
}
