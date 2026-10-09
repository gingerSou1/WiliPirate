/* SPDX-License-Identifier: MIT; Copyright (c) 2026 gingerSou1 */
#include "ui.h"
#include "display/font5x7.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static unsigned char pixels[FA_HEIGHT][FA_WIDTH][3];
static unsigned clears;
static void pixel(int x,int y,uint16_t c){pixels[y][x][0]=(unsigned char)(((c>>11)&31)*255/31);pixels[y][x][1]=(unsigned char)(((c>>5)&63)*255/63);pixels[y][x][2]=(unsigned char)((c&31)*255/31);}
static void rect(int x,int y,int w,int h,uint16_t c){assert(x>=0&&y>=0&&w>0&&h>0&&x+w<=FA_WIDTH&&y+h<=FA_HEIGHT);for(int yy=y;yy<y+h;yy++)for(int xx=x;xx<x+w;xx++)pixel(xx,yy,c);}
static void clear(uint16_t c){++clears;rect(0,0,FA_WIDTH,FA_HEIGHT,c);}
static void text(int x,int y,int scale,uint16_t fg,uint16_t bg,const char *s){
 assert(x>=0&&y>=0&&scale>0&&x+(int)strlen(s)*6*scale<=FA_WIDTH&&y+8*scale<=FA_HEIGHT);
 for(;*s;s++,x+=6*scale){assert((unsigned char)*s>=FONT5X7_FIRST&&(unsigned char)*s<=FONT5X7_LAST);const uint8_t *g=font5x7[(unsigned char)*s-FONT5X7_FIRST];for(int yy=0;yy<8*scale;yy++)for(int xx=0;xx<6*scale;xx++){bool on=xx/scale<5&&yy/scale<7&&((g[xx/scale]>>(yy/scale))&1);pixel(x+xx,y+yy,on?fg:bg);}}
}
static const struct fa_draw draw={clear,rect,text};
static void output(const char *folder,const char *name){char path[1024];int n=snprintf(path,sizeof path,"%s/%s.ppm",folder,name);assert(n>0&&(size_t)n<sizeof path);FILE *f=fopen(path,"wb");assert(f);fprintf(f,"P6\n480 320\n255\n");assert(fwrite(pixels,sizeof pixels,1,f)==1);assert(fclose(f)==0);}
int main(int argc,char **argv){
 struct fa_state s;fa_init(&s);fa_render(&s,&draw,true);if(argc==2)output(argv[1],"home");
 s.page=FA_PROTOCOLS;fa_render(&s,&draw,true);if(argc==2)output(argv[1],"protocols");
 for(int i=0;i<FA_COUNT;i++)for(int p=FA_GUIDE;p<=FA_PREVIEW;p++){s.instrument=(enum fa_instrument)i;s.page=(enum fa_page)p;unsigned before=clears;fa_render(&s,&draw,true);assert(clears==before+1);if(argc==2){char name[64];snprintf(name,sizeof name,"%s-%d",p==FA_GUIDE?"guide":"preview",i);output(argv[1],name);}fa_render(&s,&draw,false);}
 for(int p=FA_TOOLS;p<=FA_SETTINGS;p++){s.page=(enum fa_page)p;fa_render(&s,&draw,true);if(argc==2){char name[32];snprintf(name,sizeof name,"secondary-%d",p);output(argv[1],name);}}
 for(unsigned i=0;i<3;i++){fa_can_demo(&s.can);}fa_can_capture(&s.can);s.review=2;
 for(int p=FA_GLITCH;p<=FA_CAN_REVIEW;p++){s.page=(enum fa_page)p;unsigned before=clears;fa_render(&s,&draw,true);assert(clears==before+1);fa_render(&s,&draw,false);if(argc==2){char name[32];snprintf(name,sizeof name,"v003-%d",p);output(argv[1],name);}}
 puts("PASS: all screens/guide facts/fallbacks render within 480x320 with full clears");return 0;
}
