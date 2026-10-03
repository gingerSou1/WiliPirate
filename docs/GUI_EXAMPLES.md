# GUI-packaged OneWili examples

Inspected the official [Windows v0.4.0 release](https://github.com/freewili/freewili-gui/releases/tag/v0.4.0),
asset fwcom-0.4.0.zip (84,305,978 bytes), on 2026-10-03.
Local SHA-256: 136c24cddbd1fbcffcad7e8a0a770f301b21d669680b7dc191c7ea219c5807f0.
The archive was downloaded to temporary research storage, not installed or run.
This hash identifies the inspected archive; it is not a publisher-signature claim.
Paths below are inside fwcom-0.4.0/onewili-python/examples/.

| Example | Observation | CM0 consequence |
| --- | --- | --- |
| list_devices.py | pyfwfinder USB enumeration | Host discovery, not Linux Apps entry. |
| explore.py | onewili.connect(), menu traversal, finally close | Use the CM0 transport for an on-device app. |
| toggle_gpio_25.py | set_io_toggle(25) after USB connect | An output-changing example; do not execute or adopt its pin. |
| toggle_gpio_25_loop.py | repeated toggle, finally close | Not a safe startup pattern. |
| watch_gpio_events.py | binary=True and FTDI event queue | Not evidence of CM0 streaming support. |
| capture_pwm.py | PWM writes, logic-analyzer binary capture, cleanup | Hardware-active host example; not a CM0 read-only check. |
| typeb_detect.py | NFC raw Type B detection after USB connect | Does not establish the CM0 event/data route. |
| verify_menu_paths.py | USB menu probe and direct transport helper | No generic typed settings snapshot/restore API established. |

No example was executed. No archive files are redistributed. The BSP's pinned
OneWili is the authoritative API version for WiliPirate, rather than assuming
the GUI archive's generated bindings match that revision. Package inspection
and the public README/changelog jointly establish why the GUI's PC workflows
must not be substituted for the documented CM0 app contract.
