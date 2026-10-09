/* SPDX-License-Identifier: MIT; Copyright (c) 2026 gingerSou1 */
#include "model.h"
static const char *names[FA_COUNT]={"Oscilloscope","Logic Analyzer","Protocol Analyzer","Signal Generator","CAN / CAN FD","UART","I2C","SPI"};
/* Facts are transcribed from the pinned official pinout/Orca docs. Physical
 * orientation and electrical limits are not qualified, so no connect action
 * or instrument-operation API exists. See docs/CONNECTION_GUIDES.md. */
static const struct fa_guide guides[FA_COUNT]={
 {"10-pin analog header (CN23)","Analog input pin positions unverified","Ground pin position unverified","Input protection / bandwidth unverified","Probe / adapter compatibility unverified","Stock: Analog IO (voltage plot)","Desktop PicoScope needs external hardware","Pinout or electrical compatibility not yet verified."},
 {"20-pin GPIO header","Capture channel mapping unverified","Header GND: physical pins 19 / 20","FW2 input limits unverified","Maestro: documented logic connector","Stock: Logic Analyzer","Desktop: Logic Analyzer; DOM / VCD","Pinout or electrical compatibility not yet verified."},
 {"Select a protocol first","No instrument operation is available","Ground/reference requires review","Electrical compatibility requires review","Orca is not required for every tool","Stock protocol capabilities documented","Desktop interoperability unverified","Pinout or electrical compatibility not yet verified."},
 {"Analog header / GPIO: depends on output","Output pin positions unverified","Ground/reference requires review","Outputs disabled by this app","Cable / module compatibility unverified","Stock: Analog IO / Logic Player / PWM","Desktop: Logic Player (documented)","Pinout or electrical compatibility not yet verified."},
 {"20-pin header; Orca DB-15 optional","Header 16 / 18: mapping still unverified","Humpback DB-15: 6 CAN H, 14 CAN L, 8 GND","Limits / cable orientation not qualified","Humpback routes host CAN; power input optional","Stock: CAN (FD)","Desktop: CAN FD; DBC support documented","Termination and listen-only not qualified."},
 {"20-pin GPIO header","Pin 5: RX GPIO9 | Pin 9: TX GPIO8","Header GND: physical pins 19 / 20","Logic level / FW2 buffers unverified","Breakout + labelled grabbers: proposed","Stock: IO UART Log","Desktop: UART (documented)","TX crosses to RX only after compatibility review."},
 {"20-pin GPIO header: MAIN I2C0","Pin 10: SDA GPIO16 | Pin 8: SCL GPIO17","Header GND: physical pins 19 / 20","Bus voltage / pull-ups unverified","Maestro Qwiic documented; optional","Stock: IO I2C Log","Desktop: I2C (documented)","Poll FPGA prerequisite observed; no power action."},
 {"20-pin GPIO header: MAIN SPI1","CS 1 | MISO 12 | MOSI 13 | SCLK 15","Header GND: physical pins 19 / 20","Logic level / FW2 buffers unverified","Breakout + labelled grabbers: proposed","Stock: IO SPI Log","Desktop: SPI (documented)","Pinout or electrical compatibility not yet verified."}
};
const char *fa_name(enum fa_instrument i) { return (unsigned)i<FA_COUNT ? names[i] : "Unavailable"; }
const struct fa_guide *fa_connection(enum fa_instrument i) { return &guides[(unsigned)i<FA_COUNT ? i : FA_PROTOCOL]; }
struct fa_rect fa_tile(unsigned i) { return (struct fa_rect){16+(int)(i%2)*232,82+(int)(i/2)*80,216,70}; }
static bool hit(struct fa_rect r,unsigned x,unsigned y) { return x>=(unsigned)r.x && y>=(unsigned)r.y && x<(unsigned)(r.x+r.w) && y<(unsigned)(r.y+r.h); }
void fa_init(struct fa_state *s) { *s=(struct fa_state){.page=FA_HOME,.instrument=FA_SCOPE};fa_can_init(&s->can); }
bool fa_home(struct fa_state *s) { if(s->page==FA_HOME)return false;s->page=FA_HOME;return true; }
bool fa_back(struct fa_state *s) {
 if(s->page==FA_HOME)return false;
 if(s->page==FA_GLITCH_GUIDE)s->page=FA_GLITCH;
 else if(s->page==FA_GLITCH)s->page=FA_TOOLS;
 else if(s->page==FA_CAN_CONFIG)s->page=FA_GUIDE;
 else if(s->page==FA_CAN_LISTEN)s->page=FA_CAN_CONFIG;
 else if(s->page==FA_CAN_MONITOR)s->page=FA_CAN_LISTEN;
 else if(s->page==FA_CAN_REVIEW)s->page=s->review_return;
 else if(s->page==FA_PREVIEW)s->page=FA_GUIDE;
 else if(s->page==FA_GUIDE && s->instrument>=FA_CAN)s->page=FA_PROTOCOLS;
 else s->page=FA_HOME;
 return true;
}
bool fa_touch(struct fa_state *s,bool down,unsigned x,unsigned y) {
 bool rising=down&&!s->down;s->down=down;
 if(!rising||x>=FA_WIDTH||y>=FA_HEIGHT)return false;
 if(s->page!=FA_HOME&&hit((struct fa_rect){418,10,50,34},x,y))return fa_home(s);
 if(s->page==FA_HOME||s->page==FA_PROTOCOLS) {
  for(unsigned i=0;i<4;i++)if(hit(fa_tile(i),x,y)){
   s->instrument=(enum fa_instrument)(s->page==FA_PROTOCOLS ? FA_CAN+i : i);
   s->page=s->instrument==FA_PROTOCOL ? FA_PROTOCOLS : FA_GUIDE;return true;
  }
 }
 if(s->page==FA_HOME) {
  for(unsigned i=0;i<3;i++)if(hit((struct fa_rect){16+(int)i*152,250,144,34},x,y)){s->page=(enum fa_page)(FA_TOOLS+i);return true;}
 } else {
  if(hit((struct fa_rect){16,250,140,34},x,y))return fa_back(s);
  if(s->page==FA_GUIDE&&hit((struct fa_rect){170,250,294,34},x,y)){s->page=s->instrument==FA_CAN?FA_CAN_CONFIG:FA_PREVIEW;return true;}
  if(s->page==FA_PREVIEW&&hit((struct fa_rect){170,250,294,34},x,y)){s->page=FA_GUIDE;return true;}
  if(s->page==FA_TOOLS&&hit((struct fa_rect){16,120,448,50},x,y)){s->page=FA_GLITCH;return true;}
  if(s->page==FA_GLITCH&&hit((struct fa_rect){170,250,294,34},x,y)){s->page=FA_GLITCH_GUIDE;return true;}
  if(s->page==FA_CAN_CONFIG&&hit((struct fa_rect){170,250,294,34},x,y)){s->page=FA_CAN_LISTEN;return true;}
  if(s->page==FA_CAN_LISTEN&&hit((struct fa_rect){170,250,294,34},x,y)){s->page=FA_CAN_MONITOR;return true;}
  if(s->page==FA_CAN_MONITOR){
   if(hit((struct fa_rect){16,210,216,30},x,y)){fa_can_demo(&s->can);return true;}
   if(hit((struct fa_rect){248,210,216,30},x,y)&&s->can.count){fa_can_capture(&s->can);s->review=0;s->review_return=FA_CAN_MONITOR;s->page=FA_CAN_REVIEW;return true;}
  }
  if(s->page==FA_CAN_REVIEW&&s->can.captured&&hit((struct fa_rect){170,250,294,34},x,y)){s->review=(s->review+1)%s->can.captured;return true;}
  if(s->page==FA_CAPTURES&&s->can.captured&&hit((struct fa_rect){170,250,294,34},x,y)){s->review=0;s->review_return=FA_CAPTURES;s->page=FA_CAN_REVIEW;return true;}
 }
 return false;
}
