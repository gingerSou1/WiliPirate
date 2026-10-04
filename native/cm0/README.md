# WiliPirate CM0 native preparation

Prepared source, NOT a built or hardware-qualified deployment candidate.
Uses the pinned official WiliCM0BSP C++ socket-only adapter, not the Python
adapter or any CLI/direct transport. MAIN and DISPLAY stay stock. The UI has
Help, Mode and Exit; all five logical modes remain STUBS. No bus implementation.
The original Python core and Tk preview remain the behavioral reference.

Build on a development Linux ARM64 environment or with a matching host cross
compiler/sysroot (not on the physical Wili during this milestone):

```sh
cmake -S native/cm0 -B build/cm0 -DCMAKE_BUILD_TYPE=Release
cmake --build build/cm0 --target WiliPirate
cmake --install build/cm0 --prefix "$PWD/dist/m1d-cm0"
```

For cross compilation supply a reviewed CMAKE_TOOLCHAIN_FILE selecting Linux,
aarch64 and the matching sysroot. The target rejects desktop/Windows builds.
Initialize the already-pinned OneWili dependency on the host before configure;
no BSP source modifications or driver build are needed. Verify final ELF64
little-endian AArch64, dynamic dependencies and target glibc compatibility;
produce a checksum manifest before proposing deployment. Do not use the existing
Python staging tool for this binary. There is no .uf2 artifact for this model.

Future artifact directory: dist/m1d-cm0/wilipirate/ containing WiliPirate,
run.sh, this README and dependency licenses. Future installed location:
CM0 /home/apps/wilipirate/run.sh, through Linux > Apps. No transfer now.
The launcher's stdout/stderr log records failures. No root, package install,
service start, power changes or firmware update is performed by this app.

Input uses the shared documented press latch: gray Help, yellow cycles
HiZ/UART/I2C/SPI/GPIO, red or X exits. Exit wins simultaneous presses;
other combined presses are ignored. SIGINT/SIGTERM request loop exit.
Unexpected button/read/render errors stop the loop. Cleanup makes one checked
Reset Display request and closes the socket; it does not retry writes, reset
the board or promise restoration of the exact prior panel. SIGKILL, power loss
and failed transport can prevent cleanup. HOME is not an app termination key.
The adapter initializes its API session/console parser and automatically probes
Device State and API-session ownership; these
are mandatory preflight review items. It does not initialize external buses.

No native ARM64 binary was built here: a matching Linux development environment
was unavailable. ARM bare-metal compiler syntax/constexpr checks are not Linux
link/ABI or runtime validation. See docs/ON_DEVICE_ARCHITECTURE.md in the repo
for the complete comparison, source evidence and first-launch gates.
