# Minima + Waveshare SN65HVD230 standalone bench preparation

2026-10-09. Offline source/research/compile checkpoint only. No wiring, upload,
device discovery, CAN operation or FREE-WILi firmware/SD modification occurred.
The [FW2 connection gate](CAN_BENCH_READINESS.md) remains **CLOSED**.

## Electrical decision: level-shift both logic directions

The specified module is the Waveshare **SN65HVD230 CAN Board**, not an MCP2515
controller. Match the physical board to the cited schematic/revision before use.

| Interface | Guaranteed limits | Decision |
| --- | --- | --- |
| Minima D4 -> module CAN_TX | SN65HVD230 digital absolute maximum is VCC + 0.5 V: 3.8 V at a 3.3 V supply. D input high threshold is 2.0 V, low 0.8 V. | **Do not connect 5 V TX directly.** Shift down to the module's 3.3 V domain. An absolute maximum is not an operating target. |
| Module CAN_RX -> Minima D5/P102 | SN65HVD230 R output high minimum 2.4 V at -8 mA, low maximum 0.4 V at 8 mA. RA4M1 other-peripheral input VIH = 0.8 VCC, VIL = 0.2 VCC: 4.0/1.0 V at 5 V. | Direct RX high is **not guaranteed**. Shift up using a TTL-threshold 5 V buffer. |
| Module power | SN65HVD230 recommended supply 3.0-3.6 V; nominal 3.3 V. | Use the dedicated bench 3.3 V regulator below, never direct 5 V or FW2 VREF. |
| Bus limits | SN65HVD230 common-mode operating range -2 to +7 V; 1 Mbps device rating. | Classical 500 kbps is within its rating. These limits do not qualify the unidentified FW2 transceiver or a ground offset. No CAN FD bench traffic. |

Primary evidence: [TI SN65HVD230 SLOS346O](https://www.ti.com/lit/ds/symlink/sn65hvd230.pdf),
sections 8.1/8.3/8.6, and [Renesas RA4M1 R01DS0355EJ0110](https://www.renesas.com/en/document/dst/ra4m1-group-datasheet?r=1054146),
table 2.4. Minima D4/P103 and D5/P102 are established by the
[official core variant](https://github.com/arduino/ArduinoCore-renesas/blob/1.6.0/variants/MINIMA/variant.cpp)
and [CAN pin macros](https://github.com/arduino/ArduinoCore-renesas/blob/1.6.0/variants/MINIMA/pins_arduino.h).
Changing GPIO input settings or assuming typical switching thresholds is not
a substitute for guaranteed voltage compatibility.

## Module schematic and verified connector net names

The [official Waveshare schematic](https://files.waveshare.com/upload/c/c5/SN65HVD230-CAN-Board-Schematic.pdf),
dated 2011-09-05, was downloaded and visually reviewed using the existing
Windows PDF renderer. SHA-256:
`7f1862fb51dc0252b89cca1e81af58cc27bb59250ff78bdcf52dddb7fc3f87338`.

| Connector | Schematic pin | Net / chip connection |
| --- | --- | --- |
| P1 four-pin logic/power | 1 | CAN_TX -> U1 D pin 1 |
| P1 | 2 | CAN_RX <- U1 R pin 4 |
| P1 | 3 | GND -> U1 pin 2 |
| P1 | 4 | 3.3V -> U1 VCC pin 3 |
| can1 two-pin bus | 1 | CAN L -> U1 pin 6 |
| can1 | 2 | CAN H -> U1 pin 7 |

R2 is a fitted 120-ohm resistor across H/L in the schematic, with no termination
jumper shown. R1 is 10 kohms from Rs to GND (slope control); Vref is unconnected.
Do not reinterpret chip pin numbers as connector numbers or infer physical
left/right order from the schematic symbol. Confirm the actual board's labels,
pin-1 reference and fitted parts during a separately approved inspection.
No onboard supply regulator or logic translator is shown. Add local bypassing
in the proposed interface; do not assume a module variant includes it.

## Specific proposed interface components

Use fixed-direction, non-inverting push-pull buffers:

- TX down-shift: **TI SN74LVC1G125DBVR**, SOT-23-5, on a verified pin-labelled
  breakout, powered from the dedicated bench 3.3 V rail. Input accepts up to 5.5 V independently
  of the 3.3 V supply; output is the 3.3 V domain. Ioff supports powered-off
  isolation. [Manufacturer datasheet](https://www.ti.com/lit/ds/symlink/sn74lvc1g125.pdf)
- RX up-shift: **TI SN74AHCT125N**, PDIP-14, powered from the same Minima 5V rail
  that supplies MCU VCC. Its input thresholds are 2.0/0.8 V; output high is at
  least 4.4 V at 4.5 V supply with a light <=50 uA load. Keep the reviewed bench
  5V rail within 4.75-5.25 V, so even the maximum MCU high threshold is 4.2 V.
  Do not load the RX output with LEDs or low-value resistors. Unused inputs
  must not float. [Manufacturer datasheet](https://www.ti.com/lit/ds/symlink/sn74ahct125.pdf)

These are source-supported circuit recommendations, not a physically tested
adapter. They require supply/layout and waveform checks before bus qualification.
Do not substitute 74HC for AHCT (different thresholds), an automatic bidirectional
translator, or a generic I2C MOSFET shifter without a new electrical review.

Power module/U2 from **Microchip MCP1700-3302E/TO**, fixed 3.3 V TO-92 LDO,
derived from Minima 5V, with 1 uF X7R ceramic input and output capacitors at the
regulator. Its recommended input is 2.3-6 V and current rating 250 mA, subject
to thermal derating. Review a conservative <=100 mA bench budget and thermal
dissipation before separately approved power tests.
[MCP1700 datasheet, table 3-1 and application circuit](https://ww1.microchip.com/downloads/en/DeviceDoc/MCP1700-Data-Sheet-20001826F.pdf).

The [official Minima schematic](https://docs.arduino.cc/resources/schematics/ABX00080-schematics.pdf)
labels header +3V3 as the MCU USB LDO output. Its spare current capacity is not
established here; leave that header rail unconnected. Do not parallel the new
regulator with it or any FW2 supply/reference. Both proposed logic domains
derive from the same reviewed Minima 5V supply.

## Exact proposed logic-side netlist

| Connection | Required net |
| --- | --- |
| U2 SN74LVC1G125DBVR pin 5 VCC / pin 3 GND | Dedicated bench 3.3V / Minima GND |
| U2 pin 2 A | Minima D4; add 10 kohm pull-up from A to Minima 5V for recessive reset state |
| U2 pin 4 Y | Waveshare P1 pin 1 CAN_TX; add 10 kohm pull-up from this node to bench 3.3V |
| U2 pin 1 /OE | 10 kohm to bench 3.3V (disabled by default); reviewed removable jumper to GND enables TX path |
| U3 SN74AHCT125N pin 14 VCC / pin 7 GND | Minima 5V / Minima GND |
| U3 pin 2 1A | Waveshare P1 pin 2 CAN_RX |
| U3 pin 3 1Y | Minima D5 |
| U3 pin 1 /1OE | GND |
| U3 unused /OE pins 4, 10, 13 | 5V (disabled) |
| U3 unused A pins 5, 9, 12 | GND |
| U3 unused Y pins 6, 8, 11 | Unconnected |
| U4 MCP1700-3302E/TO pin 1 GND / pin 2 VIN / pin 3 VOUT | Minima GND / Minima 5V / dedicated bench 3.3V |
| U4 capacitors | 1 uF X7R from VIN to GND and 1 uF X7R from VOUT to GND, short leads |
| Waveshare P1 pins 3/4 | Minima GND / dedicated bench 3.3V |
| Local bypass | 100 nF ceramic directly across VCC/GND at U2, U3 and the module (three capacitors) |

Confirm package pin-1 and breakout routing against each manufacturer diagram.
The default-disabled TX buffer plus CAN_TX pull-up keeps the module input
recessive while that buffer is disabled; it is **not galvanic isolation** or
whole-bus passive certification. The sketch cannot verify the jumper state.
Future physical use must power the Minima/module/buffers together; do not drive
signals into an unpowered RX buffer or hot-plug partially powered logic.
Module Rs is not exposed as a software control in this schematic.

## Minimal proposed schematic

```text
Minima D4 -- U2 LVC1G125 @3V3 --> Waveshare P1.1 CAN_TX
Minima D5 <- U3 AHCT125 @5V ---- Waveshare P1.2 CAN_RX
Minima GND -------------------- Waveshare P1.3 GND / U2/U3 grounds
Minima 5V -- MCP1700-3302 ------ bench 3.3V -> Waveshare P1.4 / U2 VCC
Minima 5V --------------------- U3 VCC (NEVER module VCC)
Minima 3V3 header: unused; do not parallel supplies.

Waveshare can1.2 H ---- reviewed passive bus ---- FW2 source header 18 H
Waveshare can1.1 L ---- reviewed passive bus ---- FW2 source header 16 L
Minima/module GND ---- reviewed reference path - FW2 source header 19/20 GND

FW2 actual header contacts/orientation and compatibility: NOT QUALIFIED.
FW2 power/VREF and all unused header contacts: UNCONNECTED/INSULATED.
Waveshare R2: 120 ohms at one end. Exactly one 120-ohm termination at other end.
For reliable traffic: add a normal-mode ACK node with its own transceiver.
```

FW2 numbers are documentary mapping from the pinned official Orca template;
they are **not** physical wiring permission. The actual FW2 board's common-mode
ratings, termination state and header/cable orientation remain gates. No Humpback
is used. No FW2 VREF or target-facing power selection/enable is proposed.
Use a short twisted H/L pair and reviewed ground path, not MCU TX/RX on the bus.

The module's fixed R2 makes it an end node. With a third ACK node, choose the
far-end termination there and keep the FW2 monitor tap unterminated, after its
state is independently qualified. Do not add another external resistor in
parallel with R2. Two 120-ohm end resistors imply about 60 ohms unpowered;
three imply 40 ohms. Neither has been measured here. Do not remove R2 or modify
any board under this offline task.

## Counter sketch and error-handling limits

[Sketch](../bench/uno_r4_minima_can_counter/src/can_counter.ino) uses the Minima's
onboard CAN with Arduino_CAN, no additional CAN-controller library. CAN is
closed at boot. Manual serial `s` requests a run; `x` stops it. These are future
operation instructions only after deployment/testing approval.

Classical 500 kbps, standard ID **0x321**, eight data bytes:
bytes 0-3 = little-endian counter, bytes 4-7 = little-endian enqueue-time
`millis()`. First counter is 0; nominal interval is 100 ms, with no catch-up
bursts. After 100 accepted submissions it closes CAN; another run requires an
explicit `s`. The software's bound does not bound automatic on-wire CAN retries.
Scheduling is cooperative and serial output can introduce jitter; no precision
timestamp claim.

Initialization failure, negative `CAN.write` result or reported asynchronous
CAN error stops the run and closes CAN. No retry/recovery is automatic. The
installed official driver returns success after mailbox submission and ignores
TX-complete notifications in its public callback: **Queued is not ACKed**.
Its error latch is not a complete per-frame ACK/error counter. `CAN.end()` is
controller cleanup, not proven electrical shutdown/recovery; physical output
and bounded stop behavior remain unverified. Keep a reviewed manual stop path.

A passive FREE-WILi does **not** ACK. One transmitter plus that monitor cannot
provide reliable acknowledged continuous traffic. Supply another qualified
normal-mode receiver/controller/transceiver (the available R4 WiFi is an option
with a second verified transceiver/interface). Do not make FW2 active to hide
missing ACKs. The prepared sketch is intended for the ACK-equipped reference
bench; no-ACK experiments need their own tightly bounded approved procedure.

## Build and offline checks

Use existing PlatformIO with [configuration](../bench/uno_r4_minima_can_counter/platformio.ini):

```text
platformio run --project-dir bench/uno_r4_minima_can_counter
```

No upload/monitor target is included in verification. Platform Renesas RA 1.9.0,
Arduino Renesas UNO core 1.6.0, Arduino_CAN 1.0 and existing Arm GCC 7.2.1 were
reused. Compile succeeded: **40,536 bytes flash, 3,632 bytes RAM**.
Output stays ignored at `build/can-bench-minima/minima_counter/firmware.bin`;
it is an Arduino firmware image, never a DISPLAY UF2. No FW2 UF2 was built.
Binary size: 40,536 bytes; SHA-256
`aaaf5649769a24197db43f48a185f3b2818f8f9868f75f5ad9447a671fedd2b3`.
PlatformIO needed installed-tool lock access outside the sandbox; compilation
succeeded, while its later online upgrade check failed. No tool/package was
installed or upgraded and no board was enumerated.

Host doubles execute the actual sketch to verify manual arming, ID/payload,
100 ms scheduling, wraparound, no catch-up bursts, bounded 100 submissions,
manual stop and initialization/write/asynchronous failures. This is software
control-flow testing, not controller interrupts, bus timing, power or ACK proof.
Host sketch test: PASS. Existing Python regression: **62/62 PASS**.
Document link/fence checks and `git diff --check`: PASS. Preserved M2, v002 and
M3 diagnostic artifact hashes: unchanged. No existing Field Analyzer source,
official FreeWili dependency or historical release was modified by this task.

## Parts and next review gate

Additional parts: U2 SN74LVC1G125DBVR with a verified SOT-23-5 breakout;
U3 SN74AHCT125N; U4 MCP1700-3302E/TO with two 1 uF X7R capacitors;
three 100 nF capacitors; three 10 kohm resistors; removable
TX-enable jumper; labelled passive FW2 breakout and short twisted pair;
one qualified far-end 120-ohm termination (if not already on that end node).
Reliable traffic additionally requires an ACK-capable controller/transceiver
and its correctly level-matched interface. The existing WiFi board alone is
not enough. No purchase, wiring or board modification is authorized here.

Before power: inspect exact module/header/package orientation and fitted parts,
review regulator/current/thermal budget and Minima 5V rail tolerance, verify power-domain sequencing,
unpowered nets/termination and ground paths. **STOP** on any mismatch or unknown.
Then, only under separate approval, verify logic rails and translated TX/RX with
FW2 disconnected, qualify an ACK-equipped isolated reference bench, and finally
qualify FW2 mode/output/lifecycle off the bus before its passive connection.
Any 5V on module VCC/CAN_TX, extra termination, missing ACK source for continuous
traffic, unexpected FW2 output or unqualified VREF/startup effect is a STOP.
