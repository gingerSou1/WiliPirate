"""Hardware-free behavior and startup boundary regression tests."""
import ast
import importlib.util
import io
from pathlib import Path
import socket
import subprocess
import sys
import unittest
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[1]
SPEC = importlib.util.spec_from_file_location("wilipirate_app", ROOT / "apps/wilipirate/app.py")
app = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = app
SPEC.loader.exec_module(app)


class UnreadableInput:
    def readline(self, *args):
        raise AssertionError("Default Linux Apps launch must not read stdin")


class AppTests(unittest.TestCase):
    def test_menu_launch_without_stdin_or_hardware(self):
        output = io.StringIO()
        with patch("builtins.open", side_effect=AssertionError("file access")), \
             patch("os.open", side_effect=AssertionError("device access")), \
             patch.object(subprocess, "Popen", side_effect=AssertionError("process access")), \
             patch.object(socket, "socket", side_effect=AssertionError("network access")):
            status = app.main([], stdin=UnreadableInput(), stdout=output)
        self.assertEqual(status, 0)
        text = output.getvalue()
        for expected in ("WiliPirate", "HiZ", "help", "info", "mode", "unknown", "unavailable"):
            self.assertIn(expected, text)

    def test_runtime_imports_are_standard_library_only(self):
        tree = ast.parse((ROOT / "apps/wilipirate/app.py").read_text(encoding="utf-8"))
        imports = set()
        for node in ast.walk(tree):
            if isinstance(node, ast.Import):
                imports.update(alias.name for alias in node.names)
            elif isinstance(node, ast.ImportFrom):
                imports.add(node.module)
        self.assertLessEqual(imports, {"__future__", "argparse", "dataclasses", "sys"})

    def test_help_info_and_hiz(self):
        for command in ("help", "info", "mode", "mode hiz", " MODE HiZ ", "\tHeLp\t"):
            with self.subTest(command=command):
                self.assertTrue(app.execute(command).success)
        self.assertIn("electrical HiZ is not enforced", app.execute("info").text)

    def test_modes_are_listed_and_all_hardware_modes_refused(self):
        listing = app.execute("mode").text
        for mode in ("I2C", "SPI", "UART", "GPIO"):
            with self.subTest(mode=mode):
                self.assertIn(mode, listing)
                reply = app.execute("mode " + mode)
                self.assertFalse(reply.success)
                self.assertIn("remaining in HiZ", reply.text)
        self.assertIn("current; no I/O", app.execute("mode").text)

    def test_invalid_commands_never_become_operations(self):
        for command in ("scan", "power on", "gpio high 25", "[0x50 r]", "mode 2",
                        "help extra", "info extra", "exit extra", "mode i2c extra",
                        "mode hiz; scan", "eval print(1)", "__import__('os')"):
            with self.subTest(command=command):
                self.assertFalse(app.execute(command).success)
                self.assertIn("no hardware operations", app.execute("info").text)

    def test_control_characters_and_oversize_rejected_without_echo(self):
        for command in ("info\x1b[2J", "mode\x00hiz", "help\nexit", "info\r", "help\x7f", "x" * 257):
            with self.subTest(command=repr(command)):
                result = app.execute(command)
                self.assertFalse(result.success)
                self.assertNotIn("\x1b", result.text)
        self.assertTrue(app.execute(" " * 252 + "help").success)

    def test_blank_input_is_noop(self):
        self.assertEqual(app.execute(" \t ").text, "")
        self.assertTrue(app.execute("").success)

    def test_batch_stops_on_failure(self):
        output = io.StringIO()
        status = app.main(["--command", "mode i2c", "--command", "help"], stdout=output)
        self.assertEqual(status, 2)
        self.assertNotIn("Show commands", output.getvalue())

    def test_batch_exit_stops_sequence(self):
        output = io.StringIO()
        self.assertEqual(app.main(["--command", "exit", "--command", "bad"], stdout=output), 0)
        self.assertNotIn("Unknown command", output.getvalue())

    def test_console_recovers_after_error_and_exits(self):
        output = io.StringIO()
        status = app.main(["--console"], stdin=io.StringIO("mode i2c\nhelp\nexit\n"), stdout=output)
        self.assertEqual(status, 0)
        self.assertEqual(output.getvalue().count("HiZ> "), 3)
        self.assertIn("Show commands", output.getvalue())

    def test_console_eof_and_crlf(self):
        output = io.StringIO()
        self.assertEqual(app.main(["--console"], stdin=io.StringIO("info\r\n"), stdout=output), 0)
        self.assertIn("closed (EOF)", output.getvalue())
        self.assertNotIn("Control characters", output.getvalue())

    def test_console_discards_whole_overlong_line(self):
        output = io.StringIO()
        input_text = "x" * 260 + "exit\nhelp\nexit\n"
        self.assertEqual(app.main(["--console"], stdin=io.StringIO(input_text), stdout=output), 0)
        self.assertIn("Command too long", output.getvalue())
        self.assertIn("Show commands", output.getvalue())

    def test_console_interrupt(self):
        class InterruptedInput:
            def readline(self, *args):
                raise KeyboardInterrupt
        output = io.StringIO()
        self.assertEqual(app.main(["--console"], stdin=InterruptedInput(), stdout=output), 130)
        self.assertIn("no hardware operations performed", output.getvalue())

    def test_entry_point_runs_with_closed_stdin(self):
        result = subprocess.run([sys.executable, "-B", str(ROOT / "apps/wilipirate/app.py")],
                                stdin=subprocess.DEVNULL, capture_output=True, text=True, timeout=5)
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn("WiliPirate", result.stdout)


if __name__ == "__main__":
    unittest.main()
