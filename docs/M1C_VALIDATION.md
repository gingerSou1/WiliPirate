# M1C host UI validation and reproduction

Date: 2026-10-04. Windows desktop, Python 3.12, Tk 8.6.
Baseline: main at 2c121b7. Development branch: feature/wili-ui.

## Run the preview

From the WiliPirate repository (Python with Tk support required):

```powershell
cd C:\path\to\WiliPirate
py -3.12 -B tools/ui_preview.py
```

No dependency installation was needed on this desktop. Tk is a host preview
requirement, not a CM0 dependency. On another host without Tk, do not install
anything on the Wili to resolve it. The original console remains available.
There are no transport, device, backend selection or connection options.

1. Start: HiZ, STUB ONLY, HW OFF and help output are visible. The bottom prompt
   initially selects `help`. HiZ is explicitly logical, not electrical isolation.
2. Click Mode twice (or F3 twice): UART then I2C. These are logical transitions.
3. Press Down eight times from the initial selection to select `scan`; press
   OK or Enter. Expect `I2C hardware backend not enabled.` in the transcript.
4. Click Info/F2: the core reports Mode I2C, Backend STUB and hardware disabled.
5. Left/Older and Right/Newer move through transcript pages. Up/Down select
   commands; the selection wraps. The scrollback holds at most 256 wrapped lines.
6. Click HiZ/Home/F6 to return logically to HiZ. Cancel/Esc/F8, Exit/F5, the
   selected `exit` command, or closing the window ends the session and window.

Five colored menu buttons represent the official panel menu positions:
gray Help, yellow Info, green Mode, blue Run, red Exit. F1-F5 map in that order.
F7 is OK; F9/Page moves toward newer transcript text. Native host button clicks
are synthetic button presses, not proof of CM0 touchscreen event delivery.
This version uses a bounded command picker, not a full on-screen keyboard.
Sample write/transfer arguments only reach stubs. Arbitrary commands can still
be tested through the original console or Controller.submit.

## Architecture

`tools/ui_preview.py` adds the existing Python package to the host import path.
`ui/controller.py` owns UI selection, transcript and scroll offset, routes every
command through the unchanged Application.submit and reads Application.view.
`ui/panel.py` is a pure renderer: 60 columns by 17 rows, represented by 68
15-character text cells. ASCII normalization and escaped caption data keep
cells within the documented text limits. `ui/tk_preview.py` draws those cells
in a 480x320 host canvas and supplies documented synthetic button bitmasks.

All five application backends remain the original explicit stubs. The renderer
receives no device object, API client or backend reference. No actual panel API
calls are issued. Multiple simultaneous synthetic bits are ignored except
Cancel/Exit, which takes priority. Physical polling, timing and shared-latch
ownership are not implemented. Host fonts/menu geometry are approximations.

## Recorded checks

- Complete suite: **46 tests passed**, including all 32 unchanged M1B tests.
- New tests exercise all 25 logical transitions through the controller,
  five-mode cycling, selection/run routing, invalid masks, exit priority,
  history bounds/scrolling, unavailable operations and immutable rendering.
- Text layout checks cover all 68 cells, ASCII, 30-byte maximum escaped captions,
  coordinates, menu labels and closed-session presentation.
- AST import/call guards cover the UI package. A separate runtime audit exercises
  all 14 button positions and every picker command in every mode, denying file,
  process and network activity during controller/render execution.
- The Tk adapter has a mocked event/cleanup test. A real Tk host smoke check
  constructed and drew the window, selected I2C, dispatched scan, observed the
  unavailable stub response and closed the session/window via Cancel.
- An initial text-literal syntax error was corrected; the complete suite then
  passed. The final font uses pixel sizing to fit fixed text-cell coordinates.
- `git diff --check` passed. Application files, original tests, staging tool,
  BSP pin/sources and docs/HARDWARE_VALIDATION.md are unchanged from 2c121b7.
- main and feature/wilipirate remain at 2c121b7. No push or merge was performed.

The official browser GUI was NOT run: browser automation reported no available
browser. An attempted host screenshot was unreliable and was not included;
use the exact steps above. Tk creation/drawing checks do not establish visual
pixel parity with firmware or substitute for a human readability review.

## Limits and next step

See EMULATOR_RESEARCH.md for sourced support boundaries and the newly located
embedded Linux htop example. Python Editor is disabled in the official web
build; its desktop Host and physical CM0 targets do not use the simulator.
WiliPirate did not run in the official emulator. This preview faithfully uses
the documented CM0 text/button contract while keeping execution on the host.
It is not a replacement runtime, a firmware app, or a deployable LCD adapter.

The next small step is a human host-preview review of navigation/readability.
Any later real display adapter needs separately authorized framework work and
a supported bridge-only route proven to fail closed. Exact firmware fonts,
menu layout, touch, live button polling, cleanup/restoration and connection
failure behavior remain unvalidated. No physical bus experiment is proposed
or implemented here. M1A remains deferred and M2 has not started.
