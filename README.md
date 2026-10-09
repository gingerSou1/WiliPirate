# WiliPirate — Field Analyzer

**Created by gingerSou1 · FREE-WILi 2 · MIT for original WiliPirate work**

A portable measurement, protocol-analysis and embedded troubleshooting interface
built around supported FREE-WILi capabilities. The direction is instrument
access, connection guidance and a future handheld-to-desktop capture workflow,
not a Bus Pirate clone or duplicated stock driver collection.

## Current status: v003 offline checkpoint; CAN development paused

**v002 UI is physically validated:** navigation and five-second physical HOME
recovery passed by user
report. Its [rollback artifact](releases/field-v002/manifest.json) is preserved;
electrical safety and instrument operation remain unvalidated.
v003 adds simulated CAN monitoring, bounded result decoding and volatile RAM
capture/review, plus **Tools -> Glitching / Fault Injection** with inert planned
sections and a Connection Guide. **Live CAN reception is not qualified or
enabled.** No glitching controls operate.
See [v003 findings and checks](docs/V003_CAN_INVESTIGATION.md).

Implemented: a navy/teal four-tile home, instrument Connection Guides, CAN/UART/
I2C/SPI selection, Back/Home, and Tools/Captures/Settings placeholders.
**Oscilloscope, Logic Analyzer, Protocol Analyzer and Signal Generator are all
unavailable instruments in this build.** No acquisition, transmission, signal
output, hardware configuration, persistent recording or AI operation is implemented.
Guides show documented facts and explicitly mark unverified pinout/electrical
compatibility. They never enable hardware or require a connection to continue.

**CAN development is paused in favor of simpler hardware interfaces.** No next
interface has been selected or implemented. The standalone CAN bench research
and compile-tested Arduino counter sketch are preserved for future review;
they do not authorize wiring, transmission or deployment. The original Control
Lab is disassembled. No vehicle/OBD-II, transmit or autonomous-control capability
is claimed for WiliPirate.

## Review and offline build

- [Project charter](docs/PROJECT_CHARTER.md)
- [Documentation index](docs/INDEX.md)
- [UI source and host build](native/field_analyzer/README.md)
- [Offline validation](docs/FIELD_ANALYZER_VALIDATION.md)
- [Research and integration classifications](docs/FIELD_ANALYZER_RESEARCH.md)
- [Connection Guide source facts and gaps](docs/CONNECTION_GUIDES.md)

Root CMake builds host UI checks by default. The supported native DISPLAY
adapter is separately gated because standard BSP startup changes VREF/power
and internal GPIO. No no-change native startup is claimed. Any device candidate
needs explicit standard-startup approval, and any physical deployment needs
separate approval. No installed device or SD file is modified by this pivot.

## Preserved history and licensing

[archive/](archive/README.md) preserves byte-identical M2/M3 source, tests and
engineering records, with a checksum manifest and original milestone commits.
The known-good M2 release remains at `releases/m2-ui-v001/WiliPirate.uf2` unchanged.
WiliPirateI2CDiag is retired from active development/deployment; its installed
SD file and ignored local UF2 remain untouched. Its FPGA prerequisite/raw
response evidence is retained, including unresolved photographed-byte errors.

[MIT License](LICENSE): Copyright (c) 2026 gingerSou1, for original WiliPirate
code/documentation only. Third-party ownership/notices and unresolved binary
redistribution questions remain in [THIRD_PARTY.md](docs/THIRD_PARTY.md).
No official FreeWili repository or stock firmware is modified. See [AGENTS.md](AGENTS.md)
for permanent ownership and approval boundaries. Repository:
[gingerSou1/WiliPirate](https://github.com/gingerSou1/WiliPirate).
