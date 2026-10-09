# M3 diagnostic checkpoint: build unfinished

**Current status (2026-10-09): offline diagnostic build/validation passed.**
The stopped checkpoint below remains the historical record. Its stop condition
was superseded by the user's explicit offline resume request; physical access
and deployment remain unauthorized. See the resume results at the end.

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

## 2026-10-09 resume: offline target build passed

Recovered a clean working tree at local checkpoint
`f0749ea4092dbb4b366fa3e8666df11daedb6aa6` on `feature/wilipirate-panel`.
No prior `docs/M3_SESSION_CHECKPOINT.md` existed. Reused yesterday's archives,
compilers, successful pioasm installation, synthetic test binary and diagnostic
sources. No dependency download, environment migration or source rewrite was
needed. WSL fallback was unnecessary.

The old CMake cache contained the archiver wrapper path, but generated
`CMakeCXXCompiler.cmake` still set `CMAKE_AR-NOTFOUND`. A fresh
`build/m3-picotool-final/` configuration specified `CMAKE_AR:FILEPATH` and
`CMAKE_RANLIB:FILEPATH` before compiler detection. This resolved the blocker
within approximately two minutes of resuming. Official picotool 2.3.0 was
built/installed into the workspace only, still with USB support disabled.
The successful pioasm prerequisite was not rebuilt.

The separate standalone project then compiled successfully in `build/m3-diag/`:

| Measurement | Result |
| --- | --- |
| Artifact | `build/m3-diag/WiliPirateI2CDiag.uf2` |
| SHA-256 | `95719e2cec6415dc974297973d5938f192a2a431c18f8899063d2f385e804a0e` |
| File size | 66,560 bytes |
| UF2 blocks | 130 |
| Payload bytes | 33,164 |
| Payload range | `0x20000000` to exclusive end `0x2000818c` |
| Official target/metadata check | SRAM; WiliPirateI2CDiag v001; expected description |
| Processor/build | FREE-WILi 2 DISPLAY RP2350B / Cortex-M33, `no_flash`, MinSizeRel |
| Toolchain | Arm GNU Toolchain 15.2.Rel1 / GCC 15.2.1 |
| SDK | Pinned Pico SDK 2.3.0 with its pinned TinyUSB sources |
| Host utilities | CMake 3.31.6, Ninja 1.12.1, Zig 0.14.1 / Clang 19.1.7, Python 3.12 |

Only the diagnostic target was built. The root M2 CMake project was not used.
The official validator passed during the build and independently afterward.
An additional block audit checked magic, numbering, payload lengths and SRAM
bounds for every block; no payload targets QSPI flash or PSRAM. ELF inspection
found a single file-bearing LOAD segment in SRAM, entry `0x20000179`, plus two
zero-file-size scratch-SRAM stack segments. Its linked metadata declares only
DISPLAY; AgentIO and USB/UART stdio are disabled.

The application compiled with `-Werror` for its own sources. Existing upstream
PIO-USB inline/noinline warnings and two host picotool warnings were left intact;
no upstream patch was made. Byte comparison against the original archives
confirmed all 2,275 regular files of the cached BSP, OneWili and base SDK copies
were unchanged. TinyUSB was the already-prepared SDK gitlink source copy.

## Resume validation and limits

- Python suite: **55/55 passed**; import/call guards, runtime audits, M1A identity
  and three diagnostic boundary checks passed.
- Reused host binary: **14 synthetic capture cases passed**. This exercises
  fragmented/multiline/empty envelopes, wrong paths, malformed identities,
  duplicate responses, firmware refusal, incomplete data, buffer/timing limits,
  a body larger than 4 KiB, clock wrap and consumed attempts. No fixture claims
  to describe the actual Poll address schema.
- Static checks confirm one stock Poll call site and consumed manual-attempt
  gating, bounded reads/deadline, recovery-aware opening and send timer, and no
  target GPIO, bus configuration, CAN, rail or VREF setters in the app.
- Linked recovery functions and the raw-send entry are present; target CAN,
  `ow_io_*`, VREF setter, rail-release and USB initialization symbols were not
  found. Internal GPIO/I2C symbols are present for standard board/display setup.
- Source review confirms every retained byte is accessible through raw pages
  at eight bytes per row, ten rows per page, with page count rounded up for
  the final partial page. Exact hex, offsets and byte count enable complete
  reconstruction; metadata/chunk records also have paging. This is manual LCD
  retrieval, not an automatic file export or a truncated first-screen preview.
- M2 source/artifact and M1A evidence are unchanged. M2 SHA-256 remains
  `6bc0a08050c1e18659882bc486a1f03b6e16529916b60112240f548aeb1e6672`.

Build/configure/test logs and block-audit measurements are retained in ignored
`build/m3-configure.log`, `build/m3-build.log`, `build/m3-regression.log` and
`build/m3-validation.json`. Source/runtime HOME paths were reviewed and compiled;
physical HOME behavior, blocked-write recovery, installed firmware compatibility
and actual Poll results remain unverified. The five-second receive window does
not bound opening, UART submission or MAIN's hardware operation. No new retry,
power workaround or address decoder was added.

See [the manual empty-bus procedure](MANUAL_TEST.md). No physical device was
enumerated, accessed, installed or launched. No commit, push or merge was made
during the resume. Stop after offline validation for deployment review.
