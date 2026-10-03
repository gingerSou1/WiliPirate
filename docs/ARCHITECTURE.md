# WiliPirate architecture (Milestone 0)

Research date: 2026-10-03. Source revisions are pinned in SOURCES.json.
VERIFIED below means the specific API/contract is established by source, not
that WiliPirate has been run on a physical FREE-WILi. No device was accessed.

## Decision and application contract

Use a Python 3.10+ Linux user-space application on the FREE-WILi 2 CM0,
following WiliCM0BSP's Python examples. CM0 is the BCM2837/CM3-class Linux
module; it is not an ARM Cortex-M0. Stock MAIN and DISPLAY remain in place.
Do not build Bus Pirate, Bit Pirate, WiliBSP or replacement RP2350 firmware.

Source layout: apps/wilipirate/app.py plus an executable run.sh, docs/, tests/,
and tools/. The standalone BSP is pinned under vendor/wilicm0bsp as a Git
submodule; no Bus Pirate or Bit Pirate source tree is incorporated. See
[docs/apps.md](https://github.com/freewili/wilicm0bsp/blob/d27edf1c18bc2c18a76c4c9cdbf200c19884cc06/docs/apps.md), [apps/template/app.py](https://github.com/freewili/wilicm0bsp/blob/d27edf1c18bc2c18a76c4c9cdbf200c19884cc06/apps/template/app.py) and [tools/fw.py](https://github.com/freewili/wilicm0bsp/blob/d27edf1c18bc2c18a76c4c9cdbf200c19884cc06/tools/fw.py).

The normal deployment location is /home/apps/wilipirate/ on CM0 Linux.
Linux > Apps > wilipirate > run.sh runs as the console user, with the app's
folder as cwd. stdin is disconnected; stdout/stderr go to a unique log under
~/.local/state/freewili/apps/. A reported PID proves spawning only, so a future
device check must examine the log and exit result. MAIN SD /apps/ is a different
DISPLAY app surface and is not our deployment destination.

Milestone 1 is deliberately dependency-free and uses this supported launcher
contract without opening OneWili. Menu launch prints identification, safety
status, help, info and the mode list to its log and exits. An explicit
--console option provides the interactive HiZ> terminal. Repeated --command
arguments support deterministic local use; no eval, shell execution or arbitrary
Python scripting is exposed. There is no LCD/touch UI in this milestone.
The staged app folder is relocatable and does not need the repository or BSP.

Future hardware adapters will bundle the BSP-pinned OneWili CM0 package and its
pinned result dependency on the development host, preserving licenses. They
must not install packages or modify the Linux image on application startup.
The M1 staging tool copies only WiliPirate source and documentation; it does
not call BSP setup/install, connect to hardware, or fetch dependencies.
No persistent app data is written in M1. If needed later, use
~/.local/share/wilipirate/ and ~/.config/wilipirate/ rather than the code folder.

## Hardware access path

Python -> onewili_cm0.connect_cm0() -> fwcm0 api -> running fwcm0-bridge
-> FPGA mailbox -> stock MAIN -> firmware-owned peripherals / DISPLAY.

See [docs/architecture.md](https://github.com/freewili/wilicm0bsp/blob/d27edf1c18bc2c18a76c4c9cdbf200c19884cc06/docs/architecture.md), [agents/hardware.md](https://github.com/freewili/wilicm0bsp/blob/d27edf1c18bc2c18a76c4c9cdbf200c19884cc06/agents/hardware.md) and [cm0/README.md](https://github.com/freewili/onewili/blob/9ce9df83b89f83681507f19f960958e23f20ac37/cm0/README.md). The bridge owns its internal Linux
SPI/UART link; those devices are transport resources, not user bus adapters.
Never open /dev/spidev*, /dev/tty*, GPIO chip devices, stop the bridge or reset
the FPGA to acquire an external protocol. Use the official request/reply API.
The bridge socket is /run/fwcm0-bridge.sock. MAIN protocol 1.2+ and matching
bridge/firmware/gateware are prerequisites, not permission to upgrade a device.
Only one CM0 API/interactive-console owner can use that channel at a time.
The ordinary Linux shell and MAIN USB commands can coexist.

Opening OneWili already probes Device State. A future adapter must explicitly
open once, check every Result, handle busy/missing/disconnected sessions,
close in finally/context-manager cleanup, and never automatically repeat an
ambiguous hardware write. Keep one device instance on one application thread.
The mailbox limits (511 command-text bytes / 4095 captured-reply bytes) rule out
assuming arbitrary USB-sized transactions. FTDI binary event buffers and USB
directory-list events are not forwarded through this CM0 transport.

## Relevant API evidence and limits

| Area | Official API or example | Consequence |
| --- | --- | --- |
| Identity | hardware.system.device_state(); [apps/hello_python/app.py](https://github.com/freewili/wilicm0bsp/blob/d27edf1c18bc2c18a76c4c9cdbf200c19884cc06/apps/hello_python/app.py) | Read-only connection check; no settings changed by the example. |
| Display text | gui.show_text(text), gui.clear_display(); [docs/gui.md](https://github.com/freewili/onewili/blob/9ce9df83b89f83681507f19f960958e23f20ac37/docs/gui.md) | Overlay is replaced/truncated, not a scrolling console. |
| Graphics/controls | gui.panels.add_panel(), gui.controls.add_text(), add_button(), add_plot(); [docs/gui_panels.md](https://github.com/freewili/onewili/blob/9ce9df83b89f83681507f19f960958e23f20ac37/docs/gui_panels.md), [docs/gui_controls.md](https://github.com/freewili/onewili/blob/9ce9df83b89f83681507f19f960958e23f20ac37/docs/gui_controls.md) | Command surface exists; target firmware compatibility must be tested. |
| Input | gui.panels.read_buttons(); [docs/gui_panels.md](https://github.com/freewili/onewili/blob/9ce9df83b89f83681507f19f960958e23f20ac37/docs/gui_panels.md) | Polled, read-and-clear press latch; one consumer. Shared latch, no release/long-press history. Raw touch-coordinate delivery to CM0 remains UNKNOWN. |
| GPIO | io.gpio.read_all(); [apps/gpio_poll/app.py](https://github.com/freewili/wilicm0bsp/blob/d27edf1c18bc2c18a76c4c9cdbf200c19884cc06/apps/gpio_poll/app.py), [docs/gpio.md](https://github.com/freewili/onewili/blob/9ce9df83b89f83681507f19f960958e23f20ac37/docs/gpio.md) | Returns an integer bitfield. Upstream reports a CM0 read test in [docs/platform-support.md](https://github.com/freewili/wilicm0bsp/blob/d27edf1c18bc2c18a76c4c9cdbf200c19884cc06/docs/platform-support.md). External electrical levels are not established by an internal bitfield alone. |
| UART | io.uart.u_art_write(bytes), toggle_stream(); [docs/uart.md](https://github.com/freewili/onewili/blob/9ce9df83b89f83681507f19f960958e23f20ac37/docs/uart.md) | API exists; receive/event semantics on CM0 need confirmation. Description text contains obvious I2C copy/paste errors. Do not invent read(timeout). |
| I2C | io.i2c.i2c_write(address, register, bytes), i2c_read(), i2c_poll(); [docs/i2c.md](https://github.com/freewili/onewili/blob/9ce9df83b89f83681507f19f960958e23f20ac37/docs/i2c.md) | poll says it tests all addresses but returns Result with no typed address list. read has no address/count arguments. User-facing scan output is unproven. |
| SPI | io.spi.s_pi_write(bytes); [docs/spi.md](https://github.com/freewili/onewili/blob/9ce9df83b89f83681507f19f960958e23f20ac37/docs/spi.md) | Full-duplex bytes returned, CS automatically asserted/deasserted per transaction. Arbitrary Bus Pirate CS-held sequences are not established. |
| Power | hardware.power_management.*; [docs/power_management.md](https://github.com/freewili/onewili/blob/9ce9df83b89f83681507f19f960958e23f20ac37/docs/power_management.md) | Zone APIs exist, but several query calls return only Ok/Err, not typed telemetry. Board zones are not interchangeable with target power. |
| VIO/Vout | io.gpio.set_io_voltage_source(), io.analog_out.set_v_prog_vout(); [docs/gpio.md](https://github.com/freewili/onewili/blob/9ce9df83b89f83681507f19f960958e23f20ac37/docs/gpio.md), [docs/analog_out.md](https://github.com/freewili/onewili/blob/9ce9df83b89f83681507f19f960958e23f20ac37/docs/analog_out.md) | Documented writes; neither is invoked in M1. Readback, isolation, current limiting and connector safety remain unqualified. |

The I2C/SPI/UART settings menus demonstrate configuration methods, but do not
establish an atomic snapshot/restore API. GPIO direction menu commands are
not a generic, proven set-all-inputs operation. Do not infer pin maps from
method names. The DISPLAY BSP distinguishes internal RP2350B GPIO from MAIN
external header GPIO and warns that the header level shifters require VIO;
see [AGENTS.md](https://github.com/freewili/wilibsp/blob/be4bdd63d31a80f95410e583710cf4e43a7be7fa/AGENTS.md). Its low-level display drivers are not CM0 Python APIs.

The public GUI repository contains README/changelog and release assets rather
than the GUI implementation. Its README points CM0 applications to WiliCM0BSP.
Host OneWili examples commonly use onewili.connect() (USB discovery); that is
not a substitute for connect_cm0(). Package-example findings are recorded in
GUI_EXAMPLES.md. See [README.md](https://github.com/freewili/freewili-gui/blob/275df3608c76e04f10f36a3964fdbceadd76d4f0/README.md) and [CHANGELOG.md](https://github.com/freewili/freewili-gui/blob/275df3608c76e04f10f36a3964fdbceadd76d4f0/CHANGELOG.md).

## Bus Pirate versus Bit Pirate

These are reference implementations, not firmware targets for FREE-WILi.
Bus Pirate 5/6 share a Pico SDK C firmware architecture; Bit Pirate uses C++
and Arduino/ESP-IDF services. Neither hardware layer can be linked into a
Python CM0 script. New Python code against OneWili is the smaller boundary.

| Concern | Bus Pirate reference | Bit Pirate reference | WiliPirate decision |
| --- | --- | --- | --- |
| Command parser | src/ui/ui_parse.c, src/syntax_compile.c | src/Transformers/TerminalCommandTransformer.cpp | Reimplement strict help/info/mode token parsing. No autocorrection of hardware commands. |
| Bus Pirate syntax | Compiler -> bytecode -> runner -> result formatting | src/Models/ByteCode.h and service executeByteCode methods | Preserve familiar vocabulary later; defer brackets, repeats, numbers and transactional syntax until backend semantics are known. |
| Protocol abstraction | src/modes.h function table, setup/cleanup hooks | src/Interfaces/II2cService.h, ISpiService.h, IUartService.h | Future Python adapters advertise actual supported operations; no generic bit-banging fallback. |
| I2C | src/mode/hwi2c.c, src/commands/i2c/scan.c | src/Services/I2cService.cpp wraps Wire | Reimplement with OneWili; address scan result decoding unresolved. |
| SPI | src/mode/hwspi.c, src/pirate/hwspi.c | src/Services/SpiService.cpp wraps SPI/ESP drivers | Reimplement; automatic OneWili CS lifecycle limits syntax fidelity. |
| UART | src/mode/hwuart.c | src/Services/UartService.cpp uses HardwareSerial | Reimplement; prove CM0 receive behavior before promising a bridge. |
| GPIO/DIO | src/mode/dio.c, src/mode/hiz.c | src/Services/PinService.cpp | Read-only first; no inherited upstream pin mappings or voltage assumptions. |
| 1-Wire | src/mode/hw1wire.c, PIO routines | src/Services/OneWireService.cpp | UNKNOWN official CM0 API; do not software-time it across Linux/mailbox. |
| Hex/ASCII | src/syntax_post.c, terminal helpers | service bytecode output and terminal views | New Python formatting later; no import of firmware terminal dependencies. |
| Macros | protocol_macro dispatch in modes.h | bounded repeat/pipeline transformer | Defer; no macro execution in M1. |
| Scripting | src/commands/global/script.c | README host Python scripting / serial tools | Future bounded command files, not arbitrary eval; hardware adapter still gates every action. |
| Terminal UX | mode prompt, help, VT100 status | shared serial/web commands, named modes | Adopt HiZ> and explicit mode list; plain-text portable console first. |

Source anchors: [src/modes.h](https://github.com/DangerousPrototypes/BusPirate5-firmware/blob/f8ccc1c4b5c392ffb09fb21348bcedf219919816/src/modes.h), [src/syntax_compile.c](https://github.com/DangerousPrototypes/BusPirate5-firmware/blob/f8ccc1c4b5c392ffb09fb21348bcedf219919816/src/syntax_compile.c), [src/syntax_post.c](https://github.com/DangerousPrototypes/BusPirate5-firmware/blob/f8ccc1c4b5c392ffb09fb21348bcedf219919816/src/syntax_post.c), [src/Transformers/TerminalCommandTransformer.cpp](https://github.com/geo-tp/ESP32-Bit-Pirate/blob/8022f80381bb12bc87c5d4d69bd296db661d6d1c/src/Transformers/TerminalCommandTransformer.cpp),
[src/Interfaces/II2cService.h](https://github.com/geo-tp/ESP32-Bit-Pirate/blob/8022f80381bb12bc87c5d4d69bd296db661d6d1c/src/Interfaces/II2cService.h), [README.md](https://github.com/geo-tp/ESP32-Bit-Pirate/blob/8022f80381bb12bc87c5d4d69bd296db661d6d1c/README.md). See THIRD_PARTY.md for licenses and reuse policy.
No Bus Pirate or Bit Pirate implementation, assets, tables or driver code is
copied, translated or linked in M1. Only the interaction concepts are reused.

## Safe state and boundaries

HiZ is the application's no-I/O state. Startup, help, info, mode inspection,
invalid input, exit, EOF and interruption perform zero hardware calls.
mode hiz succeeds; I2C/SPI/UART/GPIO are listed as unavailable and cannot be
entered. No fake ACKs, simulated bus results, power control or pin values.
This does not force previously configured pins into electrical high impedance,
turn off power owned by another app, or establish that connecting a target is safe.
The UI must say external pin/power state is unknown, not certify physical HiZ.

Future mode transitions need explicit ownership, verified connector maps,
voltage/pull-up/direction policy, and failure cleanup. Target power must require
an explicit user action. Never restore an unknown state or change all power
zones as a shortcut. Cancellation of a write is not proof it did not occur.

## Milestone 2 recommendation (not implemented)

First implement GPIO read-only snapshot, following the upstream gpio_poll
example. This has stronger CM0 evidence than I2C address-list decoding,
UART reception or arbitrary SPI framing. It proves the request/reply path
without driving external pins. An I2C scan is the next candidate only after
its result format and electrical setup are established.

Exact proposed first hardware test, requiring later approval:

1. Record the FREE-WILi 2 board revision, stock MAIN/DISPLAY/FPGA versions,
   Linux image, Python and fwcm0 versions. Disconnect all external targets.
   Confirm an already compatible bridge with API support; if absent, stop
   and request a separate maintenance decision, not an automatic upgrade.
2. With the operator's normal CM0/FPGA power prerequisites already met, launch
   the staged no-I/O M1 app from Linux > Apps. Check the log for WiliPirate,
   HiZ, help/info/mode, normal exit and no hardware actions.
3. In a separately approved M2 read-only adapter, open connect_cm0(), record
   hardware.system.device_state().unwrap(), then call io.gpio.read_all().unwrap()
   exactly ten times at 0.5-second intervals, recording each 32-bit hex value.
   Close the connection and verify one reopen succeeds. No direction, VIO,
   pull-up, target-power, clock, bus-write or streaming setters may be called.
4. Pass requires ten successful typed reads, no errors, orderly close/reopen,
   and no settings writes. Stop on the first busy, power-zone, timeout or
   disconnected error. No automatic power changes or retries. This establishes
   API access only; floating values are not a connector voltage measurement.
5. A later external-input test needs a documented header mapping, confirmed
   input configuration and voltage limits, and a reviewed current-limited
   fixture. No pin number or wiring is proposed without that evidence.

## Unknowns and excluded claims

Actual device/firmware compatibility and physical app-menu launch are untested.
No display/touch interaction has been qualified from this CM0 application.
I2C scan addresses, UART receive delivery, target-power readback and electrical
HiZ enforcement remain unresolved. 1-Wire/JTAG/SWD official CM0 APIs were not
found; that is UNKNOWN, not proof that the hardware could never support them.
Logic-analyzer FTDI acquisition is UNSUPPORTED over the researched CM0 route.
Radio/BLE/Wi-Fi menu presence does not prove all USB event features work on CM0.
Neither upstream's electrical specifications are inherited by FREE-WILi.
