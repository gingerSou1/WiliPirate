# FW2 panel research and reuse matrix

Date: 2026-10-07. Source inspection only; no device enumeration or connection.
Status: STOP before implementation. I2C scan result path and unchanged-VIO
launch cannot be established with the inspected public interfaces.

Follow-up: [the offline gate investigation](PANEL_OFFLINE_GATES.md) establishes
public generic command/response hooks. Their use is acceptable in principle;
the exact stock Poll body remains unverified. It also examines smaller startup
paths and stock menu/panel alternatives. Its findings supersede any blanket
exclusion of raw command hooks below.

## Current official sources

GitHub API `commits/HEAD` was queried and revision-addressed archives inspected
under ignored `build/panel-research/`. Upstream files were not modified.

| Repository | Current HEAD inspected |
| --- | --- |
| [WiliBSP](https://github.com/freewili/wilibsp/commit/be4bdd63d31a80f95410e583710cf4e43a7be7fa) | be4bdd63d31a80f95410e583710cf4e43a7be7fa |
| [OneWili](https://github.com/freewili/onewili/commit/b0eeccda21b0594c8062cd17e9755f0c26b0cf6b) | b0eeccda21b0594c8062cd17e9755f0c26b0cf6b |
| [FW2 docs](https://github.com/freewili/freewili2-docs/commit/0a4e3c224fdd4d0b61e6b02f64a14feaba28d725) | 0a4e3c224fdd4d0b61e6b02f64a14feaba28d725 |
| [FW2 firmware releases](https://github.com/freewili/FREE-WILi2-Firmware/commit/3d3302c701f2345b2131bc5da1edfa9f86ef5dcd) | 3d3302c701f2345b2131bc5da1edfa9f86ef5dcd |

WiliBSP's `libs/onewili` gitlink is b0eeccda, matching current OneWili HEAD.
The similarly named `freewili/freewili-firmware` at 3ac632861adc8f1f61de03f83a13e5c90a68383a
was checked and excluded: it is OG firmware, not FW2. The FW2 release repository
provides release assets/notes, not the stock MAIN implementation source needed
for a complete hardware-operation trace. No private-source access is assumed.

## Application model and safety gate

The [official running-apps contract](https://docs.freewili.com/files-and-apps/running-apps/)
supports SD `/apps/*.uf2` launching. WiliBSP
[app storage](https://github.com/freewili/wilibsp/blob/be4bdd63d31a80f95410e583710cf4e43a7be7fa/docs/app-storage.md)
and [CMake helper](https://github.com/freewili/wilibsp/blob/be4bdd63d31a80f95410e583710cf4e43a7be7fa/bsp/CMakeLists.txt)
establish `fw2_display_app`, version/description/power metadata, `no_flash`,
UF2 conversion and validation. Valid payload windows are SRAM
0x20000000..0x20070000 or PSRAM 0x11000000..0x11800000; QSPI flash is excluded.
SRAM is the simplest prospective choice. A RAM app temporarily takes over
DISPLAY execution; it is not a panel embedded into still-running stock DISPLAY.
Stock flash remains stored and MAIN continues; this is not permanent firmware
replacement. A `.uf2` suffix alone does not prove a non-flash artifact.

The [template](https://github.com/freewili/wilibsp/blob/be4bdd63d31a80f95410e583710cf4e43a7be7fa/apps/template/main.c)
calls `board_init`, `fw2_app_recovery_init`, `st7796_init` and
`picpwr_release_unused`; the loop services recovery and power.
The native display is 480x320. A prospective layout can fit two columns and
three rows plus a safety footer and discoverable Back/Exit/About controls.
No WiliPirate implementation exists yet.

[board.c](https://github.com/freewili/wilibsp/blob/be4bdd63d31a80f95410e583710cf4e43a7be7fa/bsp/platform/board.c)
calls `ioexp_init` through peripheral initialization.
[ioexp.c](https://github.com/freewili/wilibsp/blob/be4bdd63d31a80f95410e583710cf4e43a7be7fa/bsp/platform/ioexp.c)
sets `s_p2 = P2_EXT_VREF`, writes full output/direction registers, chooses the
CC1101 antenna route and resets other controls. These are unconditional startup
writes; matching stock boot defaults does not preserve the live launch state.
No supported state-preserving startup variant was established. Omitting required
initialization or editing the BSP would be an unapproved workaround.

[Recovery](https://github.com/freewili/wilibsp/blob/be4bdd63d31a80f95410e583710cf4e43a7be7fa/bsp/input/app_recovery.c)
requires continuous servicing; five-second HOME triggers a normal watchdog
reboot so the loader can resume stock DISPLAY. This is the documented exit
model, not a C return into stock firmware. Clean restoration and unchanged
power/VIO for WiliPirate remain UNVERIFIED. Synchronous OneWili use must employ
`fw2_app_recovery_open_onewili` per the app contract.

## Reuse matrix

All function names below are from the current
[WiliBSP OneWili header](https://github.com/freewili/onewili/blob/b0eeccda21b0594c8062cd17e9755f0c26b0cf6b/wilibsp/include/onewili.h)
and [generated implementation](https://github.com/freewili/onewili/blob/b0eeccda21b0594c8062cd17e9755f0c26b0cf6b/wilibsp/src/onewili.c).
The common supported interface is `onewili_fwgui`, not direct DISPLAY GPIO or
Linux transport. Callable means the binding exists; it does not mean the
installed device or safe lifecycle has been qualified. No calls were executed.

| Area | Stock capability/UI and official example | Callable/read/write/configuration | Power/VIO and safe state | WiliPirate work required |
| --- | --- | --- | --- | --- |
| GPIO | IO GPIO panel; BSP `apps/toggleled` and `apps/hello_vref` demonstrate MAIN header control | `ow_io_gpio_read_all` returns uint32; `set_io_high`, `set_io_low`, `set_io_toggle`, `set_pwm`; direction settings and `set_io_voltage_source` exist (all under `ow_io_gpio_`) | Header level shifting requires VIO; FPGA route documented. Read bitfield alone does not establish electrical state. Examples explicitly change VIO and are unsuitable for this milestone | Format stock readings; explicit ownership/configuration UI later; no custom pin driver and no startup writes |
| UART | IO UART Log receive/manual-send panel | `ow_io_uart_u_art_write`, `ow_io_uart_toggle_stream`; `ow_io_uart_settings_baud_rate`, data bits, module and RTS/CTS settings. Binding callable; complete receive-event delivery to this panel remains UNVERIFIED | Shared header/FPGA routing and VIO must be qualified; stream enable is a state change; no transmit or tile-entry configuration | Stock request/event presentation and bounded buffers after qualification |
| I2C | IO I2C Log Green Scan/Yellow Send; generated Poll command | `ow_io_i2c_i2c_poll` status only; `i2c_read` returns bytes but lacks request parameters described by console docs; `i2c_write` address/register/bytes; frequency/pull-up settings available | Documentation conflict: OneWili says zone 6 required; FW2 panel docs say direct I2C0 needs no FPGA. Resolve against qualified stock source before any call. Pull-ups/VIO/bus ownership unknown; probing is active bus traffic | Reuse stock Poll if its address-result contract can be established; currently BLOCKED, no substitute scanner |
| SPI | IO SPI Log transfer panel | `ow_io_spi_s_pi_write` returns full-duplex bytes; slave enable/data; frequency, CS pin, data bits, CPOL/CPHA settings | FPGA/header/VIO routing; a read transaction transmits clock/data. No transfer on entry; no assumed held-CS semantics | Transaction UI over stock API; no custom SPI driver |
| CAN/CAN-FD | IO CAN (FD) panel exists (panel docs incomplete); BSP `apps/canblast` exercises official link | `ow_io_canfd_write_canfd`, periodic transmit, filter/config register APIs; `enable_canfd_receive_queue`, `receive_canfd` return structured frames; display transport mirrors CAN binary reports | CAN switched rail and transceiver/configuration ownership required; receive setup can alter state; no transmission or implicit rail request | Queue/event presentation and explicit configuration later; reuse stock controller |
| Logic Analyzer | Shared stock analyzer view in GPIO/I2C/SPI/UART/MDIO; PIO capture described by FW2 panel docs | `ow_io_logic_analyzer_setup_logic_analyzer`, `setup_analog`, `start`, `stop`, `trigger` exist. No complete display-link sample-read path established | Display transport explicitly does not mirror logic-analyzer binary reports. Stock local PIO view and generated capture commands must not be conflated. Protocol routing rails/VIO may still matter | Leave unavailable until supported acquisition/result path is proved; no replacement capture engine |

Stock panel evidence: [GPIO](https://github.com/freewili/freewili2-docs/blob/0a4e3c224fdd4d0b61e6b02f64a14feaba28d725/docs/panels/gpio.md),
[UART](https://github.com/freewili/freewili2-docs/blob/0a4e3c224fdd4d0b61e6b02f64a14feaba28d725/docs/panels/uart.md),
[I2C](https://github.com/freewili/freewili2-docs/blob/0a4e3c224fdd4d0b61e6b02f64a14feaba28d725/docs/panels/i2c.md),
[SPI](https://github.com/freewili/freewili2-docs/blob/0a4e3c224fdd4d0b61e6b02f64a14feaba28d725/docs/panels/spi.md),
[CAN](https://github.com/freewili/freewili2-docs/blob/0a4e3c224fdd4d0b61e6b02f64a14feaba28d725/docs/panels/can-fd.md).
These establish documented UI behavior, not audited stock functions.
Official examples: [toggleled](https://github.com/freewili/wilibsp/tree/be4bdd63d31a80f95410e583710cf4e43a7be7fa/apps/toggleled),
[canblast](https://github.com/freewili/wilibsp/tree/be4bdd63d31a80f95410e583710cf4e43a7be7fa/apps/canblast).
No dedicated external-I2C scanner example was found in the inspected BSP apps.
Internal sensor drivers are not substitutes for MAIN's external bus interface.

## Exact I2C proof and stop

```text
WiliPirate SCAN (not implemented)
 -> ow_io_i2c_i2c_poll(dev) (supported generated binding)
 -> ow__call(dev, "i\\i\\p", resp, sizeof resp)
 -> FwGUI DISPLAY UART0 link to stock MAIN
 -> stock menu Poll handler / existing hardware scan (implementation unavailable)
 -> address result delivery (UNVERIFIED)
 -> WiliPirate device list (BLOCKED)
```

The generated function explicitly executes `(void)resp` and returns `OW_OK`;
its public signature has no address output. The
[OneWili I2C contract](https://github.com/freewili/onewili/blob/b0eeccda21b0594c8062cd17e9755f0c26b0cf6b/docs/i2c.md)
also says Poll returns no value. The stock I2C panel documentation describes
one-byte reads from answering non-reserved addresses and a count summary,
but does not establish that those log records are exported to a RAM app.
Documented `i2cmon` and `i2cslv` byte events are not a documented scan-address
result schema. `ow_poll_text_line` availability does not prove scan logs arrive
there, identify a particular scan or are complete. Parsing guessed human text,
using raw commands, reconstructing scanning with reads or direct bit-banging
would not satisfy the requested supported reuse proof.

The official [display transport contract](https://github.com/freewili/onewili/blob/b0eeccda21b0594c8062cd17e9755f0c26b0cf6b/wilibsp/README.md)
supports MAIN calls over internal UART0 at 8 Mbaud. It does not supply the missing
MAIN handler or scan result schema. Exact MAIN hardware function names therefore
remain UNVERIFIED; none are invented here. The shared C parser's previously
recorded identity-validation issue is an additional qualification concern,
not repaired as part of this pivot.

## Validation and next gate

Windows host regression: `python -B -m unittest discover -s tests -v`:
50 tests discovered, 49 passed, one native constexpr/compiler check skipped
because a C++ compiler is unavailable. Import/call guards, runtime audits,
all M1B regressions and M1A report identity passed. `git diff --check` passed.
No native panel compile
was attempted because architecture gates failed. The known M1E wrong-command
tests remain unchanged and failing as previously recorded; they were not rerun
or relabeled as successes. No CM0 staging package was made.

Before implementation, obtain an official documented state-preserving app
startup path and stock Poll result interface/source, including power conditions,
failure behavior and firmware versions. Public evidence currently proves a RAM
app mechanism and broad stock API reuse, not all seven success criteria.

Proposed smallest eventual physical test, only after those offline gates and
explicit approval: with no target attached, launch a validated SRAM-only UI app
from stock SD Apps; record pre/post VIO and power through a supported observation
method, navigate all six inert tiles, exercise Back and HOME recovery, and verify
normal stock operation returns. No scan, transmission, target power enable or
VIO change in that first test. It is not executable now: there is no qualified
app or preservation path. A subsequent separately approved I2C scan would need
electrical prerequisites and a known safe target. No physical action is requested
or authorized by this report.
