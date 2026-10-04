"""Exercise all pure UI actions under a process/file/network audit guard."""
import dataclasses
import enum
from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))
sys.path.insert(0, str(ROOT / "apps/wilipirate"))
from ui.controller import Controller, COMMANDS, CANCEL
from ui.panel import frame
from wilipirate.model import Mode


def audit(event, args):
    if event == "open" or event.startswith(("socket.", "subprocess.", "ctypes.", "winreg.")) or event in {
        "os.system", "os.posix_spawn", "os.exec", "os.spawn", "os.startfile", "os.putenv",
        "os.mkdir", "os.remove", "os.rename", "os.chmod", "os.truncate",
    }:
        raise AssertionError("Unexpected UI access: " + event)


sys.addaudithook(audit)
for mode in Mode:
    for bit in range(14):
        ui = Controller()
        ui.submit("mode " + mode.value)
        ui.buttons(1 << bit)
        frame(ui)
        ui.close()
    for command in COMMANDS:
        ui = Controller()
        ui.submit("mode " + mode.value)
        ui.submit(command)
        frame(ui)
        ui.buttons(CANCEL)
        assert ui.view.closed
print("PASS: all pure UI actions and rendering performed no file/process/network access")
