"""WiliPirate M1: a CM0 launcher-compatible console with no hardware backend."""
from __future__ import annotations

import argparse
from dataclasses import dataclass
import sys

VERSION = "0.1.0-m1"
PROMPT = "HiZ> "
MAX_COMMAND_LENGTH = 256
SAFETY = (
    "HiZ: no hardware operations. External pin/power state is unknown; "
    "electrical HiZ is not enforced."
)
MODES = ("HiZ", "I2C", "SPI", "UART", "GPIO")
HELP = (
    "help          Show commands\n"
    "info          Show application and safety status\n"
    "mode          List modes\n"
    "mode hiz      Stay in the no-I/O state\n"
    "exit          Close the console\n"
    "I2C, SPI, UART and GPIO are unavailable in Milestone 1."
)


@dataclass(frozen=True)
class Reply:
    text: str
    success: bool = True
    exit_requested: bool = False


def execute(command: str) -> Reply:
    """Interpret one command. No transport is imported, constructed or called."""
    if len(command) > MAX_COMMAND_LENGTH:
        return Reply("Command too long (maximum 256 characters).", False)
    if any((ord(char) < 32 and char != "\t") or ord(char) == 127 for char in command):
        return Reply("Control characters are not allowed in commands.", False)
    words = command.lower().split()
    if not words:
        return Reply("")
    if words == ["help"]:
        return Reply(HELP)
    if words == ["info"]:
        return Reply(f"WiliPirate {VERSION} | FREE-WILi 2 CM0 application\n{SAFETY}")
    if words == ["exit"]:
        return Reply("WiliPirate closed; no hardware operations performed.", exit_requested=True)
    if words == ["mode"]:
        return Reply("\n".join(
            f"{index}. {name} - " + ("current; no I/O" if index == 1 else "unavailable in M1")
            for index, name in enumerate(MODES, 1)
        ))
    if len(words) == 2 and words[0] == "mode":
        if words[1] == "hiz":
            return Reply(SAFETY)
        if words[1] in {name.lower() for name in MODES[1:]}:
            return Reply("Mode unavailable in M1; remaining in HiZ. No hardware operations.", False)
        return Reply("Unknown mode. Use mode to list names; remaining in HiZ.", False)
    if words[0] in {"help", "info", "mode", "exit"}:
        return Reply("Invalid arguments. Type help for usage; remaining in HiZ.", False)
    return Reply("Unknown command. Type help; remaining in HiZ.", False)


def console(stdin, stdout) -> int:
    """Explicit terminal session. EOF and Ctrl-C leave the no-I/O state intact."""
    while True:
        print(PROMPT, end="", file=stdout, flush=True)
        line = stdin.readline(MAX_COMMAND_LENGTH + 2)
        if not line:
            print("\nWiliPirate closed (EOF).", file=stdout, flush=True)
            return 0
        if len(line) > MAX_COMMAND_LENGTH and not line.endswith("\n"):
            while line and not line.endswith("\n"):
                line = stdin.readline(MAX_COMMAND_LENGTH + 2)
            reply = Reply("Command too long (maximum 256 characters).", False)
        else:
            reply = execute(line.removesuffix("\n").removesuffix("\r"))
        if reply.text:
            print(reply.text, file=stdout, flush=True)
        if reply.exit_requested:
            return 0


def main(argv=None, *, stdin=None, stdout=None) -> int:
    parser = argparse.ArgumentParser(description="WiliPirate M1: no hardware operations")
    group = parser.add_mutually_exclusive_group()
    group.add_argument("--console", action="store_true", help="read commands from a terminal")
    group.add_argument("--command", action="append", metavar="TEXT", help="run a command; repeatable")
    args = parser.parse_args(argv)
    stdout = sys.stdout if stdout is None else stdout
    print(f"WiliPirate {VERSION}", file=stdout, flush=True)
    print(SAFETY, file=stdout, flush=True)
    if args.console:
        try:
            return console(sys.stdin if stdin is None else stdin, stdout)
        except KeyboardInterrupt:
            print("\nWiliPirate interrupted; no hardware operations performed.", file=stdout, flush=True)
            return 130
    # Linux Apps disconnects stdin. Default launch never tries to read it.
    for command in args.command if args.command is not None else ("help", "info", "mode"):
        reply = execute(command)
        if reply.text:
            print(reply.text, file=stdout, flush=True)
        if not reply.success:
            return 2
        if reply.exit_requested:
            break
    if args.command is None:
        print("For interactive use from a terminal: ./run.sh --console", file=stdout, flush=True)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
