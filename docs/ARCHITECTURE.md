# WiliPirate Field Analyzer architecture

The active application is a Field Analyzer navigation preview, not the retired
six-protocol menu or I2C scanner. See PROJECT_CHARTER.md and ROADMAP.md.
The previous CM0/M1B contract is preserved byte-for-byte at
[archive/m2-ui/docs/ARCHITECTURE.md](../archive/m2-ui/docs/ARCHITECTURE.md).
Its stub runtime and host Tk preview remain unchanged.

## Current layers

v003 extends the pure model with a bounded normalized CAN frame/result decoder,
eight-frame simulation ring and frozen volatile RAM snapshot. No live adapter
is linked. CAN Guide -> Configure -> Listen -> simulated Monitor -> Capture ->
Review is explicit about simulation and the closed live gate. Tools contains
the inert Glitching pane and its guide; it is not another home instrument.
See [qualification findings](V003_CAN_INVESTIGATION.md).

```text
Native touch / physical CANCEL
  -> field_analyzer/model (page, selected instrument, held-touch suppression)
  -> constant Connection Guide dataset (documented facts + explicit gaps)
  -> field_analyzer/ui (backend-independent 480x320 rendering)
  -> host draw sink / guarded official DISPLAY adapter
```

There is no instrument backend, OneWili connection, stock-panel shortcut,
physical recording, storage, export, AI client or configuration setter. Home selection
opens a guide before the unavailable instrument preview; Protocol Analyzer
first selects CAN/UART/I2C/SPI. Back retraces guide/protocol/home. On-screen HOME
returns to the app home. Physical HOME's long hold is the official native
recovery mechanism, not the virtual navigation action.

The renderer uses rounded native primitives, the pinned BSP font, navy/teal
colors and high contrast. Tests execute the production state and renderer on a
host, check bounds/full clears and generate PNG previews. This is layout and
navigation validation, not electrical or device verification.

## Build and startup boundary

Root CMake now selects host Field UI validation by default. Old M2/M3 targets
are not active build dependencies. A device candidate uses the official SRAM
application helper in a separate build directory, retaining the `WiliPirate`
target, `WiliPirate.uf2` filename and SD `/apps/WiliPirate.uf2` identity. It requires both
FIELD_BUILD_DEVICE and FIELD_STANDARD_STARTUP_APPROVED explicitly enabled.

The standard BSP initialization still changes VREF/internal GPIO/power; no
supported no-change startup was established. The native configuration fails
closed without startup approval. No flag grants deployment permission. Host
validation causes no physical operations, and all hardware/deployment remains
blocked for review under this milestone's charter. Do not silently use a
minimal undocumented startup, patch the BSP or remove recovery requirements.

If the standard-startup exception is approved for an offline candidate, the
adapter uses ordinary board/display/touch/HOME/About initialization and only
DISPLAY metadata. It makes no instrument, VREF/power setter, output, SD or
OneWili calls, and no extra release policy. Startup still cannot be described
as electrically inert. Actual HOME/touch behavior remains a physical test.

## Future interfaces

A separately approved instrument layer must report unavailable capabilities
and fail closed, never switch to direct hardware or a replacement driver.
Capture records need explicit timebase/settings/loss metadata; formats and SD
ownership must be proved before writes. No backend discovery or environment
switch can turn current placeholders into real instruments.

CAN development is paused; its simulation and research are preserved. Future
live CAN requires connector/termination/level and listen-only/no-ACK qualification
on the standalone bench. GUI interoperability and AI are separate
contracts; no automatic hardware actions or unverified navigation shortcuts.
Official dependencies remain pinned/read-only. M2 release identity and the
installed retired diagnostic are preserved; archive cleanup is not permission
for device SD operations.
