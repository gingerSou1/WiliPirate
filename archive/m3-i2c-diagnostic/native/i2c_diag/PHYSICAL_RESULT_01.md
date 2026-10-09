# First disconnected-device Poll: firmware power refusal

Analysis recorded 2026-10-09. Physical outcomes are user-reported, not a new
agent/device test. No hardware was accessed during this analysis.

## Reported result

The user reports successful diagnostic launch and physical HOME recovery, with
all external targets disconnected. One manual Poll produced:

| Field | User report |
| --- | --- |
| Status | COMPLETE FRAME: firmware failed |
| Captured bytes / chunks | 119 / 1 |
| Matching Poll frames | 1 |
| Diagnostic mask | `0x1000` (firmware failure flag) |
| Command | `i\i\p` |
| MAIN sequence / success flag | 4 / 0 |

This establishes a reported MAIN-side refusal, not a successful scan or an
empty-bus result. It does not independently establish the physical FPGA rail
state: MAIN may be consulting reported/cached rail state. Live coprocessor
status was not supplied. No other diagnostic error bits were reported.

## Exact supplied transcription, retained without correction

```text
00000: 46 50 47 41 20 70 6F 77
00008: 65 72 20 7A 6F 6E 65 20
00016: 69 73 20 6E 6F 74 20 65
00024: 6E 61 62 6C 65 64 21 20
00032: 50 6C 65 61 73 65 20 65
00040: 6E 61 62 6C 65 20 74 68
00048: 65 20 75 73 65 20 74 68
00056: 69 73 20 66 75 6E 63 74
00064: 69 6F 6E 61 6C 69 74 79 0A
00072: 5B 69 5C 69 5C 70 20 30
00080: 44 33 38 33 45 31 33 35
00088: 43 38 31 34 39 45 33 20
00096: 34 20 35 30 4O 57 45 52
00104: 5A 4F 4E 45 20 36 20 46
00112: 50 47 41 20 30 30 0A
```

The row-by-row ASCII prefix reads:

```text
FPGA power zone is not enabled! Please enable the use this functionality
```

Do not repair the grammar or assume this is the verified original string.
The supplied newline follows the final `y`, but its labelled offset overlaps
the next row. Relative header text transcribes as command `i\i\p`, timestamp
`0D383E135C8149E3` (hexadecimal), and sequence `4`; the full frame is not yet
byte-verified. The subsequent transcribed text is `50[?]WERZONE 6 FPGA 00`
followed by LF, where `[?]` represents invalid hex `4O`.

| Location | Problem | Required photograph recheck |
| --- | --- | --- |
| Row `00064`, offset `00072` | Nine values instead of eight; `0A` overlaps the next row's `5B`. | All bytes in row 64 and the frame/newline boundary. |
| Offsets 98-99 | `35 30` decode as `50`, not the documented `EP` prefix. | Could be `45 50` if this is `EPOWERZONE`; that is a hypothesis, not a correction. |
| Offset 100 | `4O` is not a hexadecimal byte. | Check both digits; `4F` would decode as `O`, but is unconfirmed. |
| Offsets 116-118 | `30 30 0A` decode as `00` + LF; no closing bracket occurs in the supplied rows. | Recheck the flag/bracket/line ending; `30 5D 0A` would be a conventional failed-frame ending. |

There are **120 transcribed values but only 119 distinct labelled positions**,
because offset 72 appears twice. One value is invalid hex. Concatenating the
rows therefore cannot produce a verified 119-byte response. Respecting row
labels instead creates a byte conflict and an invalid byte. Removing the
newline to force the count would leave the frame after plain text on the same
line, inconsistent with the diagnostic's reported complete-frame recognition.

The documented body `EPOWERZONE 6 FPGA` is consistent with the reported refusal,
but it must not be substituted for the transcription. Photographs/corrected
hex are needed before recording a complete decoded stream or capture hash.

## Official prerequisite and the standalone DISPLAY distinction

Pinned OneWili `b0eeccda21b0594c8062cd17e9755f0c26b0cf6b` documents Poll as
requiring FPGA power zone 6. The source-backed general control is:

```text
Stock command: h\p\s 6 1
Generated C API: ow_hardware_power_management_set_zone(device, 6, 1)
```

The parameters are decimal signed integers. This API is documented for the
stock power-management path, not proof that the diagnostic can use it to move
a rail. The pinned BSP power-channel description says MAIN asks DISPLAY to
apply the single-zone setting via FwGUI command `0x6C`. The diagnostic's pinned
OneWili receive dispatcher handles text/binary/SDFS/stream traffic and discards
other GUI commands. It has no `0x6C` power handler. Thus an acknowledgement from
`h\p\s` cannot be treated as evidence that this standalone app enabled FPGA.

The supported DISPLAY-side rail helpers are
`picpwr_ensure_awake(picpwr_zone_bit(PICPWR_ZONE_FPGA))` for an additive request,
or `picpwr_keep_awake(...)` for ongoing ownership/reassertion. Zone 6 is bit 5,
mask `0x20`. These use the existing coprocessor power driver; no custom driver
is required. `ensure_awake` seeds from two distinct agreeing live status frames,
preserves other powered rails by ORing the bit, retains cached sleep/wake
fields and observes a send-spacing guard. A true return can mean queued/sent,
not completed: confirm the actual rail through fresh `picpwr_rails` status.
Do not replace the whole awake mask with `0x20`.

The existing `ow_fwgui_send_power_zones(actual_live_mask)` reports actual rail
state to MAIN over event 48; it is not a power-enable command. Its public
contract says this lets MAIN initialize zone-gated hardware. That report should
follow verified readback after opening/changing state, rather than telling MAIN
an intended or fabricated mask. Installed MAIN behavior still needs testing.
The BSP's `POWER_ZONES` CMake names do not include `FPGA`; simply adding that
word to metadata would fail configuration at this pin.

## Electrical effects and Poll bus

Zone 6 powers the FPGA and its external memory/SRAM. The additive helper has
no VREF, target-GPIO or I2C-configuration calls, and source shows it does not
explicitly select an external voltage or enable another target rail. VREF
selection is a separate I/O-expander/API operation. Nevertheless, powering the
FPGA and reporting rails can activate/reinitialize hardware. FPGA startup
outputs, level-shifter behavior, MAIN initialization and possible header
transients are not established by the public MAIN/board-manager source scope.
Do not claim external pins, target power or VREF are electrically unchanged.
The helper retains cached settings, not a proven snapshot of every original
board configuration. All targets, header/VREF wiring and accessories must
remain disconnected for the next proposed experiment.

The pinned FW2 pinout associates this IO I2C menu with **MAIN I2C0** on the
20-pin header: **physical pin 10 = SDA / MAIN GPIO16**, **physical pin 8 = SCL /
MAIN GPIO17**. GPIO numbers are not connector pin numbers. This is separate
from DISPLAY's internal I2C1 and the NFC menu. The panel documentation says
I2C0 reaches the header directly, while the API declares the zone-6 guard.
The reported refusal confirms that this installed command path enforces an
FPGA prerequisite; it does not prove that the I2C electrical path goes through
FPGA. Header-buffer component/voltage details in the pinout still have explicit
verification caveats. No wiring recommendation is made here.

## Smallest proposed diagnostic change, approval required

Add a separate explicit manual **FPGA preparation** action before the existing
single-Poll action, using only the official DISPLAY-side helper:

1. Record fresh live rail masks. If FPGA is already on, do not cycle or resend
   power; only report the truthful state to MAIN. This also tests stale MAIN
   power-state reporting as a possible cause.
2. If off, require an explicit manual action and make at most one additive
   `ensure_awake` request for zone 6. No startup enable, whole-mask overwrite,
   automatic power retries, VREF change, GPIO/bus settings or extra zone request.
3. Wait within a finite recovery-serviced deadline for distinct fresh status
   frames confirming FPGA on; preserve/log before/after masks and unexpected
   changes. Unavailable status, rate-limit refusal or timeout ends preparation
   without Poll or another power request.
4. Report the actual live mask through the official OneWili power-state event,
   keep its traffic separate from capture, then permit the existing one manual
   Poll with unchanged raw capture. No assumed address decoder.

This is a candidate for a disconnected-device test, not an electrically
qualified change. Power ownership/what remains enabled after HOME must be
reviewed before approval. Do not automatically disable FPGA or restore a stale
whole-board mask afterward; other CPUs can have acquired a rail meanwhile.
Do not add the ongoing `keep_awake` policy unless its reassertion behavior is
separately approved. No code or configuration was changed in this analysis.

## Primary source evidence

- [Poll prerequisite](https://github.com/freewili/onewili/blob/b0eeccda21b0594c8062cd17e9755f0c26b0cf6b/docs/i2c.md#i2c_poll)
  and [power command](https://github.com/freewili/onewili/blob/b0eeccda21b0594c8062cd17e9755f0c26b0cf6b/docs/power_management.md#set_zone).
- [Generated set-zone implementation, line 5962](https://github.com/freewili/onewili/blob/b0eeccda21b0594c8062cd17e9755f0c26b0cf6b/wilibsp/src/onewili.c#L5962).
- [BSP power channel and zone map](https://github.com/freewili/wilibsp/blob/be4bdd63d31a80f95410e583710cf4e43a7be7fa/docs/drivers/power.md)
  and [additive helper](https://github.com/freewili/wilibsp/blob/be4bdd63d31a80f95410e583710cf4e43a7be7fa/bsp/input/picpwr.c#L79).
- [OneWili GUI-command dispatch](https://github.com/freewili/onewili/blob/b0eeccda21b0594c8062cd17e9755f0c26b0cf6b/wilibsp/src/onewili_fwgui.c#L223)
  and [live-state report contract](https://github.com/freewili/onewili/blob/b0eeccda21b0594c8062cd17e9755f0c26b0cf6b/wilibsp/include/onewili_fwgui.h#L43).
- [External I2C pinout](https://github.com/freewili/freewili2-docs/blob/0a4e3c224fdd4d0b61e6b02f64a14feaba28d725/docs/hardware/pinout.md)
  and [panel topology description](https://github.com/freewili/freewili2-docs/blob/0a4e3c224fdd4d0b61e6b02f64a14feaba28d725/docs/panels/i2c.md#power).

Only documentation was written. No code change, build, install, device access,
power request, commit or push occurred. Exact byte reconstruction remains
blocked on the inconsistent transcription; stop for photograph review and
explicit approval before any diagnostic or hardware change.
