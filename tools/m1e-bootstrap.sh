#!/bin/sh
set -eu
apt-get install -y --no-install-recommends debian-archive-keyring
test ! -e /opt/wilipirate-trixie
debootstrap --force-check-gpg --keyring=/usr/share/keyrings/debian-archive-keyring.gpg --variant=minbase --include=ca-certificates trixie /opt/wilipirate-trixie https://deb.debian.org/debian
cat /opt/wilipirate-trixie/etc/os-release
grep -q '^VERSION_CODENAME=trixie$' /opt/wilipirate-trixie/etc/os-release
touch /opt/wilipirate-trixie/M1E_ISOLATED_ROOT
chroot /opt/wilipirate-trixie apt-get update
chroot /opt/wilipirate-trixie apt-get install -y --no-install-recommends cmake make gcc-14-aarch64-linux-gnu g++-14-aarch64-linux-gnu libc6-dev-arm64-cross binutils-aarch64-linux-gnu file qemu-user python3 ca-certificates
