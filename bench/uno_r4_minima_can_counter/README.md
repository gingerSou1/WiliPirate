# Minima CAN counter bench source

Offline-prepared and compile-tested Arduino source. **No deployment/operation
approval.** Read [electrical requirements and schematic](../../docs/CAN_MINIMA_WAVESHARE_MILESTONE.md)
before use. The exact Waveshare SN65HVD230 requires TX down-shifting and RX
up-shifting with this 5 V Minima; direct logic connections are not qualified.
FW2 physical connection remains blocked.

Build with existing PlatformIO: `platformio run --project-dir bench/uno_r4_minima_can_counter`.
It uses only bundled Arduino_CAN. Build/cache files are under ignored `build/`.
No upload, firmware change or physical serial session is part of verification.

The sketch is idle until manual `s`, submits 0x321 counter/uptime frames at
500 kbps every nominal 100 ms, stops after 100 submissions or an error, and
supports manual `x`. It requires another ACK-capable node for reliable traffic;
successful queuing does not establish wire transmission or ACK.

Host-only test doubles in `tests/` execute the production sketch without board
drivers. Example using an existing host C++17 compiler:

```text
c++ -std=c++17 -Wall -Wextra -Werror -Ibench/uno_r4_minima_can_counter/tests bench/uno_r4_minima_can_counter/tests/sketch_test.cpp -o build/can-bench-sketch-test
build/can-bench-sketch-test
```
