# Install WiliPirate M2 UI Preview

WiliPirate — Created by gingerSou1

This guide describes the preserved **v001 DISPLAY RAM application** for
**FREE-WILi 2**. No application compilation is required. GPIO, UART, I2C,
SPI, CAN and LOGIC are unavailable placeholders. Original-code licensing and
some dependency redistribution terms remain unresolved; see
[third-party notices and questions](THIRD_PARTY.md). These instructions do not
create a license grant.

## Before installation

- Use a FREE-WILi 2 with its stock SD Apps launcher and supported RAM application
  loader. Other FREE-WILi models are not established as supported.
- Validation covered one device whose USB descriptor reported FW2 v07. That
  is not a complete MAIN/DISPLAY/loader version inventory or a minimum-version
  guarantee. Confirm compatibility using official FreeWili documentation or
  support before installing on another stack. Do not change stock firmware
  to work around an unavailable application mechanism.
- Disconnect external targets, probes, GPIO/header wiring, external VREF
  sources and accessories before launching or recovering the app.
- Standard BSP startup configures internal buses, clocks, expander outputs,
  radio routing and display/input hardware. It selects external VREF for VIO
  and can request or release inherited app-owned power rails. Previous
  electrical settings are not guaranteed to be restored by HOME recovery.
  This preview is not an electrically SAFE/HiZ tool.

See [startup details](M2_DISPLAY.md#startup-side-effects-accepted-by-m2).

## Download the preserved file

On the repository's `feature/wilipirate-panel` branch, open
[`releases/m2-ui-v001/WiliPirate.uf2`](../releases/m2-ui-v001/WiliPirate.uf2)
and use GitHub's raw-file download. A fixed-closeout download is available at
[WiliPirate.uf2 at commit 1c77ea3](https://github.com/gingerSou1/WiliPirate/raw/1c77ea3b5837716f8643b2597cf0543486c958d0/releases/m2-ui-v001/WiliPirate.uf2).
Save the binary as `WiliPirate.uf2`, not a GitHub HTML page.

The expected size is **53,248 bytes**. Source baseline: `b069a71`.
No GitHub Release has been published; use this preserved repository artifact.
The [manifest](../releases/m2-ui-v001/manifest.json) and
[notices](../releases/m2-ui-v001/notices/) accompany it. The manifest and
[closeout record](M2_DEVICE_VALIDATION.md) are historical evidence and retain
the original test-device identifier; it is not an installation requirement.

## Verify SHA-256

Expected SHA-256:

```text
6bc0a08050c1e18659882bc486a1f03b6e16529916b60112240f548aeb1e6672
```

From the directory containing the downloaded file, use PowerShell:

```powershell
Get-FileHash -LiteralPath .\WiliPirate.uf2 -Algorithm SHA256
```

Or Linux:

```sh
sha256sum WiliPirate.uf2
```

Or macOS:

```sh
shasum -a 256 WiliPirate.uf2
```

Compare all 64 hexadecimal characters; case does not matter. If the size or
hash differs, stop and obtain the correct file. Do not rebuild a substitute.
This checksum establishes identity with the recorded artifact, not a digital
signature or a guarantee of application security.

## Install through the supported SD Apps mechanism

Use the official WiliBSP `install-app` workflow. It validates the UF2, hands
MAIN's SD reader to the host, copies and flushes the file under SD `/apps/`,
then returns SD ownership to MAIN. Do not manually interrupt that handoff,
eject/unmount the device during the workflow, or treat another mounted drive
as the application SD card.

Prerequisites are a working official WiliBSP host-tool setup, its Python
dependencies (`pyfwfinder` and `pyserial`), a host/device
USB connection and an available MAIN SD card. No compiler is needed for this
transfer. Follow the pinned [official SD application installation guide](https://github.com/freewili/wilibsp/blob/be4bdd63d31a80f95410e583710cf4e43a7be7fa/docs/app-storage.md)
for the installer workflow and host dependencies; the
[official tooling quick start](https://github.com/freewili/wilibsp/blob/be4bdd63d31a80f95410e583710cf4e43a7be7fa/README.md#quick-start)
explains the CLI launchers. Its build/debug prerequisites are not required to
transfer this prebuilt UF2. This repository does not bundle host tools into it.

Example from a WiliPirate checkout with the pinned `wilibsp/` dependency and
official host tools already configured:

```sh
python -B wilibsp/tools/fw.py install-app releases/m2-ui-v001/WiliPirate.uf2 --device YOUR_DEVICE_SERIAL
```

Replace `YOUR_DEVICE_SERIAL` with your own device identifier. With a standalone
download, supply that file's path to the same official installer. Check the
installer's result; stop on validation, connection or transfer errors.
The required destination is **SD `/apps/WiliPirate.uf2`**, not CM0 Linux
`/home/apps/`. If a same-named app already exists, preserve its file before
deliberately replacing it using the supported workflow.

**Do not use BOOTSEL, a firmware updater, `flash`, a debug-probe workflow or
a stock-firmware replacement procedure.** This file is a RAM application.

## Launch, navigate and exit

1. After the installer has returned SD ownership to MAIN, open the stock
   **Apps** menu and select **WiliPirate.uf2**.
2. Tap any of the six tiles to view its unavailable placeholder. Use touch
   **BACK** or physical **CANCEL** to return to the menu.
3. Hold physical **PAGE for five seconds** to show About (name, version 001
   and source URL), then release it to return to the app.
4. Hold physical **HOME for five seconds** to reboot DISPLAY through the
   official recovery loader and return to the stock application. The on-screen
   Exit/Help button explains this action; it does not itself exit.

## If the application fails

Keep external wiring disconnected. For missing touch, an unresponsive page or
an unusable display, use physical HOME for five seconds; touch is not required
for the app's recovery mechanism. Verify that the stock interface returns.
HOME recovery was user-tested for v001; missing-touch failure handling was not
separately physically validated.

If HOME recovery does not restore stock operation, stop using the preview and
follow official FreeWili support/recovery guidance. Do not flash firmware as an
application troubleshooting step. Once stock operation is restored, the app
file may be removed through supported SD file management to avoid relaunching.
Report the artifact checksum, observed failure and firmware component versions
if known; redact serial numbers, personal paths and unrelated logs before
posting publicly.

The historical [validation record](M2_DEVICE_VALIDATION.md) describes the exact
test scope. No bus tools, electrical compatibility tests or M3 functionality
are supplied by this preview.
