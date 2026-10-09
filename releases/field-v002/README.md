# WiliPirate v002 rollback artifact

Preserved UI/navigation application for FREE-WILi 2, not a stock firmware image.
The user reports successful physical UI/navigation and five-second HOME recovery;
electrical safety and instrument operation have not been validated.

`WiliPirate.uf2`: 61,440 bytes, SHA-256
`53ca890ce882f8cc088657abd8f7cc4482292e150e5544a60044604e9a3c9a2f`.
It is preserved byte-identically; checkpointing does not rebuild or authorize
installation/launch. See [manifest](manifest.json) and
[physical closeout](../../docs/V002_DEVICE_VALIDATION.md).

Original WiliPirate work is MIT; dependencies retain their own terms. Preserve
the existing [FreeWili/Pico SDK/SEGGER notice set](../m2-ui-v001/notices/).
[Third-party questions](../../docs/THIRD_PARTY.md) remain unresolved; this
checkpoint does not establish a blanket binary redistribution grant or publish
a GitHub Release. Standard BSP startup has documented electrical effects.
