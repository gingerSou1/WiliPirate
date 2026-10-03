# WiliPirate development

Read [the pinned WiliCM0BSP guide](vendor/wilicm0bsp/AGENTS.md), its
[app contract](vendor/wilicm0bsp/docs/apps.md), and
[our architecture](docs/ARCHITECTURE.md) before changing the app.
If the submodule is absent, the immutable source links in docs/ARCHITECTURE.md
and docs/SOURCES.json identify the same upstream revision.

- This is a Python CM0 Linux app, not MAIN/DISPLAY/ESP32 replacement firmware.
- Entry: apps/wilipirate/run.sh; staged runtime: /home/apps/wilipirate/run.sh.
- M1 uses no hardware, OneWili connection, power zone, pins or persistent data.
  HiZ means no operations by this app, not electrically measured isolation.
- Default launch has no stdin dependency. Logs belong to the Linux Apps launcher.
  Future app data/config belongs in ~/.local/share/wilipirate/ and
  ~/.config/wilipirate/; never write it alongside installed source.
- Only the feature/wilipirate branch is authorized for development. No push,
  merge, remote creation, hardware testing, flashing, device package installs,
  BSP modifications or Milestone 2 without explicit user approval.
- Do not edit submodule sources or import Bus Pirate/Bit Pirate source trees.
- Local checks: python -B -m unittest discover -s tests -v; git diff --check.
  Stage locally: python -B tools/stage.py --output dist/apps.
- BSP driver CMake tests apply when changing that driver, not this pure Python
  application. Never run upstream hardware examples as host validation.
