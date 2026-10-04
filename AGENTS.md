# WiliPirate development

Read [the pinned WiliCM0BSP guide](vendor/wilicm0bsp/AGENTS.md), its
[app contract](vendor/wilicm0bsp/docs/apps.md), and
[our architecture](docs/ARCHITECTURE.md) before changing the app.
If the submodule is absent, the immutable source links in docs/ARCHITECTURE.md
and docs/SOURCES.json identify the same upstream revision.

- This is a Python CM0 Linux app, not MAIN/DISPLAY/ESP32 replacement firmware.
- Entry: apps/wilipirate/run.sh; staged runtime: /home/apps/wilipirate/run.sh.
- M1B uses logical mode state and explicit stub backends only. No hardware,
  OneWili connection, power zone, pins or persistent data.
  HiZ means no operations by this app, not electrically measured isolation.
- Default launch has no stdin dependency. Logs belong to the Linux Apps launcher.
  Future app data/config belongs in ~/.local/share/wilipirate/ and
  ~/.config/wilipirate/; never write it alongside installed source.
- M1C development is authorized on feature/wili-ui; preserve feature/wilipirate. No push,
  merge, remote creation, hardware testing, flashing, device package installs,
  BSP modifications or Milestone 2 without explicit user approval.
- Do not edit submodule sources or import Bus Pirate/Bit Pirate source trees.
- Local checks: python -B -m unittest discover -s tests -v; git diff --check.
  Stage locally: python -B tools/stage.py --output dist/apps.
- BSP driver CMake tests apply when changing that driver, not this pure Python
  application. Never run upstream hardware examples as host validation.

## M1B safety boundary

WiliPirate must never silently fall back from the supported bridge to direct
hardware access. If a required bridge/API is unavailable, fail closed and
report the unavailable capability. Do not work around fwcm0 api's direct
hardware fallback. M1A and all physical validation remain deferred.

Preserve docs/HARDWARE_VALIDATION.md exactly as recorded in ea864b8; it is the
M1A evidence, not a progress checklist to overwrite. No device enumeration or
connection is needed for M1B. UI research is documentation-only.

Runtime code lives in apps/wilipirate/wilipirate/: console -> application/parser
-> state -> backend interface -> explicit stubs. No transport implementation,
dynamic backend discovery or backend-selection environment variable is permitted.
Run the complete tests, including import/call guards, runtime audit and the
M1A report identity test. These are regression checks, not a Python sandbox.

## M1C host UI boundary

Read docs/EMULATOR_RESEARCH.md before UI changes. The official simulator does
not execute CM0 Python. ui/ is a host-only panel preview, separate from the
unchanged apps/wilipirate core and staging artifact. No real display/input
connection, OneWili import or transport is permitted. All commands must pass
through Application.submit. Preserve all 32 M1B tests and the M1A report.

## M1D preparation

Read docs/ON_DEVICE_ARCHITECTURE.md for the native-vs-CM0 decision. Source-only
native/cm0 uses the official WiliCM0BSP C++ socket adapter; it never calls the
unsafe Python adapter/CLI. This explicitly authorized preparation is separate
from unchanged M1B/M1C code. No physical deployment or execution. The native
WiliBSP template changes VREF/radio on startup and is not approved for use.
Run the full host suite; distinguish compile-only checks from Linux runtime
qualification. Do not call the prepared binary on any real bridge.
