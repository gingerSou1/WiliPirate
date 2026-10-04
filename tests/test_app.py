"""Logical application and console contracts, without hardware."""
import io
import os
from pathlib import Path
import subprocess
import sys
import unittest
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "apps/wilipirate"))
from wilipirate.application import Application
from wilipirate.backends import StubBackend, stub_backends
from wilipirate.console import console, main
from wilipirate.model import Mode
from wilipirate.parser import ParseError, parse
from wilipirate.state import Session


class AppTests(unittest.TestCase):
    def test_starts_in_hiz_with_stub(self):
        view = Application().view
        self.assertEqual(view.mode, Mode.HIZ)
        self.assertEqual(view.prompt, "HiZ> ")
        self.assertEqual(view.backend, "STUB")
        self.assertFalse(view.available)

    def test_all_25_transitions_are_logical_only(self):
        with patch.object(StubBackend, "request", side_effect=AssertionError("backend called")):
            for source in Mode:
                for destination in Mode:
                    with self.subTest(source=source, destination=destination):
                        app = Application()
                        app.submit("mode " + source.value)
                        reply = app.submit("mode " + destination.value)
                        self.assertTrue(reply.success)
                        self.assertEqual(reply.text, "Mode: " + destination.value)
                        self.assertEqual(app.view.prompt, destination.value + "> ")
                        self.assertFalse(app.view.available)

    def test_sessions_do_not_share_mode(self):
        first, second = Application(), Application()
        first.submit("mode spi")
        self.assertEqual(second.view.mode, Mode.HIZ)

    def test_invalid_mode_preserves_current_mode(self):
        app = Application()
        app.submit("mode i2c")
        for value in ("invalid", "2", "'uart'"):
            self.assertFalse(app.submit("mode " + value).success)
            self.assertEqual(app.view.mode, Mode.I2C)

    def test_mode_list_marks_current(self):
        app = Application()
        app.submit("mode gpio")
        text = app.submit("mode").text
        for mode in Mode:
            self.assertIn(mode.value, text)
        self.assertEqual(text.count("(current)"), 1)
        self.assertIn("GPIO (current)", text)

    def test_help_info_in_every_mode(self):
        app = Application()
        with patch.object(StubBackend, "request", side_effect=AssertionError("backend called")):
            for mode in Mode:
                app.submit("mode " + mode.value)
                self.assertIn("mode hiz|uart|i2c|spi|gpio", app.submit("help").text)
                info = app.submit("info").text
                for expected in ("Backend: STUB", "Mode: " + mode.value,
                                 "External pin/power state is unknown", "electrical HiZ is not enforced"):
                    self.assertIn(expected, info)
        app.submit("mode i2c")
        self.assertIn("scan", app.submit("help").text)

    def test_explicit_stub_per_mode_never_returns_fake_data(self):
        backends = stub_backends()
        self.assertEqual(set(backends), set(Mode))
        self.assertEqual(len({type(item) for item in backends.values()}), 5)
        for mode, backend in backends.items():
            for operation in ("scan", "read", "write", "transfer"):
                result = backend.request(operation, ("DEAD", "BEEF"))
                self.assertFalse(result.available)
                self.assertEqual(result.message, f"{mode.value} hardware backend not enabled.")

    def test_stub_commands_fail_without_changing_mode(self):
        operations = {"uart": ("read", "write 41"), "i2c": ("scan", "read", "write 50 AA"),
                      "spi": ("transfer 9F 00" ,), "gpio": ("read", "write 25 1")}
        app = Application()
        for mode, commands in operations.items():
            app.submit("mode " + mode)
            for command in commands:
                reply = app.submit(command)
                self.assertFalse(reply.success)
                self.assertEqual(reply.text, f"{mode.upper()} hardware backend not enabled.")
                self.assertEqual(app.view.mode.value.lower(), mode)

    def test_unsupported_operations_refused(self):
        app = Application()
        for command in ("scan", "read", "transfer AA"):
            self.assertFalse(app.submit(command).success)
        app.submit("mode uart")
        self.assertIn("Command unavailable in UART", app.submit("scan").text)

    def test_missing_backend_fails_closed(self):
        session = Session()
        session.handle(parse("mode i2c"))
        del session._backends[Mode.I2C]  # Fault injection, not a runtime plugin mechanism.
        reply = session.handle(parse("scan"))
        self.assertFalse(reply.success)
        self.assertIn("no fallback attempted", reply.text)
        self.assertEqual(session.view.backend, "UNAVAILABLE")

    def test_exit_closes_session_and_rejects_later_commands(self):
        app = Application()
        app.submit("mode uart")
        self.assertTrue(app.submit("exit").exit_requested)
        self.assertTrue(app.view.closed)
        self.assertEqual(app.view.mode, Mode.HIZ)
        self.assertFalse(app.submit("mode spi").success)


class ParserTests(unittest.TestCase):
    def test_case_whitespace_and_opaque_payload(self):
        self.assertEqual(parse(" \tMoDe UART ").name, "mode")
        self.assertEqual(parse("WRITE AaBb /dev/not-opened").arguments, ("AaBb", "/dev/not-opened"))
        self.assertEqual(parse("  \t").name, "")
        self.assertTrue(Application().submit("MODE UART").success)

    def test_bad_syntax_and_arity(self):
        for line in ("bad", "help extra", "info extra", "exit extra", "mode uart extra",
                     "scan extra", "read extra", "write", "transfer", "mode hiz; scan",
                     "mode uart | info", "`info`", "[0x50 r]", "eval print(1)"):
            with self.subTest(line=line), self.assertRaises(ParseError):
                parse(line)

    def test_controls_and_unicode_separators(self):
        for line in ("info\x1b[2J", "mode\x00hiz", "help\nexit", "info\r", "help\x7f",
                     "info\u0085", "mode\u2028uart"):
            with self.subTest(line=repr(line)), self.assertRaises(ParseError):
                parse(line)

    def test_length_limit(self):
        self.assertEqual(parse(" " * 252 + "help").name, "help")
        with self.assertRaises(ParseError):
            parse("x" * 257)


class ConsoleTests(unittest.TestCase):
    def test_default_launch_never_reads_stdin(self):
        class NoInput:
            def readline(self, *args):
                raise AssertionError("stdin read")
        output = io.StringIO()
        self.assertEqual(main([], stdin=NoInput(), stdout=output), 0)
        self.assertIn("Mode: HiZ", output.getvalue())
        self.assertIn("Backend: STUB", output.getvalue())

    def test_requested_transcript_and_dynamic_prompts(self):
        output = io.StringIO()
        lines = "mode i2c\nscan\nmode uart\ninfo\nmode hiz\nexit\n"
        self.assertEqual(main(["--console"], stdin=io.StringIO(lines), stdout=output), 0)
        text = output.getvalue()
        for expected in ("HiZ> Mode: I2C", "I2C> I2C hardware backend not enabled.",
                         "I2C> Mode: UART", "UART> WiliPirate", "Backend: STUB", "UART> Mode: HiZ"):
            self.assertIn(expected, text)

    def test_console_errors_do_not_reset_mode(self):
        output = io.StringIO()
        lines = "mode spi\nmode invalid\ninfo extra\ninfo\nexit\n"
        main(["--console"], stdin=io.StringIO(lines), stdout=output)
        self.assertEqual(output.getvalue().count("SPI> "), 4)

    def test_eof_and_crlf_close_session(self):
        app, output = Application(), io.StringIO()
        self.assertEqual(console(app, io.StringIO("mode uart\r\ninfo\r\n"), output), 0)
        self.assertTrue(app.view.closed)
        self.assertIn("Mode: UART", output.getvalue())
        self.assertIn("closed (EOF)", output.getvalue())

    def test_oversize_line_is_discarded_and_console_recovers(self):
        output = io.StringIO()
        lines = "x" * 260 + "exit\nhelp\nexit\n"
        main(["--console"], stdin=io.StringIO(lines), stdout=output)
        self.assertIn("Command too long", output.getvalue())
        self.assertIn("Show commands", output.getvalue())

    def test_keyboard_interrupt_closes_session(self):
        class Interrupted:
            def readline(self, *args):
                raise KeyboardInterrupt
        app = Application()
        app.submit("mode gpio")
        self.assertEqual(console(app, Interrupted(), io.StringIO()), 130)
        self.assertTrue(app.view.closed)
        self.assertEqual(app.view.mode, Mode.HIZ)

    def test_batch_preserves_mode_and_stops_on_stub_failure(self):
        output = io.StringIO()
        status = main(["--command", "mode i2c", "--command", "info", "--command", "scan",
                       "--command", "mode spi"], stdout=output)
        self.assertEqual(status, 2)
        self.assertIn("Mode: I2C", output.getvalue())
        self.assertNotIn("Mode: SPI", output.getvalue())

    def test_batch_exit_stops_sequence(self):
        output = io.StringIO()
        self.assertEqual(main(["--command", "exit", "--command", "bad"], stdout=output), 0)
        self.assertNotIn("Unknown command", output.getvalue())

    def test_no_hardware_or_backend_selection_flags(self):
        for args in (["--backend", "real"], ["--hardware"], ["--device", "COM1"],
                     ["--console", "--command", "info"]):
            with patch("sys.stderr", io.StringIO()), self.assertRaises(SystemExit) as error:
                main(args, stdout=io.StringIO())
            self.assertEqual(error.exception.code, 2)

    def test_entry_point_with_closed_stdin(self):
        result = subprocess.run([sys.executable, "-B", str(ROOT / "apps/wilipirate/app.py")],
                                stdin=subprocess.DEVNULL, capture_output=True, text=True, timeout=5)
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn("Mode: HiZ", result.stdout)


if __name__ == "__main__":
    unittest.main()
