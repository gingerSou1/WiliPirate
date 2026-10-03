# WiliPirate

WiliPirate is a FREE-WILi 2 Wili/OneWili application for the CM0 Linux
application environment, inspired by Bus Pirate and ESP32 Bit Pirate.
Selected bus functionality will be reimplemented against supported FREE-WILi
APIs. It preserves stock MAIN/DISPLAY firmware and is not a firmware port.

**Milestones 0/1:** source research and a minimal no-I/O Python application.
No Bus Pirate/Bit Pirate code is incorporated. Hardware functionality,
touchscreen UI, device deployment and hardware testing are deferred.

## Try locally (Python 3.10+)

```sh
python -B apps/wilipirate/app.py
python -B apps/wilipirate/app.py --console
python -B apps/wilipirate/app.py --command help --command info --command mode
```

On Windows, `py -3.12` may replace `python` when installed.
The console starts at `HiZ>`. It supports `help`, `info`, `mode`, `mode hiz`
and `exit`. I2C, SPI, UART and GPIO are listed as unavailable and cannot be
entered. Unknown commands and hardware operations are rejected.

HiZ is an application no-I/O state, **not a guarantee of electrical isolation**.
External pin and target-power state is unknown. The app makes no hardware
calls or power changes. It does not enable, disable or reconfigure pins.

## Check and stage on the development host

```sh
python -B -m unittest discover -s tests -v
python -B tools/stage.py --output dist/apps
git diff --check
```

Staging produces a self-contained `dist/apps/wilipirate/` containing `app.py`,
`run.sh`, an app README and attribution notes. It refuses an existing target.
No packages are installed. Generated output is ignored by Git.
After future deployment approval, this folder goes under `/home/apps/` on
CM0 Linux, and its executable `run.sh` is selected in Linux > Apps.
No deployment is performed by the staging tool.

The normal menu launcher disconnects stdin, so default launch prints its
minimal interface to the app log and exits. Use `run.sh --console` from a
terminal for interactive input. See the [app README](apps/wilipirate/README.md).

## Research and boundaries

- [Architecture, API evidence and exact proposed first hardware test](docs/ARCHITECTURE.md)
- [Capability matrix](docs/CAPABILITY_MATRIX.md)
- [GUI-packaged Python example review](docs/GUI_EXAMPLES.md)
- [Pinned source revisions](docs/SOURCES.json)
- [Upstream licenses and reuse decisions](docs/THIRD_PARTY.md)
- [Validation record](docs/VALIDATION.md)

The reference BSP submodule is pinned; it is not required to run this no-I/O
scaffold. Initialize it with `git submodule update --init --recursive` only
when its source is needed. No BSP source edits are part of this project.

Next proposed milestone: GPIO read-only snapshots through OneWili after
explicit approval and environment review. I2C scan result decoding, UART
receive behavior, pin mapping, electrical limits and target-power policy
still require qualification. No physical FREE-WILi has been accessed.
