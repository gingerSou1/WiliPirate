# Field Analyzer roadmap

## FA1 — navigation and research (current)

Four-instrument home, protocol choice, reusable connection guides, secondary
placeholders, pure state/rendering, offline host validation and preservation.
All operational instruments remain unavailable. Review visual previews and the
standard-startup conflict before any native candidate or physical launch.

## CAN receive qualification — paused

CAN development is paused in favor of simpler hardware interfaces. No next
interface is selected or authorized by this checkpoint. The v003 simulation
and standalone bench research remain preserved; live CAN remains disabled.
The original Control Lab is disassembled; future qualification uses the
separately reviewed standalone bench plan below.

Use the standalone CAN plan on a controlled bench, never a vehicle. First establish
CAN-H/L and ground mapping, suitable transceiver/adapter, termination and voltage
limits; version-qualified passive/listen-only mode; exact receive schema/timebase;
loss handling, response correlation and approved power ownership. No CAN TX,
diagnostic requests, probing or direct OBD-II connection. A documented mode
binding is not proof of electrically passive operation.

After those gates and explicit approval, the smallest feature is bounded receive
display of IDs, payloads, timestamps and loss counts through existing stock APIs.
Leave storage, export and other instruments out until separately established.

## Later, separately approved

- Establish a versioned capture record and save/export interoperability before
  implementing persistence. Validate GUI import or a documented intermediate
  format end to end; do not advertise seamless handoff.
- Qualify logic capture acquisition on DISPLAY; existing command bindings do
  not deliver logic binary samples over the current display link.
- Qualify analog acquisition/protection and trigger/data access before any
  oscilloscope feature. Desktop PicoScope is a separate external instrument.
- Add UART, I2C and SPI observation only through supported result paths. I2C
  address scanning is retired as the immediate development objective.
- Signal generation requires verified connector/limits, deliberate setup and
  explicit output authorization. No enabled output by default.
- Optional desktop AI analysis after capability/privacy review; never autonomous
  hardware control. Wi-Fi/NFC are not added to this milestone.

No automatic transition to a later phase, build/deploy, commit or publication.
