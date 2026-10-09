// Host-only test double; never part of the device build.
#pragma once
#include <cstdint>
#include <string>
#define PIN_CAN0_TX 4
#define PIN_CAN0_RX 5
inline uint32_t fakeMillis;
inline uint32_t millis() { return fakeMillis; }
struct FakeSerial {
 std::string input;
 void begin(unsigned) {}
 int available() { return static_cast<int>(input.size()); }
 int read() { int c=input.front();input.erase(0,1);return c; }
 template<class T> void print(const T&) {}
 template<class T> void println(const T&) {}
};
inline FakeSerial Serial;
