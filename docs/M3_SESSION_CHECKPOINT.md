# M3 diagnostic resume checkpoint

Date: 2026-10-09. Branch: `feature/wilipirate-panel`.
Source/HEAD checkpoint: `f0749ea4092dbb4b366fa3e8666df11daedb6aa6`.

The offline resume resolved picotool's stale generated archiver configuration
in a fresh workspace-local build directory, using cached sources/tools only.
Successful pioasm was reused. No dependency source or M2 code was changed.

Diagnostic target build and official SRAM/metadata validation passed. Artifact:
`build/m3-diag/WiliPirateI2CDiag.uf2`, 66,560 bytes, 130 UF2 blocks.
SHA-256: `95719e2cec6415dc974297973d5938f192a2a431c18f8899063d2f385e804a0e`.
All 33,164 payload bytes target SRAM: `0x20000000` to exclusive end
`0x2000818c`. No QSPI flash or PSRAM payload. Host checks: 55 Python tests and
14 synthetic capture cases passed. M2 artifact checksum remains unchanged.

Full raw retrieval uses paginated LCD hex with offsets and a byte count;
metadata and chunk timing are also paginated. There is no file export. Record
every page before HOME clears RAM. Physical capture schema and recovery remain
unverified; no device was accessed or deployed to.

Details: [validation record](../native/i2c_diag/VALIDATION.md),
[manual empty-bus instructions](../native/i2c_diag/MANUAL_TEST.md), and
[source investigation](M3_I2C_STOCK_API.md).

The later commit/push request authorizes publishing the diagnostic source,
tests and documentation on `feature/wilipirate-panel`. This is not a GitHub
Release or physical deployment approval. The diagnostic UF2 remains ignored
build output; do not track it as a release artifact without separate approval
and resolution of the applicable third-party redistribution questions.

Wait for explicit deployment/manual-test approval. Do not merge, replace stock
firmware, alter the M2 artifact, wire a PN532 or automatically start physical
testing. No commit was made during the offline resume itself.
