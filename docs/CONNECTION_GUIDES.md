# Connection Guide evidence

v003 documentary update: the official [Humpback guide](https://freewili.com/orcas/humpback-orca.html),
checked 2026-10-09, specifies DB-15 male-pin view/label up: CAN H 6, CAN L 14,
shared ground 8. It routes the existing host transceiver/channel and adds no
termination; host termination is software-selectable. This newer source narrows
the historical gaps below, but actual module revision/cable/electrical limits
and passive behavior remain unqualified. The CAN screen incorporates these
documented DB-15 facts without enabling a connection or instrument operation.
See [CAN qualification](V003_CAN_INVESTIGATION.md).

The reusable UI component shows connector, signals, ground, limits, accessory,
stock tool, desktop suggestion and precautions. Published facts are documentary
evidence, not electrical or physical verification on this user's device. Missing
information uses the required fallback text and no connection is required to
view a placeholder. No configuration/API call is associated with a guide.

Source: [pinned official FW2 pinout](https://github.com/freewili/freewili2-docs/blob/0a4e3c224fdd4d0b61e6b02f64a14feaba28d725/docs/hardware/pinout.md).
Header orientation/keying and FREE-WILi 2 buffer component/electrical ratings
are explicitly unverified in that source; FreeWili 1 ratings must not be copied
as FREE-WILi 2 safe limits. Connector numbers below are physical positions,
not the corresponding MAIN GPIO numbers.

| Instrument | Published connector facts | Gap preventing connection/operation |
| --- | --- | --- |
| UART | 20-pin header: pin 5 RX/GPIO9, pin 9 TX/GPIO8, GND 19/20; CTS 7, RTS 11. | Voltage/input limits, orientation and cable compatibility; crossover is conditional on voltage review. |
| I2C | MAIN I2C0: pin 10 SDA/GPIO16, pin 8 SCL/GPIO17, GND 19/20. | Voltage/pull-ups/orientation and installed power policy; previous Poll refused FPGA-zone prerequisite. No enable or scan occurs. |
| SPI | CS 1/GPIO13, MISO 12/GPIO12, MOSI 13/GPIO15, SCLK 15/GPIO14, GND 19/20. | Limits/orientation/settings and observation route. No bus initialization on selection. |
| CAN | Header positions 16/18 are associated with CAN FD; ground positions 19/20 are published. | Source explicitly does not establish CAN-H/L versus TX/RX assignment. DB-15 pinout, termination, isolation/voltage and listen-only behavior unqualified. No direct vehicle/OBD-II claim. |
| Logic | 20-pin digital connector; documented stock Logic Analyzer. | Exact active capture channels, levels and sample path. No adopted arbitrary GPIO-range example. |
| Oscilloscope | CN23 10-pin analog connector has four inputs/two outputs in stock documentation. | Exact physical analog/ground pin order, protection, bandwidth and practical acquisition/trigger limits. |
| Generator | Stock waveform, PWM and Logic Player concepts exist. | Different outputs have different connectors/limits. None is active or configured in WiliPirate. |

## Orcas and simple accessories

Source: [pinned official Orca documentation](https://github.com/freewili/freewili2-docs/blob/0a4e3c224fdd4d0b61e6b02f64a14feaba28d725/docs/hardware/orca-modules.md).

- Humpback exposes the internal ISO CAN FD channel through DB-15 and adds
  VBATT input. Exact bus pin assignments, supply/protection/termination and
  automotive suitability are not established by that description alone.
- Maestro offers a documented logic connector, IO test points, switches, Qwiic,
  SD prototyping, voltage selection/injection jumper and stacking. Its external
  debug probe is described as useful for OG; FW2 has built-in probes. A voltage
  jumper is a real electrical control, not a safe default recommendation.
- SAN DIEGO additionally documents RS485 and single-pair Ethernet alongside
  ISO CAN FD. Its DB-15/VBATT needs interface-specific documentation before use.
- Jambu is a powered LED-driver board, not a generic acquisition probe. It is
  not recommended for this milestone.
- Direct header access, labelled breakout/ribbon and mini-grabbers are proposed
  arrangements, not verified products or complete wiring plans. Orca is not
  required for every interface. Do not purchase or connect a specific adapter
  based on these provisional strings alone.

No connector image or unlabeled diagram is substituted for a verified pinout.
No voltage maximum, transceiver choice or module pin mapping has been invented.
All targets remain disconnected pending a separately reviewed electrical plan.
