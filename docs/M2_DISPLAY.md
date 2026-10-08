# M2: native DISPLAY UI prototype

**M2 — Native DISPLAY UI: COMPLETE / PHYSICALLY VALIDATED.**
The user reports all physical checks passed on FX0141. See
[physical results and exact preserved baseline](M2_DEVICE_VALIDATION.md).
The offline checks below are automated/source evidence; the physical results
are user observations. M3 is planned, not started.

Date: 2026-10-07. Baseline `5141f45`, branch `feature/wilipirate-panel`.
The attached M2 request explicitly permits standard WiliBSP initialization.
The earlier zero-transient-VREF launch gate is superseded for this UI milestone;
its research remains the description of real startup effects.

## Implementation

`native/display/main.c` follows `board_init` -> `fw2_app_recovery_init` ->
`st7796_init` -> `ft6336_init` -> full render -> backlight on -> standard
`picpwr_release_unused`. The loop services recovery, physical CANCEL Back,
touch edges and recovery-aware 10 ms sleep. The BSP About renderer uses an
application redraw callback rather than an app framebuffer snapshot.

The renderer uses the official ST7796 primitives and printable-ASCII 5x7 font.
It follows the direct-render pattern of existing BSP apps, with no added LVGL
dependency or new UI framework. A complete 480x320 clear precedes every page.
Two columns of three 216x50 touch tiles open GPIO, UART, I2C, SPI, CAN and
LOGIC placeholders. Every placeholder says `Not implemented - M2 UI prototype`.
Back is a 448x38 touch target; a held contact fires once so it cannot activate
a newly displayed page. Gaps and out-of-display samples do nothing.

The main Exit/Help button opens explicit physical HOME instructions; it does
not directly reset the CPU. HOME held five seconds triggers the official BSP
watchdog recovery, returning through the loader to stock DISPLAY. PAGE held
five seconds shows the version/source About screen. No electrical SAFE/HiZ
status appears. If touch initialization fails, the footer reports unavailable
touch while HOME recovery remains active. Normal physical display/touch/HOME
behavior passed according to the user on FX0141; the missing-touch failure
path was not separately reported as tested. Host previews are not device screenshots.

M2 does not link/open OneWili, initialize any interface backend, configure
target power, explicitly select VIO, initialize/transmit radio, poll I2C,
transfer SPI, transmit/receive UART/CAN or capture logic. Unused drivers in the
BSP static archive are build dependencies, not application operations.

## Dependency and official build contract

The new root `wilibsp/` submodule is pinned at
`be4bdd63d31a80f95410e583710cf4e43a7be7fa`. It is unmodified. Root CMake follows
the official external-project contract: select board `freewili2` before SDK
import, add only `wilibsp/bsp`, and call `fw2_display_app(WiliPirate ...)`.
The app declares name WiliPirate, version 001, source URL and only the DISPLAY
power zone. `FW2_AGENTIO` defaults OFF; no remote-control harness is initialized.

The helper selects `no_flash`, produces `WiliPirate.uf2` and applies the official
post-build UF2/metadata checks. The intended installation surface is MAIN
SD `/apps/WiliPirate.uf2`, not the CM0 Linux Apps folder and not a flash image.
There is no installation/flashing target in this project.

Build prerequisites: official Pico SDK **2.3.0**, its normal initialized
dependencies, Arm GNU Toolchain **14.2.Rel1** arm-none-eabi, CMake, Ninja, Python
and a host C/C++ compiler for the SDK pioasm/picotool tools. The SDK may fetch
its matching host picotool source; that build uses `PICOTOOL_NO_LIBUSB` and is
an offline conversion step, not device discovery. Root CMake rejects a different
SDK version. Never choose `copy_to_ram` or replace the board definition.

From a fresh recursive checkout on Linux/WSL:

```sh
git submodule update --init --recursive
cmake -S . -B build/display -G Ninja \
  -DPICO_SDK_PATH=/path/to/pico-sdk/2.3.0 \
  -DPICO_TOOLCHAIN_PATH=/path/to/arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi \
  -DCMAKE_BUILD_TYPE=MinSizeRel
cmake --build build/display --target WiliPirate --parallel 4
python3 -B wilibsp/tools/check_app_uf2.py \
  build/display/native/display/WiliPirate.uf2
python3 -B wilibsp/tools/check_app_repo.py .
```

No `flash`, `ramrun`, `install-app`, discovery, screenshot or probe command is
part of offline verification. Do not execute the Cortex-M33 ELF on a host.

## Startup side effects accepted by M2

Standard board initialization sets core voltage/250 MHz clocks, retimes PSRAM,
resets/clears LED PIO state, configures internal SPI/I2C, radio chip select,
backlight and the expander. `ioexp_init` overwrites output/direction registers,
selects external Trig_IN/VREF for header VIO, resets mic/IR/USB controls and
selects the normal CC1101 antenna route. These are not preserved live settings.
The DISPLAY power declaration and standard unused-rail release can request,
maintain and drop app-owned rails inherited from stock. The app makes no extra
power/VIO/radio requests beyond this standard lifecycle.

Stored stock MAIN/DISPLAY firmware is preserved by SRAM loading. During app
execution stock DISPLAY code is temporarily replaced by this app's code; HOME
reboots DISPLAY rather than returning to a saved context. Exact pre-launch
electrical settings are not guaranteed on exit. First physical validation must
use disconnected external targets, and requires separate explicit approval.

## Offline checks

```sh
cmake -S native/display/tests -B build/display-host -G Ninja
cmake --build build/display-host
ctest --test-dir build/display-host --output-on-failure
mkdir -p build/display-preview
build/display-host/render_test build/display-preview
python -B -m unittest discover -s tests -v
git diff --check
```

The host navigation executable exercises every tile/Back transition, exit help,
blank space, edge boundaries and held-touch suppression. The render executable
calls the production UI renderer using the unmodified BSP font and a software
draw sink. It verifies every page/missing-touch variant fits the LCD and starts
with a complete clear, and optionally writes eight PPM previews. This is software
layout/navigation validation, not an emulator of the board or electrical behavior.
New Python source checks constrain the app's calls to the accepted BSP lifecycle
and reject interface/OneWili calls. These are regression checks, not a sandbox.

Windows full suite: 52 tests discovered, 51 passed, one legacy CM0 constexpr
compiler check skipped. The unchanged four CM0 preparation checks also passed
under WSL with the newly available host compiler, including that constexpr
check. All original 50 project tests are therefore covered across these runs.
Host CTest: 2/2 passed. BSP external repository setup: passed. Diff check: passed.
Main menu, I2C placeholder and Exit/Help host previews were visually inspected;
all eight pages passed renderer bounds checks. The CM0 M1E wrong-response
regressions remain untouched; no CM0 runtime was run or package produced.

Host build environment: Windows/WSL Ubuntu with host-only build-essential,
CMake and Ninja installed for this task. The existing isolated Trixie root
and CM0 binary/evidence were not changed. Arm's official 14.2.Rel1 archive was
downloaded to ignored `build/toolchains/` and matched its published SHA-256:
`62a63b981fe391a9cbad7ef51b17e49aeaa3e7b0d029b36ca1e9c3b2a9b78823`.
The compiler reports 14.2.1 (Arm release 14.2.Rel1), distinct from host GCC 15.2.

The initial app compile caught a missing diagnostics include; it was corrected
in WiliPirate. An unchanged bundled Pico-PIO-USB header emits an inline/noinline
attribute warning. `-Werror` is scoped to WiliPirate's own C files, preserving
the dependencies' normal warning policy. No upstream code was patched. A failed
early compiler probe left empty C optimization flags in the local cache; the
SDK's normal `-g -Os -DNDEBUG` MinSizeRel C flags were restored for the final
artifact. No alternative compiler/loader or firmware image was substituted.

Final optimized target build: **PASS**. Artifact:
`build/display/native/display/WiliPirate.uf2`, **53,248 bytes**, 104 UF2 blocks,
26,536 payload bytes. Every payload byte is in SRAM, from `0x20000000` through
`0x200067a7` (exclusive end `0x200067a8`). Official validator: SRAM target,
WiliPirate v001, expected description, valid app metadata. No QSPI-flash or
PSRAM payload, and no embedded second-CPU/platform image.

SHA-256:
`6bc0a08050c1e18659882bc486a1f03b6e16529916b60112240f548aeb1e6672`.

ELF: Cortex-M33 ARM executable, entry `0x20000179`; file-bearing LOAD segment
at `0x20000000`. The two scratch-RAM stack segments have zero file bytes.
Sections: text 23,156 bytes, rodata 2,292, data 1,056, BSS 6,472; heap and each
core stack reserve 2,048. Linked-symbol inspection found no OneWili `ow_*`,
CC1101/IR/PDM/USB-store initialization or stdio-UART initialization symbols.
Normal internal board/display/touch/keyboard/power code is present, as intended.
No device code was executed during those offline checks. The subsequent
authorized SD installation and user-run physical checks are recorded separately.
Raw build log, symbols and artifact measurements
are retained in ignored `build/display-build.log`, `build/display-symbols.txt`
and `build/display-validation.json`.

Modified/created source scope: root CMake and `.gitmodules`, pinned `wilibsp/`,
`native/display/` source/host tests, `tests/test_display_preparation.py`, project
instructions/README, M2 guide and architecture/research/attribution pointers.
No change to `apps/`, `native/cm0/`, `ui/`, old tests/tools, pinned CM0 dependency
or immutable M1A evidence. `archive/cm0-prototype` remains at
`bc738b88976917dffb693265fcee162716f58ec0`.

## M3 preparation only

I2C is the first planned functional tool. Establish the exact stock MAIN Poll
handler, response format, empty/error behavior, firmware versions, power/VIO
prerequisites and supported result-delivery path before implementing SCAN.
`ow_io_i2c_i2c_poll` is status-only. Public generic `ow_raw_send` /
`ow_raw_next_response` retain response bodies but do not establish addresses or
command correlation. Those findings and M1E validation limitations remain
relevant. Reuse the existing stock scan operation; do not write another scan
loop, patch upstream or add undocumented fallback paths. Other tools remain
placeholders until separately authorized milestones.

## Original first physical validation plan (now completed by user report)

1. Obtain explicit approval and record installed firmware/loader compatibility.
   Disconnect all external targets, GPIO/header wiring, VREF sources and protocol
   accessories before loading. Normal BSP side effects are expected.
2. Independently recheck the exact UF2 checksum and official SRAM-only validation.
   Copy only that RAM application to stock SD `/apps/` using the supported app
   file-transfer mechanism. Do not use flash/debug-probe workflows or install
   any vendor firmware/system components.
3. Launch from stock Apps. Verify the complete menu, each of six inert tiles,
   touch Back and physical CANCEL, held-contact behavior, Exit/Help and missing
   tool functionality. No target power enable or protocol experiment.
4. Exercise PAGE About and release/redraw. Hold physical HOME five seconds and
   verify normal stock DISPLAY operation resumes. Record failures, firmware
   versions and observed settings; do not claim prior VIO/power restoration.
5. Stop and review evidence before any target connection or M3 scan approval.

The earlier offline-only stop was superseded by explicit SD-installation
approval and the user's successful physical report. M2 is now closed. The
closeout authorizes documentation/artifact preservation and one user-repository
branch push, not new builds, M3 development, device operations or a merge.
