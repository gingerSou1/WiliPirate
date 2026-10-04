# M1D: on-device architecture decision

Date: 2026-10-04. Branch: feature/wili-ui. Baseline: c91eb71.
Research and host checks only. No physical connection, deployment, installation,
firmware change, device command or hardware example execution occurred.

## Decision and delivery status

**Choose a CM0 Linux C++ application using the official WiliCM0BSP
`wilicm0::Device` socket-only adapter.** This is a supported native Linux
application model, not WiliBSP DISPLAY firmware and not a custom transport.
The existing Python core and Tk UI remain unchanged as behavioral references.

This choice preserves executing stock MAIN and DISPLAY firmware and avoids
board-level startup/power policy. It is based on source evidence, not existing
Python investment: the Python adapter is deliberately not used. Its fwcm0 api
fallback finding remains valid. The official C++ adapter already has the
bridge-only behavior the Python route lacks; using it is not implementing a
workaround inside that unsafe CLI or replacing the bridge.

WiliBSP provides the best normal Apps/SD experience and a supported RAM image,
but its current simplest startup explicitly changes external VREF selection
and radio routing. That violates this milestone's constraints even for a stub UI.
Do not omit required initialization or patch the BSP to conceal those effects.

A minimal C++ CM0 UI and build/launch recipe are prepared in native/cm0. **No
ARM64 Linux binary or installable candidate has been produced.** The desktop
lacks a usable Linux ARM64 toolchain/sysroot. Source/compile-time checks passed;
Linux linking, fake-bridge integration and installed-stack/UI behavior remain
qualification gates. No physical launch is authorized or performed.

## Comparison

| Question | WiliBSP loadable DISPLAY application | CM0 Linux/OneWili application |
| --- | --- | --- |
| File/location | WiliPirate.uf2 at normal SD:/apps/WiliPirate.uf2 | Folder at CM0 /home/apps/wilipirate/, run.sh entry; not MAIN SD:/apps |
| Install/remove | Copy verified RAM/PSRAM UF2 to SD; remove that file while not running | Stage folder/dependencies on PC; copy to CM0; stop process before removing folder; preserve any separate user data |
| Launcher | Normal main-screen Apps/App Explorer selects UF2 | Linux > Apps > wilipirate > run.sh |
| CPU | DISPLAY RP2350B | CM0 Linux module (BCM2837/CM3-class), not Cortex-M0 |
| Execution | Bare-metal application replaces executing stock DISPLAY code temporarily; MAIN stays stock | Linux process; MAIN and DISPLAY remain executing stock firmware |
| Storage vs execution | Supported helper uses no_flash SRAM payload; optional distinct PSRAM helper/bootstrap | Python source on Linux filesystem, interpreted in process memory; not a DISPLAY UF2 |
| Display | Direct ST7796 LCD driver, optional graphics/LVGL layers | Generated OneWili GUI panel/control requests via MAIN to stock DISPLAY |
| Input | uartkbd internal UART/DMA keypad; FT6336U I2C touch driver when initialized | Panel/keypad shared read-and-clear bitmask; no verified general CM0 touch-coordinate route |
| Exit | Application must service recovery; hold HOME 5 seconds triggers DISPLAY watchdog reboot | App closes owned resources and exits; must separately restore app-owned UI state |
| Stock restoration | Flash image remains stored and loader is intended to resume it; depends on responsive recovery/working firmware | Stock CPUs were never replaced; process exit alone does not restore prior panel |
| Eventual external buses | Generated OneWili API over DISPLAY-to-MAIN UART; direct DISPLAY GPIO is not equivalent to MAIN external header | Generated OneWili API over internal mailbox/bridge; external operations require later capability/electrical validation |
| Startup side effects | board_init changes clocks, buses, expander/VREF/radio; recovery manages power | Python adapter can fall back; selected C++ adapter opens only the bridge socket, API session and Device State probe |
| Fail closed today | UF2 storage validator rejects flash/mixed regions; no equivalent guarantee for board initialization | Official C++ adapter throws/closes on failure without alternate transport; current Python adapter is excluded |
| Maintainability | C/C++ toolchain, pinned BSP/SDK, board ownership and native app contract; Python semantics would need parity tests | Linux C++ toolchain, pinned OneWili/bridge; small logical model checked against Python reference, no board drivers |

## Native path: source trace and exact boundary

### Build and load

The simplest official example is [apps/template/main.c](https://github.com/freewili/wilibsp/blob/be4bdd63d31a80f95410e583710cf4e43a7be7fa/apps/template/main.c)
with [its CMake target](https://github.com/freewili/wilibsp/blob/be4bdd63d31a80f95410e583710cf4e43a7be7fa/apps/template/CMakeLists.txt). It calls board_init,
recovery initialization, LCD initialization, draws text, releases unused rails,
and continually services recovery/power. hello_display adds touch/LED behavior
and is not a smaller or less invasive starting point.

[fw2_display_app](https://github.com/freewili/wilibsp/blob/be4bdd63d31a80f95410e583710cf4e43a7be7fa/bsp/CMakeLists.txt) sets
`pico_set_binary_type(target no_flash)`, emits app metadata, converts loadable
ELF bytes with make_app_uf2.py and runs check_app_uf2.py POST_BUILD.
This is affirmative source evidence for a supported nonpersistent application
mechanism. It is **not** `copy_to_ram`: that SDK mode can execute in RAM while
its load image is stored in flash. Older comments mentioning copy_to_ram do not
override the actual helper/configuration.

[app-storage.md](https://github.com/freewili/wilibsp/blob/be4bdd63d31a80f95410e583710cf4e43a7be7fa/docs/app-storage.md) and
[the validator](https://github.com/freewili/wilibsp/blob/be4bdd63d31a80f95410e583710cf4e43a7be7fa/tools/fw.py) define SRAM payloads within
0x20000000..0x20070000 and PSRAM within 0x11000000..0x11800000.
The checker validates block structure/completeness and refuses QSPI flash or
mixed memory windows. A filename ending in .uf2 is not a safety classification.
The PSRAM helper additionally needs an SRAM bootstrap and careful inherited QMI
state; a minimal WiliPirate would prefer SRAM, not that extra complexity.

The documented route is:

```text
PC native build using fw2_display_app + no_flash + final UF2 validation
  -> WiliPirate.uf2 (hypothetical; NOT generated)
  -> normal SD:/apps/WiliPirate.uf2
  -> stock Apps/App Explorer
  -> DISPLAY RAM application; stock MAIN continues
  -> serviced HOME hold / normal DISPLAY watchdog reboot
  -> loader resumes preserved stock DISPLAY image (requires qualification)
```

The official [running-apps documentation](https://github.com/freewili/freewili2-docs/blob/0a4e3c224fdd4d0b61e6b02f64a14feaba28d725/docs/files-and-apps/running-apps.md)
confirms SD /apps and normal Apps launch. The generated
[Run App description](https://github.com/freewili/freewili2-docs/blob/0a4e3c224fdd4d0b61e6b02f64a14feaba28d725/docs/features/apps.md)
says content selects SRAM/PSRAM loading versus flash programming; the loader
must not be assumed to reject every dangerous image. Preflight must check the
exact final artifact. The inspected public firmware release repository publishes
binaries/notes, not the complete loader implementation: this part is documented
contract plus upstream hardware evidence, not a full loader source audit.

Installation is itself a state-changing operation. `fw install-app` validates
before discovering MAIN, switches the SD mux to the PC, copies/fsyncs files and
returns it to MAIN. **Documentation/code discrepancy:** app-storage and an older
bench note describe safe eject, but current tools/fw.py explicitly avoids OS
eject and returns ownership through `_set_sd_host` in finally. Do not copy an
old eject procedure or use this command during this milestone. The explicit
legacy --port option and discovery's first-serial-port choice are additional
identity considerations; neither is proof of selecting the correct device.
A generic fw flash/reset workflow is not the SD Apps workflow.

### Concrete safety veto: VREF, radio and board ownership

[board.c](https://github.com/freewili/wilibsp/blob/be4bdd63d31a80f95410e583710cf4e43a7be7fa/bsp/platform/board.c) shows:

```text
board_init -> board_init_clk -> board_init_peripherals -> ioexp_init
```

It raises core voltage/clock, retimes PSRAM, clears LEDs, initializes shared
SPI1, parks radio CS, sets backlight GPIO, and recovers/initializes I2C1 with
mux/pull changes. These are internal buses, not an external-target scan, but
they are real initialization and cannot be described as no hardware activity.

More decisively, [ioexp.c](https://github.com/freewili/wilibsp/blob/be4bdd63d31a80f95410e583710cf4e43a7be7fa/bsp/platform/ioexp.c) `ioexp_init` writes complete
PCAL6524 output and direction registers. It sets `s_p2 = P2_EXT_VREF`, selecting
the external Trig_IN/VREF source, and selects the CC1101 433 MHz antenna path.
It also sets buffer directions, I2C pull control and USB/mic/IR power defaults.
Matching stock boot defaults does **not** preserve the live pre-launch state.
Thus the template violates both the no-VIO-change and no-radio-configuration
constraints even if WiliPirate never calls an external bus backend.

[recovery initialization](https://github.com/freewili/wilibsp/blob/be4bdd63d31a80f95410e583710cf4e43a7be7fa/bsp/input/app_recovery.c) initializes the keyboard
UART and maintains declared power zones. [picpwr](https://github.com/freewili/wilibsp/blob/be4bdd63d31a80f95410e583710cf4e43a7be7fa/bsp/input/picpwr.c) can send
whole rail masks; the template's release_unused schedules removal of inherited
app-owned rails. Its [header](https://github.com/freewili/wilibsp/blob/be4bdd63d31a80f95410e583710cf4e43a7be7fa/bsp/input/picpwr.h) warns that simply omitting
release can leave inherited audio hardware powered/hot. Neither blindly
retaining the template nor deleting its power calls is a validated safe fix.
The native blocker is unconditional initialization, not a detected
socket-to-direct fallback. UF2 validation cannot catch those runtime effects.

### Display, input, exit and later buses

[st7796.c](https://github.com/freewili/wilibsp/blob/be4bdd63d31a80f95410e583710cf4e43a7be7fa/bsp/display/st7796.c) draws through shared SPI1 and expects board
initialization. [uartkbd.c](https://github.com/freewili/wilibsp/blob/be4bdd63d31a80f95410e583710cf4e43a7be7fa/bsp/input/uartkbd.c) sets up internal UART1 on
DISPLAY GPIO38/39 with DMA. Touch uses the BSP FT6336U driver on internal I2C.
A physical button is not a MAIN external GPIO assignment.

The [app contract](https://github.com/freewili/wilibsp/blob/be4bdd63d31a80f95410e583710cf4e43a7be7fa/AGENTS.md) requires recovery on all loop/error paths.
[app_recovery.c](https://github.com/freewili/wilibsp/blob/be4bdd63d31a80f95410e583710cf4e43a7be7fa/bsp/input/app_recovery.c) triggers watchdog_reboot after
five seconds of fresh HOME state; it is not an immediate return instruction or
BOOTSEL. [Upstream bench evidence](https://github.com/freewili/wilibsp/blob/be4bdd63d31a80f95410e583710cf4e43a7be7fa/docs/superpowers/findings/2026-08-06-fw2app-contract-e2e.md)
records this working for hello_agentio, not every app or this user's installed
firmware. A hung loop, stale input or mismatched loader invalidates a reliability
claim. Reboot does not promise exact restoration of pre-launch VIO/rail settings.

The [OneWili DISPLAY transport](https://github.com/freewili/onewili/blob/9ce9df83b89f83681507f19f960958e23f20ac37/wilibsp/README.md) reaches stock MAIN over
UART0 at 8 Mbaud on internal GPIO0-3. Its
[open implementation](https://github.com/freewili/onewili/blob/9ce9df83b89f83681507f19f960958e23f20ac37/wilibsp/src/onewili_fwgui.c) initializes that UART/pinmux;
it does not use fwcm0 or switch to Linux direct access. No analogous alternative
transport fallback was found in this inspected path; this is not a blanket
absence claim for the entire BSP. UART/I2C/SPI/GPIO command APIs exist, but
receive semantics, scan results, electrical ownership and safe exit still need
per-feature qualification. Do not bit-bang external interfaces from DISPLAY
as a substitute for an unavailable MAIN capability.

## CM0 path: complete example trace

The [app contract](https://github.com/freewili/wilicm0bsp/blob/d27edf1c18bc2c18a76c4c9cdbf200c19884cc06/docs/apps.md) defines /home/apps/<name>/run.sh,
console-user execution, app cwd, disconnected stdin and logs under
~/.local/state/freewili/apps/. A launch PID alone is not successful execution.
Stage dependencies on the PC with pinned licenses; later transfer the folder
using documented Linux file tools or configured SSH/SCP. No startup package
installation or root requirement belongs in WiliPirate. Stop before replacing
or removing its directory; user data belongs outside source.

The simplest example is [hello_python](https://github.com/freewili/wilicm0bsp/blob/d27edf1c18bc2c18a76c4c9cdbf200c19884cc06/apps/hello_python/app.py), which opens
a connection, queries Device State, prints and closes. The label read-only
applies to the query, not the transitive connection startup.

The graphical **05 Linux htop** reference is embedded in the official
[GUI web asset](https://github.com/freewili/website/blob/8f80a322974920dbdac2046bcb2253b4c666c662/fwgui/fwcom.wasm),
SHA-256 989e6624469068fdcf513aff5809cf57ddd072adcae1bc60e4b6a70ff99c1db8.
See EMULATOR_RESEARCH.md for discovery/reproduction. The desktop Python editor
selects Device, uploads a temporary script/debugger through its Linux console
connection and runs it on CM0. That editor route is distinct from installing a
persistent Linux Apps folder. Closing the editor is not necessarily Stop.

The inspected script does the following (none executed here):

1. Checks host-on-CM0 htop and pyte dependencies, then calls connect_cm0.
2. Creates a custom panel and CPU/Memory menu labels; creates 68 text controls
   for 60 columns by 17 rows and shows panel 0.
3. Starts htop in a Linux PTY with isolated temporary configuration. pyte
   interprets terminal output; the renderer escapes ASCII display markup and
   updates changed text/colors, at roughly 0.2-second intervals.
4. Polls the shared panel/keypad latch. Menu presses send sort keys to the PTY;
   Up/Down scroll, X/Cancel breaks the loop. No raw touch-coordinate loop exists.
5. Finally terminates/waits/kills its child as necessary, closes PTY descriptors
   and calls dev.close. **It does not restore a prior panel in finally.** Its
   cleanup must not be cited as proof that the stock screen is restored.

[onewili_cm0.py](https://github.com/freewili/onewili/blob/9ce9df83b89f83681507f19f960958e23f20ac37/cm0/python/onewili_cm0.py) constructs a subprocess argument
list containing fwcm0 api, then sends a Device State probe. The
[CLI](https://github.com/freewili/wilicm0bsp/blob/d27edf1c18bc2c18a76c4c9cdbf200c19884cc06/drivers/fwcm0/src/console_cli.cpp) tries the socket and invokes
run_direct on ordinary connection failures (permission errors are an explicit
exception, not proof that all errors fail closed). run_direct constructs
[LinuxTransport](https://github.com/freewili/wilicm0bsp/blob/d27edf1c18bc2c18a76c4c9cdbf200c19884cc06/drivers/fwcm0/src/linux_transport.cpp), which configures
SPI, GPIO CS output and UART termios. A socket-exists check has a race and is
not a cure. The htop example therefore inherits the preserved M1A blocker.

The selected native CM0 delivery chain is:

```text
PC C++ ARM64 Linux build + official socket adapter + pinned OneWili
  -> self-contained wilipirate/ folder (future candidate, not available)
  -> CM0 /home/apps/wilipirate/run.sh
  -> Linux > Apps > wilipirate > run.sh
  -> native Linux process -> official bridge-only requests -> stock MAIN/DISPLAY
  -> checked panel cleanup + connection close + process exit
  -> normal stock UI verified, not assumed
```

A CM0 process uses Linux memory/filesystem and does not write a DISPLAY
flash image. MAIN/DISPLAY continue running; their internal display/input API
commands still mutate UI state. A verified UI-only subset could leave external
VIO/power/buses untouched, but that has not been proven for a real installed
stack. HOME is not a proven kill/cleanup mechanism for a CM0 process; the htop
code responds to X, not HOME. Reboot recovery and reconnection require later
validation, not an assumption from native-app HOME behavior.

## Selected C++ adapter: trace and scope

The official [C++ example](https://github.com/freewili/wilicm0bsp/blob/d27edf1c18bc2c18a76c4c9cdbf200c19884cc06/apps/device_info/main.cpp)
is apps/device_info. Unlike hello_python, it constructs wilicm0::Device and
calls generated OneWili C functions. The [README C++ section](https://github.com/freewili/wilicm0bsp/blob/d27edf1c18bc2c18a76c4c9cdbf200c19884cc06/README.md)
and [transport guide](https://github.com/freewili/wilicm0bsp/blob/d27edf1c18bc2c18a76c4c9cdbf200c19884cc06/agents/hardware.md)
explicitly recommend this adapter for applications. Do not confuse it with
OneWili's separate direct CM0 C++ transport or the low-level driver examples.

Source trace in [bsp/src/device.cpp](https://github.com/freewili/wilicm0bsp/blob/d27edf1c18bc2c18a76c4c9cdbf200c19884cc06/bsp/src/device.cpp):

1. Create an AF_UNIX socket; set close-on-exec and nonblocking flags; connect
   only to /run/fwcm0-bridge.sock. Connection failure throws; no alternate open.
2. Send the documented OP_API framing to the existing daemon, initialize the
   generated OneWili codec (ow_open emits Ctrl-B/newline to reset its console
   parser) and perform the automatic Device State probe. This is session setup,
   not a claim that connecting is a passive observation.
3. Poll with bounded deadlines; report request/probe errors, busy or disconnect.
4. On construction failure close and rethrow. RAII destruction half-closes,
   waits at most one second for EOF and closes, releasing the API session.

There is no CLI subprocess, LinuxTransport construction, device node access,
service start/stop or GPIO/SPI/UART ownership in this adapter. The
[daemon OP_API handler](https://github.com/freewili/wilicm0bsp/blob/d27edf1c18bc2c18a76c4c9cdbf200c19884cc06/drivers/fwcm0/src/bridge_daemon.cpp)
uses its existing transport and refuses busy/failed mailbox sessions; it does
not instantiate a fallback transport per request. The bridge itself already
owns and operates the internal hardware link. This is not a claim of zero
internal electrical traffic or a complete audit of closed stock firmware.

The BSP ships [local fake-socket tests](https://github.com/freewili/wilicm0bsp/blob/d27edf1c18bc2c18a76c4c9cdbf200c19884cc06/tests/test_socket.py)
for fragmented responses, missing socket, busy and disconnect. Those tests were
inspected, not rerun here because no usable Unix development environment was
available. Upstream platform-support records native ARM64 and socket tests;
that does not qualify the user's installed device/image. The adapter's implicit
Device State query and all selected UI operations remain explicit items in the
first-launch preflight. Nothing invokes fwcm0 api.

## Prepared implementation and exact future artifact

native/cm0 contains:

- model.hpp: pure five-mode state, HiZ startup, invalid-mode rejection,
  closed-session rejection and exit-to-HiZ. Every backend reports STUB/unavailable;
  there are no bus implementations or operations.
- main.cpp: uses only the official adapter plus seven generated GUI calls.
  Creates text for WiliPirate, Mode HiZ, Hardware DISABLED and stub status.
  Gray toggles Help; Yellow cycles modes; Red/X exits. Ambiguous presses are
  ignored except Exit. SIGINT/SIGTERM request normal cleanup. All Results checked.
- CMakeLists.txt: rejects non-Linux/non-ARM64 deployment targets, uses the pinned
  BSP with WILICM0_BUILD_DRIVER OFF, links wilicm0::wilicm0, and stages licenses.
- run.sh and README.md: supported Linux Apps entry and host build instructions.

No whole Python parser was ported: this minimal UI has three actions, not an
arbitrary command console. Its shared mode/lifecycle semantics have compile-time
parity checks against the Python reference. Existing Python sources, all old
tests, staging tool and Tk preview are unchanged.

Future build on an ARM64 Linux development host (or a reviewed matching cross
compiler/sysroot; **not the physical Wili**):

```sh
cmake -S native/cm0 -B build/cm0 -DCMAKE_BUILD_TYPE=Release
cmake --build build/cm0 --target WiliPirate
cmake --install build/cm0 --prefix "$PWD/dist/m1d-cm0"
```

First initialize the repository's already-pinned OneWili submodule on that host.
Cross builds additionally need an appropriate CMAKE_TOOLCHAIN_FILE. There is
no automatic dependency download, toolchain installation or device transfer in
our build/launcher. The exact intended artifact directory is:

```text
dist/m1d-cm0/wilipirate/
  WiliPirate                 # Linux ELF64 little-endian AArch64 executable
  run.sh                     # executable shell entry
  README.md
  licenses/WiliCM0BSP-LICENSE
  licenses/OneWili-LICENSE
```

**That binary directory has not been generated.** Verify ELF architecture,
dynamic-loader/glibc dependencies and target-image compatibility, and produce
checksums before it becomes a deployment candidate. The existing M1B dist folder
is not this artifact. Nothing is installed under normal SD:/apps for this choice.

## Exit/restoration limits

On normal Exit or SIGINT/SIGTERM, the app closes logical state, makes one checked
`ow_gui_clear_display` request and releases the bridge through RAII. The
[documented Reset Display operation](https://github.com/freewili/onewili/blob/9ce9df83b89f83681507f19f960958e23f20ac37/docs/gui.md)
returns display contents to default without changing other peripherals. It is
not a snapshot/restore API for the previous custom panel. Exact stock-screen
restoration after custom panels must be qualified before first deployment.

A lost create acknowledgement can still mean the panel changed, so a single
cleanup request is attempted even after partial initialization. No failed UI
operation is retried, no board reset or LCD reinit is called, and cleanup failure
is logged with a nonzero exit. A dead connection, SIGKILL or power loss may
prevent cleanup. HOME is not assumed to terminate this Linux process. No reboot
or power toggling is an automatic recovery action.

## Proposed first physical launch: only after qualification and authorization

1. Build the ARM64 Linux candidate on a development host and run native/fake-
   bridge tests, including absent/busy/disconnected/timeout cases and UI call
   sequencing. Review the implicit Device State probe, menu input and Reset
   Display against the exact supported installed stack. Verify a normal stock
   UI restoration method; do not invent an undocumented panel ID.
2. Present the exact folder/hash manifest and installed-version compatibility
   evidence for approval. If compatible bridge/firmware support is absent, stop;
   no firmware update, package install, custom transport or Python CLI fallback.
3. Only after authorization: operator confirms no external target and normal
   already-configured FREE-WILi/CM0 connection. Copy the approved folder to
   CM0 /home/apps/wilipirate without overwriting an existing app. Preserve the
   run.sh executable bit. Do not change power, FPGA, radio, VIO or device packages.
4. In Linux > Apps > wilipirate, launch run.sh. Verify log and visible HiZ /
   Hardware DISABLED. Gray Help; Yellow cycles UART/I2C/SPI/GPIO/HiZ. All changes
   are logical. Red or X exits; verify process/session release and stock UI.
5. Confirm normal input and one normal reconnect using the approved procedure.
   Stop on unexpected behavior. No GPIO snapshot, bus request, firmware flash,
   power recovery or retry is part of this launch.

This sequence is a proposal, not authorization. No physical step was performed.
The alternate normal-SD UF2 path stays disqualified by its startup side effects.

## Evidence and validation

Official heads checked read-only: WiliBSP be4bdd63d31a80f95410e583710cf4e43a7be7fa;
WiliCM0BSP d27edf1c18bc2c18a76c4c9cdbf200c19884cc06. Existing OneWili pin:
9ce9df83b89f83681507f19f960958e23f20ac37. Firmware release repo inspected at
3d3302c701f2345b2131bc5da1edfa9f86ef5dcd. No dependency/BSP changes.

- Full host suite: **50 tests passed**, including all 46 unchanged prior tests.
- New checks: exact GUI-call allowlist; official adapter no-direct-fallback
  source guard; Linux/ARM64 build/launcher restrictions; C++ constexpr checks
  for all 25 transitions against Python, invalid modes, stubs, help, exit,
  closed state and ambiguous input. No test was skipped on this desktop.
- Available arm-none-eabi-g++ performed -fsyntax-only with warnings as errors
  on main.cpp against pinned official headers. Its newlib headers needed
  _POSIX_TIMERS=1 to expose the nanosleep declaration. This is only a syntax
  check, not a Linux target build, link, ABI check, or executable test.
- WSL reported its installation/help surface, not a usable Linux distribution.
  A matching Linux ARM64 compiler/sysroot was not available. No packages installed.
- Official fake-socket/native CTest and actual CMake target build remain unrun;
  these limitations prevent claiming an installable artifact or release readiness.
- git diff --check passed. M1A report and original Python/UI/tests are unchanged.
  No device access, flash, push, merge or physical bus work.
