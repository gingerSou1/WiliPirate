# WiliPirate

WiliPirate is a FREE-WILi 2 CM0 Linux Wili/OneWili application inspired by
Bus Pirate and ESP32 Bit Pirate. **Milestone 1B is entirely host-side:** a
usable mode-aware console with explicit STUB backends and no hardware access.
Stock firmware is preserved. No upstream Bus Pirate/Bit Pirate code is copied.

## Run locally (Python 3.10+; no dependencies)

```sh
python -B apps/wilipirate/app.py --console
```

On Windows, `py -3.12` may replace `python`.

```text
HiZ> mode i2c
Mode: I2C
I2C> scan
I2C hardware backend not enabled.
I2C> mode uart
Mode: UART
UART> info
WiliPirate 0.1.0-m1b | FREE-WILi 2 CM0 application
Mode: UART
Backend: STUB
Hardware: not enabled
```

The session starts in HiZ. Supported commands are `help`, `info`, `mode`,
`mode hiz`, `mode uart`, `mode i2c`, `mode spi`, `mode gpio`, and `exit`.
Mode changes update only application state; even selecting a bus performs no
initialization or pin/power changes. Invalid modes preserve the current mode.
`help` lists the current mode's stub requests. `scan` (I2C), `read` and
`write <arguments...>` (UART/I2C/GPIO), and `transfer <arguments...>` (SPI)
only report an unavailable backend. Arguments remain opaque and no fake
ACKs, bytes, measurements or successful transfers are returned.

HiZ does not establish electrical isolation. External pin/power state is
unknown. All modes use stubs; no real backend or transport can be selected by
flags, environment variables or discovery. M1A and physical validation are deferred.

## Launcher and batch use

```sh
python -B apps/wilipirate/app.py
python -B apps/wilipirate/app.py --command "mode uart" --command info --command exit
```

Default launch prints help/info/modes and exits without reading stdin, matching
the documented Linux Apps contract. Explicit `--console` reads terminal input
and keeps the selected mode until exit. Batch commands share one session and
stop at the first failure (exit status 2); a stub operation is a failure, not
hardware success. Normal exit/EOF return 0; console Ctrl-C returns 130.
Each new invocation starts in HiZ. There is no LCD/touch renderer in M1B.

## Check and stage locally

```sh
python -B -m unittest discover -s tests -v
python -B tools/stage.py --output dist/m1b/apps
git diff --check
```

The staging tool copies only the app entry point, package modules, launcher
and app/attribution documentation into `dist/m1b/apps/wilipirate/`. It refuses
an existing destination. No dependencies are fetched or installed, and no
files are transferred to a device. Use a new output directory to stage again.
The previous M1 staged folder, if present, is not updated in place.

After a separate approval and safe framework validation, the eventual device
location is `/home/apps/wilipirate/run.sh`, selected through Linux > Apps.
It must have its executable bit preserved. The [app README](apps/wilipirate/README.md)
travels with the staged folder. Current development does not deploy it.

## Design and evidence

- [Current layered architecture and mandatory fail-closed policy](docs/ARCHITECTURE.md)
- [Interactive UI research and unresolved htop reference](docs/UI_RESEARCH.md)
- [Capability matrix (hardware evidence, not stub functionality)](docs/CAPABILITY_MATRIX.md)
- [Preserved M1A safety finding](docs/HARDWARE_VALIDATION.md)
- [M1B validation](docs/M1B_VALIDATION.md) and [earlier host checks](docs/VALIDATION.md)
- [Source revisions](docs/SOURCES.json), [GUI archive review](docs/GUI_EXAMPLES.md),
  and [upstream licenses/reuse decisions](docs/THIRD_PARTY.md)

The pinned BSP reference is unmodified and unnecessary for M1B execution.
WiliPirate must never silently fall back from the supported bridge to direct
hardware access. Missing required bridge/API capability must fail closed.
No workaround for the discovered fwcm0 fallback is included.
