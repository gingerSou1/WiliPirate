# WiliPirate architecture (Milestone 1B)

Research date: 2026-10-03. Source revisions are pinned in SOURCES.json.
VERIFIED below means the specific API/contract is established by source, not
that WiliPirate has been run on a physical FREE-WILi. No device was accessed.

## M1C host UI boundary

The M1B application below remains unchanged. The official emulator cannot run
CM0 Python; see [current source research](EMULATOR_RESEARCH.md). M1C adds only
`ui/` and `tools/ui_preview.py`: synthetic button input -> UI controller ->
Application.submit/view -> pure text-cell frame -> host Tk canvas. Commands
always go through the existing parser/state/stubs. No OneWili client or live
renderer is provided, and the device staging file list remains unchanged.
The [host validation guide](M1C_VALIDATION.md) records exact reproduction steps,
checks, and the remaining gap to the supported physical CM0 panel mechanism.
The located htop example informs text/button contracts only; its transport,
subprocess and dependency-install code is not reused or executed.

## Current M1B architecture (2026-10-04)

M0/M1 research remains the baseline below. M1B supersedes the earlier restriction
on selecting modes: HiZ, UART, I2C, SPI and GPIO are now logical application
states, all using explicit stubs. No hardware operations or device UI calls are
implemented. M1A is deferred; its immutable [finding](HARDWARE_VALIDATION.md)
and commit ea864b8 are preserved unchanged.

```text
app.py / run.sh
  -> wilipirate.console (terminal input and log output)
  -> wilipirate.application + parser (strict command parsing)
  -> wilipirate.state.Session (per-session mode and lifecycle)
  -> wilipirate.backends.Backend (request/result interface)
  -> HiZStub / UARTStub / I2CStub / SPIStub / GPIOStub
```

| Layer | Contract |
| --- | --- |
| Console | Reads only explicit --console input, displays Reply text and ViewState.prompt. Default Linux Apps invocation needs no stdin. |
| Parser | Returns immutable Command(name, arguments). Case-insensitive verbs/mode names; opaque payload arguments preserve case. Rejects bad arity, controls, chaining and >256 characters. No eval or shell. |
| Application | UI-independent submit(line) and immutable view snapshot; catches parse errors without changing state. An eventual panel UI uses the same entry point. |
| State manager | Starts each session in HiZ; supports all 25 transitions without a backend call. Invalid modes preserve the current mode. Exit/EOF/Ctrl-C close the session; closed sessions reject further commands. |
| Hardware abstraction | Backend declares mode, operation vocabulary, label, availability, and request(operation, arguments) -> BackendResult. No connection/open/initialize API is exposed in M1B. |
| Stubs | Five explicit types composed by a fixed factory. Every request returns unavailable and a message; never fake ACKs/data. Missing registry entries report unavailable with no fallback. |

Commands: help, info, mode, mode hiz/uart/i2c/spi/gpio, exit. Mode-specific
stub requests are scan (I2C), read/write (UART/I2C/GPIO), and transfer (SPI).
write/transfer take opaque arguments solely to exercise dispatch; no physical
syntax/encoding or transfer is implemented. Unsupported-mode requests fail.
info always exposes the current logical mode and STUB status in the shipped
configuration. Batch sessions stop on first failed/stub request with status 2;
interactive sessions report it and continue. No real-backend flags, imports,
plugins, environment switches, network discovery or device enumeration exist.

The runtime uses only standard-library modules and its local package. The
staging tool copies an explicit file list, not all Python files or the BSP;
it preserves relocatability and refuses existing destinations. The unchanged
shell entry point invokes Python with -B, avoiding runtime bytecode writes.

### Mandatory fail-closed rule and M1A hazard

**WiliPirate must never silently fall back from the supported bridge to direct
hardware access. If a required bridge/API is unavailable, fail closed and
report the unavailable capability.** Explicit M1B stubs are the build's only
backends, not an automatic replacement for a failed hardware connection.

In the pinned BSP, [run_console_cli](../vendor/wilicm0bsp/drivers/fwcm0/src/console_cli.cpp)
tries its bridge socket and calls run_direct(api) when connection fails.
That constructs [LinuxTransport](../vendor/wilicm0bsp/drivers/fwcm0/src/linux_transport.cpp),
which configures SPI mode/word size/speed, GPIO chip select and UART termios.
The OneWili CM0 Python adapter starts fwcm0 api, so even a documented read-only
Device State request cannot make this connection fallback safe. A socket-file
existence check does not eliminate the race. Do not invoke the adapter, doctor
or CLI, patch the BSP, or introduce a custom bypass as part of M1B.

Future integration must separately establish a supported, version-qualified
bridge-only path that cannot acquire/configure hardware when unavailable;
busy/disconnect/timeout must terminate the attempt and report the capability
unavailable. No automatic power enable, peripheral initialization or retry of
ambiguous writes. M1B provides no transport implementation at all.

Import/call allowlists cover every runtime module. A subprocess audit exercises
imports and commands while rejecting hardware imports, processes, networking,
device/file operations and configuration changes; ordinary Python import reads
are allowed during import only. Tests cover all logical transitions, failure
paths, dynamic prompts, packaging and the original M1A report's Git blob hash.
These guards detect accidental integration, not malicious Python execution.

### Eventual interactive Wili UI

See [UI_RESEARCH.md](UI_RESEARCH.md) for supported panel/text/update/button APIs
and the unlocated htop-style example. M1B ships a functional console/log UI;
it does not claim an LCD/touch implementation or on-device launch verification.
An eventual renderer will consume ViewState/Reply and submit commands through
Application. Display/input calls themselves are peripheral operations and remain
excluded now, even while the bus backends are stubs. No on-device prerequisites
are installed or enabled by the application.

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

The original Milestone 1 was deliberately dependency-free; M1B retains that boundary and uses this supported launcher
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

## Previously researched hardware access path (blocked by M1A)

The following describes upstream behavior, not a safe connection procedure.
The current pinned adapter must not be invoked: its fallback can initialize
hardware. Any future integration requires separately verified supported
bridge-only behavior before the lifecycle described below is applicable.

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
| Identity | hardware.system.device_state(); [apps/hello_python/app.py](https://github.com/freewili/wilicm0bsp/blob/d27edf1c18bc2c18a76c4c9cdbf200c19884cc06/apps/hello_python/app.py) | Device State query is described as read-only, but connection can initialize hardware via the M1A fallback; the example is not safe to execute under current constraints. |
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
All five modes can be selected logically in M1B; their hardware backends remain
unavailable. No mode selection initializes or configures any peripheral. No fake ACKs, simulated bus results, power control or pin values.
This does not force previously configured pins into electrical high impedance,
turn off power owned by another app, or establish that connecting a target is safe.
The UI must say external pin/power state is unknown, not certify physical HiZ.

Future mode transitions need explicit ownership, verified connector maps,
voltage/pull-up/direction policy, and failure cleanup. Target power must require
an explicit user action. Never restore an unknown state or change all power
zones as a shortcut. Cancellation of a write is not proof it did not occur.

## Earlier Milestone 2 proposal (withdrawn pending safety qualification)

The M0 candidate was a GPIO snapshot using the upstream gpio_poll example.
M1A invalidated its connection assumption: opening connect_cm0 can initialize
hardware through the direct fallback. The earlier timed GPIO-read procedure
must not be executed. Its historical text remains in the M0/M1 Git history;
the M1A findings themselves remain unchanged in HARDWARE_VALIDATION.md.

Physical work is deferred. Before proposing any GPIO read, trace the exact
installed API and transport and establish no initialization, configuration,
mux, direction, pull, power or peripheral writes. Upstream read-test reports
alone are insufficient. A supported bridge-only path, explicit authorization
and resumed framework validation are prerequisites. No wiring, snapshot,
scan or physical bus experiment is part of M1B.

## Unknowns and excluded claims

Actual device/firmware compatibility and physical app-menu launch are untested.
No display/touch interaction has been qualified from this CM0 application.
I2C scan addresses, UART receive delivery, target-power readback and electrical
HiZ enforcement remain unresolved. 1-Wire/JTAG/SWD official CM0 APIs were not
found; that is UNKNOWN, not proof that the hardware could never support them.
Logic-analyzer FTDI acquisition is UNSUPPORTED over the researched CM0 route.
Radio/BLE/Wi-Fi menu presence does not prove all USB event features work on CM0.
Neither upstream's electrical specifications are inherited by FREE-WILi.
