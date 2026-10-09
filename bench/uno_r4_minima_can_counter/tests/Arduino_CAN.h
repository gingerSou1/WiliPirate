// Host-only test double; never part of the device build.
#pragma once
#include <array>
#include <vector>
#include <cstdint>
enum class CanBitRate { BR_500k };
struct CanStandardId { uint32_t id;explicit CanStandardId(uint32_t v):id(v){} };
struct CanMsg {
 uint32_t id;std::array<uint8_t,8> bytes{};
 CanMsg(CanStandardId i,unsigned n,const uint8_t *p):id(i.id){for(unsigned j=0;j<n;j++)bytes[j]=p[j];}
};
struct FakeCan {
 bool opened=false,beginOk=true,error=false;int result=1;unsigned beginCalls=0,endCalls=0;
 std::vector<CanMsg> writes;
 bool begin(CanBitRate){++beginCalls;opened=beginOk;return beginOk;}
 void end(){opened=false;++endCalls;}
 void clearError(){error=false;}
 bool isError(int &e){e=error?7:0;return error;}
 int write(const CanMsg &m){writes.push_back(m);return result;}
};
inline FakeCan CAN;
