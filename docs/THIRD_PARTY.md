# Upstream references and attribution

Original WiliPirate code and documentation are licensed under the
[MIT License](../LICENSE), copyright © 2026 gingerSou1. This grant applies only
to original WiliPirate work. It does not relicense official FreeWili code,
vendored dependencies or other third-party material, supersede their terms,
or resolve third-party binary redistribution permissions.

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
with Bus Pirate electrical hardware. The original-code MIT license does not
grant rights to Bus Pirate trademarks or third-party assets.

## Unresolved public distribution questions

WiliPirate — Created by gingerSou1. This attribution applies to original
WiliPirate work, not official FreeWili code or other dependencies. The project
MIT license does not establish permission to redistribute the complete v001
binary while third-party permissions remain unresolved.

The preserved v001 [notice directory](../releases/m2-ui-v001/notices/) contains:

| Preserved notice | What is established | Remaining question |
| --- | --- | --- |
| `LICENSE` | WiliBSP MIT notice, copyright 2026 Dave Robins. | This is the BSP license, not a WiliPirate project license. |
| `Pico-SDK-LICENSE.txt` | Pico SDK BSD-3-Clause copyright, conditions and disclaimer. | Retain it with any authorized binary distribution; assess component-specific terms as well. |
| `SEGGER-RTT-NOTICE.txt` | SEGGER attribution and technical header commentary. | This text contains no explicit redistribution grant. Establish the applicable RTT version/provenance and license terms before claiming permission. |
| `WiliBSP-THIRD-PARTY-NOTICES.md` | Names RTT, FatFs and separately licensed harvested drivers. | Confirm which components are linked into v001 and whether all required notices accompany it; the list alone does not establish completeness. |

The pinned [RTT source](https://github.com/freewili/wilibsp/blob/be4bdd63d31a80f95410e583710cf4e43a7be7fa/bsp/third_party/segger_rtt/SEGGER_RTT.c)
and [header](https://github.com/freewili/wilibsp/blob/be4bdd63d31a80f95410e583710cf4e43a7be7fa/bsp/third_party/segger_rtt/SEGGER_RTT.h)
carry the same attribution/technical commentary. A public upstream repository
or its root MIT license must not be assumed to resolve separately licensed RTT
terms. This is an unresolved permission question, not a conclusion that use
violates a license.

Before a public release, resolve RTT redistribution permission. Complete a
linked-component notice
inventory from retained build/link evidence (including SDK components and the
BSP font), without rebuilding or changing the validated artifact. If evidence
is insufficient, record the gap rather than assuming unused archive members
were linked. Do not add guessed licenses or modify official dependency sources.

Keep the existing UF2, manifest and upstream notices intact while these questions
are reviewed. Any future notices or license changes require explicit approval.
