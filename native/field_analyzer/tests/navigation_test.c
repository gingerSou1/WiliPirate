/* SPDX-License-Identifier: MIT; Copyright (c) 2026 gingerSou1 */
#include "model.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static void tap(struct fa_state *s,unsigned x,unsigned y){(void)fa_touch(s,false,x,y);assert(fa_touch(s,true,x,y));}
int main(void){
 struct fa_state s;fa_init(&s);assert(s.page==FA_HOME);
 for(unsigned i=0;i<4;i++){
  fa_init(&s);struct fa_rect r=fa_tile(i);tap(&s,(unsigned)r.x+10,(unsigned)r.y+10);
  assert(s.page==(i==2?FA_PROTOCOLS:FA_GUIDE));assert(!fa_touch(&s,true,(unsigned)r.x+10,(unsigned)r.y+10));
  if(i!=2){tap(&s,200,265);assert(s.page==FA_PREVIEW);assert(fa_back(&s));assert(s.page==FA_GUIDE);assert(fa_back(&s));assert(s.page==FA_HOME);}
  else for(unsigned p=0;p<4;p++){
   s.page=FA_PROTOCOLS;r=fa_tile(p);tap(&s,(unsigned)r.x+10,(unsigned)r.y+10);assert(s.page==FA_GUIDE&&s.instrument==(enum fa_instrument)(FA_CAN+p));
   tap(&s,200,265);
   if(p==0){
    assert(s.page==FA_CAN_CONFIG);tap(&s,200,265);assert(s.page==FA_CAN_LISTEN);
    tap(&s,200,265);assert(s.page==FA_CAN_MONITOR);
    (void)fa_touch(&s,false,300,220);assert(!fa_touch(&s,true,300,220));
    tap(&s,50,220);assert(s.can.count==1);assert(!fa_touch(&s,true,50,220));
    tap(&s,300,220);assert(s.page==FA_CAN_REVIEW&&s.can.captured==1);
    tap(&s,200,265);assert(s.review==0);assert(fa_back(&s));assert(s.page==FA_CAN_MONITOR);
    assert(fa_back(&s));assert(s.page==FA_CAN_LISTEN);assert(fa_back(&s));assert(s.page==FA_CAN_CONFIG);
    assert(fa_back(&s));assert(s.page==FA_GUIDE);
   }else {assert(s.page==FA_PREVIEW);tap(&s,200,265);assert(s.page==FA_GUIDE);}
   assert(fa_back(&s));assert(s.page==FA_PROTOCOLS);
  }
 }
 for(unsigned i=0;i<3;i++){fa_init(&s);tap(&s,20+i*152,265);assert(s.page==(enum fa_page)(FA_TOOLS+i));tap(&s,440,20);assert(s.page==FA_HOME);}
 fa_init(&s);tap(&s,20,265);tap(&s,60,140);assert(s.page==FA_GLITCH);
 tap(&s,200,265);assert(s.page==FA_GLITCH_GUIDE);assert(fa_back(&s));assert(s.page==FA_GLITCH);
 assert(fa_back(&s));assert(s.page==FA_TOOLS);assert(fa_back(&s));assert(s.page==FA_HOME);
 fa_can_demo(&s.can);fa_can_capture(&s.can);tap(&s,180,265);assert(s.page==FA_CAPTURES);
 tap(&s,200,265);assert(s.page==FA_CAN_REVIEW);assert(fa_back(&s));assert(s.page==FA_CAPTURES);
 fa_init(&s);assert(!fa_touch(&s,true,480,320));assert(!fa_back(&s));
 (void)fa_touch(&s,false,0,0);assert(!fa_touch(&s,true,1,100));
 assert(fa_connection(FA_I2C)!=NULL);assert(fa_connection((enum fa_instrument)999)!=NULL);
 assert(strcmp(fa_name((enum fa_instrument)-1),"Unavailable")==0);
 assert(fa_connection((enum fa_instrument)-1)==fa_connection(FA_PROTOCOL));
 puts("PASS: home/protocol/guide/placeholder/secondary/back/home/held-touch/boundaries");return 0;
}
