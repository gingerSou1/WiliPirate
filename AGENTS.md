# WiliPirate development

For native DISPLAY work, read [wilibsp/AGENTS.md](wilibsp/AGENTS.md) completely.
If output is truncated, continue in chunks through EOF before changing code.

## Current architecture pivot (2026-10-07)

Work on `feature/wilipirate-panel` follows
[the preservation decision](docs/PANEL_ARCHITECTURE_DECISION.md) and
[the current FW2 reuse research](docs/PANEL_REUSE_MATRIX.md).
`archive/cm0-prototype` preserves the exact CM0/M1E checkpoint at bc738b8.
The CM0 and host-preview rules below continue to govern their preserved trees;
they do not select the runtime for a future native FW2 panel.
M2 is explicitly authorized as a UI-only native DISPLAY RAM application under
`native/display/`, using the pinned unmodified WiliBSP at `wilibsp/`.
Standard board/expander/display/touch/recovery initialization and its normal
VREF/power side effects are allowed; document them and require disconnected
external targets for the eventual first physical test. This supersedes the
earlier unchanged-VIO launch gate for M2. Do not add interface operations,
OneWili connections or extra VIO/radio/power setup. The stock I2C response gate
remains a prerequisite for M3, not for this UI-only milestone.
Preserve CM0/M1B/M1C code, tests, dependencies and evidence. Do not fix the M1E
parser, modify upstream sources or create a CM0 deployment package.
Local source/documentation commits and offline builds/tests are authorized.
No physical device work, persistent firmware/platform changes, direct flashing,
deployment, pushes or merges are allowed without explicit approval.

Read [the pinned WiliCM0BSP guide](vendor/wilicm0bsp/AGENTS.md), its
[app contract](vendor/wilicm0bsp/docs/apps.md), and
[our architecture](docs/ARCHITECTURE.md) before changing the app.
If the submodule is absent, the immutable source links in docs/ARCHITECTURE.md
and docs/SOURCES.json identify the same upstream revision.

- The preserved `apps/wilipirate/` tree is a Python CM0 Linux app, not
  MAIN/DISPLAY/ESP32 replacement firmware. M2 uses a separate DISPLAY RAM app.
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
WiliBSP template changes VREF/radio on startup. It was not approved for M1D;
the separate M2 UI explicitly permits normal BSP initialization.
Run the full host suite; distinguish compile-only checks from Linux runtime
qualification. Do not call the prepared binary on any real bridge.
