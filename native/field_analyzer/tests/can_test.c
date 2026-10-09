/* SPDX-License-Identifier: MIT; Copyright (c) 2026 gingerSou1 */
#include "can_model.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
int main(void){
 struct fa_can_session s;fa_can_init(&s);assert(!fa_can_live_available());
 struct fa_can_frame f={.id=0x7ff,.length=8};
 assert(fa_can_accept(&s,&f));f.id=0x800;assert(!fa_can_accept(&s,&f));
 f.extended=true;f.id=0x1fffffff;assert(fa_can_accept(&s,&f));
 f.id=0x20000000;assert(!fa_can_accept(&s,&f));
 f.id=1;f.length=9;assert(!fa_can_accept(&s,&f));
 f.fd=true;f.length=64;f.timestamp_us=0xffffffff;
 memset(f.data,0xa5,64);assert(fa_can_accept(&s,&f));
 assert(s.frames[s.count-1].data[63]==0xa5);
 f.length=65;assert(!fa_can_accept(&s,&f));assert(!fa_can_accept(&s,0));
 fa_can_init(&s);for(unsigned i=0;i<20;i++)fa_can_demo(&s);
 assert(s.count==8&&s.received==20&&s.overwritten==12&&s.rejected==0);
 fa_can_capture(&s);assert(s.captured==8);
 struct fa_can_frame saved[8];memcpy(saved,s.capture,sizeof saved);
 fa_can_demo(&s);assert(!memcmp(saved,s.capture,sizeof saved));
 fa_can_init(&s);f=(struct fa_can_frame){.id=0,.length=0};memset(f.data,0xff,64);
 assert(fa_can_accept(&s,&f));assert(s.frames[0].data[0]==0);
 assert(fa_can_decode_queue(&s,"1 3 7 123 0 0 4294967295 4 DE AD BE EF"));
 assert(s.frames[1].id==0x123&&s.frames[1].data[3]==0xef&&s.frames[1].timestamp_us==0xffffffff&&s.upstream_dropped==7);
 assert(fa_can_decode_queue(&s,"0 0 17 0 0 0 0 0"));assert(s.count==2&&s.upstream_dropped==17);
 assert(!fa_can_decode_queue(&s,"1 0 0 123 0 0 0 2 FF"));
 assert(!fa_can_decode_queue(&s,"1 0 0 123 0 0 0 1 100"));
 assert(!fa_can_decode_queue(&s,"1 0 0 800 0 0 0 0"));
 assert(!fa_can_decode_queue(&s,"1 0 0 1 0 0 -1 0"));
 assert(!fa_can_decode_queue(&s,"0 0 0 1 0 0 0 0"));
 assert(!fa_can_decode_queue(&s,"0 0 0 0 0 0 0 0 extra"));
 assert(!fa_can_decode_queue(&s,0));
 char oversized[513];memset(oversized,'1',512);oversized[512]=0;assert(!fa_can_decode_queue(&s,oversized));
 f=(struct fa_can_frame){.fd=true,.length=9};assert(!fa_can_accept(&s,&f));
 const unsigned lengths[]={0,1,8,12,16,20,24,32,48,64};
 for(unsigned i=0;i<sizeof lengths/sizeof lengths[0];i++){f.length=(uint8_t)lengths[i];assert(fa_can_accept(&s,&f));}
 assert(!fa_can_decode_queue(&s,"\n0 0 0 0 0 0 0 0"));
 puts("PASS: CAN IDs, classic/FD lengths, timestamps, bounded ring/loss, frozen RAM capture, fail-closed live gate");return 0;
}
