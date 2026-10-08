# M2 closeout and known-good baseline

**M2 — Native DISPLAY UI: COMPLETE / PHYSICALLY VALIDATED.**
User report recorded 2026-10-07. Device: **FREE-WILi 2, FX0141**.
Branch: `feature/wilipirate-panel`. M3 I2C integration: **planned, not started**.
No application rebuild, source modification, device access or reinstallation
was performed during closeout.

## User-observed physical results

The user reports all of the following passed on FX0141. These are user-observed
physical results, not automated tests or independently captured device footage.
No test duration, repeated-cycle count or electrical measurements were supplied.

| Check | User-reported result |
| --- | --- |
| Display rendering | PASS |
| Six-tile menu | PASS |
| Touch input | PASS |
| Placeholder pages | PASS |
| Back navigation | PASS |
| About page | PASS |
| Physical HOME recovery | PASS |
| Stock interface restoration | PASS |
| Existing stock functionality | PASS |

The user confirmed external targets, probes and ORCA accessories disconnected
before installation. Normal BSP initialization was explicitly authorized.
The agent installed the file; the user manually launched and tested the app.
Existing stock functionality PASS records the user's report of normal operation;
it is not an exhaustive protocol, RF or electrical qualification.

## Exact preserved release artifact

Source commit used for the validated build: **`b069a71`**
(`feat: add UI-only loadable DISPLAY panel for M2`).
Application metadata version: **001**. Filename: **WiliPirate.uf2**.
Size: **53,248 bytes**.

SHA-256:
`6bc0a08050c1e18659882bc486a1f03b6e16529916b60112240f548aeb1e6672`.

The unchanged, byte-for-byte validated file is tracked at
[releases/m2-ui-v001/WiliPirate.uf2](../releases/m2-ui-v001/WiliPirate.uf2),
with [manifest](../releases/m2-ui-v001/manifest.json) and notices.
It was copied from the original build output and compared by checksum during
closeout, not rebuilt. This branch artifact is not a newly published GitHub
Release or authorization to deploy it again.

Official UF2/metadata validation passed: SRAM only, 104 blocks, 26,536 payload
bytes; payload range `0x20000000` to exclusive end `0x200067a8`. No QSPI-flash
or PSRAM payload. The only file-bearing ELF LOAD segment is in DISPLAY SRAM.

Build configuration: root external-project CMake, `freewili2` board,
RP2350B / Cortex-M33 (`rp2350-arm-s`), `fw2_display_app` / `no_flash`,
MinSizeRel (`-g -Os -DNDEBUG`), FW2_AGENTIO OFF, POWER_ZONES DISPLAY.
Official WiliBSP revision:
`be4bdd63d31a80f95410e583710cf4e43a7be7fa` (unmodified).
Official Pico SDK 2.3.0 and Arm GNU Toolchain 14.2.Rel1 (compiler 14.2.1).
See [build evidence and reproduction](M2_DISPLAY.md).

## Installation evidence (automated host operation)

The project-specific Windows `.venv` used pyfwfinder 0.6.0 and pyserial 3.5.
Imports and dependency checks passed. Official discovery identified one
FREE-WILi2, serial FX0141, MAIN COM3; its USB descriptor reported FW2 v07.
That descriptor is not a version inventory of every firmware component.

Exact authorized command, run once:

```powershell
.\.venv\Scripts\python.exe -B wilibsp/tools/fw.py install-app build/display/native/display/WiliPirate.uf2 --device FX0141
```

Installer exit code: 0. Output:

```text
verified WiliPirate.uf2: SRAM app, no QSPI-flash payloads
installed WiliPirate.uf2 to D:\apps\WiliPirate.uf2
```

The official installer copied/flushed/renamed the file and returned SD ownership
to MAIN. Device destination: **SD `/apps/WiliPirate.uf2`**. The file was no longer
host-readable after handoff, so a separate installed-file checksum was not
obtained; no additional handoff was attempted. The user subsequently confirmed
successful physical use. No BOOTSEL, firmware updater, flashing, application
rebuild, automatic launch or recovery attempt was used.

## Automated host checks, separate from physical results

At M2 source/build validation: Windows Python suite discovered 52 tests:
51 PASS, one legacy constexpr/compiler check skipped. All four unchanged CM0
preparation checks passed under WSL, including that skipped check. Native host
CTest passed 2/2 (navigation and production-renderer layout checks). Official
UF2 metadata/target check, BSP external-project setup and `git diff --check`
passed. Linked-symbol inspection found no OneWili/interface initialization.
The actual menu, placeholder and BSP About host renders were reviewed by the
user before physical approval; these are not device screenshots.

Closeout checks: Python 52 discovered, 51 PASS / one compiler skip; existing
native CTest binaries 2/2 PASS; preserved UF2 byte comparison, SHA-256 and official
SRAM/metadata validation PASS; `git diff --check` PASS. No compilation was run.
Original CM0 M1E
wrong-response regressions remain untouched and are not claimed repaired.
The immutable M1A report and all prior CM0 source/tests/research remain intact.
`archive/cm0-prototype` remains at
`bc738b88976917dffb693265fcee162716f58ec0`.

## Limitations and standard initialization behavior

This is a UI prototype: GPIO, UART, I2C, SPI, CAN and LOGIC remain unavailable
placeholders. No OneWili connection, target bus operation, scan or capture is
implemented. No electrical SAFE/HiZ certification is displayed or established.
Physical validation covers this artifact on FX0141, not every installed stack.

Normal BSP startup changes core clocks/voltage, PSRAM timing, internal buses,
LED/backlight/radio-select state and expander outputs/directions. It selects
external VREF for VIO and can request/release inherited app-owned power rails.
HOME exits by DISPLAY watchdog reboot through the official loader; passing
stock UI restoration does not establish exact pre-launch VIO/power restoration.
Transient electrical behavior and missing-touch failure handling are unverified.
These effects were accepted for M2 with external connections removed.

## Next milestone and stop

M3 will add I2C first by reusing existing stock MAIN functionality where supported.
Before implementation, establish the Poll handler, exact response acquisition
and interpretation, firmware prerequisites, command correlation and electrical
ownership. The generated Poll wrapper returns status only; do not assume device
addresses or implement another scan loop. Other tiles remain placeholders.

M3 is not started or authorized by this closeout. No further development, new
build, device access, firmware/configuration changes, reinstall or merge.
