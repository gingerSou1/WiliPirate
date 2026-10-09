# WiliPirate replacement: startup electrical-effects audit

Recorded 2026-10-09. Read-only source inspection; no physical observation,
device access, native build or approved electrical exception.

## Decision: outcome C

**Update: conditional offline-build approval received 2026-10-09.** The user
approved generating/validating one replacement candidate with the documented
M2-style startup. This does not authorize executing startup, device access,
SD changes, installation or launch. The physical gate remains CLOSED. See
[the build record](FIELD_ANALYZER_VALIDATION.md#conditional-offline-build-result).
The earlier exception request below is retained as the investigation record;
its electrical unknowns are not resolved by compilation.

No supported no-target-change startup was established. `board_init()` reaches
`ioexp_init()`, which selects external VREF and writes full expander output and
direction defaults. Inherited/PSRAM variants still perform those writes. M2
uses this same path; successful M2 launch/HOME is not electrical certification.
Keep the build gate closed until a fresh explicit standard-startup exception
is approved. No override, trimmed startup or upstream patch was implemented.

Identity is corrected independently: target **WiliPirate**, metadata WiliPirate
v002, future output `build/field-device/WiliPirate.uf2`, replacement destination
`/apps/WiliPirate.uf2`. There is no second Field Analyzer app or launcher.

## Evidence scope

BSP `be4bdd63d31a80f95410e583710cf4e43a7be7fa`; SDK 2.3.0
`98a542c1a62fb549ffb5d66a3e5892b06276b670`. Sources are the unchanged cached
official copies. Facts describe attempted writes/software ordering, not measured
voltages/edges. The inspected FW2 release repository publishes images, not
complete stock loader/MAIN/DISPLAY implementation. Loader handoff state and
complete stock equivalence are therefore **unknown**. Exact selected SDK
callbacks will also need ELF review after a separately approved native build.

## Complete explicit app/BSP path plus default SDK runtime

```text
Stock Apps -> supported SD SRAM loader [implementation/state unavailable]
 -> SDK crt0 stack/data/BSS setup
 -> newlib runtime_init -> ordered preinit callbacks / constructors
    -> boot-ROM/runtime state, peripheral resets, clocks, PSRAM setup
 -> Field main -> pure fa_init
 -> board_init -> board_init_clk(250000)
    -> Vcore/clock changes -> PSRAM retiming -> board_init_peripherals(true)
    -> PIO1/LED clear -> SPI1 -> radio CS -> backlight off
    -> internal I2C1 recovery/setup -> ioexp_init
 -> recovery init -> UART keyboard/status + DISPLAY power ownership
 -> LCD init -> touch init -> About callback -> render -> backlight on
 -> recovery/touch loop; possible internal I2C recovery
 -> HOME watchdog reboot -> loader/stock startup [state restoration unknown]
```

The official `fw2_display_app` selects `no_flash`, not flash-stored
`copy_to_ram`. No stored stock image/bootloader replacement is involved.

## Electrical-effects table

Internal means a documented DISPLAY/onboard net, not universally harmless.
DISPLAY GPIO numbers are not MAIN header GPIO numbers. Unknown net mapping or
transients remain unknown. Stock-equivalence limitations follow the table.

| Source/function and sequence | Attempted electrical effect | Class / target relevance | Required; safely avoidable? | Evidence |
| --- | --- | --- | --- | --- |
| SDK `pico_crt0/crt0.S` -> newlib `runtime_init` -> `pico_runtime/runtime.c` | Initializes CPU, RAM state/stacks and ordered callbacks before main. | Internal CPU; stock handoff state unknown. | Standard entry; no qualified substitute. | SDK CRT/runtime source; no stock loader source. |
| SDK `runtime_init_early_resets` / post-clock resets | Resets DISPLAY GPIO-bank/pads/peripherals while excluding selected QSPI/PLL/USB/syscfg resets; unresets around clocks. | Internal SoC; connected control nets can change. Not a MAIN GPIO command. | Standard defaults; SDK skip knobs do not establish a BSP-qualified preserve-state composition. | `pico_runtime_init/runtime_init.c`; callback selection needs ELF review. |
| SDK `runtime_init_usb_power_down` | Conditionally powers down DISPLAY USB PHY if its reset-state condition holds. | Internal/device-port-facing; actual port effect conditional. | Standard conditional hook; no tested alternative. | Same runtime source; disabling USB stdio does not prove absence of this write. |
| SDK `runtime_init_clocks` | Oscillator/PLLs, system/peripheral/USB/ADC/HSTX clocks and ticks; conditional core-voltage minimum adjustment. | Internal core/shared peripheral timing. | Standard SDK path; not a target VREF setter. | `runtime_init_clocks.c`. Stock clock equality unverified. |
| SDK `runtime_init_setup_psram` | GPIO47/QMI CS1, PSRAM commands/timing/mapping. Board declares 8 MiB PSRAM. | Internal memory, not an external instrument operation. | Standard board/runtime; no qualified omission. | `hardware_psram/psram.c`; `bsp/boards/freewili2.h`. |
| BSP `board_init` -> `board_init_clk` | Vcore 1.25 V, 10 ms wait, system 250 MHz, clk_peri from clk_sys, PSRAM parameter/reinitialize calls. | Internal core/memory/shared timing. | Full supported SRAM path; other clocks still call expander/peripheral setup. | `platform/board.c`, `board.h`. Core VREG differs from target VREF. |
| `board_init_peripherals(true)` -> `ws2812_clear_once` | PIO1 reset; inverted DISPLAY GPIO21 LED data; two zero-color frames; releases state-machine resources. | Internal LEDs/PIO; inherited state not retained. | Full/inherited board path; no public SRAM switch to omit only this step. | Board and `leds/ws2812_driver.c`. No extra RGB rail request from Field. |
| `spi_bus_init`, then `st7796_init` | DISPLAY SPI1; GPIO10/11 SPI mux/12 mA drive; GPIO8 LCD DC and GPIO9 CS outputs; LCD traffic drives DC/CS/SCLK/MOSI. | Documented internal LCD/shared-radio bus, not MAIN external SPI1. | Needed supported display setup; cannot assume inherited settings. | `spi_bus.c`, `st7796.c`, board map. |
| Board radio GPIO40 CS | Initializes GPIO, output direction, then high to park CS. | Internal radio control; no TX operation. Brief transitions not measured. | Full board path; no qualified bypass. | Board source; low-to-high ordering must not be labelled glitch-free. |
| Board backlight GPIO25 | Output low during startup, high after complete render. | Internal LCD; not MAIN header GPIO25. | Required visibility; intentional state change. | Board map, app main. |
| `board_i2c1_init` / private recovery | DISPLAY GPIO26/27 input/output and pulls; up to nine SCL clocks/STOP on stuck SDA; assigns internal I2C1 at 400 kHz. | Internal touch/sensor/expander bus, not MAIN external I2C0. | Supported setup; not a target scan. | Board source/map. Expander pull-control exposure is a separate unknown. |
| `ioexp_init`: outputs then directions, I2C1 address 0x23 | Output bytes `F0 6C 08` to 0x04; direction bytes `00 00 04` to 0x0C. All outputs except P2 bit2/MCLR input. | **Target-facing VREF**, internal controls and unknown unnamed/buffer wiring. Overwrites full inherited expander state. | All inspected board variants call it. Omission is not a documented safe alternative. | `platform/ioexp.c`, exact writes expanded below. |
| `fw2_app_recovery_init` -> `uartkbd_init` | Internal UART1 at 62500, DISPLAY GPIO38/39 UART_AUX, FIFO/no HW flow, DMA ring/parser setup. | Internal coprocessor link. | Required keyboard/HOME/status; do not remove recovery. | `input/uartkbd.c`, recovery source. |
| Recovery init/task -> `picpwr_keep_awake(DISPLAY)` / task | Maintains zone 2. When a request/reassert is needed, sends full power configuration seeded from live/cached state. | Internal board power; collateral target/state preservation **unknown**. | Supported DISPLAY ownership. Hiding the required rail in metadata is not proof of safety. | `picpwr.c`/frame serialization; full config includes sleep/wake fields. |
| `ft6336_init` and polling | Reads touch 0x38 ID/registers; writes mode 0; repeated failures can recover I2C1. | Internal touch/shared bus; no target command. | Required touch; fallback/keyboard recovery intended. | `input/ft6336.c`. Recovery can recur after startup. |
| About/render/main loop/HOME | Registers UI restore, writes LCD, services recovery, watchdog reboot on HOME. | Internal UI/reset; subsequent stock startup may reconfigure hardware. | App contract; not an exact prior-state restoration. | App/recovery source; physical behavior needs qualification. |

### Expander defaults, bit by bit

These are attempts, not proof of chip acknowledgement or actual levels.
Transfers have 2 ms bounds. `ioexp_init` returns success, but board initialization
ignores that result; failure does not establish a known VREF state.

- P0 `0xF0`: SPI1 buffer-direction bits4-7 high; USB host-port-1 enable bit0
  low; unnamed bits1-3 low. Complete external exposure of unnamed/buffer nets
  cannot be established without the schematic.
- P1 `0x6C`: LCD reset bit2 high, GPIO25 direction bit5 high, I2C pull-control
  bit6 high; antenna V1_1 bit3 high/V2_1 bit1 low selects CC1101 433 route;
  USB host-port-2 enable bit4 and microphone power bit7 low; unnamed bit0 low.
  Do not infer external bus exposure from the pull-control signal name alone.
- P2 `0x08`: external VREF bit3 high; internal/programmable-Vout bit4, 5 V bit5
  and 3.3 V reference bit6 low. IR power bit0 and USB-device D+ pull-up bit1
  low; MCLR bit2 input; unnamed bit7 output low.
- Directions `00 00 04`: all outputs except MCLR. Latched values before
  directions avoid a reset-startup problem, but do not guarantee no transitions
  when the inherited pins already drive other values.

External-VREF selection can disconnect a previous internal reference and alter
header buffer reference/supply behavior. It does not itself program numeric
Vout, but is target-facing configuration. Unchanged target-power pins and
glitch-free switching are not established.

### Power config implications

PZCONFIG serializes awake/sleep masks plus wake/wake2, not a partial single-bit
update. Two distinct agreeing status frames and send spacing protect additive
awake requests; masks are clamped to zones 1-17. Cached sleep/wake fields are
not readback of arbitrary pre-launch settings and initially come from BSS
defaults. Exact board-manager/current-version effects are not publicly audited.
Do not promise all power settings are preserved because awake ORs in DISPLAY.

## M2 comparison and stock equivalence

M2 source `b069a71` uses the same board/recovery/LCD/touch/About/render/backlight
path and pins/SDK. It additionally calls `picpwr_release_unused`, scheduling
release of unrequested app-owned audio/sub-GHz/RGB/NFC rails. Field omits that
policy and adds no instrument, VREF setter, target GPIO, OneWili or output call.
Omission reduces one extra policy change; it does not prove electrical safety.

M2's display/touch/HOME/stock return passed by user report. No transient VIO/pin
measurement, arbitrary-setting preservation or exact rail restoration evidence
was supplied. Keep that distinction.

The expander comment says stock boots at external `vVIO`; LCD/touch cite stock
reference behavior. Those support intended defaults only. Complete equivalence
for SDK resets/clocks, GPIO/routing, power/sleep/wake and loader/installed firmware
is unverified. A boot-default match is not preservation of live launch settings.

## Supported alternatives examined

| Alternative | Result |
| --- | --- |
| `board_init_inherited()` | Avoids clock/QMI retiming but still clears LEDs and runs expander/peripheral initialization. |
| `board_init_clk(other_clock)` | Changes clock choice; retains expander writes. |
| `board_init_psram()` / PSRAM application | Still runs expander/peripherals; adds a different memory/startup contract. |
| LCD/touch without board/expander setup | No documented equivalent qualified RAM composition/inherited-state contract found. |
| SDK skip/weak-hook overrides | Public SDK mechanisms, but no qualified complete FW2 preserve-state startup established. Not implemented. |
| Restore VREF/power afterward | Cannot erase startup transients; no actual complete snapshot/restore API established. |

## Specific approval required

Recommend a **disconnected-target, UI-only standard-startup exception** using
the M2-style path, without the extra rail-release policy. It must acknowledge
VREF selection/full expander defaults, onboard routing/GPIO, SDK runtime/clock/
memory setup and normal DISPLAY power maintenance. This does not authorize
target connections, instrument/output operations or an electrical-safety claim.

After explicit exception approval, build `WiliPirate` in `build/field-device/`,
validate its SRAM/metadata/payload/linked-call properties, run full host checks
and record size/hash. Approval to replace/launch on physical SD, commit/push or
remove the diagnostic remains separate. No new UF2/hash exists yet and no
candidate is marked deployable.

## Primary source references

- [SDK CRT](https://github.com/raspberrypi/pico-sdk/blob/98a542c1a62fb549ffb5d66a3e5892b06276b670/src/rp2_common/pico_crt0/crt0.S),
  [newlib init](https://github.com/raspberrypi/pico-sdk/blob/98a542c1a62fb549ffb5d66a3e5892b06276b670/src/rp2_common/pico_clib_interface/newlib_interface.c#L181),
  [runtime resets](https://github.com/raspberrypi/pico-sdk/blob/98a542c1a62fb549ffb5d66a3e5892b06276b670/src/rp2_common/pico_runtime_init/runtime_init.c#L56),
  [clocks](https://github.com/raspberrypi/pico-sdk/blob/98a542c1a62fb549ffb5d66a3e5892b06276b670/src/rp2_common/pico_runtime_init/runtime_init_clocks.c#L32),
  [PSRAM](https://github.com/raspberrypi/pico-sdk/blob/98a542c1a62fb549ffb5d66a3e5892b06276b670/src/rp2_common/hardware_psram/psram.c#L344).
- [Board path](https://github.com/freewili/wilibsp/blob/be4bdd63d31a80f95410e583710cf4e43a7be7fa/bsp/platform/board.c#L18),
  [pin map](https://github.com/freewili/wilibsp/blob/be4bdd63d31a80f95410e583710cf4e43a7be7fa/bsp/platform/board.h),
  [expander](https://github.com/freewili/wilibsp/blob/be4bdd63d31a80f95410e583710cf4e43a7be7fa/bsp/platform/ioexp.c#L124).
- [SPI](https://github.com/freewili/wilibsp/blob/be4bdd63d31a80f95410e583710cf4e43a7be7fa/bsp/platform/spi_bus.c),
  [LEDs](https://github.com/freewili/wilibsp/blob/be4bdd63d31a80f95410e583710cf4e43a7be7fa/bsp/leds/ws2812_driver.c#L47),
  [LCD](https://github.com/freewili/wilibsp/blob/be4bdd63d31a80f95410e583710cf4e43a7be7fa/bsp/display/st7796.c#L84),
  [touch](https://github.com/freewili/wilibsp/blob/be4bdd63d31a80f95410e583710cf4e43a7be7fa/bsp/input/ft6336.c#L71).
- [Keyboard](https://github.com/freewili/wilibsp/blob/be4bdd63d31a80f95410e583710cf4e43a7be7fa/bsp/input/uartkbd.c#L103),
  [recovery](https://github.com/freewili/wilibsp/blob/be4bdd63d31a80f95410e583710cf4e43a7be7fa/bsp/input/app_recovery.c#L43),
  [power driver](https://github.com/freewili/wilibsp/blob/be4bdd63d31a80f95410e583710cf4e43a7be7fa/bsp/input/picpwr.c#L79),
  [power frame](https://github.com/freewili/wilibsp/blob/be4bdd63d31a80f95410e583710cf4e43a7be7fa/bsp/input/picpwr_frame.c).

See [replacement plan](FIELD_REPLACEMENT_PLAN.md). No exception was enabled.
Stop before implementing it and request explicit approval.
