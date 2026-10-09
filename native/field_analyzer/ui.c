/* SPDX-License-Identifier: MIT; Copyright (c) 2026 gingerSou1 */
#include "ui.h"
#include <string.h>
#include <stdio.h>
enum { NAVY=0x0843, SURFACE=0x1105, WHITE=0xffff, MUTED=0xbdf7, TEAL=0x36b6 };
static void label(const struct fa_draw *d,int x,int y,int scale,uint16_t bg,const char *text){d->text(x,y,scale,WHITE,bg,text);}
static void rounded(const struct fa_draw *d,struct fa_rect r,uint16_t color){
 const int radius=9;d->rect(r.x+radius,r.y,r.w-2*radius,r.h,color);
 d->rect(r.x,r.y+radius,r.w,r.h-2*radius,color);
 for(int y=0;y<radius;y++){int inset=radius;while(inset>0&&(radius-inset)*(radius-inset)+(radius-y)*(radius-y)<=radius*radius)--inset;++inset;
  d->rect(r.x+inset,r.y+y,r.w-2*inset,1,color);d->rect(r.x+inset,r.y+r.h-1-y,r.w-2*inset,1,color);}
}
static void button(const struct fa_draw *d,struct fa_rect r,const char *text){rounded(d,r,TEAL);d->text(r.x+(r.w-(int)strlen(text)*6)/2,r.y+12,1,NAVY,TEAL,text);}
void fa_render(const struct fa_state *s,const struct fa_draw *d,bool touch){
 d->clear(NAVY);
 label(d,16,12,3,NAVY,"WILIPIRATE");d->text(16,43,2,TEAL,NAVY,"Field Analyzer");
 if(s->page!=FA_HOME)button(d,(struct fa_rect){418,10,50,34},"HOME");
 if(s->page==FA_HOME||s->page==FA_PROTOCOLS){
  const char *sub[]={"Analog Waveforms","Digital Timing","CAN | UART | I2C | SPI","PWM | Waveforms"};
  const char *protocol_sub[]={"Bench monitoring: planned","Serial inspection: planned","Bus inspection: planned","Bus inspection: planned"};
  for(unsigned i=0;i<4;i++){struct fa_rect r=fa_tile(i);rounded(d,r,SURFACE);d->rect(r.x+12,r.y+12,4,44,TEAL);
   if(s->page==FA_HOME && i>=2){label(d,r.x+25,r.y+10,2,SURFACE,i==2?"Protocol":"Signal");label(d,r.x+25,r.y+28,2,SURFACE,i==2?"Analyzer":"Generator");}
   else label(d,r.x+25,r.y+18,2,SURFACE,fa_name((enum fa_instrument)(s->page==FA_HOME?i:FA_CAN+i)));
   d->text(r.x+25,r.y+51,1,MUTED,SURFACE,s->page==FA_HOME?sub[i]:protocol_sub[i]);}
 }else if(s->page==FA_GUIDE){
  const struct fa_guide *g=fa_connection(s->instrument);
  label(d,16,76,2,NAVY,"Connection Guide");
  d->text(16,99,1,TEAL,NAVY,fa_name(s->instrument));
  const char *lines[]={g->connector,g->signals,g->ground,g->limits,g->accessory,g->stock,g->desktop,g->precaution};
  for(int i=0;i<8;i++)d->text(16,117+i*15,1,i==7?TEAL:MUTED,NAVY,lines[i]);
  if(strcmp(g->precaution,"Pinout or electrical compatibility not yet verified.")!=0)
   d->text(16,237,1,TEAL,NAVY,"Pinout or electrical compatibility not yet verified.");
 }else if(s->page==FA_TOOLS){
  label(d,16,85,2,NAVY,"Tools");
  button(d,(struct fa_rect){16,120,448,50},"GLITCHING / FAULT INJECTION");
  label(d,16,185,1,NAVY,"Planning only. All fault-injection outputs disabled.");
 }else if(s->page==FA_GLITCH){
  label(d,16,76,2,NAVY,"Glitching / Fault Injection");
  const char *titles[]={"Voltage Glitch","Clock Glitch","Trigger & Synchronization","Experiment Log"};
  const char *details[]={"Power interruption experiments | Capability: unverified","Timing disturbance experiments | Capability: unverified","External trigger / timing options | Capability: unverified","Configurations / results | Storage: not implemented"};
  for(int i=0;i<4;i++){
   d->text(16,104+i*34,1,TEAL,NAVY,titles[i]);
   label(d,16,116+i*34,1,NAVY,details[i]);
  }
  label(d,16,239,1,NAVY,"Development: planned. No operational controls.");
 }else if(s->page==FA_GLITCH_GUIDE){
  label(d,16,80,2,NAVY,"Glitching Connection Guide");
  const char *lines[]={"Pinout / electrical limits require verification.","Suitable Orca modules require verification.","Supported glitching capabilities require verification.","Voltage / clock / trigger controls: unavailable.","Experiment log storage: not implemented.","No wiring or fault-injection procedure is qualified."};
  for(int i=0;i<6;i++)label(d,16,115+i*20,1,NAVY,lines[i]);
 }else if(s->page==FA_CAN_CONFIG || s->page==FA_CAN_LISTEN){
  label(d,16,80,2,NAVY,s->page==FA_CAN_CONFIG?"CAN Configure":"CAN Listen");
  label(d,16,115,1,NAVY,"LIVE DISABLED: pinout / passive behavior unqualified.");
  label(d,16,140,1,NAVY,"Channel 0 only documented on FW2. Channel 1 reserved.");
  label(d,16,165,1,NAVY,"Bitrate / FD data rate: not configured. No bus access.");
  label(d,16,190,1,NAVY,"SIMULATION ONLY | No ACK, TX or hardware commands.");
  label(d,16,215,1,NAVY,"RAM capture / review available for synthetic data.");
 }else if(s->page==FA_CAN_MONITOR){
  char line[76];label(d,16,76,2,NAVY,"CAN Monitor - SIMULATION");
  snprintf(line,sizeof line,"Received %u | Local overwritten %u | Rejected %u",s->can.received,s->can.overwritten,s->can.rejected);label(d,16,103,1,NAVY,line);
  label(d,16,118,1,NAVY,"Upstream loss / bus errors: unknown (no live transport)");
  unsigned start=s->can.count>4?s->can.count-4:0;
  for(unsigned i=start;i<s->can.count;i++){
   const struct fa_can_frame *f=&s->can.frames[i];
   snprintf(line,sizeof line,"%08X %s %s %2u bytes | t=%u us",(unsigned)f->id,f->extended?"EXT":"STD",f->fd?"FD":"CAN",f->length,(unsigned)f->timestamp_us);
   label(d,16,140+(int)(i-start)*16,1,NAVY,line);
  }
  button(d,(struct fa_rect){16,210,216,30},"ADD SIMULATED FRAME");
  button(d,(struct fa_rect){248,210,216,30},s->can.count?"CAPTURE RAM / REVIEW":"CAPTURE: NO FRAMES");
 }else if(s->page==FA_CAN_REVIEW){
  char line[76];label(d,16,76,2,NAVY,"CAN Review - SIMULATION");
  if(s->review<s->can.captured){
   const struct fa_can_frame *f=&s->can.capture[s->review];
   snprintf(line,sizeof line,"Frame %u/%u ID %08X %s %s | %u bytes",s->review+1,s->can.captured,(unsigned)f->id,f->extended?"EXT":"STD",f->fd?"FD":"CAN",f->length);label(d,16,105,1,NAVY,line);
   snprintf(line,sizeof line,"Synthetic drain-time %u us | RAM only; no export",(unsigned)f->timestamp_us);label(d,16,125,1,NAVY,line);
   for(unsigned row=0;row<4;row++){
    unsigned used=0;
    for(unsigned j=row*16;j<f->length&&j<(row+1)*16;j++)used+=(unsigned)snprintf(line+used,sizeof line-used,"%02X ",f->data[j]);
    line[used]=0;label(d,16,150+(int)row*20,1,NAVY,line);
   }
  }else label(d,16,115,1,NAVY,"No synthetic capture available.");
 }else{
  const char *title=s->page==FA_PREVIEW?fa_name(s->instrument):s->page==FA_TOOLS?"Tools":s->page==FA_CAPTURES?"Captures":"Settings";
  label(d,16,85,2,NAVY,title);label(d,16,126,2,NAVY,"Not implemented");
  label(d,16,163,1,NAVY,"Navigation preview only. No instrument operations.");
  label(d,16,185,1,NAVY,s->page==FA_CAPTURES?"Synthetic CAN RAM snapshot only; no storage / export.":s->page==FA_SETTINGS?"No voltage, GPIO, power or configuration controls.":"No acquisition, transmission or signal generation.");
 }
 if(s->page==FA_HOME){const char *secondary[]={"TOOLS","CAPTURES","SETTINGS"};for(unsigned i=0;i<3;i++)button(d,(struct fa_rect){16+(int)i*152,250,144,34},secondary[i]);}
 else{button(d,(struct fa_rect){16,250,140,34},"BACK");
  if(s->page==FA_GUIDE)button(d,(struct fa_rect){170,250,294,34},s->instrument==FA_CAN?"CAN CONFIGURE (live disabled)":"VIEW PLACEHOLDER (no connection)");
  if(s->page==FA_PREVIEW||s->page==FA_GLITCH)button(d,(struct fa_rect){170,250,294,34},"REVIEW CONNECTION GUIDE");
  if(s->page==FA_CAN_CONFIG)button(d,(struct fa_rect){170,250,294,34},"LISTEN (simulation only)");
  if(s->page==FA_CAN_LISTEN)button(d,(struct fa_rect){170,250,294,34},"OPEN SIMULATED MONITOR");
  if(s->page==FA_CAN_REVIEW)button(d,(struct fa_rect){170,250,294,34},"NEXT CAPTURED FRAME");
  if(s->page==FA_CAPTURES&&s->can.captured)button(d,(struct fa_rect){170,250,294,34},"REVIEW SIMULATED CAN CAPTURE");
 }
 d->text(16,301,1,MUTED,NAVY,touch?"UI only | No instruments enabled | HOME 5s exits device":"Touch unavailable | CANCEL: Back | HOME 5s: recovery");
}
