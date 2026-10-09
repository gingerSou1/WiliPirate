/* SPDX-License-Identifier: MIT; Copyright (c) 2026 gingerSou1 */
#include "can_model.h"
#include <string.h>
#include <stdlib.h>
#include <errno.h>
void fa_can_init(struct fa_can_session *s){memset(s,0,sizeof *s);}
bool fa_can_live_available(void){return false;}
bool fa_can_accept(struct fa_can_session *s,const struct fa_can_frame *f){
 bool length_ok=f && (f->length<=8 || (f->fd&&(f->length==12||f->length==16||f->length==20||f->length==24||f->length==32||f->length==48||f->length==64)));
 if(!f || f->id>(f->extended?0x1fffffffu:0x7ffu) || !length_ok){
  ++s->rejected;return false;
 }
 if(s->count==FA_CAN_CAPACITY){
  memmove(s->frames,s->frames+1,(FA_CAN_CAPACITY-1)*sizeof s->frames[0]);++s->overwritten;
 }else ++s->count;
 s->frames[s->count-1]=*f;
 /* Unused payload bytes are not captured as data. */
 memset(s->frames[s->count-1].data+f->length,0,64-f->length);
 ++s->received;return true;
}
static bool number(const char **p,unsigned base,uint32_t *v){
 while(**p==' '||**p=='\t')++*p;
 if(!((**p>='0'&&**p<='9')||(base==16&&((**p>='a'&&**p<='f')||(**p>='A'&&**p<='F')))))return false;
 char *end;errno=0;unsigned long long n=strtoull(*p,&end,(int)base);
 if(end==*p || errno || n>0xffffffffu || (*end&&*end!=' '&&*end!='\t'))return false;
 *p=end;*v=(uint32_t)n;return true;
}
bool fa_can_decode_queue(struct fa_can_session *s,const char *body){
 /* Bound the input before any libc numeric parser sees it. */
 unsigned size=0;if(!body){++s->rejected;return false;}
 while(size<512&&body[size])++size;
 if(size==512){++s->rejected;return false;}
 uint32_t v[8];const char *p=body;
 for(unsigned i=0;i<8;i++)if(!number(&p,i==3?16:10,&v[i])){++s->rejected;return false;}
 if(v[0]>1||v[1]>32||v[4]>1||v[5]>1||v[7]>64){++s->rejected;return false;}
 struct fa_can_frame f={.id=v[3],.extended=v[4]!=0,.fd=v[5]!=0,.timestamp_us=v[6],.length=(uint8_t)v[7]};
 for(unsigned i=0;i<f.length;i++){uint32_t b;if(!number(&p,16,&b)||b>255){++s->rejected;return false;}f.data[i]=(uint8_t)b;}
 while(*p==' '||*p=='\t')++p;
 if(*p || (!v[0]&&(v[1]||v[3]||v[4]||v[5]||v[6]||v[7]))){++s->rejected;return false;}
 if(v[0]&&!fa_can_accept(s,&f))return false;
 s->upstream_dropped=v[2];return true;
}
void fa_can_demo(struct fa_can_session *s){
 unsigned n=s->next_demo++;
 struct fa_can_frame f={.id=(n%2)?0x1abcdeu:0x123u,.timestamp_us=n*10000u,
  .length=(n%3==2)?64:8,.extended=(n%2)!=0,.fd=(n%3==2)};
 for(unsigned i=0;i<f.length;i++)f.data[i]=(uint8_t)(n+i);
 (void)fa_can_accept(s,&f);
}
void fa_can_capture(struct fa_can_session *s){
 memcpy(s->capture,s->frames,s->count*sizeof s->frames[0]);s->captured=s->count;
}
