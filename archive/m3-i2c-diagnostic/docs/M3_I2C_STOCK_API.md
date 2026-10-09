# M3 I2C Phase 1: stock API investigation

Date: 2026-10-08. WiliPirate branch: `feature/wilipirate-panel`.
Baseline: `68e3d35bc14915acdff2b6c47dc9bb0c455c60bb`.
Documentation/source inspection only. No implementation, build, hardware access,
deployment, dependency modification, firmware change, commit or push.

## Decision

The existing stock Poll command is callable from a native DISPLAY application
through official OneWili/FwGUI APIs. A supported, complete detected-address
result path is **not yet established**. Do not implement a replacement scanner
or infer a device list from success status. The MAIN handler and its response
schema remain unavailable in the public sources inspected.

Two separate information losses are confirmed: the generated Poll wrapper
discards the response body, and the generic response API strips response
identity fields. Generic hooks are public APIs, but their existence does not
resolve the stock result schema or fail-closed correlation requirement.

## Pinned scope and sources

The local DISPLAY `wilibsp/` submodule is uninitialized. Its pinned public
sources were inspected remotely, without populating or modifying dependencies.
The GitHub tree confirms its OneWili gitlink; no newer revision was substituted.
The old CM0 OneWili pin in `docs/SOURCES.json` is not the DISPLAY dependency.

| Source | Revision | Relevant files |
| --- | --- | --- |
| WiliBSP | `be4bdd63d31a80f95410e583710cf4e43a7be7fa` | `libs/onewili` gitlink; recovery adapter |
| DISPLAY OneWili | `b0eeccda21b0594c8062cd17e9755f0c26b0cf6b` | generated C API, parser, FwGUI transport, I2C docs/manifest |
| FW2 documentation | `0a4e3c224fdd4d0b61e6b02f64a14feaba28d725` | I2C panel and console command descriptions |
| FW2 firmware distribution | `3d3302c701f2345b2131bc5da1edfa9f86ef5dcd` | release images/manifests; no MAIN handler implementation |

Source links used throughout this report address those exact revisions.
The [firmware distribution tree](https://github.com/freewili/FREE-WILi2-Firmware/tree/3d3302c701f2345b2131bc5da1edfa9f86ef5dcd)
contains firmware images and packaging scripts, not `fwMenuI2C` or its driver.
The [generated feature page](https://github.com/freewili/freewili2-docs/blob/0a4e3c224fdd4d0b61e6b02f64a14feaba28d725/docs/features/i2c.md)
names `MenuX/fwMenuI2CConfig.h` as input; that header was not available in the
inspected public trees. A complete source trace into MAIN hardware is blocked
here, not replaced with guessed function names.

## Command trace and result contract

```text
Future explicit WiliPirate SCAN action (not implemented)
  -> official generated Poll or public raw command hook
  -> 0x02 + i\i\p + newline
  -> official FwGUI transport over internal UART0
  -> stock MAIN OneWili dispatch / fwMenuI2C Poll [implementation unavailable]
  -> existing MAIN external I2C hardware operation [driver details unavailable]
  -> response/event delivery [Poll body and address schema unverified]
  -> WiliPirate device list [not yet qualified]
```

The [I2C reference](https://github.com/freewili/onewili/blob/b0eeccda21b0594c8062cd17e9755f0c26b0cf6b/docs/i2c.md#i2c_poll)
and [manifest](https://github.com/freewili/onewili/blob/b0eeccda21b0594c8062cd17e9755f0c26b0cf6b/python/onewili/api_manifest.json)
specify path `i\i\p`, no parameters and no typed return value. Documentation
also says master commands fail while I2C slave mode is active.

The [DISPLAY transport](https://github.com/freewili/onewili/blob/b0eeccda21b0594c8062cd17e9755f0c26b0cf6b/wilibsp/src/onewili_fwgui.c#L16)
uses UART0 at 8 Mbaud with GPIO0-3 hardware flow control. It packages command
bytes as FwGUI terminal-input events (`0x18`, chunk marker `0x01`) and accepts
OneWili text responses via command `0x5D`. Other GUI command traffic is discarded
by this transport; the existence of stock display logs does not establish an
application-visible text result. See receive dispatch at lines 223-228.

The [stock panel description](https://github.com/freewili/freewili2-docs/blob/0a4e3c224fdd4d0b61e6b02f64a14feaba28d725/docs/panels/i2c.md#reading-the-log)
describes probing addresses 0-126 while skipping reserved addresses, per-address
hexadecimal address/read-byte logs, and a final device count. It records logs
only while that stock screen is displayed. This is documented panel behavior,
not evidence that console Poll uses the same implementation or exports that
list over the DISPLAY OneWili text route. Exact address encoding, empty-bus
output, response ordering and completion markers remain unverified.

The [common parser](https://github.com/freewili/onewili/blob/b0eeccda21b0594c8062cd17e9755f0c26b0cf6b/wilibsp/src/onewili.c#L40)
expects a response envelope with command path, hexadecimal timestamp, sequence,
body and final success flag. Spontaneous text events have a separate `[*id ...]`
form. Documented `i2cmon` and `i2cslv` events carry monitor/slave bytes, not a
documented Poll address list. A raw empty body or success flag cannot establish
that zero devices were detected.

## Information retained and lost

| Public interface | What the caller receives | Limitation |
| --- | --- | --- |
| `ow_io_i2c_i2c_poll` | `ow_status` | Lines 998-1006 allocate/read a response, discard its body, then return status. No addresses or diagnostic body escape. |
| `ow_raw_send` | Bytes submitted or negative error | Sends the existing command without waiting; submission is not MAIN completion. |
| `ow_raw_next_response` | Parsed body, separate `ok`, API status | Lines 354-369 use the same parser; command path, timestamp and sequence are not exposed or matched to the request. |
| `ow_poll_text_line` | Event ID and arguments | Not a scan schema; can consume a response into a short global stash. |

References: [generated Poll](https://github.com/freewili/onewili/blob/b0eeccda21b0594c8062cd17e9755f0c26b0cf6b/wilibsp/src/onewili.c#L998),
[raw hook declarations](https://github.com/freewili/onewili/blob/b0eeccda21b0594c8062cd17e9755f0c26b0cf6b/wilibsp/include/onewili.h#L77),
[raw response implementation](https://github.com/freewili/onewili/blob/b0eeccda21b0594c8062cd17e9755f0c26b0cf6b/wilibsp/src/onewili.c#L322).

The raw hooks preserve the parsed middle, not the complete wire envelope. The
parser skips identity fields without validating them. Single-flight operation
reduces overlap but does not prove stale or unrelated responses cannot be
accepted. The existing [CM0 investigation](M1E_1_RESPONSE_VALIDATION.md) recorded
this class of failure; source inspection confirms the limitation in the
separate pinned DISPLAY package. No DISPLAY runtime regression was executed.

The raw response stash is global, holds 32 frames, and stores at most 191
characters plus a terminator per frame. Long responses are truncated before
parsing, even though the main response buffer defaults to 4096 bytes. Stash
overflow is counted; this is not proof that every truncation is separately
reported. Do not assume event polling interleaved with raw reception preserves
a long scan list. Event queues can also drop older events when full.

## Errors, timeout and recovery

- Generated calls map a false completion flag to `OW_ERR_FAILED`. Raw receive
  can return `OW_OK` for a parsed error frame with `ok == 0`; both must be checked.
- Other statuses distinguish argument, I/O, timeout, protocol and buffer errors.
  These are not empty-bus results. Poll-specific NACK, stuck-bus, clock-stretch
  limits, partial results and scan cancellation remain unknown.
- The default read timeout is 5000 ms, passed repeatedly to transport reads.
  It is not a demonstrated five-second end-to-end scan deadline: fragmented
  input or unrelated traffic can extend the wait. Raw receive accepts an
  explicit timeout; zero requests nonblocking reception.
- FwGUI reads compute a deadline per read. Command transmission uses blocking
  UART writes; no explicit write timeout was established. MAIN hardware scan
  duration and timeout do not follow from the host-side read timeout.
- Use the official recovery-aware opener for any later qualified integration.
  It services recovery during open and slices subsequent reads into at most
  10 ms intervals. It does not establish an overall scan/write deadline.
- Transport open also arms SD access. Returning `OW_OK` from open is not proof
  MAIN accepted Poll; SD-arm failures are explicitly nonfatal in that opener.

References: [status/default limits](https://github.com/freewili/onewili/blob/b0eeccda21b0594c8062cd17e9755f0c26b0cf6b/wilibsp/include/onewili.h#L12),
[synchronous read/call](https://github.com/freewili/onewili/blob/b0eeccda21b0594c8062cd17e9755f0c26b0cf6b/wilibsp/src/onewili.c#L118),
[FwGUI read/open](https://github.com/freewili/onewili/blob/b0eeccda21b0594c8062cd17e9755f0c26b0cf6b/wilibsp/src/onewili_fwgui.c#L360),
[recovery adapter](https://github.com/freewili/wilibsp/blob/be4bdd63d31a80f95410e583710cf4e43a7be7fa/bsp/input/app_recovery_onewili.h).

## Electrical and installed-firmware gaps

OneWili's reference/manifest requires FPGA zone 6 for Poll. Stock FW2 panel docs
say MAIN I2C0 reaches the header directly and does not need that zone. The
actual dispatcher gating and installed version must resolve this discrepancy;
do not automatically enable a rail to bypass an error. The
[error reference](https://github.com/freewili/onewili/blob/b0eeccda21b0594c8062cd17e9755f0c26b0cf6b/docs/errors.md)
documents `EPOWERZONE` refusal, but does not qualify this installed Poll path.

VIO selection/voltage, pull-ups, frequency, bus ownership, connector pinout and
peripheral read side effects require separate verification. Scanning is active
bus traffic, and documented one-byte reads may change a peripheral's state.
The PN532 target's particular mode/address/electrical setup is not qualified
by this investigation. M2's single-device v07 USB descriptor is not a complete
firmware inventory or proof these pinned APIs match the installed MAIN.

## Smallest supported approach and next step

The smallest candidate is one explicit Scan action, the official
`onewili_fwgui` library, recovery-aware opening, and one stock Poll request.
Keep MAIN's existing bus driver, use no per-address replacement scan loop, and
leave the other five tools unavailable. Receiving a body through the public
raw hooks is a candidate only if an official result schema and safe correlation
mechanism can be established. Neither is proved here; implementation should
remain gated. No custom transport/parser bypass or upstream patch is proposed.

First obtain official handler/result documentation or an official supported
full-envelope/correlated response interface. Then propose a separately approved,
controlled capture experiment on the actual DISPLAY route: identify firmware
versions and electrical settings, capture one stock Poll on an empty bus and
one known compatible peripheral, record all response/event bytes, verify empty,
positive and error outcomes, elapsed time, recovery and stale-response behavior.
A PC console capture alone does not establish DISPLAY-route delivery. Do not
simulate faults by shorting or miswiring the bus. Observation requires a capture
surface that retains identity; the current raw-return API alone cannot prove it.

After those gates, the smallest implementation would expose Scan and render
only validated stock results, with an explicit unavailable/error state for
missing capability, malformed/incomplete data or failed correlation. No
automatic retry, mode/power reconfiguration or fabricated empty list.

## Phase 1 checks and stop

Document links/anchors, Markdown fences, whitespace, baseline identity,
protected-file scope and the preserved M2 UF2 checksum were checked locally.
No host suite was run: this task changes documentation only and prohibits
builds. No hardware behavior was experimentally verified. Earlier evidence
files and official dependencies remain unchanged. Stop for review; the proposed
capture and implementation are not authorized by this phase.

## Phase 2: response-path investigation

Date: 2026-10-08. Same baseline and dependency pins as Phase 1. This section
refines the earlier recommendation: a full-text observation path is demonstrated
by an official DISPLAY application. Poll's actual body remains unknown, but a
supported transport observation pattern is no longer an entirely missing gate.
No code or hardware execution was performed.

### Official full-text observation pattern

The pinned [canblast RAW diagnostic](https://github.com/freewili/wilibsp/blob/be4bdd63d31a80f95410e583710cf4e43a7be7fa/apps/canblast/main.c#L438)
submits a command through `ow_raw_send`, then reads `dev.t.read` directly and
prints escaped text-stream bytes before the common response parser runs. Its
512-byte read chunks do not represent a whole-response size limit. This exposes
the echoed path, timestamp, sequence, body, final flag and any text events
present in the official OneWili text route. It does not expose discarded GUI
traffic or prove that Poll produces addresses on that route.

This is an existing official application pattern, not a proposed new UART or
I2C driver. It is a source reference only: canblast startup enables CAN power,
configures CAN and enables streams ([lines 639-673](https://github.com/freewili/wilibsp/blob/be4bdd63d31a80f95410e583710cf4e43a7be7fa/apps/canblast/main.c#L639)).
Running that application would exceed an isolated I2C investigation's scope.
Its 700 ms capture window is a debug choice, not a validated Poll timeout.

The official [toggleled](https://github.com/freewili/wilibsp/blob/be4bdd63d31a80f95410e583710cf4e43a7be7fa/apps/toggleled/main.c#L30)
and [hello_sdcard](https://github.com/freewili/wilibsp/blob/be4bdd63d31a80f95410e583710cf4e43a7be7fa/apps/hello_sdcard/main.c#L66)
examples explicitly say opening is not a MAIN handshake; failed operations
surface later. Both use recovery-aware opening. These are lifecycle examples,
not external-I2C address-result examples. None of the four inspected OneWili
application bodies (also including dualcpu) provides a qualified Poll decoder.

### Correlation: FIFO labels versus actual response identity

The official [fast-CAN header](https://github.com/freewili/onewili/blob/b0eeccda21b0594c8062cd17e9755f0c26b0cf6b/wilibsp/include/onewili_fast.h#L1)
documents in-order MAIN command execution/replies for its pipeline. The
[implementation](https://github.com/freewili/onewili/blob/b0eeccda21b0594c8062cd17e9755f0c26b0cf6b/wilibsp/src/onewili_fast.c#L29)
stores user tokens in a local FIFO and returns the next token when the next
raw response arrives. Tokens are not added as wire request IDs or recovered
from the response. This demonstrates an official ordering assumption, not
verification of Poll's echoed path or immunity to stale/unsolicited replies.
The helper can count a parse failure as an answer; do not reuse that policy
to certify a scan result. It is a CAN helper, not a generic correlated Poll API.

Official PC Python provides a more defensive reference:
[ResponseFrame](https://github.com/freewili/onewili/blob/b0eeccda21b0594c8062cd17e9755f0c26b0cf6b/python/onewili/framing.py#L40)
retains path, timestamp, sequence, response and success;
[MenuBase](https://github.com/freewili/onewili/blob/b0eeccda21b0594c8062cd17e9755f0c26b0cf6b/python/onewili/menubase.py#L15)
checks the echoed path against the request. Its generated Poll still returns
`Ok(None)` on success, not an address list. The PC parser may help interpret a
captured frame offline; it is not an approved CM0 Python adapter or proof that
the DISPLAY route receives identical content.

Full-text DISPLAY capture makes path checking possible without changing the
official transport. Sequence/timestamp fields should be retained as evidence,
but their source generation, reset behavior and relationship to request order
remain unverified. No request-ID argument is documented for Poll. Do not invent
one or treat the response sequence as an echoed application token. Path matching
cannot distinguish a late reply to an earlier Poll on the same path.

For the first experiment, isolate one Poll per clean diagnostic launch with no
concurrent API calls, record/drain pending input before sending, retain the full
capture, and do not retry after timeout. Draining local data does not cancel an
in-flight MAIN operation. Production correlation after timeout/reconnect needs
separate qualification even if the initial captures succeed.

### Response capacity and timeout: what supported APIs can handle

| Layer | Established limit or behavior | Supported handling and remaining gap |
| --- | --- | --- |
| FwGUI link | 512-byte incoming frame payload; text frames feed an 8192-byte FIFO; UART IRQ ring is 32768 bytes. | Chunked transport reads preserve longer text streams when serviced fast enough. No Poll-size upper bound or delivery guarantee is established. |
| Common C parser | Default `OW_RESP_MAX` is 4096, including frame/reassembly storage; response output also needs a terminator. | Buffer overflow returns an error. Header permits compile-time override, consistently for library and application; that requires a separately approved build and does not fix other limits. |
| Raw response stash | 191 characters per stored frame; constants are internal and fixed. | Receiving directly through the official transport avoids this stash. A larger output buffer alone cannot restore text already truncated there. |
| Observation capture | Raw transport reads carry text fragments, not decoded addresses. | Accumulate in a bounded diagnostic buffer, retain lengths/bytes and mark overflow/incomplete capture as inconclusive. Do not run another consumer on the same text stream. |
| Receive deadline | Transport read deadline applies per read, not total command completion. | A collector can use nonblocking or short reads and its own elapsed-time budget while servicing recovery. UART send and MAIN scan duration remain unbounded by that budget. |

Sources: [FwGUI sizes](https://github.com/freewili/onewili/blob/b0eeccda21b0594c8062cd17e9755f0c26b0cf6b/wilibsp/src/onewili_fwgui.c#L37),
[compile-time defaults](https://github.com/freewili/onewili/blob/b0eeccda21b0594c8062cd17e9755f0c26b0cf6b/wilibsp/include/onewili.h#L26),
[fixed stash](https://github.com/freewili/onewili/blob/b0eeccda21b0594c8062cd17e9755f0c26b0cf6b/wilibsp/src/onewili.c#L335).

The [read-line loop](https://github.com/freewili/onewili/blob/b0eeccda21b0594c8062cd17e9755f0c26b0cf6b/wilibsp/src/onewili.c#L118)
can continue processing events/fragments before returning. Thus a zero-timeout
`ow_raw_next_response` does not itself establish a strict total CPU-time budget
under continuous traffic. Direct chunk reads in the official RAW pattern avoid
that parser loop. The FwGUI pump drains a snapshot of the IRQ ring, but no new
hard real-time guarantee is inferred from source inspection.

Use [link-health APIs](https://github.com/freewili/onewili/blob/b0eeccda21b0594c8062cd17e9755f0c26b0cf6b/wilibsp/include/onewili_fwgui.h#L49)
to snapshot overruns, dropped frames, checksum/length errors and text high-water
marks before/after capture. Any loss, capture overflow or missing complete
response invalidates a claimed complete result. Buffering bytes before rendering
is preferable to assuming RTT printing itself cannot delay reception.

### Power-zone discrepancy: partial resolution

The disagreement concerns two distinct layers:

- The stock panel description says the physical I2C0/header route does not
  require the FPGA; this is documentary evidence of electrical routing.
- The generated command manifest marks Poll as requiring zone 6. Public C
  Poll code sends the command without a local power-zone test; it neither
  enforces that metadata nor automatically switches the rail.
- The error documentation describes MAIN-side power refusal, but the actual
  MAIN dispatcher/handler implementation and installed version are absent.
  Whether this Poll path enforces an obsolete/generalized zone annotation
  cannot be resolved from the available source.

Therefore do not silently decide the metadata is harmless or switch FPGA power
on as a workaround. Record existing power state and actual refusal/success.
State reporting through official APIs is distinct from changing rail ownership;
any power-state experiment needs explicit approval and an agreed electrical plan.
Boot/grace-period behavior described in the error docs also means one immediate
success would not prove steady-state zone enforcement is absent.

### Smallest controlled experiment still needed

Propose a separately authorized diagnostic DISPLAY RAM application using only
the official RAW observation pattern and recovery lifecycle, separate from M2.
This phase does not authorize its implementation, build, deployment or execution.
Do not run canblast to obtain this capture and do not alter the validated UF2.

1. Before authorizing a run, identify installed MAIN/DISPLAY/loader versions,
   verify connector/VIO/pull-up/frequency settings and expected peripheral read
   behavior, define capture capacity and a finite receive window, and agree how
   capture bytes will be retrieved. Include normal BSP startup and SD-client
   open side effects in the approval. Keep HOME recovery available.
2. With no external target, record pre-existing rail/link state and pending text,
   then issue exactly one stock Poll. Capture the complete text stream before
   generated parsing, with timing and before/after loss counters. On refusal,
   missing reply or overflow, report that result and stop; do not enable rails,
   repeat Poll or interpret it as an empty bus.
3. If the first result is complete and electrical prerequisites are confirmed,
   perform a separately approved fresh diagnostic launch with one known safe
   I2C peripheral, again one Poll. Compare the full frame/event capture against
   the empty-bus case and independently known 7-bit address. The spare PN532 is
   a candidate only after its particular mode, voltage, pulls and read semantics
   have been verified; ORCA is not a prerequisite.

This pair of successful captures is the smallest positive/negative observation
needed to discover whether Poll exports addresses, only status/count, or another
result. Preserve the exact body; do not predefine its schema. A success flag
without exported address data fails the address-list integration gate.

Do not deliberately change rail state or inject bus/transport faults in this
first experiment. Those are later qualification tasks. The pair cannot by
itself prove maximum response size, stale-reply rejection, blocked-write timeout,
stuck-bus recovery or zone-off enforcement. If a zone refusal prevents either
capture, review the exact error and power policy before proposing another run.

### Phase 2 outcome and checks

Source establishes a supported full-text DISPLAY capture pattern, FIFO-only
official fast correlation, a PC path-checking reference, buffer-loss diagnostics
and the absence of a local Poll power guard. It does not establish Poll's actual
address payload or MAIN hardware/error semantics. The next action is the bounded
capture proposal above, not a functional I2C implementation.

Documentation links, Markdown and whitespace, baseline/protected-file identity
and M2 checksum were checked. Only this investigation document was updated.
No upstream files, M2 source or artifact, earlier evidence, commits or remotes
were changed. Stop for review.

## Phase 3 stopped checkpoint

The user subsequently authorized a separate offline diagnostic application.
Its sources and instructions are under [native/i2c_diag](../native/i2c_diag/README.md).
The capture core's 14 synthetic cases and all 55 Python host tests passed.
The SDK host picotool build remains unfinished; no diagnostic target was built
and no diagnostic UF2 or physical capture exists. The user stopped all builds
and troubleshooting and authorized a local checkpoint only. See
[the unfinished-build record](../native/i2c_diag/VALIDATION.md) before any later
resume. No hardware work, deployment or push is authorized by that checkpoint.

## First reported physical Poll and power follow-up

The user subsequently reports successful diagnostic launch/HOME recovery and
one complete failed Poll frame with 119 captured bytes and error `0x1000`.
The supplied RAW HEX transcription contains overlapping offsets, invalid hex
and an inconsistent closing token, so the exact full response is not yet
reconstructed. It does report an FPGA-power refusal, confirming a software
prerequisite on this installed command path, not a successful scan or proof of
the physical rail state.

See [the preserved transcription, uncertain bytes and source-backed power
recommendation](../native/i2c_diag/PHYSICAL_RESULT_01.md). The official stock
command is `h\p\s 6 1`, but standalone DISPLAY integration should use the
existing coprocessor power helper plus actual readback/state reporting; the
current OneWili transport discards the stock DISPLAY power-control GUI request.
Any proposed manual zone-6 preparation remains unimplemented and requires
separate approval. No hardware was accessed during this follow-up analysis.
