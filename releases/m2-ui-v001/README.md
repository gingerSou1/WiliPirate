# WiliPirate M2 UI v001 — validated baseline

Exact existing DISPLAY SRAM application, **53,248 bytes**, built from source
`b069a71`. Preserved without rebuilding. User-reported physical validation:
FREE-WILi 2 **FX0141**, all M2 checks PASS.

[WiliPirate.uf2](WiliPirate.uf2)

SHA-256:
`6bc0a08050c1e18659882bc486a1f03b6e16529916b60112240f548aeb1e6672`.

See [validation/installation record](../../docs/M2_DEVICE_VALIDATION.md),
[build configuration and side effects](../../docs/M2_DISPLAY.md) and
[manifest](manifest.json). This app is UI-only; its six hardware tools are
placeholders. Standard BSP startup can change VREF/power and requires targets
disconnected. Install only through the official SD Apps mechanism with approval;
do not flash it or treat it as stock firmware.

`notices/` retains the linked BSP, SDK and RTT notices. Original WiliPirate
source licensing remains as recorded in the repository; this closeout creates
no new license grant. No upstream source was modified.
