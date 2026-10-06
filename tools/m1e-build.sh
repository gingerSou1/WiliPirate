#!/bin/sh
# Run inside the isolated Trixie root, with the repository copied to /src.
set -eu
cd /src
mkdir -p build/m1e-evidence
exec >build/m1e-evidence/build.log 2>&1
cat /etc/os-release
grep -q '^VERSION_CODENAME=trixie$' /etc/os-release
test "$(aarch64-linux-gnu-gcc-14 -dumpversion)" = 14
dpkg-query -W -f='${Version}\n' libc6-arm64-cross | grep -q '^2\.41-'
dpkg-query -W >build/m1e-evidence/packages.txt
aarch64-linux-gnu-gcc-14 --version
aarch64-linux-gnu-g++-14 --version
aarch64-linux-gnu-readelf --version
qemu-aarch64 --version
aarch64-linux-gnu-g++-14 -print-sysroot
aarch64-linux-gnu-g++-14 -print-search-dirs
aarch64-linux-gnu-g++-14 -E -x c++ -v /dev/null
for lib in crt1.o libc.so libstdc++.so libgcc_s.so; do
  aarch64-linux-gnu-g++-14 -print-file-name="$lib"
done
cmake -S native/cm0 -B build/cm0 -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_TOOLCHAIN_FILE=/src/vendor/wilicm0bsp/drivers/fwcm0/cmake/pi-toolchain.cmake \
  -DCMAKE_C_COMPILER=aarch64-linux-gnu-gcc-14 \
  -DCMAKE_CXX_COMPILER=aarch64-linux-gnu-g++-14
cmake --build build/cm0 --target WiliPirate --parallel 2
file build/cm0/WiliPirate
aarch64-linux-gnu-readelf -h -l -d -V build/cm0/WiliPirate
qemu-aarch64 -L /usr/aarch64-linux-gnu /usr/aarch64-linux-gnu/lib/ld-linux-aarch64.so.1 --list build/cm0/WiliPirate
python3 -B native/cm0/test_bridge.py /src/build/cm0/WiliPirate
