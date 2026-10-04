# Milestone 1A - Physical framework validation

Date: 2026-10-04 (America/Chicago).
Status: BLOCKED before device connection; physical validation is NOT complete.
Only desktop enumeration, source review and host tests were performed.

## Repository preflight

Before any modification, git status, git branch -vv, git log --oneline -10
and git remote -v confirmed:

- Current branch: feature/wilipirate; working tree clean.
- HEAD: 77cc5d9, test: validate WiliPirate scaffold and research references.
- Previous milestones present: f6cacf7 (scaffold), 6b494df (capability matrix),
  eac151f (architecture), 98de0b3 (initial repository, main).
- No WiliPirate remote configured.

No application code, tests, BSP, firmware or device files were changed.

## Observed results

| Check | Result | Evidence / limit |
| --- | --- | --- |
| No external target connected | UNCONFIRMED | Requested operator confirmation; cannot establish physical wiring from Windows enumeration. |
| Desktop enumeration | PASS, host inventory only | Win32_SerialPort lists COM1, USB COM13 and COM15 (VID_2E8A/PID_000C), and USB COM17 (VID_093C/PID_205A). No serial handle opened or device command sent. Port roles/device firmware not established. |
| Existing desktop GUI | Not observed | Process query matching fwcom/freewili/putty/terminal returned no rows. This is not proof that no other connection method exists. |
| Normal FREE-WILi connection | NOT VERIFIED | No confirmed existing GUI session or SSH endpoint/user supplied. No port guessed and no network scan performed. |
| Stock CM0 / OneWili bridge | NOT VERIFIED | No access to the running CM0; installed service, socket, binary version and firmware compatibility unknown. |
| Linux Apps WiliPirate launch | NOT RUN | Requires a confirmed safe CM0 connection and supported launcher access. No deployment, file transfer or installation attempted. |
| help / info / mode / mode hiz / exit on device | NOT RUN | Existing host tests pass; host results are not physical command results. |
| Device State on device | SKIPPED | Exact command is documented read-only, but the connection path is not yet safe to invoke; see below. No returned values available. |
| Exit and normal environment functionality | NOT VERIFIED | No physical app launch occurred. |
| Reconnect once | NOT RUN | No initial physical session established; do not report desktop enumeration as a reconnect. |
| GPIO snapshots | SKIPPED FOR SAFETY | Exact stock firmware implementation and absence of all initialization/configuration/power writes have not been established. |
| Existing host regression suite | PASS | py -3.12 -B -m unittest discover -s tests -v: 18 tests passed on Windows. Includes command, EOF, exit, interrupt, no-I/O startup and relocatable staging checks. |

Windows device enumeration required host sandbox escalation after Access denied.
That elevation was for the desktop query only; no device/root login occurred.
The installed desktop Python also ran outside the sandbox. No packages installed.

## Device State source review and connection hazard

Pinned OneWili revision: 9ce9df83b89f83681507f19f960958e23f20ac37.
Its [System.device_state binding](https://github.com/freewili/onewili/blob/9ce9df83b89f83681507f19f960958e23f20ac37/python/onewili/menus/system.py)
issues the no-argument h\a\g command and decodes SD owner, host-streaming
flag, active-stream mask and system clock. The pinned BSP's
[hello_python example](../vendor/wilicm0bsp/apps/hello_python/app.py)
explicitly documents its Device State call as read-only.
This supports the command's read-only classification, not arbitrary connection
initialization or the actual installed device firmware's compatibility.

The [CM0 Python transport](https://github.com/freewili/onewili/blob/9ce9df83b89f83681507f19f960958e23f20ac37/cm0/python/onewili_cm0.py)
opens `fwcm0 api` and sends Device State as its initial probe.
The pinned BSP [console_cli.cpp](../vendor/wilicm0bsp/drivers/fwcm0/src/console_cli.cpp)
contains a critical fallback in run_console_cli: if try_connect(sock_path)
fails, it calls run_direct(api), which constructs LinuxTransport.
[linux_transport.cpp](../vendor/wilicm0bsp/drivers/fwcm0/src/linux_transport.cpp)
then writes SPI mode/word-size/speed, configures a GPIO line as output with a
chip-select level, and writes UART termios configuration.

Consequently, do not blindly run connect_cm0(), fwcm0 api, or the BSP doctor
under this milestone's preservation constraints. Merely checking that a socket
file exists cannot eliminate a connect-failure race or establish behavior of
the installed executable. Establish a supported path that cannot take this
fallback before a Device State request. Do not modify/rebuild/install the
bridge to work around it. No custom socket protocol or raw hardware workaround
was attempted.

The OneWili GPIO read_all binding documents a read of i\g\u; that does not
trace the complete MAIN firmware implementation or power-zone behavior. An
upstream read-test report is insufficient for the stricter no-write requirement
of this milestone. GPIO remains skipped.

## Safe continuation prerequisites

1. Operator confirms external targets are disconnected and identifies the
   normal existing GUI console or configured non-root SSH connection.
2. Inspect the already-running bridge/service and installed versions using
   documented OS status queries; do not start/restart services or enable rails.
3. Establish that the selected connection path cannot initialize peripherals
   or change pin/power settings. If uncertain, stop rather than connect.
4. Once those conditions are met, use the supported /home/apps/wilipirate/run.sh
   launch mechanism. Record the actual launch log, command results, exit,
   normal environment operation and one reconnect. Default app launch logs
   help/info/mode and exits because Linux Apps disconnects stdin; the remaining
   console commands require the supported terminal path.

No firmware update, power/VIO/pull changes, GPIO/mux writes, bus initialization,
root access, undocumented interface, package install or WiliBSP change was
performed. Milestone 2 has not begun.

## Smallest proposed Milestone 2 experiment

Do not schedule a bus operation until M1A completes. The smallest conditional
candidate remains one GPIO input snapshot, with no external target attached,
but only after tracing the exact installed API/firmware and safe transport to
establish no initialization, configuration, mux, direction, pull, power or
peripheral writes. If that cannot be proven, there is no approved GPIO
experiment; remain at the documented Device State/framework validation stage.
