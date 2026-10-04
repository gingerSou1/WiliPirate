# Interactive Wili UI research for M1B

M1C update: the embedded official GUI **05 Linux htop** example has now been
located. See [EMULATOR_RESEARCH.md](EMULATOR_RESEARCH.md). The M1B search record
below is retained as historical evidence.

Date: 2026-10-04. Research only; no example, GUI, display command or hardware
connection was executed. No dependency/submodule pins were changed.

## Verified application mechanism

The pinned [WiliCM0BSP app contract](../vendor/wilicm0bsp/docs/apps.md) specifies
CM0 Linux /home/apps/<name>/run.sh, selected through Linux > Apps. It runs as
the console user, with disconnected stdin and logged stdout/stderr. Therefore
an interactive LCD app needs an explicit UI/input loop, not Python input().
WiliPirate keeps the portable Python app/run.sh structure and relocatable
local package. M1B --console is for host terminals; default launch remains
stdin-independent. Nothing here changes MAIN or DISPLAY firmware.

The pinned Python [template](../vendor/wilicm0bsp/apps/template/app.py),
[hello_python](../vendor/wilicm0bsp/apps/hello_python/app.py), and
[gpio_poll](../vendor/wilicm0bsp/apps/gpio_poll/app.py) demonstrate CM0 entry,
connection ownership/cleanup and logging, not a complete interactive LCD UI.
Their connection calls are NOT copied into M1B because of the M1A fallback.

## Official OneWili rendering/input surface

Evidence is pinned to OneWili 9ce9df83b89f83681507f19f960958e23f20ac37:

| Operation | Source | Future UI use / boundary |
| --- | --- | --- |
| gui.show_text / clear_display | [gui.md](https://github.com/freewili/onewili/blob/9ce9df83b89f83681507f19f960958e23f20ac37/docs/gui.md) | Overlay text is truncated/replaced, not a scrolling console. |
| gui.panels.add_panel / show_panel | [gui_panels.md](https://github.com/freewili/onewili/blob/9ce9df83b89f83681507f19f960958e23f20ac37/docs/gui_panels.md) | Establish/show a custom panel, with optional standard menu. Reinitialization mutates display state. |
| gui.controls.add_text / add_log_list / add_button | [gui_controls.md](https://github.com/freewili/onewili/blob/9ce9df83b89f83681507f19f960958e23f20ac37/docs/gui_controls.md) | Build text/history/control surfaces. Declaration alone does not prove a CM0 touch-input route. |
| gui.control_properties.set_control_value_text / set_list_item_text | [gui_control_properties.md](https://github.com/freewili/onewili/blob/9ce9df83b89f83681507f19f960958e23f20ac37/docs/gui_control_properties.md) | Update existing controls rather than repeatedly rebuilding a panel. |
| gui.panels.set_menu_text / read_buttons | [gui_panels.md](https://github.com/freewili/onewili/blob/9ce9df83b89f83681507f19f960958e23f20ac37/docs/gui_panels.md) | Label physical buttons; poll the shared read-and-clear latch. Repeated presses coalesce, release/long-press history is absent. One polling consumer only. |

These are verified API declarations/documentation, not a validated WiliPirate
LCD/input path. Even read_buttons clears shared state; it is not a purely
observational action. Rendering and input polling remain prohibited in M1B.
A future panel adapter can map Help/Info/Mode/HiZ/Exit actions into Application
commands, render the immutable ViewState and Reply, and use a bounded history.
Raw touch coordinates, control-click delivery on CM0, exact firmware support,
layout/font choices, and reliable restoration of the stock UI need evidence
and later validation. Do not infer USB event delivery through the mailbox.

## htop-style example: not located

The requested htop-style application was not present in the inspected sources:

- Pinned WiliCM0BSP tree and official main tree d27edf1c18bc2c18a76c4c9cdbf200c19884cc06:
  Python apps are hello_python, gpio_poll and template.
- Pinned OneWili examples and official main tree b0eeccda21b0594c8062cd17e9755f0c26b0cf6b:
  the CM0 example listed is rainbow_leds.py; Python examples cover discovery,
  menu exploration, GPIO, PWM/binary capture and NFC. Main-tree inspection did
  not update WiliPirate's pin.
- Official Windows FreeWili GUI v0.4.0 archive inspected in
  [GUI_EXAMPLES.md](GUI_EXAMPLES.md): no htop/monitor Python example was found;
  the dashboard DLL is not an inspectable Python application example.
- Public searches for freewili/onewili with htop did not locate a primary source.

Requested an exact source link/path from the user. Until provided, the example's
entry point, event loop, cleanup, API version and safety cannot be claimed as
verified. No UI implementation is attributed to it. This reference gap does
not block the independent host-only console and logical-state implementation.

## Required before eventual on-device UI

1. Resume M1A only with explicit authorization; preserve its recorded finding.
2. Establish a supported bridge-only API path with no direct-hardware fallback,
   including connection races, absent/busy bridge, errors and disconnects.
3. Qualify actual stock firmware/bridge versions and the display/input API path.
4. Implement a separately reviewed renderer with bounded polling/history,
   checked Results, cancellation and restoration of app-owned display state.
   It must not configure buses or target power merely by selecting a mode.
5. After separate deployment approval, stage/copy the app through the supported
   mechanism, validate logs and input, exit cleanly, and verify normal UI/reconnect.
   No device packages or firmware changes are implied by this plan.

M1B includes no OneWili imports, renderer calls, bridge invocation or hardware
backend. It provides the reusable Application.submit / Application.view boundary
so a later UI can share the parser and state manager without rewriting them.
