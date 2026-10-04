"""Subprocess audit harness: import and exercise the app without device access."""
# Preload standard library dependencies before enabling strict runtime I/O denial.
import abc
import argparse
import dataclasses
import enum
import io
import os
from pathlib import Path
import sys

app_root = Path(sys.argv[1]).resolve()
sys.path.insert(0, str(app_root))
# Prime argparse's lazy stdlib locale/help imports before denying all runtime file reads.
argparse.ArgumentParser().format_help()
importing = True


def audit(event, args):
    if event.startswith(("socket.", "subprocess.", "ctypes.", "winreg.")) or event in (
        "os.system", "os.posix_spawn", "os.exec", "os.spawn", "os.startfile", "os.putenv",
        "os.mkdir", "os.remove", "os.rename", "os.chmod", "os.truncate",
    ):
        raise AssertionError("Forbidden runtime access: " + event)
    if event == "open":
        path, mode, flags = args
        if not importing:
            raise AssertionError("Application attempted file/device access")
        # Import machinery may only READ source or bytecode from the app/stdlib.
        p = Path(path).resolve()
        if not (p.is_relative_to(app_root) or p.is_relative_to(Path(sys.base_prefix).resolve())):
            raise AssertionError("Unexpected import file: " + str(p))
        if p.suffix not in (".py", ".pyc") or flags & (os.O_WRONLY | os.O_RDWR | os.O_CREAT | os.O_TRUNC):
            raise AssertionError("Unexpected import operation")
    if event == "import" and args[0].split(".")[0] in {
        "onewili", "onewili_cm0", "serial", "smbus", "smbus2", "spidev", "gpiod", "RPi",
    }:
        raise AssertionError("Hardware import attempted")


sys.addaudithook(audit)
from wilipirate.application import Application
from wilipirate.console import main
from wilipirate.model import Mode
importing = False

for mode in Mode:
    app = Application()
    for line in ("mode " + mode.value, "help", "info", "mode", "scan", "read", "write AA",
                 "transfer FF", "mode bad", "power on", "exit"):
        app.submit(line)
    assert app.view.closed
for argv, input_text in (([], ""), (["--console"], "mode i2c\nscan\nmode uart\ninfo\nexit\n"),
                         (["--command", "mode spi", "--command", "info"], "")):
    assert main(argv, stdin=io.StringIO(input_text), stdout=io.StringIO()) == 0
print("PASS: runtime import and command paths performed no hardware/file/process/network access")
