# v003 CAN receive qualification and Glitching pane

Follow-up scope: the original Control Lab is disassembled. The user selects a
standalone UNO R4 Minima plus an unidentified external transceiver and passive
FW2 monitor. Do not require rebuilding the original lab. See the
[bench readiness checklist](CAN_BENCH_READINESS.md) for the unresolved module,
electrical/firmware gates and ACK-node requirement. This does not authorize
physical wiring/testing or deployment.

## Implemented offline

Tools -> Glitching / Fault Injection contains Voltage Glitch, Clock Glitch,
Trigger & Synchronization and Experiment Log. Each is planned, capability
unverified or storage not implemented. Its Connection Guide states that
pinouts, electrical limits, suitable Orcas and supported capabilities require
verification. There are no operational glitch controls or output routines.

CAN navigation: Connection Guide -> Configure -> Listen -> Monitor -> RAM
Capture -> Review. Configure and Listen explain the closed live gate; only
explicitly labelled simulation can continue. No bitrate/channel setter, live
transport, controller initialization, power request or transmit API is linked.
Synthetic frames are added individually by touch, never automatically.

The pure host-tested model accepts normalized classic/FD frames and decodes
the documented `receive_canfd` result body. It checks input length (<512 bytes),
numeric fields, standard/extended IDs, exact payload count and legal lengths.
It rejects malformed/unknown fields without accepting a frame. Timestamp wrap
is retained as unsigned 32-bit uptime; no arrival-time or cross-clock claim.
The eight-frame ring drops oldest with a separate local overwrite count;
upstream queue loss is retained separately. Synthetic RAM capture freezes up
to eight complete frames (including all 64 FD bytes) for review. It is volatile,
not SD storage/export or a complete bus recording. Other instruments remain
unavailable; four home tiles and Back/Home are preserved.

## Pinned source findings

Read-only evidence: WiliBSP `be4bdd63d31a80f95410e583710cf4e43a7be7fa`,
OneWili `b0eeccda21b0594c8062cd17e9755f0c26b0cf6b`. These are pinned sources,
not a claim that the connected firmware was inspected or matches them.

| Topic | Source-established behavior | Unresolved qualification |
| --- | --- | --- |
| Receive APIs | `enable_canfd_stream` (`i\c\o`), queue enable (`i\c\e`) and `receive_canfd` (`i\c\v`). DISPLAY can use official FwGUI binary transport and `ow_binary_poll`. | Installed firmware compatibility and live behavior remain untested. Polling receive automatically arms a queue; it is not a read-only capability probe. |
| Passive mode | CAN1 `c_an1_listen_only()` (`h\s\p\y`) and CAN2 equivalent (`h\s\p\n`) return status only. | No state readback, idempotence, sequencing with mode/rate changes, persistence or electrical no-ACK guarantee established by the generated binding. No configuration attempted. |
| Channel | Source documents channel 0 as the existing FW2 channel; channel 1 is reserved for future Orcas. | Do not offer a second usable channel merely because the API has CAN2 settings. Binary report has no explicit channel field. |
| Bit timing | CAN1 mode/rate/FD data rate setters: `h\s\p\a`, `h\s\p\b`, `h\s\p\c`; arguments are integers. | Generated settings docs do not establish the complete integer-to-rate/mode mapping. No guessed numeric settings or physical defaults. |
| Queue data | Result: frame, queued, dropped, hex ID, extended flag, FD flag, timestampUs, byte length, hex bytes. Empty results preserve dropped count. Depth 32; overflow drops oldest. | Loss counts are scoped to this queue; they are not all controller/transport/bus losses. Classic RTR distinction is not provided by this result. |
| Queue timing | timestampUs is MAIN uptime when drained from controller, 32-bit, wraps about 71 minutes. | Not hardware bus-arrival time; host parser rejects negative signed text rather than guess its interpretation. Live binding conversion needs separate verification. |
| Binary data | `canRxReport`, event 39, exactly 84 bytes: LE u64 ns timestamp at 0, GPIO at 8, R0 ID at 12, R1 header at 16, 64 payload bytes at 20; separate header error bool. Official canblast demonstrates ID reconstruction/DLC lengths. | Binary timestamp provenance/resolution, channel attribution and full header/error semantics need qualification. No custom binary/wire driver implemented. |
| Stream differences | Docs describe stream enabled 0/1; official canblast also uses binary mode 2. | Mode 2 is example evidence, not a resolved documentation/version contract. Prefer a separately qualified official path. |
| Transport loss | FwGUI: IRQ ring 32 KiB, binary stream 32 KiB, text stream 8 KiB; stats include overruns, checksum/length errors and dropped frames. Binary parser counts unknown/size mismatch frames. | Combined drops cannot all be attributed to CAN. Do not display zero loss merely because no adapter is connected. |
| Timeout/recovery | Binary poll is zero-timeout; default command timeout 5 s. BSP recovery-aware OneWili wrapper services HOME between short waits. | A future live adapter needs bounded poll work, explicit command failure/timeout reporting and HOME service on every path. Current main is unchanged. |
| Power | CAN command docs require zone 15. WiliBSP offers CAN metadata/power maintenance. | Enabling CAN changes power policy; it is outside the existing DISPLAY-only startup exception. No CAN rail or power-zone message added. |

Primary pinned references: [CAN API](https://github.com/freewili/onewili/blob/b0eeccda21b0594c8062cd17e9755f0c26b0cf6b/docs/canfd.md),
[settings](https://github.com/freewili/onewili/blob/b0eeccda21b0594c8062cd17e9755f0c26b0cf6b/docs/neptune_settings.md),
[binary event types](https://github.com/freewili/onewili/blob/b0eeccda21b0594c8062cd17e9755f0c26b0cf6b/wilibsp/include/onewili_events.h),
[FwGUI counters](https://github.com/freewili/onewili/blob/b0eeccda21b0594c8062cd17e9755f0c26b0cf6b/wilibsp/include/onewili_fwgui.h),
[official canblast example](https://github.com/freewili/wilibsp/blob/be4bdd63d31a80f95410e583710cf4e43a7be7fa/apps/canblast/main.c).
The example includes transmit/configuration/power operations and must not be
run or copied wholesale as a passive monitor.

## Electrical evidence and closed safety gate

Official [CAN specification](https://freewili.com/specs/can-fd.html) advertises
CAN FD/SIC at 8 Mbit. The current [Humpback page](https://freewili.com/orcas/humpback-orca.html),
checked 2026-10-09, documents existing host transceiver/controller routing,
DB-15 male-pin view with label up, CAN H pin 6, CAN L pin 14, shared signal/power
ground pin 8 and optional DC V+ pin 15. It adds no controller, transceiver or
termination; host termination is software-selectable. This improves the older
pinned guide's documentary gaps but does not verify the user's actual module,
cable orientation, board edition or electrical compatibility. Do not infer
galvanic isolation from the phrase ISO CAN FD. No optional DC V+ connection is
needed to establish a proposed CAN-only plan; no power wiring is prescribed.

Still required: exact board/transceiver ratings and schematic, actual connector
and cable continuity, common-mode/ground/voltage compatibility, termination
state, CAN Control Lab topology and rates, installed firmware's passive/no-ACK
behavior, inherited periodic TX state, mode-setting order and recovery effects.
Live controls remain disabled. Hardware marketing is not electrical qualification.

## Smallest next physical step, only after separate approval

First review the CAN Control Lab schematic, transceiver part, bus supply/common
mode, bitrate and existing two-node/termination arrangement, alongside the
actual FREE-WILi 2/module revision and connector documentation. No pin-to-pin
wiring instruction is qualified yet. Resolve the listen-only state/readback and
startup power-policy contract before developing a live adapter.

Then propose a narrowly approved passive bench qualification with no external
target-power connection, an independently monitored TXD/bus and a separate
known-good receiver/ACK source. Verify that the candidate emits no ACK, error or
data frames, including startup/stop/HOME, and compare received frames/loss with
the lab reference. A transmitter alone cannot verify passive capture while
relying on WiliPirate for ACK. Do not run canblast or perform active diagnostics.
This is a review proposal, not approval to access, connect or operate hardware.

## Offline validation

Final checks: Python regression 62/62 passed; native host CTest 3/3 passed
(navigation, CAN, rendering). Tests cover guide/configure/listen/simulated
monitor/capture/review/Back/Home, held-touch suppression, disabled live gate,
ID boundaries, all FD DLC lengths, invalid/oversize/short/trailing inputs,
timestamp wrap values, zero payload, overflow counters and snapshot freezing.
Renderer checks all pages/touch fallback within 480x320 and complete clears;
Glitching and full 64-byte CAN review host previews were visually inspected.
The first sandboxed host compilation failed with AccessDenied; the same cached
compiler succeeded with approved host-only escalation. No tool was installed.

Local documentation links, `git diff --check`, archive hashes and preserved
UF2 identities/memory-region checks passed. Protected M2 source, validated
release and immutable hardware record show no Git diff. No v003 UF2 is generated:
the prior startup exception covered one v002 offline candidate only, and this
task explicitly preserves that boundary. Existing v002, M2 and M3 bytes are
checked for identity. No device enumeration, installation, launch or Git write.
