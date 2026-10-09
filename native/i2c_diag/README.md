# M3 Phase 3: one-Poll DISPLAY diagnostic

WiliPirate — Created by gingerSou1. Original code: [MIT](../../LICENSE).
**Offline diagnostic candidate; not physically validated or approved to deploy.**
**Stopped checkpoint: picotool is unfinished and no diagnostic UF2 exists.**
See [the actual build/test status](VALIDATION.md). Build commands below are
reference instructions, not permission to resume the stopped work.
This separate app does not change the six-tile M2 source or its validated UF2.
It uses the stock command `i\i\p` through the pinned official OneWili/FwGUI
transport, following the official canblast full-text observation pattern.
No CAN code, custom I2C driver or address-list decoder is included.

## Behavior and bounds

- Initially no OneWili link is opened and no Poll is sent. Press/release GREEN
  to arm, then press GREEN again to initiate the sole attempt. An inherited
  held button cannot immediately trigger a scan.
- The attempt is consumed before opening. Any opening, timer, pending-input or
  send failure ends that run without retry. Another Poll requires HOME recovery
  and a separately approved fresh launch.
- Link opening uses the official recovery adapter. A 100 ms quiet preflight
  rejects pre-existing text or transport loss, rather than attributing it to
  Poll. Pending bytes are retained and labelled as such; they are not a reply.
- Exactly one `ow_raw_send` submits Poll. A recovery-service timer covers the
  official blocking write; read waits use the official sliced recovery adapter.
- After submission returns, receive runs for **5000 ms**, using reads of at
  most **10 ms** and the remaining receive budget. No drawing or RTT dumping
  takes place during capture. The final short read's returned bytes are retained;
  no subsequent read starts after the budget expires.
- Capacity is **32768 raw bytes** and **1024 chunk timing records**. Filling the
  byte buffer exactly is conservatively marked truncated because later bytes
  cannot be excluded. Exhaustion, I/O errors and loss counters are explicit
  inconclusive results, never a successful empty scan.
- Only the documented response envelope is recognized. The observed path must
  be `i\i\p`; timestamp and sequence text are preserved without inventing their
  request-correlation semantics. Multiline bodies, text events and other bytes
  remain in the raw capture. No detected-address format is assumed or decoded.
- One matching complete frame with flag 0 is reported as firmware failure.
  Missing, malformed, unrelated or multiple responses are unqualified evidence.
  A complete frame with flag 1 means only that its envelope was observed,
  not that address results exist or that stale-response correlation is solved.

The receive deadline is **not** a bound on OneWili opening, UART transmission
or the stock MAIN bus operation. UART send has no supported write timeout;
the additional timer follows the official opener's recovery-service pattern.
HOME handling during a stalled write still needs physical qualification. Do
not claim guaranteed recovery when keyboard status stops arriving.

## Readout

Grey PREV / Yellow NEXT page through exact raw hexadecimal bytes or metadata.
Blue INFO/RAW switches views. Metadata includes the complete observed identity,
opening/send/receive uptime times, first/last byte timing, every recorded chunk
offset/length/read-completion time, and before/after transport counters.
Counters cover IRQ/hardware overruns, dropped frames, checksum/length errors,
text frames and buffer high-water marks. Millisecond times are read-completion
observations, not per-byte wire timestamps.

All raw bytes are available on the LCD in 80-byte pages, including non-text
bytes. Preserve those pages and the INFO pages before exit; RAM is lost on
HOME recovery. This candidate has no SD logging, persistent configuration,
automatic export or hidden debugger operation. Large captures make manual
readout cumbersome; approve any later machine-readable export separately.
PAGE held five seconds shows name/version/repository About; HOME held five
seconds requests official DISPLAY watchdog recovery to stock. Recovery is
serviced while idle, capturing and in terminal/error states. Avoid About
during capture so rendering does not add avoidable receive pressure.

## Startup and safety

Standard `board_init`, recovery/display initialization and backlight setup are
real hardware operations. They change clocks/core voltage, PSRAM timing,
internal buses, LED/backlight/radio-select state and expander outputs/directions.
The expander selects external VREF for VIO; pre-launch electrical settings are
not preserved. Recovery metadata requests only the DISPLAY zone and its task
maintains that normal lifecycle. There is **no** extra rail request/report,
`release_unused`, VREF setter, target GPIO call or I2C configuration call.
Inherited rails are not deliberately released, and may remain powered.

On the manual attempt, the official adapter initializes the internal DISPLAY
UART0 link at 8 Mbaud with GPIO0-3 hardware flow control. Those internal pins
are unrelated to external target GPIO configuration. Opening also arms the
official SD client, including its initialization cleanup; the app itself makes
no SD file calls. These unavoidable supported transport effects are distinct
from a target bus/VIO setting. Stock MAIN Poll's own hardware side effects and
power gating remain unverified; absence of app configuration does not establish
electrical isolation or prove MAIN leaves all bus settings unchanged.

No targets, probes, header/VREF wiring or accessories should be connected for
the first proposed empty-bus run. A known-peripheral test requires a separately
reviewed wiring, voltage, pull-up and read-side-effect plan. The app does not
enable a rail or adjust VIO to get around `EPOWERZONE`. On refusal, stop and
record the raw error. Exact previous power/VREF restoration on HOME is not
guaranteed. See [response-path investigation](../../docs/M3_I2C_STOCK_API.md).

## Offline build

Use unmodified official sources at these exact pins:

| Dependency | Revision |
| --- | --- |
| WiliBSP | `be4bdd63d31a80f95410e583710cf4e43a7be7fa` |
| DISPLAY OneWili | `b0eeccda21b0594c8062cd17e9755f0c26b0cf6b` |
| Pico SDK 2.3.0 | `98a542c1a62fb549ffb5d66a3e5892b06276b670` |
| SDK TinyUSB | `86ad6e56c1700e85f1c5678607a762cfe3aa2f47` |
| SDK picotool 2.3.0 | `6f6458d792b93685a11423b244a585eaa99eafcf` |

Requirements: CMake 3.20+, Ninja, Python, a supported Arm `arm-none-eabi`
toolchain and host C/C++ compiler. Prepare the SDK-matched official pioasm and
picotool utilities first; build picotool with `PICOTOOL_NO_LIBUSB=ON` for purely
offline conversion. Use its installed CMake package and pioasm package. Source
preparation may download pins; the actual configure/build must not fetch tools
or packages. Do not modify official sources to make a build pass.

From the repository root, with prepared dependency/tool paths:

```sh
cmake -S native/i2c_diag -B build/m3-diag -G Ninja \
  -DWILIBSP_PATH=/path/to/pinned/wilibsp \
  -DONEWILI_PATH=/path/to/pinned/onewili \
  -DPICO_SDK_PATH=/path/to/pico-sdk-2.3.0 \
  -DPICO_TOOLCHAIN_PATH=/path/to/arm-toolchain \
  -Dpioasm_DIR=/path/to/pioasm/lib/cmake/pioasm \
  -Dpicotool_DIR=/path/to/picotool/cmake-package \
  -DFETCHCONTENT_FULLY_DISCONNECTED=ON \
  -DCMAKE_BUILD_TYPE=MinSizeRel
cmake --build build/m3-diag --target WiliPirateI2CDiag --parallel 2
python -B /path/to/pinned/wilibsp/tools/check_app_uf2.py \
  build/m3-diag/WiliPirateI2CDiag.uf2
```

On Windows, quote complete `-D...=...` arguments, especially dotted versions.
The standalone CMake project does not add or build `native/display/`. It uses
`fw2_display_app`, version 001, `no_flash`, only DISPLAY metadata and no AgentIO,
USB stdio or UART stdio. There are no installation, launch or flash targets.
Keep outputs under ignored `build/`; do not replace or rename the M2 UF2.

Host core tests (native C compiler required):

```sh
cc -std=c11 -Wall -Wextra -Werror native/i2c_diag/capture.c \
  native/i2c_diag/tests/capture_test.c -o build/capture_test
build/capture_test
python -B -m unittest discover -s tests -v
git diff --check
```

Synthetic frame fixtures test only envelope/capture behavior, not the unknown
stock Poll result schema. [Offline validation](VALIDATION.md) records actual
results, compiler versions and artifact measurements.

## Proposed physical procedure (not authorized or executed)

1. Review this source, exact diagnostic artifact checksum/SRAM validation,
   installed MAIN/DISPLAY/loader versions and all startup effects. Obtain
   explicit installation, launch and observation approval, including the
   evidence readout method and recovery contingency. Use only supported SD
   `/apps/` installation; no BOOTSEL, flashing or stock-firmware replacement.
2. Empty-bus trial: disconnect all external wiring/accessories, launch the
   separately named diagnostic, arm with GREEN press/release and initiate once.
   Record every raw and INFO page and error flag. Verify HOME recovery and
   stock operation afterward. Refusal, incomplete data or losses are not
   evidence of zero devices. Do not retry or change rails/configuration.
3. Review that capture before a known-peripheral trial. Verify the connector
   pinout, master-mode settings, VIO compatibility, pull-ups, frequency, common
   ground and the peripheral's safe read behavior independently. Standard
   startup's external-VREF selection may invalidate prior settings: do not
   assume the app supplies VIO or target power. Any required external supply/
   VREF arrangement needs approval before wiring. The spare PN532 is optional
   until its specific I2C mode/address/electrical behavior is qualified; ORCA
   is not required.
4. After separate approval, perform a fresh launch and exactly one Poll with
   that known target. Compare complete raw captures against the empty-bus case
   and independently established 7-bit address. Preserve any observed body
   unchanged before deciding whether it contains addresses, count, status or
   other output. A successful envelope alone does not meet the result gate.
5. Stop for review. Do not short lines, induce faults, change rails, retry
   timeouts or expand testing to another interface. Same-path late replies,
   maximum sizes, zone-off enforcement and recovery under faults require
   separate qualification before a production Scan feature.

The project's MIT grant does not resolve third-party binary redistribution
terms. Preserve all applicable notices and the unresolved questions in
[THIRD_PARTY.md](../../docs/THIRD_PARTY.md); this build is not a public release.
