# Field Analyzer offline validation

Recorded 2026-10-09. Branch: `feature/wilipirate-panel`; HEAD remains
`8057bf917b811c54e757a4600fd88945cb5634fc`. Work is uncommitted.

## Completed

- Host CMake configure/build passed with Clang 19.1.7 via cached Zig 0.14.1.
  Production model/renderer compile with `-Wall -Wextra -Werror`.
- CTest **2/2 passed**: all primary/protocol/guide/preview/secondary transitions,
  Back/Home, held touch, coordinate boundaries, complete clears, missing-touch
  fallback and rendering bounds for the 480x320 display.
- Production host renderer generated **21 PPM/PNG previews**. Home, protocol
  selector and CAN guide PNGs were visually inspected. Preview generation uses
  the original BSP font and a software pixel sink, not a device or emulator.
- Python suite **59 tests passed**, including all existing 55 regressions and
  four Field UI boundaries/archive identity checks. Import/call/runtime guards
  and M1A report identity remain intact. The M2 root-build assertion now reads
  its archived original root CMake; the original test is preserved in archive.
- Archive manifest checks preserve 29 byte-identical files, including the
  pre-pivot root README/installation/architecture and uncommitted M3 analysis.
- Original M2 source and release UF2 remain unchanged; SHA-256
  `6bc0a08050c1e18659882bc486a1f03b6e16529916b60112240f548aeb1e6672`.
- Retired M3 source/capture tests and existing local UF2 are preserved; SHA-256
  `95719e2cec6415dc974297973d5938f192a2a431c18f8899063d2f385e804a0e`.
- Documentation, whitespace, protected-source/artifact and archive checks were
  completed. No physical SD or installed diagnostic file was accessed.

## Native startup gate and limits

The new native adapter is prepared for the official SD-loaded DISPLAY contract,
but standard startup changes VREF/internal GPIO/power. A no-change startup is
not established. Device configuration is fail-closed unless an explicit
standard-startup exception is approved; host validation is the safe default.
No new device UF2 or physical qualification is claimed without that approval.
An actual configure attempt with the device flag ON and startup approval OFF
was rejected by the intended gate before SDK setup. No native Field UF2 was
produced in this milestone while that question remained unanswered.
Host results do not establish touch/HOME/device behavior or electrical limits.

No instrument driver/API call, ADC sampling, logic capture, CAN receive/TX,
I2C Poll, GPIO/PWM/output operation, storage/export, AI client or stock-panel
shortcut is implemented. Exact connector orientation, CAN-H/L/DB-15 wiring,
input/output protection and capture interoperability remain unqualified.
Source research is version-addressed documentary evidence, not runtime proof.

The public GUI package was inspected only as ZIP data; its AI labels and
Workbench project-type assets were not executed. Legacy inaccessible docs
were not used to invent facts. Third-party redistribution questions remain.

## Stop and review

Review the UI/guide wording and resolve the standard-startup boundary first.
The next single functional milestone is controlled CAN receive qualification
after connector/electrical/listen-only evidence and explicit authorization.
Do not automatically begin hardware testing or add an output/capture feature.
No commit, push, merge, release, deployment, stock firmware change or physical
device operation occurred during the pivot.

## Startup-resolution follow-up

The [startup audit](FIELD_STARTUP_AUDIT.md) confirms target-facing external-VREF
selection and bulk expander defaults on all inspected supported board paths.
No no-change startup was established; outcome C requires a fresh explicit
exception. The exception/gate remains unapproved and OFF. No native build was
run, no Field Analyzer UF2 was generated, and no new size/hash is available.

The native project/target identity was corrected to **WiliPirate**, future file
`build/field-device/WiliPirate.uf2`, replacing the existing SD pathname. UI,
navigation, guide data and native runtime source were not redesigned/changed.
An additional static identity regression passed. Full Python results are now
**60/60 passed**; cached native host CTest remains **2/2 passed**. No native
compilation or physical qualification is inferred from those host checks.

The [replacement plan](FIELD_REPLACEMENT_PLAN.md) requires backing up and hashing
the actual installed old file on the host before replacement, reading back the
new bytes during official handoff, and returning ownership to MAIN. It is a
future approved procedure; no wrapper, SD operation or diagnostic cleanup was
executed. M2 rollback/source/archive identities remain unchanged. Stop for the
separate startup exception before an offline replacement build, then obtain
separate physical installation/launch approval after candidate validation.

## Conditional offline build result

On 2026-10-09 the user explicitly approved a narrowly scoped offline-only
replacement build using the documented M2-style standard startup. This is
**not permission to execute it physically**. Device/SD access, installation,
launch, target electrical changes, diagnostic removal and Git publication
remain unauthorized. The physical-deployment gate stays CLOSED.

The approved build used the existing root gate explicitly in its isolated
`build/field-device/` cache, with dependency fetching disabled and cached pinned
BSP/SDK/host utilities. No global default or safety gate was disabled. After
validation, the cache approval was reset OFF; its unapproved reconfiguration
correctly failed before SDK setup. The already-validated UF2 remains unchanged.

| Measurement | Result |
| --- | --- |
| Canonical candidate | `build/field-device/WiliPirate.uf2` |
| Identity | WiliPirate v002; Field Analyzer navigation preview |
| Size | **61,440 bytes** |
| SHA-256 | `53ca890ce882f8cc088657abd8f7cc4482292e150e5544a60044604e9a3c9a2f` |
| UF2 blocks / payload | 120 / 30,504 bytes |
| Payload window | SRAM `0x20000000` to exclusive end `0x20007728` |
| Official helper/validator | `no_flash`, SRAM target, valid WiliPirate v002 metadata |
| Platform | FREE-WILi 2 DISPLAY RP2350B, Cortex-M33 / `rp2350-arm-s` |
| Compiler | Existing Arm GNU Toolchain 15.2.Rel1 / GCC 15.2.1 |
| Build/SDK | MinSizeRel, Pico SDK 2.3.0, pinned unmodified BSP |
| Power metadata | DISPLAY only, mask `0x00002` |
| Physical startup / deployment | **Not approved; not executed** |

The official helper generates the validated UF2 beside the target in
`build/field-device/native/field_analyzer/`. A local post-build copy exposes
the canonical candidate above, verified byte-identical. It does not touch the
historical release or any SD file, and does not create another app identity.

### Offline integrity and behavior checks

- Independent audit of every UF2 block verified magic, numbering, sizes and
  SRAM-only addresses. No flash/mixed/PSRAM firmware payload exists.
- ELF entry is `0x20000179`. Its single file-bearing LOAD is SRAM at
  `0x20000000`, file size `0x7728`; runtime extent is `0x994c`. Two scratch-SRAM
  stack LOADs have zero file bytes. No persistent stock firmware segment.
- Main disassembly calls pure `fa_init`, then `board_init`, recovery, LCD,
  touch, About restore, render/backlight, recovery/input/touch loop. This
  matches the documented exception; no new hardware configuration step.
- The linked preinit registry includes the audited SDK boot-ROM/runtime state,
  early/post-clock resets, clock setup, USB-power-down and PSRAM callbacks,
  plus standard alarm/lock/mutex/TLS/IRQ housekeeping. No custom startup
  override was added. Conditional runtime effects are not physically measured.
- No OneWili, `ow_io_*`, instrument/ADC-read/CAN-init, VREF-setter, rail-release,
  radio-init or USB-init entry points were found. Internal board/display/touch
  GPIO/SPI/I2C and normal DISPLAY power helpers are present as approved.
- Compared with M2, the intentional startup difference remains omission of
  `picpwr_release_unused`. Field makes no extra rail request or target control
  call. This is software-path equivalence, not identical binary timing: M2's
  historical build used GCC 14.2.1, this build uses cached GCC 15.2.1.

### Portability fix and regressions

Arm GCC rejected an enum `>= 0` range check that host Clang accepted under
`-Werror`. Replaced it with an equivalent unsigned upper-bound test and added
a negative-enum fallback assertion. Valid UI/navigation/guide behavior did
not change; `-Werror` was retained. Application/runtime UI was not redesigned.

Final Python suite: **60/60 passed**. Rebuilt native host CTest: **2/2 passed**.
All **21 production-rendered PPM previews are byte-identical** to the previously
reviewed UI after the fix. Missing-touch, page bounds, Back/Home, held contact,
guides and placeholders remain covered. Existing upstream PIO-USB attribute
warnings were not patched and do not originate in the app's own code.

M2 rollback remains byte-identical, SHA-256
`6bc0a08050c1e18659882bc486a1f03b6e16529916b60112240f548aeb1e6672`.
M3 diagnostic artifact remains
`95719e2cec6415dc974297973d5938f192a2a431c18f8899063d2f385e804a0e`.
The 29-file archive checksum manifest passes; no archived source/evidence or
official dependency was patched. Installed applications were not accessed.

Logs: ignored `build/field-configure.log`, `build/field-build.log`,
`build/field-build-final.log`; measurements/source hashes/closed physical
permission status: `build/field-validation.json`. Source remains uncommitted
at HEAD `8057bf9`; generated outputs remain ignored.

### Remaining electrical questions and stop

Compilation proves neither actual VREF levels nor transient behavior. Expander
acknowledgement/state, unnamed buffer-net exposure, arbitrary prior power/sleep/
wake preservation, installed loader behavior and physical HOME/failure recovery
remain unqualified. No target connection or instrument/output is approved.
The native image will execute its documented startup if physically launched;
the build flag does not grant permission to do that.

Stop after offline validation. Request separate explicit approval for device
access/backup/replacement SD copy, then physical startup/UI launch testing.
Do not execute either automatically, remove the diagnostic, or commit/push.
# v003 offline extension

The prior v002 result is preserved below. v002 physical UI/navigation and
five-second HOME passed by user report; electrical/instrument operation remains
unvalidated. The byte-identical v002 rollback is in `releases/field-v002/`.
v003 adds simulated CAN and inert Glitching navigation. Its separate results
and safety blockers are in [the v003 record](V003_CAN_INVESTIGATION.md).
No v003 UF2 or broadened startup exception is produced.
