# M3 diagnostic checkpoint: build unfinished

Recorded 2026-10-08 on `feature/wilipirate-panel`, starting from `68e3d35`.
The user stopped all builds/troubleshooting and requested a local checkpoint.
This record describes completed work only; it is not authorization to resume.

## Saved work

- Separate diagnostic sources and standalone CMake project under
  `native/i2c_diag/`, with build/safety/physical-test instructions in its README.
- Raw capture and envelope analysis, manual one-attempt gating, short receive
  reads, bounded byte/timing storage, loss reporting and recovery service.
- Synthetic C capture tests and Python static boundary tests.
- Phase 1/2 source findings in `docs/M3_I2C_STOCK_API.md`.

## Actual completed checks

- Native capture core compiled with Zig 0.14.1 / Clang 19.1.7; its 14 synthetic
  capture cases passed. These fixtures do not establish a stock Poll schema.
- Full Python 3.12 host regression suite: **55 tests passed**, including existing
  application, UI, import/call guards, runtime audits and M1A identity checks,
  plus three new diagnostic static checks.
- Official SDK pioasm was built and installed only into the ignored workspace
  prerequisite directory. Its version configuration was corrected to 2.3.0.
- No diagnostic target compilation, ELF/UF2 generation, SRAM/metadata check,
  native integration runtime qualification or physical test was completed.

## Unfinished picotool build

SDK-matched official picotool 2.3.0 source was obtained at
`6f6458d792b93685a11423b244a585eaa99eafcf`, with USB disabled by
`PICOTOOL_NO_LIBUSB=ON`. Its host build in `build/m3-picotool/` is incomplete.

The first build could not find host compilers for its helper that copies the
official precompiled embedded-data ELF. After explicit host compiler variables
were supplied, that helper configured. The next failure was static-library
archiving: generated build rules invoked `CMAKE_AR-NOTFOUND`. Reconfiguration
with workspace archiver wrappers did not resolve those generated rules.

A proposed fresh picotool build directory was never configured: its command
was rejected before execution. The user then stopped troubleshooting. At the
stop checkpoint there was no `build/m3-picotool-final/`, no picotool executable,
and no diagnostic UF2. Do not describe this checkpoint as a successful target
build or deployment-ready diagnostic.

## Local prerequisite/evidence preservation

Ignored `build/m3-prerequisites/`, `build/m3-host/`, `build/m3-pioasm/` and
`build/m3-picotool/` retain archives, portable CMake 3.31.6/Ninja 1.12.1/Zig
0.14.1, unmodified pinned source copies, compiler wrappers, caches and partial
outputs. Zig's archive SHA-256 was verified against its official release index.
These ignored files are not part of the Git checkpoint or a public release.
The installed Arm compiler reports GNU Toolchain 15.2.Rel1; it has not yet been
used to compile this diagnostic target. No system package was installed.

The SDK copy was populated with its exact TinyUSB gitlink sources. Windows
could not create two documentation symlinks; extraction was repeated excluding
documentation. No dependency build source was patched, and the tracked official
submodule paths/pins were not changed. Existing third-party redistribution
questions remain unresolved.

## Safety and stop state

M2 source and `releases/m2-ui-v001/` remain preserved. Its UF2 SHA-256 remains
`6bc0a08050c1e18659882bc486a1f03b6e16529916b60112240f548aeb1e6672`.
No device enumeration, connection, installation, launch, firmware change or
hardware test occurred. No complete raw physical Poll response has been captured.

Process inspection after the stop request found no remaining workspace build
processes. Do not restart builds, add tooling, fix sources or expand the task
without a new user request. Physical deployment requires separate explicit
approval after offline target validation and safety review. This checkpoint
is local only; no push or GitHub Release is authorized.
