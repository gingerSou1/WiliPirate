# M1E offline build and validation — BLOCKED

Date: 2026-10-05. Baseline: feature/wili-ui at 5a5ebcf.
No physical connection, deployment, service manipulation, firmware change,
external bus operation, BSP edit, or merge occurred. M1A evidence is unchanged.

## Environment and authenticated bootstrap

Host: Ubuntu 26.04.1 LTS (Resolute), x86_64, WSL2. Only debootstrap
1.0.142ubuntu2 and debian-archive-keyring 2025.1ubuntu1 were installed on Ubuntu.
The initial incomplete bootstrap was interrupted and its exact resolved path
/opt/wilipirate-trixie was verified before removal. Fresh bootstrap used
--force-check-gpg and the Ubuntu-authenticated Debian archive keyring.
Release signature verified with key 41587F7DB8C774BCCF131416762F67A0B2C39DE4.
No authentication bypass was used.

Isolated amd64 build root: /opt/wilipirate-trixie, Debian GNU/Linux 13,
VERSION_CODENAME=trixie; base-files reports DEBIAN_VERSION_FULL=13.7.
Only approved explicit packages and their required dependencies were installed.

| Package | Installed version |
| --- | --- |
| gcc-14-aarch64-linux-gnu / g++-14-aarch64-linux-gnu | 14.2.0-19cross1 |
| libc6-arm64-cross / libc6-dev-arm64-cross | 2.41-11cross1 |
| binutils-aarch64-linux-gnu | 2.44-3 |
| cmake | 3.31.6-2 |
| make | 4.4.1-2 |
| file | 1:5.46-5 |
| qemu-user | 1:10.0.13+ds-0+deb13u1 |
| python3 | 3.13.5-1 |
| ca-certificates | 20250419 |

GCC/G++ report 14.2.0, binutils 2.44, QEMU 10.0.13. GCC 14 and target
glibc 2.41 were checked before CMake configuration. No Ubuntu target libraries
were used. The compiler reports sysroot /, not /usr/aarch64-linux-gnu.
It manages target headers under /usr/aarch64-linux-gnu/include (including
c++/14), compiler headers under /usr/lib/gcc-cross/aarch64-linux-gnu/14/include,
and searches /usr/include last. Target startup objects/libc are under
/usr/aarch64-linux-gnu/lib; compiler support libraries also use
/usr/lib/gcc-cross/aarch64-linux-gnu/14. Full resolved search paths and package
inventory are recorded locally in build/m1e-evidence/build.log and packages.txt.

## Reproduction

From WSL as root, run tools/m1e-bootstrap.sh from the checkout. It refuses an
existing build-root path and requires Release signature verification. Copy the
checkout to /opt/wilipirate-trixie/src, then:

```sh
chroot /opt/wilipirate-trixie sh /src/tools/m1e-build.sh
chroot /opt/wilipirate-trixie sh -c \
  'cd /src && python3 -B tools/m1e-qualify.py build/cm0/WiliPirate'
chroot /opt/wilipirate-trixie sh -c \
  'cd /src && WILIPIRATE_SYNTAX_CXX=aarch64-linux-gnu-g++-14 python3 -B -m unittest discover -s tests -v'
```

The build script uses the pinned BSP pi-toolchain.cmake with explicit versioned
GCC/G++ overrides, Release, existing -Wall/-Wextra/-Werror, and no PI_SYSROOT
override. WILICM0_BUILD_DRIVER and BUILD_TESTING remain OFF. Only WiliPirate is
built. No upstream hardware examples or driver CTest were run. The script
currently returns failure at the intentionally failing integration regression.

## Resulting executable

Real ARM64 binary: YES. Size: 246688 bytes. ELF64 little-endian AArch64,
ET_DYN PIE, dynamically linked, interpreter /lib/ld-linux-aarch64.so.1.
DT_NEEDED: libstdc++.so.6, libgcc_s.so.1, libc.so.6.

Required versions: GLIBC_2.17, GLIBC_2.34; GLIBCXX_3.4, GLIBCXX_3.4.19,
GLIBCXX_3.4.21; CXXABI_1.3, CXXABI_1.3.9; GCC_3.0. All direct version needs
were found in the selected target libraries by tools/m1e-qualify.py.
The target loader under QEMU resolved libc, libstdc++, libgcc_s and transitive
libm from the Trixie cross runtime. QEMU execution used -L
/usr/aarch64-linux-gnu. This supports the recorded baseline, not the unknown
installed physical image or firmware compatibility.

Unqualified build artifact SHA-256 (NOT a deployment checksum):
589cfee7eff9e7d314509ed6f451a07dee65ed7307a3586974abe654653b5d76.
Artifact and raw evidence retained locally under build/m1e-evidence/; ignored
by Git. No dist/m1d-cm0/wilipirate package or deployment checksum manifest exists.

## Integration and regression results

native/cm0/test_bridge.py launches the unchanged production binary under QEMU.
It requires an explicit isolated-root marker and refuses an existing socket.
Its fake binds /run/fwcm0-bridge.sock in the chroot; no path/transport override
or replacement Device abstraction is used. It checks OP_API framing, quiet
reset prefixes and an exact allowlist of identity/UI command paths. Replies
follow the pinned BSP socket test and OneWili C framing. It sends no external
hardware request and verifies socket EOF cleanup where available.

| Integration test | Result |
| --- | --- |
| Startup, Hardware DISABLED, Help, all five logical modes, Exit, Reset Display, EOF | PASS |
| Missing bridge, nonzero exit, No fallback | PASS |
| Mid-loop disconnect, nonzero exit, no retry/fallback | PASS |
| Malformed nonhex button payload, nonzero exit, cleanup/EOF | PASS |
| Malformed identity probe, nonzero exit, no UI commands, EOF | PASS |
| Validly framed response for wrong command path | FAIL |

Six integration tests: five passed, one failed. Existing complete suite: 50
tests attempted, 49 passed, one error because tkinter is absent in the isolated
root. This includes passing import/call guards, runtime audits, all logical
model parity checks, packaging tests and immutable M1A report identity.
The Tk event test imports tkinter even though its UI is mocked. python3-tk
would be an additional unapproved package; it was not installed. No test was
removed, weakened, or reported as passing through a skip.
The suite was also run with the existing Windows Python 3.12 installation:
50 tests, OK, with one skip for unavailable C++ compiler (49 passed).
The skipped native constexpr/model parity test passed in Trixie using GCC 14;
the Tk test passed on Windows. Together both runs exercise all 50 existing
tests successfully, without installing another package. Neither run alone
was a 50-pass result.

## Protocol blocker and next approval point

On the first button poll (g\\c\\e), the fake returns a well-framed i\\g\\u
response containing hex 10. Pinned OneWili ow__call accepts the next standard
response frame without correlating its command path or sequence; read_buttons
decodes 10 as Exit. WiliPirate returns success, rather than failing closed.
The failing test preserves this concrete regression. It does not perform GPIO:
the wrong-path bytes originate entirely in the fake.

Do not package or deploy this executable. Repairing or replacing the official
parser/adapter is outside authorization: BSP sources must remain unchanged.
A separately approved upstream-qualified dependency change or supported
validation mechanism is needed. No custom transport workaround was introduced.
The malformed-token cases passing do not establish rejection of all malformed,
oversized, extra-token, stale or unexpected responses. Those remain UNVERIFIED.

## Eventual physical deployment — not authorized or performed

After the blocker is resolved, complete regression checks and stage only
WiliPirate, run.sh, app README and pinned dependency licenses using the native
CMake app install rules. Verify no unrelated BSP install artifacts enter the
folder, Linux executable permissions, relocatability and SHA-256 manifest.
Then, with separate physical authorization and installed-stack compatibility
review, transfer the tested folder via documented FreeWili GUI Linux file tools
or configured SSH/SCP to CM0 /home/apps/wilipirate. This is not MAIN SD /apps.
Verify deployed hashes and executable bits for run.sh and WiliPirate.
Use Linux > Apps > wilipirate > run.sh, check the launcher log under
~/.local/state/freewili/apps/, verify HiZ / Hardware DISABLED, Help, logical
mode changes and Exit, then stock operation. A PID alone is not success.
Cleanup performs one checked Reset Display request; exact previous-panel
restoration, display/input compatibility and physical pin state are UNVERIFIED.
Removal: stop the app first, remove only its CM0 code folder using the same
supported Linux file tools; preserve separate user config/data if present.
The pinned vendor/wilicm0bsp/docs/apps.md is the authority for these paths.
