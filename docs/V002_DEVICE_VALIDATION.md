# WiliPirate v002 physical closeout

Recorded 2026-10-09 from the user's v003 request: physical UI validation,
navigation and five-second physical HOME recovery succeeded. These are user
observations, not automated measurements or new hardware access by the agent.

Preserved rollback: [WiliPirate.uf2](../releases/field-v002/WiliPirate.uf2),
61,440 bytes, SHA-256
`53ca890ce882f8cc088657abd8f7cc4482292e150e5544a60044604e9a3c9a2f`.
[Manifest](../releases/field-v002/manifest.json) records identity and scope.
The copy is byte-identical to the previously installed candidate. It is an
application SRAM payload, not stock firmware. No release was published.

Electrical safety, startup transients, VREF/power restoration, bus compatibility
and actual instrument operation have **not** been validated. HOME recovery is
not proof of restoration of electrical settings. The documented BSP startup
effects remain applicable. No additional startup exception is granted for v003.

Historical M2/M3 archives and M2 validated release bytes remain unchanged.
The diagnostic SD file is untouched; there was no device access in this work.
