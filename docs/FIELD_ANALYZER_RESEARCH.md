# Field Analyzer capability and integration research

Research date: 2026-10-09. Documentation/package/source inspection only; no GUI
execution, device access, instrument operation or stock firmware modification.
"Documented" is not a claim that this device's installed stack was tested.

## Source baseline

- WiliBSP `be4bdd63d31a80f95410e583710cf4e43a7be7fa` and its DISPLAY OneWili
  gitlink `b0eeccda21b0594c8062cd17e9755f0c26b0cf6b` remain unchanged.
- [FW2 product docs](https://github.com/freewili/freewili2-docs/tree/0a4e3c224fdd4d0b61e6b02f64a14feaba28d725)
  provide hardware, panel and generated feature descriptions.
- GUI [README](https://github.com/freewili/freewili-gui/blob/275df3608c76e04f10f36a3964fdbceadd76d4f0/README.md)
  and [CHANGELOG](https://github.com/freewili/freewili-gui/blob/275df3608c76e04f10f36a3964fdbceadd76d4f0/CHANGELOG.md)
  plus the published Windows v0.4.0 package were inspected without installing
  or running it. Package identity matches the earlier audit:
  `136c24cddbd1fbcffcad7e8a0a770f301b21d669680b7dc191c7ea219c5807f0`.
- Legacy WebDocs search results were not treated as authority where their
  repository was no longer accessible. No missing navigation labels or pinouts
  were filled in from search snippets.

## Stock panel reuse and control return

The SD RAM application replaces executing DISPLAY code while stored stock
firmware remains intact and MAIN remains separate. HOME causes a watchdog
reboot to stock, not a call/return to a resident stock panel. Earlier source
research is preserved in [app lifecycles](FW2_APP_LIFECYCLES.md).

| Integration | Classification | Evidence and implication |
| --- | --- | --- |
| Field UI/guide/navigation rendering on host | Supported and verified locally | Production renderer/state pass host tests; no instrument backend. |
| SD-loaded native DISPLAY app/recovery | Documented; prior M2 user-tested | Standard BSP startup has real VREF/power effects; new Field UI itself is not physically tested. |
| Open stock Logic/GPIO/UART/I2C/SPI/CAN/Analog panel from this RAM app | Unknown; no supported direct-launch API established | No stock-panel launch/return contract found. Do not claim a shortcut exists. |
| Keep stock DISPLAY panel executing alongside the custom app | Unsupported by the inspected RAM app composition | They occupy the same executing DISPLAY program; no resident-panel trampoline is documented. |
| `ow_gui_panels_show_panel(index)` | Documented, not tested for this project | Selects generated/custom GUI panel objects, not proof of a stock instrument ID or native stock-screen shortcut. |
| `ow_apps_run_app` / display app launch API | Documented, not tested here | Launches an application file; does not establish stock panel navigation, return, setting restoration or capture access. |
| Lightweight native instrument views over stock MAIN APIs | Documented candidate, not implemented | Prefer this over duplicating drivers only after result, timing, correlation and electrical ownership are qualified. |

No button injection, private memory navigation, firmware patch or copying of
stock instruments is proposed. The current guide can suggest a stock tool;
users can deliberately exit to stock using HOME. It does not automate that path
or imply preserved instrument settings/captures.

## Analog acquisition and oscilloscope limits

Sources: [Analog In API](https://github.com/freewili/freewili2-docs/blob/0a4e3c224fdd4d0b61e6b02f64a14feaba28d725/docs/features/analog-in.md),
[Analog IO panel](https://github.com/freewili/freewili2-docs/blob/0a4e3c224fdd4d0b61e6b02f64a14feaba28d725/docs/panels/analog-io.md),
and [pinout caveats](https://github.com/freewili/freewili2-docs/blob/0a4e3c224fdd4d0b61e6b02f64a14feaba28d725/docs/hardware/pinout.md).

| Question | Documentary finding / limit |
| --- | --- |
| Onboard acquisition | Four stock analog inputs with processor ADC and TLA2024 sources; generated read/stream bindings exist. |
| Data rate | TLA2024 selectable 128-3300 SPS is a converter setting, not guaranteed four-channel waveform throughput. The panel docs describe a default four-channel scan about every 40 ms. |
| Display cadence | Plot choices 5/10/20/50 ms do not force new TLA2024 conversions; faster plotting can repeat readings. |
| Resolution/range | Docs describe 12-bit sources, processor readings scaled to 0-5 V and TLA2024 programmable full-scale ranges. These are not proof of connector-safe absolute limits. |
| Protection/bandwidth | Effective input bandwidth, protection/overvoltage limits, verified CN23 positions and usable multichannel sustained capture remain unknown. |
| Trigger/buffer | Stock trigger window and analog capture commands are documented; complete native DISPLAY waveform/result delivery and depth are not established. |

The stock Analog IO screen can automatically enable zone 11. It is not a safe
zero-power-change shortcut. A useful low-rate voltage view may be feasible
later; a general oscilloscope is not established. The GUI PicoScope plugin
requires external PicoScope hardware/runtime and does not turn the handheld
into that instrument. No analog acquisition or output is implemented here.

## Logic analyzer feasibility

Sources: [logic commands](https://github.com/freewili/freewili2-docs/blob/0a4e3c224fdd4d0b61e6b02f64a14feaba28d725/docs/features/logic-analyzer.md),
[stock panel description](https://github.com/freewili/freewili2-docs/blob/0a4e3c224fdd4d0b61e6b02f64a14feaba28d725/docs/panels/i2c.md#logic-analyzer),
[OneWili DISPLAY transport](https://github.com/freewili/onewili/blob/b0eeccda21b0594c8062cd17e9755f0c26b0cf6b/wilibsp/README.md).

Stock capture is described as PIO-based, with selectable rates 10k/100k/1M/10M,
edges/one-shot/continuous triggering and contiguous pin-range configuration.
Stock panel examples show protocol-dependent channels and finite depth but
do not establish a universal channel count/depth or verified external input
levels for this app. Logic decoding is documented on desktop; that does not
prove an onboard decoder or native result API.

The pinned DISPLAY link explicitly does not mirror logic-analyzer binary
reports. Configure/start/stop bindings alone are insufficient to display or
save samples. Do not create a replacement capture engine to bypass that gap.
Classify direct sample acquisition via this link as unsupported at this pin;
other supported retrieval/export routes remain unknown and need investigation.

## CAN-first functional requirements

Sources: [OneWili CAN receive documentation](https://github.com/freewili/onewili/blob/b0eeccda21b0594c8062cd17e9755f0c26b0cf6b/docs/canfd.md),
[listen-only command](https://github.com/freewili/onewili/blob/b0eeccda21b0594c8062cd17e9755f0c26b0cf6b/docs/neptune_settings.md#c_an1_listen_only),
[official CAN example](https://github.com/freewili/wilibsp/blob/be4bdd63d31a80f95410e583710cf4e43a7be7fa/apps/canblast/main.c).

CAN/FD receive queues/events are documented; DISPLAY binary CAN events are
supported by the public transport. Queue APIs expose presence, remaining depth,
dropped count, ID, classic/FD/extended flags, timestamp, DLC and data. The queue
depth is 32 frames/channel and drops the oldest on overflow. The timestamp is
32-bit uptime microseconds at controller drain, not promised hardware bus time;
wrap/type normalization must be qualified. Polling can auto-enable queue storage
and thus is not a no-side-effect observation.

Listen-only has an explicit binding, so it is **documented but untested**, not
absent. The mode's installed-version behavior/readback, absence of ACK/TX and
transceiver state must be proved before passive monitoring. No direct register
write is proposed. CAN requires controller power/ownership and safe physical
transceiver/termination/grounding. Humpback/SAN DIEGO are documented options,
not validated wiring or vehicle adapters; exact CAN-H/L mapping remains a gap.

Use the CAN Control Lab after UI review and a separate electrical/passivity
qualification plan. Do not launch canblast as a passive test: it configures CAN,
owns rails and has active transmit paths. Do not send frames, diagnostics or
automotive probes. A real vehicle is outside this milestone.

## Capture, save, export and desktop handoff

Current UI: Captures is a placeholder with no file writes, acquisition or data
transfer. Public SDK `ow_sd_*` is a documented SD client, not an established
capture record/GUI import contract. It requires power/ownership/recovery and
must not be introduced until separately approved.

GUI release documentation names **Logic Analyzer**, **Logic Player**, **CAN FD**,
DBC names, **Save DOM / Load DOM** on Setup and `.dom` captures. The README
describes optional PulseView VCD handoff. These are documented desktop features;
no round trip from handheld capture into GUI was demonstrated. DOM is not
assumed to be an open container we can invent. VCD is a possible logic
intermediate after data/timebase mapping is established. CAN CSV/JSON are only
future format candidates; GUI import of them is unknown. Do not invent a
"Protocols -> CAN FD" hierarchy: the exact CAN navigation path was not verified.

A future versioned record should retain raw data, instrument/settings/timebase,
loss/overflow evidence, notes/optional labels, warnings and exporter identity.
Compatibility tests must include units, timestamp wrap, FD payload sizes,
unknown fields and truncated captures. Shared storage must follow the official
app contract. No persistence or recording is implemented this milestone.

## Desktop GUI and optional AI

The verified v0.4.0 package was read as an archive only. Static executable
strings contain the exact labels **AI Chat**, **AI Code Snippets** and **AI
Workbench**. Presence of labels does not demonstrate runtime workflows or
permissions. Bundled Workbench project-type guides describe rThon, Wasm C++,
Wasm Rust and ZoomIO drafting/editor/simulator workflows; some can ultimately
run on the board. They are documentation, not instructions executed here.

AI Workbench is also named in the release notes. AI Chat's detailed context
attachments/capture parsing and Code Snippets' execution/approval behavior
remain unverified from the accessible public sources. No claim of autonomous
hardware agents is made. Cloud/local provider support, key handling, capture
privacy, export formats and live operation restrictions need a GUI-specific
review before integration. Some detailed help is embedded rather than available
as public source files in the inspected repository/package.

For WiliPirate, AI is only a proposed optional explanation/hypothesis/procedure
assistant. No AI client, credentials, connectivity or hardware-control path is
implemented. Any generated procedure must require human review and explicit
authorization before hardware effects. The handheld UI works offline.

## Recommended integration path

Keep the pure UI and guides independent of transports. Prefer a documented
stock shortcut if one is later established; otherwise compose a small native
receive-only view over qualified MAIN APIs. Start with CAN receive qualification,
not a duplicate driver, ADC oscilloscope, new logic engine or autonomous agent.
Do not weaken startup, response-correlation or electrical gates to manufacture
a capability claim.
