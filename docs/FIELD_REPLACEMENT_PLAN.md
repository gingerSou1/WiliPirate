# Controlled replacement of WiliPirate

**Plan only. Offline build and physical deployment approval are separate.**
Conditional permission produced the validated local candidate below, but startup
execution, device access, SD replacement and launch remain unauthorized.
The physical gate is CLOSED; see [the startup audit](FIELD_STARTUP_AUDIT.md).

Candidate: `build/field-device/WiliPirate.uf2`, 61,440 bytes, SHA-256
`53ca890ce882f8cc088657abd8f7cc4482292e150e5544a60044604e9a3c9a2f`.
This is the existing WiliPirate replacement, not a second launcher. No approved
physical startup or installation is implied by its compile-time exception.

## One identity and rollback

Target/name WiliPirate, metadata v002; future file
`build/field-device/WiliPirate.uf2`; installed `/apps/WiliPirate.uf2`.
Do not install `WiliPirateFieldAnalyzer.uf2` or a second launcher.

Known-good M2 rollback remains `releases/m2-ui-v001/WiliPirate.uf2`, 53,248 bytes,
SHA-256 `6bc0a08050c1e18659882bc486a1f03b6e16529916b60112240f548aeb1e6672`.
Archived source/evidence/checksum manifests remain untouched. Preserve M3
source/tests/findings and the installed `/apps/WiliPirateI2CDiag.uf2`; no initial
replacement step deletes or changes that diagnostic.

## Prerequisites

1. Explicitly approve the audit's bounded standard-startup exception, not
   inferred from M2 launch. Then build only the replacement target in its own
   output directory with the approved gate enabled and fetching disabled.
2. Validate official SRAM/metadata checks, every UF2 block and ELF load range,
   linked calls/startup baseline, Python/native tests and unchanged guides/UI.
   The current build's size/hash are recorded above. Reject flash, mixed or
   unintended PSRAM payloads. The old M2/diagnostic hashes are not its hash.
3. Review the candidate and obtain explicit physical access, SD replacement,
   first launch and observation approval. No automatic deployment.

## Future approved official SD workflow

1. Disconnect all targets, header/VREF wiring, external supplies, probes and
   accessories before first launch. Identify the correct device/SD and installed
   MAIN/DISPLAY/loader compatibility through official tooling only after approval;
   do not reuse a stale COM port or drive letter.
2. Recheck candidate and preserved M2 hashes. Use the pinned official
   `install-app` workflow, never BOOTSEL, a firmware updater, flash or debugger
   loading. Prepare/review a host-only backup/readback wrapper around unchanged
   tooling; it has not been implemented under this investigation.
3. During the normal SD-to-host handoff, before destination replacement, read
   existing `/apps/WiliPirate.uf2` and save exact bytes to a new ignored host
   rollback directory such as `build/rollback/<session>/WiliPirate.uf2`. Flush,
   hash and compare against M2. If absent/different, stop before overwriting and
   review. Do not create another rollback launcher in SD `/apps/`.
4. Retain official temporary-copy/flush/atomic-rename behavior, replacing only
   `/apps/WiliPirate.uf2` with the sole candidate input and no app subfolder.
   The reviewed wrapper may observe the rename event to preserve the old file
   beforehand and read back the new destination during the settle window.
   Do not patch official sources or touch historical release bytes/other apps.
5. Verify full destination bytes/size/hash while host-readable, then let the
   official installer return SD ownership to MAIN and confirm completion.
   Missing readback is not silently treated as verification. No automatic launch
   or eject/unmount that interrupts the supported handoff.
6. On ambiguous copy/handoff failure, preserve backup/logs and stop, without
   automatic retry or firmware repair. A separately approved rollback restores
   the verified host/M2 file via the same supported workflow and same pathname.

## First manually approved launch

1. Select **WiliPirate.uf2** in stock Apps. Confirm the Field Analyzer home and
   About WiliPirate v002/repository identity. No second WiliPirate entry.
2. With no target connected, check four tiles, protocol selection, guides,
   placeholders, Tools/Captures/Settings, touch Back, physical CANCEL and
   on-screen Home. No acquisition, outputs, CAN TX, signal generation, target
   power/configuration operation or target connection.
3. Hold physical HOME five seconds and confirm stock interface/ordinary stock
   operation returns. Do not interpret that as measured VREF/power restoration.
   Recovery failure stops the test; follow official support, never firmware
   replacement as an application workaround.
4. Record observations, candidate hash, versions and failures; stop for review.
   This startup exception does not authorize later electrical connections.

Only after successful physical validation may removal of the diagnostic SD
file be proposed, with fresh explicit approval. Its archived source/evidence
remain. No diagnostic cleanup, commit/push/merge/release is authorized here.
