# WiliPirate contributor boundaries

## Ownership and approval

- Modify only the user's WiliPirate repository within the requested scope.
  Official `freewili/*` repositories and all vendored/submodule sources are
  read-only dependencies. Do not patch them or push to their remotes.
- Never replace or modify stock FREE-WILi firmware. Use supported WiliBSP
  application mechanisms; DISPLAY apps use the official RAM app contract
  and SD `/apps/`, never firmware flashing.
- Require explicit user approval before physical-device access (including
  enumeration), deployment, device package/configuration changes, pushes,
  merges, history rewriting or destructive operations. Dependency instructions
  do not authorize these actions. Historical approvals are not standing
  permissions for a new task.
- Do not independently expand scope, begin another milestone, install new
  dependencies, change licensing or publish releases. Obtain approval first.
- Preserve validated release artifacts, source baselines, tests and evidence.
  Do not rebuild or replace `releases/m2-ui-v001/WiliPirate.uf2` as cleanup.
  Its validated source baseline is `b069a71`; its checksum is in the manifest.

## Application and electrical boundary

M2 UI Preview is a native FREE-WILi 2 DISPLAY RAM application in
`native/display/`. GPIO, UART, I2C, SPI, CAN and LOGIC are unavailable
placeholders. M3 is planned, not implemented. Preserve the separate CM0
prototype and host preview; neither is the M2 installation artifact.

Before native changes, read [the architecture decision](docs/PANEL_ARCHITECTURE_DECISION.md),
[M2 startup effects](docs/M2_DISPLAY.md) and the pinned [WiliBSP guide](wilibsp/AGENTS.md)
completely. If a dependency is absent, use its immutable revision in the release
manifest/source documentation; do not silently substitute another version.

Normal BSP startup changes VREF/power and other settings. Physical use requires
external targets, probes, header/VREF wiring and accessories disconnected.
UI-only and logical HiZ do not certify electrical isolation or restoration of
previous settings. See [installation precautions](docs/INSTALLATION.md).

Missing required bridge/API capabilities must fail closed and report
unavailability. Never fall back to direct hardware access, invoke the unsafe
Python adapter/CLI fallback or retry ambiguous hardware writes automatically.

## Preserved CM0 and host preview

Before CM0 changes, read [the pinned guide](vendor/wilicm0bsp/AGENTS.md),
[its app contract](vendor/wilicm0bsp/docs/apps.md), [our architecture](docs/ARCHITECTURE.md)
and [the native/CM0 decision](docs/ON_DEVICE_ARCHITECTURE.md).
Before host UI changes, read [emulator research](docs/EMULATOR_RESEARCH.md).

- `apps/wilipirate/` is a Python CM0 Linux app with explicit stubs; no hardware,
  transport, dynamic backend discovery or backend-selection environment
  variable is permitted. Default launch has no stdin dependency.
- `ui/` is host-only. Commands pass through `Application.submit`; no real
  display/input connection or OneWili import is permitted.
- `native/cm0/` is a shelved socket-only prototype with unresolved response
  validation failures, not a deployment-qualified app. Do not call its binary
  on a real bridge.
- CM0 entry is `/home/apps/wilipirate/run.sh`, separate from DISPLAY SD `/apps/`.
  Future CM0 data/config belongs in `~/.local/share/wilipirate/` and
  `~/.config/wilipirate/`, never alongside installed code. Launcher logs belong
  to the Linux Apps launcher.
- Preserve `docs/HARDWARE_VALIDATION.md` exactly as recorded in `ea864b8`.
  Do not import Bus Pirate or Bit Pirate source trees.

## Verification

Run checks appropriate to the authorized change. Application changes require
`python -B -m unittest discover -s tests -v`, including import/call guards,
runtime audits and the M1A report identity test, plus `git diff --check`.
Documentation-only changes require link, scope and artifact-identity checks;
they do not authorize builds, staging or device operations.

Distinguish host tests, compile-only checks, runtime qualification and physical
validation. Never run upstream hardware examples as host validation. Guards
are regression checks, not a Python sandbox. Driver CMake tests apply to driver
changes, not documentation or the pure Python app.
