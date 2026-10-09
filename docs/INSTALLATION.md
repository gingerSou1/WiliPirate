# Field Analyzer build and deployment status

v002 was subsequently installed under explicit installation-only approval,
then physically UI/navigation/HOME validated by the user. See
[closeout and preserved rollback](V002_DEVICE_VALIDATION.md).
v003 is currently host-tested only; no new device build, installation or launch
is approved by the v003 task. Historical approval statements below describe
the earlier milestone and are not standing permissions.

The active Field Analyzer is a UI/navigation preview. Use
[native/field_analyzer/README.md](../native/field_analyzer/README.md) for offline
host validation and the explicit native startup gate. No physical installation
or launch is authorized by this milestone. Standard BSP startup has real
VREF/power/internal-GPIO effects and must not be called electrically inert.

The supported future application surface is the official SD-loaded DISPLAY
SRAM app mechanism; never replace stock firmware, use BOOTSEL or flash as app
installation. Device deployment remains a separate approval and compatibility
review. Capture/storage/AI and all instrument operations are unavailable.

The earlier M2 guide is preserved in
[archive/m2-ui/docs/INSTALLATION.md](../archive/m2-ui/docs/INSTALLATION.md).
It is historical guidance for the preserved M2 binary, not the current project
build. The original M2 release and checksums remain untouched.
WiliPirateI2CDiag and its manual workflows are retired; do not install, run or
delete its physical SD file during repository cleanup.

The startup investigation confirms an unavoidable target-facing external-VREF
selection on the inspected supported paths. See [the electrical-effects audit](FIELD_STARTUP_AUDIT.md).
A conditional offline-only exception produced the validated local candidate;
physical startup, device access and installation remain unapproved. The artifact
retains `WiliPirate.uf2` and will replace only SD `/apps/WiliPirate.uf2` after
separate approval. See [the backup/readback replacement plan](FIELD_REPLACEMENT_PLAN.md).
