# WiliPirate Field Analyzer UI

v003 adds an inert Tools Glitching pane/guide and a simulated CAN workflow with
bounded result decoding and volatile RAM capture/review. No live transport or
hardware settings are available. [CAN findings](../../docs/V003_CAN_INVESTIGATION.md)
document the qualification blockers. v002 physical UI/HOME passed by user
report; [rollback identity](../../docs/V002_DEVICE_VALIDATION.md) is preserved.
The existing startup exception is not reused for a v003 UF2 build.

Pure navigation/rendering plus a guarded native DISPLAY adapter. All instruments
are unavailable placeholders. Guides are constant documented facts/explicit
gaps, not configuration workflows. No OneWili, storage, capture, AI or output
backend is linked or called. See [the charter](../../docs/PROJECT_CHARTER.md).

## Host validation

Use an existing native C compiler, CMake/Ninja, and unmodified WiliBSP at
`be4bdd63d31a80f95410e583710cf4e43a7be7fa` for the font source. No device or SDK
toolchain is required for these host targets. Example from the repository root:

```sh
cmake -S . -B build/field-host -G Ninja \
  -DWILIBSP_PATH=/path/to/pinned/wilibsp -DCMAKE_BUILD_TYPE=Debug
cmake --build build/field-host --parallel 2
ctest --test-dir build/field-host --output-on-failure
mkdir -p build/field-preview
build/field-host/field_render_test build/field-preview
python -B -m unittest discover -s tests -v
git diff --check
```

Windows executables have `.exe`. Quote `-D` arguments containing spaces. This
session reused cached portable CMake 3.31.6/Ninja 1.12.1/Zig 0.14.1 and the
unmodified pinned BSP font; no compiler/environment was installed or migrated.
Host render output is PPM, with PNG review copies under ignored
`build/field-preview/`. It is not a device screenshot or simulator execution.

## Native candidate gate

Root `FIELD_BUILD_DEVICE` defaults OFF. Configuring it ON without explicit
`FIELD_STANDARD_STARTUP_APPROVED` must fail before SDK setup. The required BSP
startup changes VREF/internal GPIO/power and conflicts with a no-change native
launch invariant. A conditional exception was granted only for the recorded
offline build, never physical execution. Future builds require their applicable
approval; do not enable the exception or claim electrical isolation merely to
get another UF2. Source defaults and the validated build cache are now OFF.

If the user explicitly approves standard startup for an offline candidate,
use the already-pinned Pico SDK 2.3.0, Arm toolchain, official pioasm/picotool
packages and a **new** `build/field-device/` directory. Enable both gate flags
explicitly; disable dependency fetching. The target is `WiliPirate`, producing
`build/field-device/WiliPirate.uf2` to replace SD `/apps/WiliPirate.uf2`,
metadata WiliPirate v002, SRAM/no_flash, only DISPLAY metadata and no AgentIO or
stdio. It does not build M2 or the retired diagnostic. Even after such a build,
physical install/launch needs separate approval. No active installation command
or physical test is authorized by this milestone.

This is the existing WiliPirate application's replacement, not a second launcher.
See [startup audit](../../docs/FIELD_STARTUP_AUDIT.md) and
[controlled replacement plan](../../docs/FIELD_REPLACEMENT_PLAN.md). Historical
M2 rollback bytes must remain untouched; no new device UF2 exists while the
startup exception is awaiting approval for a future build.

The 2026-10-09 conditional offline-only exception produced the current
`build/field-device/WiliPirate.uf2` candidate. The official helper's output is
generated/validated beside its target under `native/field_analyzer/` in the
build tree; a post-build local byte-identical copy exposes the canonical path.
The local build cache was reset to approval OFF after validation. No exception
is applied globally or to physical execution. See the latest validation record
for hash/size and the still-closed deployment gate.

## Navigation

Touch primary tiles: Scope/Logic/Generator -> Connection Guide -> unavailable
preview. Protocol Analyzer -> CAN/UART/I2C/SPI -> guide -> unavailable preview.
Preview's REVIEW CONNECTION GUIDE returns to the same guide. Back retraces
those levels; on-screen HOME returns to the app home. Tools/Captures/Settings
are navigable placeholders. Native physical CANCEL also invokes Back; long
physical HOME uses official recovery, with About and missing-touch behavior.
Those device paths passed for v002 by user report; v003 remains host-tested only.

No guide requires a connection to continue. The font remains the unmodified
BSP bitmap font, so protocol labels use ASCII `I2C` and separators rather than
unsupported Unicode glyphs. Instrument titles use larger type; all guide and
placeholder content is checked within the actual 480x320 dimensions.

Historical source/artifacts and physical SD files are preserved. Review
[offline results](../../docs/FIELD_ANALYZER_VALIDATION.md) and
[source evidence](../../docs/FIELD_ANALYZER_RESEARCH.md) before any next step.
