# WiliPirate — Created by gingerSou1

**Current status: M2 UI Preview. Supported hardware: FREE-WILi 2.**

Repository: [gingerSou1/WiliPirate](https://github.com/gingerSou1/WiliPirate).

WiliPirate is a native DISPLAY RAM application with a six-tile touch menu.
**GPIO, UART, I2C, SPI, CAN and LOGIC are placeholders:** no target reads,
writes, discovery, communication or captures are implemented. M3 I2C hardware
integration is planned, not started. Wi-Fi and NFC are future ideas only.

The preserved v001 UF2 was installed through the stock SD Apps mechanism and
user-tested for rendering, touch, navigation, About and physical HOME recovery.
This is UI validation on one device, not electrical certification or a firmware
compatibility guarantee. Stock firmware is preserved by the RAM app mechanism.

## Download and installation

Use [the public installation guide](docs/INSTALLATION.md) and the exact
[preserved M2 UF2](releases/m2-ui-v001/WiliPirate.uf2). No application compilation
is required. The artifact is stored on `feature/wilipirate-panel`; there is no
published GitHub Release yet.

SHA-256:
`6bc0a08050c1e18659882bc486a1f03b6e16529916b60112240f548aeb1e6672`

**Disconnect all external targets, probes, header wiring, external VREF sources
and accessories before launching.** Standard BSP startup changes VREF/power
and other settings. UI-only does not mean electrically isolated.
Hold physical **HOME for five seconds** to return to the stock DISPLAY app.

## Licensing and attribution

Original WiliPirate licensing has not been selected. Public source availability
does not establish a redistribution grant. The preserved artifact includes
upstream notices, with unresolved licensing questions documented in
[third-party attribution](docs/THIRD_PARTY.md). No license is selected by this
cleanup. Official FreeWili code and dependencies retain their own ownership
and terms. No Bus Pirate or Bit Pirate source code is included.

The current v001 About screen shows the app name, version and repository URL.
Creator attribution here does not change the frozen binary.

## Development and preserved research

- [M2 source, build evidence and startup effects](docs/M2_DISPLAY.md)
- [Historical physical validation and exact baseline](docs/M2_DEVICE_VALIDATION.md)
- [Architecture decision](docs/PANEL_ARCHITECTURE_DECISION.md)
- [Contributor boundaries](AGENTS.md)

The shelved CM0 prototype remains in `apps/wilipirate/` and `native/cm0/`,
with a separate Tk preview in `ui/`. These are not the M2 installation path.
The CM0 checkpoint is `bc738b8`, reachable through `feature/wili-ui`; the
historically created `archive/cm0-prototype` branch is not published remotely.
The native CM0 prototype has unresolved response-validation failures and is
not deployment-qualified. See [CM0 architecture](docs/ARCHITECTURE.md),
[prototype usage](apps/wilipirate/README.md),
[host preview reproduction](docs/M1C_VALIDATION.md) and
[response-validation evidence](docs/M1E_1_RESPONSE_VALIDATION.md).

Historical evidence records the observations and permissions of its original
session; it does not grant permission for future builds or device access.
