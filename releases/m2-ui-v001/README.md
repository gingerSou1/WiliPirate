# WiliPirate M2 UI v001 — validated baseline

Exact existing DISPLAY SRAM application, **53,248 bytes**, built from source
`b069a71`. Preserved without rebuilding. User-reported physical validation:
one FREE-WILi 2, all M2 UI checks PASS.

[WiliPirate.uf2](WiliPirate.uf2)

SHA-256:
`6bc0a08050c1e18659882bc486a1f03b6e16529916b60112240f548aeb1e6672`.

Use the [public installation guide](../../docs/INSTALLATION.md) for checksum,
SD Apps transfer, launch, HOME recovery and electrical precautions. No application
compilation is required. The [manifest](manifest.json) and historical validation
record retain the original test-device identifier as evidence, not a requirement.

See [historical validation/installation record](../../docs/M2_DEVICE_VALIDATION.md),
[build configuration and side effects](../../docs/M2_DISPLAY.md) and
[manifest](manifest.json). This app is UI-only; its six hardware tools are
placeholders. Standard BSP startup can change VREF/power and requires targets
disconnected. Install only through the official SD Apps mechanism with approval;
do not flash it or treat it as stock firmware.

`notices/` retains the existing BSP, SDK and RTT notices unchanged. Original
WiliPirate code and documentation use the [project MIT License](../../LICENSE).
That license applies only to original WiliPirate work and does not supersede
third-party terms or grant permission for every component of this binary.
The RTT redistribution terms and full linked-component notice inventory remain
unresolved; see [licensing questions](../../docs/THIRD_PARTY.md#unresolved-public-distribution-questions).
Do not treat these preserved notices as confirmation of complete redistribution
permission. No upstream source was modified.
