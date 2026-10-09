# Retired WiliPirate development

The active project is now **WiliPirate — Field Analyzer**. The earlier
Bus-Pirate-inspired six-protocol menu and the direct I2C integration effort
are retired, not deleted. Stock FREE-WILi capabilities remain the foundation;
the new project prioritizes navigation, connection guidance and capture workflows.

## Recoverable material

- `m2-ui/`: byte-identical snapshots of M2 source, host tests, original root
  CMake and architecture/physical validation. Known-good source: `b069a71`;
  closeout `1c77ea3`. The validated binary remains at its original tracked
  `releases/m2-ui-v001/WiliPirate.uf2` location and must not be replaced.
- `m3-i2c-diagnostic/`: source, capture tests, documents, successful offline
  build evidence, manual procedures and the first physical-result analysis,
  including the still-uncertain RAW HEX transcription. Checkpoints: `f0749ea`
  and `8057bf9`. This snapshot also retains the uncommitted analysis present
  before the pivot; its byte hashes are in `manifest.json`.
- Original `native/display/` and `native/i2c_diag/` source/test paths remain
  available so earlier regression checks and historical Git references work.
  They are not active targets in the new root build. M3's former front-door
  instructions are now archival pointers. Do not execute archived deployment
  commands merely because they remain in the snapshot.

`manifest.json` identifies each preserved file and SHA-256. The device-identifying
M2 physical validation report remains at its unchanged original tracked path,
`docs/M2_DEVICE_VALIDATION.md`; the manifest references that path rather than
publishing a redundant archive copy. Copies are independent
preservation snapshots, not new upstream imports or changes to history.
Snapshot documents retain their original relative links/build paths to preserve
their bytes. Read them alongside the original repository layout or recover the
referenced Git revision; they are not independent deployment packages.
The original CM0 prototype and immutable `docs/HARDWARE_VALIDATION.md` remain
untouched. The historical archive/cm0-prototype branch is not published;
`bc738b8` remains reachable in Git history.

## Artifact identities

| Artifact | SHA-256 | Preservation |
| --- | --- | --- |
| M2 `WiliPirate.uf2` | `6bc0a08050c1e18659882bc486a1f03b6e16529916b60112240f548aeb1e6672` | Existing tracked release file unchanged |
| M3 `WiliPirateI2CDiag.uf2` | `95719e2cec6415dc974297973d5938f192a2a431c18f8899063d2f385e804a0e` | Existing ignored local build output retained; not newly redistributed |

The installed diagnostic on the physical SD card remains untouched. No physical
SD access or cleanup is authorized by this pivot. The user-reported first M3
test established launch/HOME recovery and raw capture of a firmware FPGA-zone
refusal, not successful I2C address acquisition. No FPGA power change was made.

Licenses, attribution, upstream notices and historical evidence are preserved.
Archiving does not resolve third-party binary redistribution permissions.
