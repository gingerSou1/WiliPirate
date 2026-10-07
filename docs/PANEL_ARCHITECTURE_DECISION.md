# Architecture decision: preserve CM0, investigate a native FW2 panel

Date: 2026-10-07. Status: pivot accepted; native implementation BLOCKED.

## Preservation and branch point

The initial checkout was clean on `feature/wili-ui` at
`bc738b88976917dffb693265fcee162716f58ec0`.
Created `archive/cm0-prototype` at that exact commit without rewriting history.
Created `feature/wilipirate-panel` at the same commit. Retaining this branch
point preserves source, tests, safety evidence and reusable parser/state/UI
concepts together. It does not select the CM0 implementation as the new runtime.
Future native code belongs in a separate app tree; no CM0 transport, launcher
or staging logic should be copied into it merely because it already exists.

Initial `git status --short` was empty. `git branch -vv` reported:

```text
feature/wili-ui bc738b8 [origin/feature/wili-ui]
main            2c121b7 [origin/main]
```

`git log --oneline --decorate --all` showed the following complete commit chain
(newest first; decorations below describe the initial checkout):

```text
bc738b8 HEAD -> feature/wili-ui, origin/feature/wili-ui: M1E.1 stop checkpoint
3b9f5e6 response identity characterization
0cbf532 ABI evidence and qualification blocker
0b14ee1 ARM64 offline build and unexpected reply acceptance
5a5ebcf stub CM0 UI preparation
d6820db on-device architecture comparison
c91eb71 M1C checks
06968e9 host panel preview
ca1009f emulator limits and htop research
2c121b7 main, origin/main, origin/feature/wilipirate, origin/HEAD: M1B validation
cffb37c logical modes/stubs
ea864b8 M1A preflight
77cc5d9 scaffold validation
f6cacf7 CM0 scaffold
6b494df capability matrix
eac151f architecture research
98de0b3 repository initialization
```

The sole remote was `origin`, fetch/push
`https://github.com/gingerSou1/WiliPirate.git`. No remote changes, fetch, push,
merge, branch deletion or history rewriting occurred.

## What the CM0 investigation established

CM0 Linux was investigated for a supported user-space add-on that could leave
stock firmware running and offer a familiar hacker console. M1A exposed the
Python adapter's direct-hardware fallback and deferred physical validation.
M1B established a strict parser, logical HiZ/modes, explicit unavailable stubs,
relocatable staging and safety regression checks. M1C established an independent
host-only panel preview and documented the official simulator's limitations.
M1D selected the official socket-only C++ adapter and prepared source-only UI.
M1E built a real AArch64 executable using Debian Trixie/GCC 14, checked ABI
requirements and ran offline fake-bridge tests under QEMU. See
[M1E evidence](M1E_VALIDATION.md) and [M1E.1 evidence](M1E_1_RESPONSE_VALIDATION.md).

The unresolved OneWili C response-validation finding accepts a plausible reply
from the wrong command path, including the constructor identity probe and
button polling. Three preserved regressions fail. This pivot does not fix that
parser, weaken those tests, update dependencies or produce a CM0 deployment
package. No physical Wili deployment occurred. Existing ignored build evidence
also remains in place; an archival Git branch preserves committed evidence,
not ignored binary files.

CM0 is shelved, not declared invalid. Linux remains suitable for Linux-dependent
applications such as SDR/ADS-B, processing pipelines or other workloads that
actually need its services. A hardware panel should first reuse stock MAIN
operations and official FW2 libraries rather than create another bus stack.

## New direction and gate

Investigate a supported DISPLAY RAM application with GPIO, UART, I2C, SPI,
CAN and Logic tiles; no custom bus drivers. Logical HiZ means this application
performs no external operations; it does not certify electrical isolation.
Entering a tile must not initialize a bus. Power/VIO changes require a separately
authorized design and must never be implicit launch behavior.

[Current research and reuse matrix](PANEL_REUSE_MATRIX.md) confirms a supported
nonpersistent app mechanism, but does not establish safe unchanged-VIO startup
or a supported I2C address-result path. Accordingly no native panel scaffold,
binary or package is produced. This is the user's explicit offline stop gate,
not a request to relax it. All old implementation/evidence files remain intact.
