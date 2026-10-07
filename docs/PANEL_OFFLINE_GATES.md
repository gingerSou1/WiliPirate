# Offline architecture gates: stock I2C results and startup preservation

Date: 2026-10-07. Baseline: `feature/wilipirate-panel` at `68f9cd4`.
Documentation-only investigation. No compilation, staging, deployment, device
enumeration, physical connection, firmware changes or upstream edits.

Follow-up: [existing application lifecycle investigation](FW2_APP_LIFECYCLES.md)
inspects the official app-contract branches and their exact BSP pins, not only
default-branch apps. It confirms actual loadable touch-UI/MAIN compositions,
while finding the same reachable expander initialization and reboot exit.

## Decision

| Gate | Verdict | Scope |
| --- | --- | --- |
| A: existing stock Poll -> addresses in WiliPirate | **UNVERIFIED** | Public generic response hooks exist, but public sources do not establish the MAIN handler or its exact Poll response body. |
| B: no transient VIO/power change across RAM app launch/use/exit | **PROVEN UNSUITABLE** | The examined full/inherited/PSRAM board initialization paths write VREF and power-control outputs. A different composed minimal lifecycle remains UNVERIFIED. |
| Setup & Actions as a custom-panel extension point | **UNVERIFIED** | Existing command browsing is documented; custom application/panel registration without a firmware build is not established. |

Recommend **D. Stop—the panel concept cannot currently meet the preservation
requirements using a proven supported path.** This is a bounded source finding,
not proof that every possible future stock application mechanism is impossible.
Neither A nor B permits implementation today. Do not implement a scan loop,
patch dependencies or relax the launch/use/exit invariant.

## Source scope and currency

Rechecked official GitHub HEADs on this date; all four principal revisions match
the previous report:

| Repository | Revision |
| --- | --- |
| WiliBSP (master) | `be4bdd63d31a80f95410e583710cf4e43a7be7fa` |
| OneWili (main; also BSP gitlink) | `b0eeccda21b0594c8062cd17e9755f0c26b0cf6b` |
| freewili2-docs | `0a4e3c224fdd4d0b61e6b02f64a14feaba28d725` |
| FREE-WILi2-Firmware | `3d3302c701f2345b2131bc5da1edfa9f86ef5dcd` |

Read the official organization repository inventory, principal branch lists,
OneWili default-branch history (13 entries returned), recent BSP history
(100 entries returned), complete returned histories of BSP `board.c` (8),
`ioexp.c` (6), and OneWili `wilibsp/src/onewili.c` (3). Inspected public app
snapshots listed below. Source archives and API responses are retained under
ignored `build/panel-research/`; no upstream checkout was changed.
Branch names/history metadata do not prove unpublished functionality; findings
below are based on current source, not a promise inferred from a branch title.

The public FW2 firmware repository is a release repository. Neither it nor the
other inspected public snapshots contains `MenuX/fwMenuI2CConfig.h`, the actual
MAIN Poll handler, or the stock I2C/Menu Explorer C++ panel implementation.
Generated docs name those source inputs, but that does not make the inputs
available. Exact missing functions must remain unknown. No inaccessible/private
implementation is cited as if inspected. A public-source search for the named
I2C configuration and scan text found documentation, not those implementations.

## Gate A: trace and loss point

The inspected path is:

```text
ow_io_i2c_i2c_poll(dev)
  -> ow__cat(..., "i\\i\\p")
  -> ow__call(dev, cmd, resp, sizeof resp)
  -> dev->t.write(0x02 + command + newline)
  -> owfw_write: internal FwGUI M_TERM_INPUT framing
  -> stock MAIN dispatch / Poll handler [source unavailable]
  -> FwGUI response frame
  -> rx_byte -> g_text FIFO -> owfw_read_text
  -> ow__read_line -> ow__parse_frame -> resp
  -> (void)resp; return OW_OK
```

Evidence at OneWili b0eeccda:

* [wilibsp/src/onewili.c](https://github.com/freewili/onewili/blob/b0eeccda21b0594c8062cd17e9755f0c26b0cf6b/wilibsp/src/onewili.c#L998):
  `ow_io_i2c_i2c_poll` constructs `i\i\p`, obtains a response body, then
  discards it at line 1005. This proves loss of **whatever body is returned**;
  it does not prove that body contains addresses.
* [wilibsp/src/onewili_fwgui.c](https://github.com/freewili/onewili/blob/b0eeccda21b0594c8062cd17e9755f0c26b0cf6b/wilibsp/src/onewili_fwgui.c):
  `owfw_write` sends marked internal terminal-input frames; `rx_byte` routes
  `OWFW_CMD_RESPONSE` to the text FIFO. Other GUI traffic is discarded, so the
  stock panel's GUI log is not automatically a OneWili response/event stream.
* [api_manifest.json](https://github.com/freewili/onewili/blob/b0eeccda21b0594c8062cd17e9755f0c26b0cf6b/python/onewili/api_manifest.json#L3767)
  records Poll, no parameters, path `i\i\p`, and required zone 6. It identifies
  the generated command mapping, not MAIN registration code.
* [I2C reference](https://github.com/freewili/onewili/blob/b0eeccda21b0594c8062cd17e9755f0c26b0cf6b/docs/i2c.md)
  identifies `fwMenuI2C`; [FW2 feature docs](https://github.com/freewili/freewili2-docs/blob/0a4e3c224fdd4d0b61e6b02f64a14feaba28d725/docs/features/i2c.md)
  identify `MenuX/fwMenuI2CConfig.h` as generator input. Actual registration
  statement, handler name and hardware polling function are **UNVERIFIED**.

### Response format: established envelope, unknown Poll body

`ow__parse_frame` in the linked C source consumes the conventional envelope
`[path timestamp sequence response-body success]`: it locates the third space,
copies the middle body, removes terminal CR/LF and reads the final success token.
Embedded body newlines are reassembled by `ow__read_line`. It rejects an oversized
body rather than returning a complete one. Path/timestamp/sequence are discarded;
the C routine does not correlate the response path with the request.

**No exact address-list schema, count, delimiters, reserved-address policy,
failure payload or empty-result body for `i\i\p` is established.** The official
[stock I2C panel documentation](https://github.com/freewili/freewili2-docs/blob/0a4e3c224fdd4d0b61e6b02f64a14feaba28d725/docs/panels/i2c.md)
describes Green Scan logging a byte per responding non-reserved address with
`R) A: <addr> R: <byte>` and `Poll Found N devices`. Its log fills only while
that panel is displayed. These are documented **panel log lines**, not a proven
wire response for the Poll command. Stock panel consumer callbacks/functions
and whether its Green action invokes the same handler are unavailable.

### Another generated result API?

No scan-address result API was found in the current generated C header,
Python manifest/menu, Rust I2C menu, WASM bindings or I2C event documentation.
[Python i2c_poll](https://github.com/freewili/onewili/blob/b0eeccda21b0594c8062cd17e9755f0c26b0cf6b/python/onewili/menus/i2c.py#L59)
calls `_call("p", [], [])` and promises `Ok(None)`;
[Rust](https://github.com/freewili/onewili/blob/b0eeccda21b0594c8062cd17e9755f0c26b0cf6b/rust/src/menus/i2c.rs)
returns `Result<(), OwError>`; WASM Poll returns only status. `i2c_read` byte
output is not a scan result retrieval operation. `i2cmon`/`i2cslv` byte events
do not document a scan address/result association.

### Public generic request/response hooks: PROVEN SUPPORTED, narrower than Gate A

[onewili.h lines 76-87](https://github.com/freewili/onewili/blob/b0eeccda21b0594c8062cd17e9755f0c26b0cf6b/wilibsp/include/onewili.h#L76)
publicly declares `ow_raw_send`, `ow_raw_next_response`, `ow_raw_stash_clear`
and `ow_raw_stash_lost_count`. The corresponding implementation at
[onewili.c line 321](https://github.com/freewili/onewili/blob/b0eeccda21b0594c8062cd17e9755f0c26b0cf6b/wilibsp/src/onewili.c#L321)
sends the same reset-prefixed command and returns the next parsed response
middle plus a separate success flag. Official
[onewili_fast.c](https://github.com/freewili/onewili/blob/b0eeccda21b0594c8062cd17e9755f0c26b0cf6b/wilibsp/src/onewili_fast.c)
uses these hooks for CAN pipelining. They are not custom transport or a new bus
driver. Sending the existing Poll through them is possible at the API level.

Important limits: no interleaving with synchronous generated calls on the same
device; responses are arrival-order, with no identity returned/correlated.
`ow_poll_text_line` can stash responses in 32 slots of 192 bytes each, truncating
longer frames; a direct response read uses `OW_RESP_MAX` (4096 by default).
The public hooks retain a body only if it arrives in the command-response
channel. They cannot recover GUI log traffic or create an undocumented body.
The old M1E correlation finding therefore still matters; nothing was patched.

**Correction to the previous research:** raw commands are not inherently an
unsupported workaround. The user explicitly accepts this public generic route
if the stock operation's exact response can be established. Its availability
removes the generated-wrapper-only obstacle, but does not complete Gate A.

History inspection found the generated Poll still status-only at current
b0eeccda and preceding 9ce9df83. No current history entry established a typed
scan result or published MAIN handler. No official BSP external-I2C scan-result
consumer was found. No scanner was implemented or executed.

## Gate B: exact initialization writes

At BSP be4bdd63:

| Entry/function | Operations |
| --- | --- |
| `board_init` | Chooses `board_init_clk(250000)` normally; after the PSRAM bootstrap chooses `board_init_inherited`. |
| `board_init_clk` | Sets RP core regulator to 1.25 V, waits 10 ms, changes system clock, re-sources peripheral clock, configures/reinitializes PSRAM timing, then initializes peripherals. Core supply is distinct from external VIO. |
| `board_init_peripherals(true)` | Resets PIO1, sends RGB-off frames, initializes shared SPI1/pins/drive strengths, parks radio CS GPIO40 high, configures backlight GPIO25 low, recovers/initializes internal I2C1, calls `ioexp_init`. |
| `board_i2c1_init` | Configures GPIO26/27, pull-ups, internal I2C1 at 400 kHz; recovery can pulse SCL up to nine times and send STOP. This is the DISPLAY internal bus, not a target scan. |
| `board_init_inherited` | Runs the same peripheral path with LED clearing; skips clock/QMI work, **does not skip expander writes**. |
| `board_init_psram` | Core voltage/clock/QMI work plus peripheral initialization without LED clear; records bootstrap state. |

Source: [board.c](https://github.com/freewili/wilibsp/blob/be4bdd63d31a80f95410e583710cf4e43a7be7fa/bsp/platform/board.c),
[board.h](https://github.com/freewili/wilibsp/blob/be4bdd63d31a80f95410e583710cf4e43a7be7fa/bsp/platform/board.h),
[spi_bus.c](https://github.com/freewili/wilibsp/blob/be4bdd63d31a80f95410e583710cf4e43a7be7fa/bsp/platform/spi_bus.c).

`ioexp_init` in
[ioexp.c](https://github.com/freewili/wilibsp/blob/be4bdd63d31a80f95410e583710cf4e43a7be7fa/bsp/platform/ioexp.c)
resets software shadows and sends the following full PCAL6524 register writes
on internal I2C1, address 0x23 (2 ms bounded transfers):

```text
output register write:   04 F0 6C 08
direction register write: 0C 00 00 04
```

Outputs are written first, then directions. All pins become outputs except
P2 bit 2 (MCLR). Defaults select SPI buffer directions, release SCREEN_NRST,
enable I2C pulls, set GPIO25 buffer direction and CC1101 433 antenna route.
USB host 1/2, mic, IR and USB-device D+ pull-up controls are cleared. Port 2
VREF bits 6/5/4 are cleared and bit 3 (external VREF) asserted. These writes
overwrite inherited output state, not just application-local flags.

[ioexp.h](https://github.com/freewili/wilibsp/blob/be4bdd63d31a80f95410e583710cf4e43a7be7fa/bsp/platform/ioexp.h)
states those mutually exclusive pins connect real rails to the user-header VIO.
Therefore startup both changes the software selection and commands an electrical
rail-selection change when the prior selection differs. The voltage may remain
similar if two sources happen to have equal voltage; that does not preserve
connection state. No claim is made about transient amplitude/timing without a
measurement. Existing external selection may receive the same bits, but the
required invariant covers arbitrary valid pre-launch state, not that special case.
The upstream [VREF hardware finding](https://github.com/freewili/wilibsp/blob/be4bdd63d31a80f95410e583710cf4e43a7be7fa/docs/superpowers/findings/2026-07-26-gpio-vref-e2e.md)
backs electrical significance and records an Ext Pin voltage/decay uncertainty.
It is upstream evidence, not a measurement performed here.

### Must every app literally call board_init?

No CMake linker rule forces that function call. The
[FW2App contract](https://github.com/freewili/wilibsp/blob/be4bdd63d31a80f95410e583710cf4e43a7be7fa/AGENTS.md)
prescribes recovery initialization immediately after `board_init`; inspected
normal BSP apps follow it or `board_init_clk` (DVI). This is a documented supported
startup convention, not proof that no specialized app can have another entry.

One actual exception is
[hello_psram_exec/main.c](https://github.com/freewili/wilibsp/blob/be4bdd63d31a80f95410e583710cf4e43a7be7fa/apps/hello_psram_exec/main.c):
it does not call `board_init` in main. But
[psram_bootstrap.c](https://github.com/freewili/wilibsp/blob/be4bdd63d31a80f95410e583710cf4e43a7be7fa/bsp/app/psram_bootstrap.c#L44)
already calls `board_init_psram`, which reaches `ioexp_init`. The example also
cycles the RGB rail. This is not a preservation-compatible minimal path.

History [3f80ca35](https://github.com/freewili/wilibsp/commit/3f80ca35ebed920b7945680bf65337409006f441)
avoids duplicate QMI initialization; it does not promise VREF preservation.
[0cb4e2c6](https://github.com/freewili/wilibsp/commit/0cb4e2c6d5bd92790cdd9f13d33c9541c3d8a4a8)
adds VIO selection; no later inspected ioexp history provides an attach/read-inherit
initializer. `fix/openocd-attach-without-reset` concerns the debugger, not an
application's runtime attach policy.

Independent official app sources checked:

| App/revision | Startup observed |
| --- | --- |
| [wilidoro e43484a0](https://github.com/freewili/wilidoro/blob/e43484a0e2f20bbb3a813b916627beb8439918cf/src/target/main.c) | `board_init`, display, touch and app HAL |
| [chordboard-invaders efd62691](https://github.com/freewili/chordboard-invaders/blob/efd62691069ca6b7d4e337a42bb7f7d1e6eb089c/apps/typing_invaders/main.c) | `board_init` |
| [wiliplayer 6b3b0725](https://github.com/freewili/wiliplayer/blob/6b3b07256963d06a2068b4be9b8198cc90eb3d9b/src/main.c) | Its own board initialization plus explicit expander initialization |
| [sensorview 9e316663](https://github.com/freewili/sensorview/blob/9e316663d48967b77b2dc4b0ca204b217cdb1968/src/main.c) | Board initialization plus expander defaults |
| [subghz a79634b7](https://github.com/freewili/subghz/blob/a79634b76e9de9f9c2df2f97a6914addb056bc3e/src/main.c) | Board initialization; shared display/radio bring-up |

Also inspected official `usbcamfw` 16400e107fe3bbb7ee58b95f62d1e8567f9fbc2a,
`WiliIR` 59669d89232e637d409eb8fbbfcb6dde18623bac,
`fw2-orca-templates` 13dcaabc8f91bab79dd3e3cadbb9578619379225,
`wasm-examples` eeecb00fdecfd29fedebc24e02a9b822969e2f9d,
and `freewiligui-sdk` 4cf128dee34e598e4d899a0266342b70274eff67.
The GUI SDK is a desktop plugin framework, not a FW2 native panel extension.
No proven native no-VREF initialization lifecycle was found in this bounded set.

### Smaller BSP components: possible building blocks, not a proven lifecycle

`st7796_init` directly configures internal SPI1, LCD GPIO/drive strength, panel
commands and DMA; it does not itself call `ioexp_init`. `ft6336_init` performs
touch chip reads/mode write on already initialized I2C1, not VREF selection.
`board_i2c1_init` itself does not write the expander. `ow_open_fwgui` configures
UART0/GPIO0-3/flow control/IRQ, resets local queues, opens the command transport,
binds peer streams and arms SD access; it does not itself call board/expander
initialization or request target power. It is the official link to already-running
stock MAIN, but not an overall state-preserving app attach contract.

Sources: [st7796.c](https://github.com/freewili/wilibsp/blob/be4bdd63d31a80f95410e583710cf4e43a7be7fa/bsp/display/st7796.c#L84),
[ft6336.c](https://github.com/freewili/wilibsp/blob/be4bdd63d31a80f95410e583710cf4e43a7be7fa/bsp/input/ft6336.c#L71),
[onewili_fwgui.c](https://github.com/freewili/onewili/blob/b0eeccda21b0594c8062cd17e9755f0c26b0cf6b/wilibsp/src/onewili_fwgui.c#L478).
Their availability suggests a composition worth asking upstream to document;
it does not prove loader/CRT/inherited clocks, buffer muxes, DMA/IRQ ownership,
live power management or stock-return behavior. No app demonstrating this entire
composition with unchanged power/VIO was found. It would be premature to label
it recommendation B or a documented minimal path.

### Preserve/restore without the first change?

`ioexp_vref_get` returns the fresh application's `s_vref` shadow, initialized to
external VREF; it does not read the hardware or previous application's selection.
All expander setters write full shadow output bytes. No supported expander
attach/snapshot/adopt API exists in this header. Reading after board initialization
cannot recover the overwritten pre-launch selection. An ADC reading cannot
identify which source is selected and does not satisfy the invariant either.

`fw2_app_recovery_init` initializes keyboard UART/DMA and, for nonzero declared
zones, calls `picpwr_keep_awake` and waits for them. `picpwr_release_unused` can
turn inherited rails off; task servicing can reassert requested rails. With zero
declared zones and no requested/released rails, the power helper has no desired
mask to enforce, but that alone does not prove loader/stock power preservation.
`picpwr_rails` is observed rail status, not a snapshot of all previous policy.
The [recovery code](https://github.com/freewili/wilibsp/blob/be4bdd63d31a80f95410e583710cf4e43a7be7fa/bsp/input/app_recovery.c)
exits by watchdog reboot, not resuming the paused stock DISPLAY context. No
supported return-state handoff proves original VIO/power remain through this
reboot. Consequently saving/restoring values on exit would not resolve Gate B.

## Alternative: stock Setup/Actions, Panels and scripts

[Setup & Actions docs](https://github.com/freewili/freewili2-docs/blob/0a4e3c224fdd4d0b61e6b02f64a14feaba28d725/docs/panels/setup-actions.md)
describe a build-generated menu tree, drill-down, parameter dialogs and display
of command success/response, including truncation indication. This already gives
users access to stock commands without launching a replacement DISPLAY RAM app.
It does not document registration of a custom six-tile panel from an add-on file
or a runtime menu-plugin ABI. Actual stock registration/dispatch code remains
unavailable in the examined public sources. Adding a compiled menu entry would
require changing stock firmware, outside the preservation boundary.

[Stock Panels](https://github.com/freewili/freewili2-docs/blob/0a4e3c224fdd4d0b61e6b02f64a14feaba28d725/docs/panels/command-panel.md)
supports custom controls/pages, editable events and `.wili` files.
[OneWili GUI panels](https://github.com/freewili/onewili/blob/b0eeccda21b0594c8062cd17e9755f0c26b0cf6b/docs/gui_panels.md)
provides `add_panel`, `add_panel_picklist`, `show_panel`, labels and a shared
read-and-clear button latch. This can render custom UI inside stock DISPLAY;
the runtime owning its actions/result processing must still be established.
No documented stock dynamic-panel action that yields the Poll address list was
found. These mechanisms are promising alternatives, not a proven solution C.

[Scripts docs](https://github.com/freewili/freewili2-docs/blob/0a4e3c224fdd4d0b61e6b02f64a14feaba28d725/docs/panels/scripts.md)
describe `/scripts` execution inside stock infrastructure, without a native BSP
board-init path. Current docs explicitly prohibit RTHON execution due to its
stack overflow. [WASM OneWili](https://github.com/freewili/onewili/blob/b0eeccda21b0594c8062cd17e9755f0c26b0cf6b/wasm/README.md)
uses generated command IDs and `ow_call`; its Poll binding still has no typed
address output. No complete safe startup/exit plus result contract was established
for a script-controlled custom panel. No scripts were run.

## Remaining evidence needed and stop

Gate A needs the official MAIN registration/handler and exact framed Poll payload
or an official supported scan-result consumer demonstrating that payload. Gate B
needs an official app-specific initialization/return contract covering inherited
expander state, power ownership, loader/CRT and stock resumption without even a
temporary VIO/power change. Documentation of a stock Panels/WASM action/result
lifecycle could instead establish another app mechanism.

These are evidence gaps, not requests to connect hardware or patch upstream.
No physical test can substitute for the requested source-backed architecture
decision in this milestone. Stop here; no implementation.

Validation: full Windows host suite discovered 50 tests: 49 passed, one native
constexpr check skipped for unavailable C++ compiler. Import/call guards,
runtime audits and M1A report identity passed. `git diff --check` passed.
Runtime, native sources, tests, tools, dependencies and M1A evidence have no
changes relative to `68f9cd4`. No build or device test was performed.
