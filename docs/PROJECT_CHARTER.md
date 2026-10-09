# WiliPirate — Field Analyzer

Created by gingerSou1. Original WiliPirate code and documentation: MIT.

## Mission

A portable measurement, protocol-analysis and embedded troubleshooting
interface for FREE-WILi 2: find the right stock instrument, understand its
connections, observe useful data, and know when desktop analysis is appropriate.
Reuse supported platform capabilities rather than duplicate stock instruments.
The handheld should eventually be useful without a computer or internet.

## Current milestone: Field Analyzer navigation preview

v002 UI/navigation/HOME passed by user report; electrical and instrument
qualification remains open. v003 authorizes offline simulated CAN result
decoding, monitoring and RAM capture/review, plus an inert Tools Glitching pane.
No live receive, CAN TX, power changes or fault injection is authorized.
The prior startup exception is not broadened; no v003 device build is performed.
See [v003 qualification](V003_CAN_INVESTIGATION.md).

Implemented: navy/teal 480x320 home with Oscilloscope, Logic Analyzer, Protocol
Analyzer and Signal Generator tiles; CAN/UART/I2C/SPI selection; Tools, Captures
and Settings; reusable connection guides; Back/Home navigation; unavailable
instrument previews. No measurement or output operation is implemented.

Guide content distinguishes documented connector facts from unverified electrical
compatibility. Missing data displays "Pinout or electrical compatibility not
yet verified." A guide never enables power, changes configuration or initiates
an instrument. Users can leave without connecting anything.

Stock instrument integration, Orca compatibility, analog/logic feasibility,
CAN receive and desktop interoperability are research, not implemented features.
CAN development is paused in favor of simpler hardware interfaces; no next
interface has been selected or implemented. The original Control Lab is
disassembled. Future CAN work must first complete the standalone bench's
electrical and passive/listen-only qualification, under separate approval.

## Boundaries

Only WiliPirate may be modified. Official dependencies are read-only. Preserve
stock firmware, M2/M3 source/evidence and artifact identities. Never modify the
physical SD card or installed diagnostic as archival cleanup. No device access,
deployment, transmission, signal generation, VREF/GPIO/power changes, autonomous
AI control, commits, pushes, merges or releases without separate approval.

The required standard BSP startup changes VREF/internal GPIO/power. Current
host builds perform none of those operations. Native candidate configuration
is fail-closed unless explicit standard-startup approval is supplied; that
approval still does not authorize hardware deployment. A zero-change native
startup contract has not been established. Do not claim otherwise.

## Future shared workflow

Connect -> Configure -> Observe -> Capture -> Review -> Save -> Export.
Future records should carry instrument, timestamp/timebase, settings, raw data,
notes, optional target label, warnings, loss counters and export format/version.
Only Captures navigation exists now. No persistence, recording or transfer is
implemented. Future app data belongs under the official `/appdata/<app-name>/`
contract after the APIs and formats are established.

AI is optional analysis assistance: hypotheses, explanations, proposed procedures
and summaries. It must not autonomously operate target hardware, power, CAN TX,
GPIO, firmware or outputs. No AI integration exists in this milestone.

See [architecture](ARCHITECTURE.md), [roadmap](ROADMAP.md),
[research](FIELD_ANALYZER_RESEARCH.md), [guides](CONNECTION_GUIDES.md), and
[preservation archive](../archive/README.md).
