// SPDX-License-Identifier: MIT; Copyright (c) 2026 gingerSou1
#include <cassert>
#include <cstdio>
#include "../src/can_counter.ino"
static void reset(){CAN=FakeCan{};Serial=FakeSerial{};fakeMillis=0;running=false;counter=lastSend=0;setup();}
static void start(){Serial.input="s";loop();}
int main(){
 reset();loop();assert(!running&&CAN.beginCalls==0&&CAN.writes.empty());
 start();assert(running&&CAN.beginCalls==1);fakeMillis=99;loop();assert(CAN.writes.empty());
 fakeMillis=100;loop();assert(CAN.writes.size()==1&&CAN.writes[0].id==0x321);
 assert(CAN.writes[0].bytes[0]==0&&CAN.writes[0].bytes[4]==100);
 fakeMillis=1000;loop();assert(CAN.writes.size()==2); // no catch-up burst
 assert(CAN.writes[1].bytes[0]==1&&CAN.writes[1].bytes[4]==0xe8&&CAN.writes[1].bytes[5]==3);
 Serial.input="x";loop();assert(!running&&!CAN.opened);
 reset();CAN.beginOk=false;start();assert(!running&&CAN.writes.empty());
 reset();start();CAN.result=-1;fakeMillis=100;loop();assert(!running&&CAN.endCalls==1);
 fakeMillis=1000;loop();assert(CAN.writes.size()==1); // no retry
 reset();start();CAN.error=true;loop();assert(!running&&CAN.writes.empty());
 reset();fakeMillis=0xfffffff0u;start();fakeMillis=0x54;loop();assert(CAN.writes.size()==1);
 reset();start();for(unsigned i=1;i<=100;i++){fakeMillis=i*100;loop();}
 assert(!running&&CAN.writes.size()==100&&CAN.writes.back().bytes[0]==99);
 fakeMillis=20000;loop();assert(CAN.writes.size()==100);
 puts("PASS: manual arming, 100ms schedule, wrap, payload/ID, bounded run, no catch-up, stop/init/write/asynchronous errors");
}
