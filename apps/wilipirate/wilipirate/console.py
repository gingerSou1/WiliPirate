"""Terminal/log UI. No display, transport or hardware package imports."""
import argparse
import sys

from . import VERSION
from .application import Application
from .model import MAX_COMMAND_LENGTH, Reply, SAFETY


def console(application, stdin, stdout) -> int:
    try:
        while not application.view.closed:
            print(application.view.prompt, end="", file=stdout, flush=True)
            line = stdin.readline(MAX_COMMAND_LENGTH + 2)
            if not line:
                print("\nWiliPirate closed (EOF).", file=stdout, flush=True)
                return 0
            if len(line) > MAX_COMMAND_LENGTH and not line.endswith("\n"):
                while line and not line.endswith("\n"):
                    line = stdin.readline(MAX_COMMAND_LENGTH + 2)
                reply = Reply("Command too long (maximum 256 characters).", False)
            else:
                reply = application.submit(line.removesuffix("\n").removesuffix("\r"))
            if reply.text:
                print(reply.text, file=stdout, flush=True)
            if reply.exit_requested:
                return 0
        return 0
    except KeyboardInterrupt:
        print("\nWiliPirate interrupted; no hardware operations performed.", file=stdout, flush=True)
        return 130
    finally:
        application.close()


def main(argv=None, *, stdin=None, stdout=None) -> int:
    parser = argparse.ArgumentParser(description="WiliPirate M1B: logical modes, STUB backends only",
                                     allow_abbrev=False)
    group = parser.add_mutually_exclusive_group()
    group.add_argument("--console", action="store_true", help="read commands from a terminal")
    group.add_argument("--command", action="append", metavar="TEXT", help="run a command; repeatable")
    args = parser.parse_args(argv)
    stdout = sys.stdout if stdout is None else stdout
    application = Application()
    print(f"WiliPirate {VERSION} | Backend: STUB", file=stdout, flush=True)
    print(SAFETY, file=stdout, flush=True)
    try:
        if args.console:
            return console(application, sys.stdin if stdin is None else stdin, stdout)
        # The normal Linux Apps launcher disconnects stdin. Never read it by default.
        for command in args.command if args.command is not None else ("help", "info", "mode"):
            reply = application.submit(command)
            if reply.text:
                print(reply.text, file=stdout, flush=True)
            if not reply.success:
                return 2
            if reply.exit_requested:
                break
        if args.command is None:
            print("For interactive use from a terminal: ./run.sh --console", file=stdout, flush=True)
        return 0
    finally:
        application.close()
