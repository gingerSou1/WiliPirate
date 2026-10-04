# M1C: official UI/emulator research

Research date: 2026-10-04. Baseline: 2c121b7. No physical device access.

## What exists and what executes

The official [browser GUI](https://freewili.com/fwgui/fwcom.html) is an
Emscripten canvas application. Its published [entry page](https://github.com/freewili/website/blob/8f80a322974920dbdac2046bcb2253b4c666c662/fwgui/fwcom.html)
loads fwcom.js, fwcom.data and fwcom.wasm (entry version 0.3.2-20260928).
This is distinct from CM0 Linux and the RP2350 bare-metal BSP.

| Surface | Established support | WiliPirate consequence |
| --- | --- | --- |
| GUI Simulator | In-process firmware/menu simulation, simulated display, storage and selected peripherals | Not a CM0 Linux machine or general board emulator |
| rTHON editor | Simulator or firmware; Python-like language with restricted classes/imports/containers | Cannot run our CPython package/dataclasses/Enum unchanged |
| C/C++ Wasm editor | Freestanding compiled Wasm for Simulator or device | Not a CPython runtime; would require an unjustified core port |
| Python editor | Desktop Host or physical Device/CM0; explicitly disabled in browser build | Neither Python target uses the rTHON simulator |
| Linux Console | Physical CM0 only; explicitly unavailable in Simulator | No supported simulator CM0 launch/bridge path |
| ZoomIO simulator | Separate RISC-V workload simulation | Not the CM0 Python application environment |

Evidence: embedded Simulator, Python Editor, Linux Console, C/C++ WASM Editor
and rTHON Language Reference help in the official
[published fwcom.wasm](https://github.com/freewili/website/blob/8f80a322974920dbdac2046bcb2253b4c666c662/fwgui/fwcom.wasm).
SHA-256: `989e6624469068fdcf513aff5809cf57ddd072adcae1bc60e4b6a70ff99c1db8`.
This is published binary/source-text evidence, not a claim that the GUI C++
implementation is public. Null-delimited UTF-8 strings contain the embedded
help and complete example scripts. Inspect as data only; never execute scripts
extracted from this artifact. No upstream code/assets are redistributed here.
The browser plugin reported no available browser, so interactive GUI execution
and exact web-build feature parity were not tested. Compiler availability in
the web build is not established merely by desktop-shared help.

## Display and input

Simulator help describes a framebuffer shown in Tools > Simulator, touch/click
input, a D-pad and named buttons. Arrow keys/Enter operate the D-pad/center;
F1-F5 map to colored buttons, F6-F9 to Home/OK/Cancel/Page. This is documented
behavior, not an observed run this milestone.

The supported CM0 UI uses custom panels and text controls. The pinned
[panel API](https://github.com/freewili/onewili/blob/9ce9df83b89f83681507f19f960958e23f20ac37/docs/gui_panels.md)
defines menu labels (15 bytes), and a shared read-and-clear press latch:
gray/yellow/green/blue/red bits 0-4; Up/Down/Left/Right/Center/OK/Cancel/Home/Page
bits 5-13. Repeated presses coalesce. This is not a stream of key releases or
a raw touch API. No CM0 touch-coordinate route is assumed.
[Text controls](https://github.com/freewili/onewili/blob/9ce9df83b89f83681507f19f960958e23f20ac37/docs/gui_controls.md)
and [text updates](https://github.com/freewili/onewili/blob/9ce9df83b89f83681507f19f960958e23f20ac37/docs/gui_control_properties.md)
allow a renderer independent of bus access.

## Located: 05 Linux htop

The missing example is embedded in the published GUI binary, not a standalone
file in the public CM0 BSP examples. Python Editor > Device > Examples includes
04 Bouncing Bomb and **05 Linux htop**. Search the artifact for
`freewili-htop-`, `COLS, ROWS = 60, 17`, or `# Python Editor`.

Inspection establishes a 60-column, 17-row ASCII text display split into
15-character controls, at 7-pixel cell width and 17-pixel line spacing.
It escapes display markup and polls the documented button mask. It runs htop
in a Linux pseudo-terminal, uses pyte, and closes its child/connection on exit.
It also calls connect_cm0: therefore it is NOT safe to run under the M1A
fallback finding. We did not execute it, install dependencies, or copy its code.
Its layout/API contracts are useful; its transport/startup code is excluded.
For a minimal official example, the rTHON Show text example exercises simulator
display only; Bouncing Bomb is a smaller CM0 animation example. Neither executes
our existing Python application inside the simulator.

## Decision before implementation

Do not port the core to rTHON/Wasm or invent a simulator transport. Implement a
host-only panel preview with the existing CPython Application.submit/view,
modeled on documented CM0 text controls and button masks. A pure controller
owns UI selection/history; a pure renderer emits bounded text cells; a separate
Tk host adapter draws those cells and supplies synthetic button events.
No OneWili imports, live GUI client, transport adapter or backend substitution.
The preview validates presentation and command routing, not firmware fonts,
menu geometry, timing, touch, bridge compatibility or physical restoration.
The existing CM0 run.sh and staged M1B application stay unchanged. This preview
is a development tool, not a deployable CM0 LCD implementation.

No WiliPirate execution in the official emulator is claimed. The smallest next
integration is a separately reviewed display/input adapter after a supported
bridge-only route and framework validation are authorized. All hardware modes
remain stubs; M2 is not started.

## Other current references checked

- [WiliCM0BSP app contract](https://github.com/freewili/wilicm0bsp/blob/d27edf1c18bc2c18a76c4c9cdbf200c19884cc06/docs/apps.md): Linux /home/apps/name/run.sh, disconnected stdin, logged output.
- [OneWili current tree](https://github.com/freewili/onewili/tree/b0eeccda21b0594c8062cd17e9755f0c26b0cf6b): CM0 example rainbow_leds; no standalone htop. Runtime dependency pin unchanged.
- [Official Wasm examples](https://github.com/freewili/wasm-examples/tree/eeecb00fdecfd29fedebc24e02a9b822969e2f9d): C/C++, Rust and other Wasm languages, not CM0 Python.
- [WASM overview](https://freewili.com/specs/wasm.html): distinguishes on-board Wasm and GUI tooling; shared drawing API does not imply a shared Python runtime.
- [FreeWili GUI README](https://github.com/freewili/freewili-gui/blob/275df3608c76e04f10f36a3964fdbceadd76d4f0/README.md): desktop Python tooling and CM0 prerequisites.
- Official freewili2-docs 0a4e3c224fdd4d0b61e6b02f64a14feaba28d725,
  FREE-WILi2-Linux 0be82562398c8b013d8adc013469cd10022a2336 and SD image
  5652b0510377d3f58a130dc1032a80bda362a0c6 were searched for example paths.
  The binary-embedded reference resolves the earlier UI_RESEARCH.md search gap.
