# Upstream references and attribution

No Bus Pirate or Bit Pirate source code or assets are included in WiliPirate.
The mode prompt, command vocabulary and protocol-adapter separation are concept
references. M1's parser and launcher are independently written Python/shell.

| Project | Inspected license | Reuse decision |
| --- | --- | --- |
| Bus Pirate 5/6 | [LICENSE](https://github.com/DangerousPrototypes/BusPirate5-firmware/blob/f8ccc1c4b5c392ffb09fb21348bcedf219919816/LICENSE.TXT): MIT, copyright 2023 Ian Lesnet, Where Labs LLC. [docs/licenses.md](https://github.com/DangerousPrototypes/BusPirate5-firmware/blob/f8ccc1c4b5c392ffb09fb21348bcedf219919816/docs/licenses.md) lists component-specific BSD-3-Clause and LGPL3 obligations. | Do not copy its Pico/PIO drivers or terminal stack. Any later selected file needs its header/dependencies audited; preserve copyright/license notices. |
| ESP32 Bit Pirate | [LICENSE](https://github.com/geo-tp/ESP32-Bit-Pirate/blob/8022f80381bb12bc87c5d4d69bd296db661d6d1c/LICENSE): MIT, copyright 2025 (no named holder in that notice). Vendored libraries have separate licenses. | Portable parser concepts are useful, but reimplement in Python. Do not presume the root MIT license covers every library. |
| WiliCM0BSP | [LICENSE](https://github.com/freewili/wilicm0bsp/blob/d27edf1c18bc2c18a76c4c9cdbf200c19884cc06/LICENSE): MIT, copyright 2026 Free-Wili; [THIRD-PARTY-NOTICES.md](https://github.com/freewili/wilicm0bsp/blob/d27edf1c18bc2c18a76c4c9cdbf200c19884cc06/THIRD-PARTY-NOTICES.md). | Pinned reference submodule, unmodified. No BSP runtime code bundled in the M1 staged app. |
| OneWili | [LICENSE](https://github.com/freewili/onewili/blob/9ce9df83b89f83681507f19f960958e23f20ac37/LICENSE): MIT, copyright 2026 Free-Wili. | Pinned through BSP; future CM0 package bundling must retain license and result dependency notices. |
| WiliBSP | [LICENSE](https://github.com/freewili/wilibsp/blob/be4bdd63d31a80f95410e583710cf4e43a7be7fa/LICENSE): MIT, copyright 2026 Dave Robins. | M2's unmodified pinned build/runtime dependency at `wilibsp/`; retains LICENSE and THIRD-PARTY-NOTICES.md. Native distribution must include applicable notices. |
| FreeWili GUI | Public README/changelog and official release distribution; no root source-code license found in inspected repository. | Research examples only; no GUI code/assets redistributed. Do not infer permission from public download availability. |

MIT permits reuse subject to retaining its copyright and permission notice in
copies/substantial portions. BSD-3-Clause additionally retains its conditions
and disclaimer and restricts endorsement. LGPL-covered code needs separate
compliance assessment before incorporation; no such code is incorporated here.
These are source-license observations, not a license grant for unrelated assets.

M2 also links the official Pico SDK 2.3.0, whose source license is BSD-3-Clause.
Keep its LICENSE.TXT alongside the BSP notices when distributing the native
artifact. Toolchain/host build tools are not bundled with the application.
The M2 renderer uses the BSP font through the library; no upstream font, driver
or UI source was copied into the application tree or modified.

Bus Pirate is a trademark of Where Labs LLC per its README. WiliPirate is an
independent application concept, not a claim of endorsement or compatibility
with Bus Pirate electrical hardware. A distribution license for original
WiliPirate code has not been selected; this milestone does not invent one.
