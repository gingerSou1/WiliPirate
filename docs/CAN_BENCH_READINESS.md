# Standalone CAN bench readiness

Research checkpoint: 2026-10-09. **NOT READY TO CONNECT.** Research and a
proposed procedure only; no hardware enumeration/operation, transmission,
firmware change, UF2 build, SD access, deployment, commit or push.
v003 is accepted as an offline development checkpoint, not a live monitor.

## Scope correction and evidence

The user confirms that the original two-Arduino Control Lab is disassembled.
Do not require rebuilding it. New scope: one UNO R4, one compatible external
CAN transceiver, FREE-WILi 2 as the intended passive monitor, and an optional
independent ACK-capable node for reliable-traffic qualification.
The user selects **UNO R4 Minima** as the baseline; UNO R4 WiFi is available as
an alternative. The user now specifies the exact **Waveshare SN65HVD230 CAN
Board**. Its published schematic/datasheets establish the intended interface;
actual board revision, fitted parts and orientation still need inspection.
Physical wiring/testing stays blocked pending electrical and FW2 verification.
The user will use the **existing FREE-WILi 2 20-position header**, not Humpback.
Bare-header CAN H, CAN L, GND and the actual connector orientation remain
unverified for physical connection. Prefer a passive, non-invasive breakout;
no target-facing supply or VREF enable/selection is part of this plan.

Read-only historical ControlLab evidence: `shared/can_transport.h`,
`platformio.ini`, `docs/can_protocol.md`, `docs/build_guid.md`,
`docs/first_power_up_checklist.md` and `docs/milestone_1_validation.md`.
These describe UNO R4 WiFi/Minima integrated RA4M1 CAN controllers using
Arduino_CAN, external transceivers, Classical CAN at 500 kbps, two 120-ohm end
terminations and common reference ground. No MCP2515 driver/controller appears
in this baseline. Historical frames use standard IDs and eight bytes; ECU
telemetry was 0x100 every 100 ms. This is historical evidence, **not an existing
bus, current module identity, current resistor inventory or installed firmware
configuration**. No bench schematic/BOM or transceiver model was found.

Official UNO board schematics are available:
[WiFi ABX00087](https://docs.arduino.cc/resources/schematics/ABX00087-schematics.pdf)
and [Minima ABX00080](https://docs.arduino.cc/resources/schematics/ABX00080-schematics.pdf).
They do not document the user's separate module/harness. Arduino's
[WiFi guide](https://docs.arduino.cc/tutorials/uno-r4-wifi/cheat-sheet/)
documents built-in CAN plus an external transceiver requirement. An MCP2515 is
a separate CAN controller, not a transceiver; it is not required by UNO R4.

The exact model is user-specified; no actual-board markings/photos have yet
been supplied. Do not substitute a generic SN65/TJA/MCP module. The official
Waveshare schematic and the offline-prepared counter sketch are reviewed in
[the Minima/Waveshare milestone](CAN_MINIMA_WAVESHARE_MILESTONE.md).
That record establishes the need for **both TX down-shifting and RX up-shifting**,
module P1/can1 assignments and its fixed 120-ohm termination. It does not
authorize wiring, Arduino upload, transmission or a FREE-WILi deployment.

## Identity and electrical readiness checklist

| Item | Current evidence | Required before wiring |
| --- | --- | --- |
| UNO R4 | User-selected Minima baseline, integrated CAN controller; WiFi available as alternative. | Confirm board revision and installed core pin definitions. Minima TX D4/RX D5; alternative WiFi TX D10/RX D13. Do not use a variant-independent pin map. |
| External transceiver | Exact Waveshare SN65HVD230 CAN Board selected; official schematic reviewed. | Match actual revision/markings and fitted parts; no generic substitution. No MCP2515, onboard regulator or logic translator is shown. |
| Supply and logic | 3.3 V module, 5 V Minima. TI input maximum VCC+0.5 V forbids direct TX; module RX high does not guarantee Minima VIH. | Verify down/up buffers and dedicated 3.3 V regulator/current budget, shared Minima 5V supply sequencing and rails; never feed 5 V directly to module VCC. |
| Module enable/silent pins | Schematic Rs has a 10-kohm ground resistor; no software standby header shown. | Inspect fitted R1. Proposed external TX buffer is disabled by default; recessive pull-up is not whole-bus isolation. |
| Module bus pins | Schematic can1.1 = CAN L, can1.2 = CAN H; P1.3 = GND. | Confirm actual pin-1/labels/orientation. Do not connect MCU TX/RX to bus wires. |
| FW2 controller/transceiver | Official Humpback documentation describes an existing internal host CAN channel and SIC transceiver. | Exact installed board/edition/controller/transceiver part, schematic and connector-side ratings still needed. MCP2518-style API layout alone is not chip identification. |
| FW2 connection | Existing bare 20-position header is selected. New official table/template identify source pin 16 CAN L, 18 CAN H and 19/20 GND. | Actual board revision, header pin-1/notch orientation, H/L/GND assignment and cable contact mapping remain physically unverified. No Humpback/DB-15 numbering substitution. |
| Ground/common mode | Both CAN bus pins and a reviewed signal-reference path are needed for a non-isolated bench. | Review power/USB/earth paths and ground offset against both actual transceiver ratings. Do not infer isolation from ISO CAN FD, or use USB grounding as the only documented reference. Stop if isolation is required but unavailable. |
| Termination | Waveshare schematic has fixed R2 = 120 ohms across H/L, no jumper shown. New arrangement not measured. | Treat Waveshare as an end node; verify actual R2 and exactly one far-end termination. A third FW2 monitor tap must not add another 120 ohms. Its software termination state needs independent qualification. |
| Bitrate | 500 kbps is historically validated and a proposed starting rate, not currently set. | Confirm Arduino transmitter rate and FW2 supported setting/value/readback; use Classical CAN, not FD, for this bench. Do not guess enum integers or rewrite firmware now. |

## Bare-header mapping research: documentary evidence, physical gate CLOSED

Sources checked 2026-10-09. The older
[official FW2 header page](https://docs.freewili.com/hardware/pinout/)
identifies CAN FD on 16/18 and GND on 19/20 but leaves H/L names unconfirmed.
The [connector-location page](https://docs.freewili.com/hardware/connectors/)
identifies the 20-position header, not an authoritative pin-1 mating-face view.
Inherited FreeWili 1 GPIO/I2C limits are not FW2 CAN limits.

A more specific official source was found in `freewili/fw2-orca-templates`,
revision `13dcaabc8f91bab79dd3e3cadbb9578619379225`:
[connector table](https://github.com/freewili/fw2-orca-templates/blob/13dcaabc8f91bab79dd3e3cadbb9578619379225/PINOUTS.md),
[source orientation image](https://github.com/freewili/fw2-orca-templates/blob/13dcaabc8f91bab79dd3e3cadbb9578619379225/assets/freewili2connector.png),
and [KiCad template schematic](https://github.com/freewili/fw2-orca-templates/blob/13dcaabc8f91bab79dd3e3cadbb9578619379225/kicad/ORCATemplate.kicad_sch).

| Bare 20-position header signal | Official table | Template CN1 symbol | Physical qualification |
| --- | --- | --- | --- |
| CAN L | 16 | 16 = CAN_L_A | UNVERIFIED on actual board/header/cable |
| CAN H | 18 | 18 = CAN_H_A | UNVERIFIED on actual board/header/cable |
| GND | 19 and 20 | Both labelled GND | UNVERIFIED physical pin identification/reference path |

The source image was visually reviewed. In its depicted orientation, even pins
are on the upper row, odd pins on the lower row, numbers increase left to right,
and the notch is between 10/12. This is a **source-diagram orientation**, not a
verified view of the user's connector or a cable's mating/solder face. Do not
mirror/rotate it onto the hardware by assumption. Breakout colors are labels,
not independent proof of net identity. Symbol drawing positions are not a
physical footprint. The template corroborates the documentary numbering but
is not the FW2 mainboard schematic or a verified Humpback routing netlist.

The [official Humpback guide](https://freewili.com/orcas/humpback-orca.html)
documents DB-15 H=6, L=14, GND=8 and routing of the existing internal CAN
transceiver/channel without extra controller/transceiver/termination. Those
are **DB-15 numbers only**. The logical correspondence would be DB-15 H to
the source-labelled header H, L to L and GND to GND; it is not a verified
connector-to-connector trace. No actual Humpback routing schematic/PCB netlist
or revision-matched FW2 mainboard schematic was located in the inspected
official material. The template repository inventory contains generic/NRF/
GPIO-joiner designs, not Humpback or the FW2 mainboard. This search result is
not a claim that such schematics do not exist elsewhere.

Remaining proof needed: revision-matched FW2 connector/transceiver schematic,
pin-1/key/mating-face identification and reviewed passive cable netlist. Where
documentation cannot settle the actual contacts, propose separately approved
unpowered continuity verification; no measurement is authorized or performed
here. Do not resolve uncertainty by powering pins or probing an active bus.

For the chosen direct-header plan, use only qualified CAN H, CAN L and signal
ground contacts via a keyed, labelled passive breakout after all gates pass.
Leave other contacts unconnected/insulated, including 20-pin 2/6 power outputs,
4 TRIG/VREF, and all analog/output contacts. No target-facing supply, numeric
VREF selection, jumper between power/VREF contacts or power feed from the UNO
is authorized. A receive-only controller mode does not make all 20 header
contacts electrically passive. Humpback is not part of this bench.
The UNO still requires its own verified transceiver; do not insert another
transceiver in series with the FW2's bus-side CAN interface.

## Minimal schematic: verified architecture, incomplete pin-level netlist

```text
UNO R4 Minima integrated CAN          Exact Waveshare SN65HVD230 board
  D4 CANTX -- LVC1G125 @3V3 -------> P1.1 CAN_TX
  D5 CANRX <- AHCT125 @5V ---------- P1.2 CAN_RX
  GND ----------------------------- P1.3 GND
  5V -- MCP1700-3302 --> bench 3.3V - P1.4 VCC (NEVER direct 5V)

Waveshare can1 connector              FREE-WILi 2 existing 20-position header
  .2 CAN H -- passive breakout ------ CAN H [source: 18; actual UNVERIFIED]
  .1 CAN L -- passive breakout ------ CAN L [source: 16; actual UNVERIFIED]
  GND ---- reviewed reference path -- GND [source: 19/20; actual UNVERIFIED]

Waveshare R2=120 ohms at one end; one reviewed 120-ohm termination at other end.
Header pin-1, key/notch, viewing face and cable orientation: UNVERIFIED.
All other FW2 header contacts unconnected/insulated. No Humpback/DB-15.
No target supply, VREF or shared power-supply connection is assigned.
```

The diagram is a conceptual netlist, not authorization to wire. The proposed
logic buffer circuit and exact package pins are in the linked milestone;
actual module/FW2 connector orientation and termination remain gates. Signal
ground/reference is shared through the
reviewed non-isolated ground arrangement; its safe physical connection order
must be approved after ground-offset/power-path review.
Published source pin numbers above do not clear physical qualification. There
is no instruction to connect hardware today.
The [official Minima core](https://github.com/arduino/ArduinoCore-renesas/blob/1.6.0/variants/MINIMA/pins_arduino.h)
and [pinout](https://docs.arduino.cc/resources/pinouts/ABX00080-full-pinout.pdf)
support D4 TX/D5 RX. The Minima datasheet's prose descriptions have a TX/RX
inconsistency alongside CANTX0/CANRX0 labels; use the matching core/pinout and
actual board revision rather than that conflicting description.

For reliable traffic add one ACK-capable controller **with its own compatible
CAN transceiver** as a short unterminated tap (or redesign end placement while
retaining exactly two end terminations). This can be an available qualified
CAN interface; it need not be a reconstruction of the original Control Lab.
FW2 remains listen-only. No particular product purchase is recommended.
The already available UNO R4 WiFi is a possible ACK node using its onboard
controller and **another separately verified transceiver**. This is optional
reuse, not a requirement to reconstruct the original Control Lab. No additional
module compatibility, firmware configuration or physical operation is approved.

## Listen-only trace and limits of proof

Pinned OneWili `b0eeccda21b0594c8062cd17e9755f0c26b0cf6b`, unchanged:

1. DISPLAY calls `ow_hardware_settings_home_neptune_settings_c_an1_listen_only`.
2. Generated C sends reset-to-root prefix 0x02, `h\s\p\y` and newline using
   `ow__call`; FwGUI carries this over the internal DISPLAY/MAIN link.
3. The wrapper consumes an Ok/Err response and discards its body. It has no
   boolean enable argument and no controller mode readback.
4. [Official Neptune settings](https://docs.freewili.com/features/neptune-settings/)
   describe the action as enabling listen-only. Available sources do not expose
   the complete stock MAIN menu-handler/controller transition implementation.

The pinned generic `ow__call` accepts the next standard response frame and checks
its success flag; it does not compare command identity/sequence against the
originating command. No device was queried. This limits what a future generic
OW_OK can prove, especially with asynchronous/stale traffic. Correlated raw
evidence and independent actual-mode verification are required; do not patch
upstream or automatically retry ambiguous configuration writes.

True controller listen-only suppresses ACK and error/data transmission.
Microchip's [listen-only explanation](https://onlinedocs.microchip.com/oxy/GUID-0C0B825F-9C9F-446F-AFDD-2324B355B920-en-US-12/GUID-0F4E010C-E292-424D-A8FD-96353D13B289.html)
describes that behavior for its documented CAN peripheral; it is supporting
protocol evidence, not identification of FW2's chip or proof of its firmware.
For a **confirmed MCP2518FD**, the
[manufacturer datasheet](https://ww1.microchip.com/downloads/en/DeviceDoc/External-CAN-FD-Controller-with-SPI-Interface-DS20006027B.pdf)
register 3-7 defines requested listen-only REQOP=3 and actual OPMOD=3.
The mode request is not sufficient: actual OPMOD must agree.

Official [Read CAN Register(s)](https://docs.freewili.com/features/canfd/#read-can-registers)
and the pinned API offer a candidate actual-mode readback path. Conditional on
confirmed chip/register map, reading only CiCON at address 0x000 can inspect
OPMOD bits 23:21 (expected 3), with REQOP bits 26:24 also 3. This is a future
approved readback proposal, not an executed command or permission for raw
register writes. Review read-side effects; do not dump arbitrary SFRs. Check
mode after rate/stream/power changes as those may reinitialize the controller.
No numeric bitrate settings, firmware version or safe initialization order
has yet been established for the installed board. Zone 15 CAN power demand is
also outside the earlier DISPLAY-only startup exception.

## ACK: what one transmitter can and cannot prove

With one UNO transmitter and a true passive FW2 receiver there is no ACK source.
A receiver can observe a correctly transmitted frame through its CRC while
the transmitter then observes an ACK error. That can be useful for a bounded
isolated receive-path experiment **if the installed receive implementation
reports such attempts**; that behavior is unverified here. Retransmissions and
error recovery depend on transmitter configuration. Do not promise one received
record per application send, finite retries, or an inevitable bus-off state.
Application write success may mean queued, not acknowledged on the wire.

Reliable acknowledged traffic needs another normal-mode CAN receiver/controller
and transceiver to supply ACK. A transceiver alone does not supply ACK. That node
may send automatic ACK/error bits even with no application-frame transmission;
it must be independently qualified, not called passive. FW2 must never become
the ACK source simply to make a two-device demonstration appear successful.

## Staged proposal and explicit stop conditions

Every physical stage requires fresh approval. The present task performs none.

1. **Documentation gate.** Obtain module markings/photo/product schematic, Minima
   board revision, FW2/module revision, transceiver datasheets and power/ground plan.
   Produce the exact bare-header netlist, including H/L/GND, pin-1, key, viewing
   face and cable contact numbering. **STOP** for missing/contradictory identity,
   unknown logic tolerance, bus ratings, pin orientation or standby state.
2. **Unpowered inspection gate.** After separate approval, verify cable
   continuity, H/L polarity, no shorts/backfeed, end resistors and tap length.
   For two simple 120-ohm terminations, expected parallel resistance is nominally
   60 ohms; three yield 40 ohms. Circuit details/tolerance must be considered.
   **STOP** for unexplained resistance, extra termination or any unverified
   VREF/power connection. Bare-header GND and H/L must be verified before any
   connection. Keep all unused contacts insulated. No hot-plug assumption.
3. **Isolated FW2 mode gate.** Keep the working lab out of the plan. Approve a
   specific stock-compatible mode/internal-CAN-power/receive procedure separately, without
   firmware flashing or v003 deployment. Inspect actual mode and electrical
   inactivity before connecting its bus. **STOP** if the mode cannot be proved,
   replies are ambiguous, power effects are unexplained, or inherited periodic
   TX/terminal-over-CAN functions cannot be excluded. No guessed raw writes.
   No target-facing supply or VREF enable is allowed. Standard BSP startup has
   documented VREF/expander effects; it cannot be reused as electrically passive.
   Any startup that conflicts with this boundary is a STOP, not an implicit
   exception. Internal CAN rail requirements need separate lifecycle review.
4. **Minimal no-ACK experiment, optional.** On an isolated reviewed two-device
   bus, a separately approved external transmitter may make a small bounded set
   of Classical 500 kbps attempts. Document ACK errors/retries independently;
   no transmit command comes from FW2. This tests possible receipt of attempts,
   not reliable traffic. **STOP** on unexpected dominant output from FW2,
   unbounded retries/load, supply/ground problems or missing error visibility.
5. **Reliable reference traffic.** Add the independently qualified ACK-capable
   node, with exactly two end terminations. Use a separately approved external
   source of bounded known traffic and compare reception against the reference.
   Monitor FW2's attributable TXD or an equivalent isolated driver-output
   observation point with approved high-impedance instrumentation. A bus ACK bit
   while another node is active cannot prove which receiver supplied it.
   **STOP** on any FW2 ACK/data/error/overload output, reference errors, frame
   mismatch, unexplained loss or electrical anomaly. No intrusive chip probing
   or modification without separate approval.
6. **ACK attribution and lifecycle.** A separate bounded no-other-ACK window can
   check that adding FW2 does not change the source's missing-ACK result; waveform
   and receive observations must agree. Observe permitted startup, stream
   changes, stop/HOME recovery and power transitions on the isolated bench;
   passive behavior in steady state does not establish safe lifecycle behavior.
   **STOP** if mode/termination changes, unexpected TX occurs or evidence cannot
   attribute behavior. Stock HOME recovery may restore active stock settings:
   qualify it off the bus first; do not assume a safe bus-connected exit.

No automated rollback/retry, actuator commands, glitching, vehicle connection,
CAN FD traffic or fault injection is part of this qualification proposal.
TI's [physical-layer guidance](https://www.ti.com/lit/an/slla270/slla270.pdf)
supports a linear differential bus, short taps, two end terminations and reviewed
grounding. Generic CAN voltages are not substitutes for exact device limits.

## Readiness sign-off

- [x] Revised standalone scope recorded; no original-lab rebuild required.
- [x] UNO R4 integrated controller/external transceiver distinction established.
- [x] Historical 500 kbps baseline and termination evidence separated from new bench.
- [x] Direct existing-header connection selected; no Humpback installation.
- [x] Official source header mapping and separate DB-15 numbering documented.
- [x] No-ACK versus reliable-traffic bench requirements distinguished.
- [x] UNO R4 Minima selected by user as baseline; WiFi available as alternative.
- [x] Exact Waveshare SN65HVD230 CAN Board selected and published schematic reviewed.
- [x] TX down-shifting, RX up-shifting and module schematic nets established.
- [ ] Actual board revision/markings, package orientation and fitted parts inspected.
- [ ] Exact supply/logic/bus/common-mode limits and ground plan reviewed.
- [ ] Actual bare-header CAN H, CAN L and GND contacts verified for board revision.
- [ ] Pin-1/notch, viewing face and passive-breakout cable orientation verified.
- [ ] Exact FW2 transceiver ratings, termination arrangement and part/revision verified.
- [ ] Target supply/VREF untouched; unused contacts insulated; startup effects qualified.
- [ ] Installed stock firmware, setting values and controller readback qualified.
- [ ] Listen-only no-ACK/no-TX and startup/HOME lifecycle proven independently.
- [ ] Reference ACK node/traffic source/instrumentation and bounded procedure approved.
- [ ] Separate physical test approval received.

Only documentation is changed in this research step. Code, artifacts and the
closed live CAN gate remain unchanged. Research checks: local document links,
`git diff --check`, preserved artifact identities and scope review; no builds.
