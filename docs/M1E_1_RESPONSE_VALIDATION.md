# M1E.1 response-validation investigation

Date: 2026-10-05. Branch: feature/wili-ui. Baseline: 0cbf532.
Investigation and isolated QEMU regression tests only. Production source,
submodule revisions and BSP sources are unchanged. No deployment candidate,
physical connection, hardware operation, additional package installation,
firmware change or main merge occurred.

## End-of-session resume checkpoint

```yaml
CURRENT: M1E.1 complete

BLOCKER: >-
  OneWili C response handling does not enforce request/response
  command-path equality.

CONSTRAINT: >-
  Official and vendored FREE-WILi/OneWili/WiliCM0BSP sources are
  read-only and must not be modified.

NEXT: >-
  Determine whether WiliPirate can implement its own fail-closed
  response-validation boundary without modifying upstream/vendored code.
  If not, reconsider the supported application architecture.
```

A real AArch64 binary was built in the authenticated Debian 13/Trixie root;
ARM64 ABI/runtime qualification passed against that recorded baseline.
The binary is **not deployment-qualified**. All three wrong-command
regressions remain intact and failing. No physical FREE-WILi testing occurred.
The next-session direction above supersedes this investigation's earlier
proposal to seek an upstream parser correction; do not modify dependencies.
Development is stopped for tonight. No parser fix or further milestone work.

## Root cause, from the pinned source

Pinned WiliCM0BSP: d27edf1c18bc2c18a76c4c9cdbf200c19884cc06.
Pinned OneWili: 9ce9df83b89f83681507f19f960958e23f20ac37.

1. native/cm0/main.cpp calls ow_gui_panels_read_buttons(bridge.get(), &pressed).
   The generated C binding constructs g\\c\\e and calls ow__call.
2. ow__call retains the complete request string in cmd. It writes quiet-reset
   byte 0x02, the command, and newline through ow_device.t.write.
3. wilicm0::Device::write_bytes sends those bytes on its existing AF_UNIX
   socket. The adapter does not parse command/reply identity. read_bytes returns
   the peer's bytes unchanged, with bounded I/O and no alternate transport.
4. ow__read_line reassembles the response, routes spontaneous [*...] text
   events separately, and supplies a standard response line. Command identity
   is still present as the first token after '['.
5. ow__call accepts the next standard response frame. Although both cmd and
   linebuf are available here, it never compares their command paths.
6. ow__parse_frame scans to the third space, skips path/timestamp/sequence,
   extracts the middle payload and trailing success flag. It neither returns
   nor checks the skipped command identity. This is where identity is discarded.
7. ow_gui_panels_read_buttons parses the payload as hexadecimal and returns
   OW_OK. main.cpp's checked() only sees status, then passes pressed to
   wilipirate::buttons. Hex 10 becomes Exit; hex 2 becomes Mode. Those are valid
   values from an invalidly correlated response.

Thus a g\\c\\e request followed by [i\\g\\u 1 1 10 1] exits successfully.
There is no GPIO request: i\\g\\u is fake response text, supplied only by the
isolated test peer. The test transcript allowlist checks all actual requests.

The same parser is used inside Device's mandatory constructor Device State
probe (h\\a\\g). A wrong-path frame carrying a plausible Device State payload
passes the probe and permits UI creation before the application receives get().

The framing comment in C describes [path hexTimestampNs seq response... ok].
The pinned Python framing.py calls path the echoed menu path. Its frame-path
test explains firmware emission as szMenuPrefix plus the current menu char.
These sources support exact echoed-path equality. The request itself carries
no sequence identifier, so this investigation does not invent an expected
sequence value, monotonic rule, timestamp freshness rule, or authentication
guarantee. Exact path correlation alone does not reject stale same-path replies.

Source evidence:
[C parser and bindings](https://github.com/freewili/onewili/blob/9ce9df83b89f83681507f19f960958e23f20ac37/c/src/onewili.c),
[official socket adapter](https://github.com/freewili/wilicm0bsp/blob/d27edf1c18bc2c18a76c4c9cdbf200c19884cc06/bsp/src/device.cpp),
[Python framing](https://github.com/freewili/onewili/blob/9ce9df83b89f83681507f19f960958e23f20ac37/python/onewili/framing.py),
[frame-path test](https://github.com/freewili/onewili/blob/9ce9df83b89f83681507f19f960958e23f20ac37/python/tests/test_frame_paths.py).

## Official upstream findings

Read live GitHub API commit, branch and all issue/PR metadata; downloaded
revision-addressed source only to ignored build/m1e1-research. No Git fetch,
checkout or pin update was performed.

| Repository/revision | Finding |
| --- | --- |
| WiliCM0BSP main d27edf1c18bc2c18a76c4c9cdbf200c19884cc06 | Still our pinned revision; no newer main adapter fix |
| WiliCM0BSP open PR #3 head 13e238ec5326d87cff2c4e1588d30189498ddfb7 | device.cpp identical to pinned source after line-ending normalization |
| OneWili main b0eeccda21b0594c8062cd17e9755f0c26b0cf6b | Regeneration for ESP32 Mode, peer streams and ISO-TP; C correlation defect remains |
| OneWili PR #6 head ab77ef8629537753aa1b795fc58a2fe946f94fdf | Same relevant C routines as our pin |

Normalized function comparisons at both OneWili revisions found
ow__parse_frame, ow__call and ow_gui_panels_read_buttons identical to the pin
(1049, 931 and 476 characters respectively). Advancing OneWili to current main
is therefore insufficient. The existing BSP pin also still selects the older
OneWili; a BSP update alone cannot be claimed to fix this.

Older fast-FwGUI branches 643cc1b1bb8f3da1d212859047af4a6e0397d028 and
e3613be24e4a0c2971e50be062cade395bc10532 date to 2026-09-20, concern the
WiliBSP transport and do not provide the current c/src/onewili.c path. They
are not evidence of a newer CM0 C parser correction.

The issue lists contained six OneWili PRs and three BSP PRs; none was identified
as a command/reply correlation fix. This is a bounded public-repository finding,
not a claim about private firmware issues, inaccessible generator sources,
every historical example or future changes.

Important existing upstream precedent: Python MenuBase._call already rejects
frame.path != path BEFORE checking success/decoding response fields. Its
test_menubase_path_check.py covers matching, mismatched and root-level paths.
This is an upstream implementation of the required identity rule, not a fix
in the C path. No Python transport/adapter was executed or substituted.

Revision links:
[OneWili main examined](https://github.com/freewili/onewili/commit/b0eeccda21b0594c8062cd17e9755f0c26b0cf6b),
[OneWili PR #6](https://github.com/freewili/onewili/pull/6),
[BSP PR #3 adapter](https://github.com/freewili/wilicm0bsp/blob/13e238ec5326d87cff2c4e1588d30189498ddfb7/bsp/src/device.cpp),
[Python path validation](https://github.com/freewili/onewili/blob/9ce9df83b89f83681507f19f960958e23f20ac37/python/onewili/menubase.py),
[Python mismatch regression](https://github.com/freewili/onewili/blob/9ce9df83b89f83681507f19f960958e23f20ac37/python/tests/test_menubase_path_check.py).

## Options evaluated in requested order

**A — Upstream-supported fix:** no published C fix found. A simple dependency
advance is not sufficient. The Python exact-path rejection is the appropriate
upstream precedent to carry into the C generator/runtime.

**B — Validate above Device using public results:** insufficient. get() returns
ow_device*, but generated functions return only status and decoded values.
ow_raw_next_response also returns payload and success, discarding identity in
the same parser. The mutable line buffer is reassembly storage, not a durable
last-response metadata API. A status/value check, permitted button-mask check,
second request or post-call inspection cannot distinguish identical payloads
from the right and wrong paths. No complete B prototype is possible using
those returned values alone.

**C — WiliPirate-owned callback wrapper:** the public ow_device layout exposes
ow_transport callbacks, so it is technically possible to decorate the adapter's
read/write callbacks after get(). That could track outbound paths and reject
inbound mismatches before OneWili sees their payloads, while delegating socket
I/O to the original callbacks. It would need bounded fragmented-frame handling,
event discrimination, terminal error latching, teardown/lifetime safety, and
an explicit policy for malformed frames and mismatches (reject, never skip and
continue). However, it cannot guard the constructor probe that has already
completed. Device has no public pre-probe callback hook or deferred-open API.
The new constructor-probe test demonstrates this gap. Mutating callbacks is
also not documented as a supported Device extension. A local correlating socket
proxy could precede construction, but adds a protocol/session implementation
and is not the smallest supported solution. No partial wrapper was installed
or presented as a complete fix; startup validation would remain UNVERIFIED.

**D — Parser/adapter modification:** the smallest complete change is an exact
request-path/response-path comparison in the common synchronous C call path,
before payload decoding. It automatically covers both constructor preflight
and application GUI calls with the existing official socket adapter. Keep all
transport and ownership behavior unchanged. A mismatch must immediately return
OW_ERR_PROTOCOL, not wait for another frame or retry the request. No guessed
sequence correlation is needed for the identified command-type invariant.

Recommendation: request an upstream generator/runtime correction and published
regenerated OneWili revision, modeled on the existing Python path check, then
separately approve qualification and pin advancement. OneWili's
[maintenance instructions](https://github.com/freewili/onewili/blob/9ce9df83b89f83681507f19f960958e23f20ac37/AGENTS.md)
say generated bindings must be fixed in the generator and regenerated, not
hand-patched. A locally approved vendored patch is the last resort and would
require explicit scope/deviation approval. Neither was implemented here.

## Regression evidence and stop point

Preserved the original failing wrong-command test unchanged. Added wrong-command
mode-change, wrong-command constructor probe and malformed-frame cases.
Executed the production ARM64 binary with the production Device adapter under
QEMU against the isolated fake bridge. Nine cases: six passed, three failed.

| Case | Result |
| --- | --- |
| Matching replies: startup, Help, all modes, Exit/cleanup/EOF | PASS |
| Unavailable bridge | PASS |
| Disconnect while polling | PASS |
| Nonhex button payload | PASS |
| Malformed identity payload | PASS |
| Malformed frame header | PASS |
| Wrong command interpreted as Exit | FAIL, unchanged regression |
| Wrong command interpreted as Mode | FAIL, UI value update was emitted |
| Wrong command accepted for constructor probe | FAIL, UI startup commands followed |

These failures establish that neither the model/UI layer nor a post-constructor
wrapper can be claimed to cover the entire invariant today. The original six
cases remain present. No failing case was weakened, marked expected failure,
removed or replaced by a passing syntax check. Existing runtime code was not
changed, so the previously recorded full regression results still apply;
the expanded native cases were rerun. git diff --check passed.

STOP: no dependency update, vendored patch or qualified deployment package.
Approval is needed before changing the dependency/parser. Upstream contact was
not performed: this report is a local proposal, not a filed issue or PR.
