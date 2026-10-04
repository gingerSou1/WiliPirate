"""Host panel contract and command-boundary regressions; no device operations."""
import ast
from pathlib import Path
import subprocess
import sys
import unittest
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))
sys.path.insert(0, str(ROOT / "apps/wilipirate"))
from wilipirate.model import Mode
from wilipirate.backends import StubBackend
from ui.controller import Controller, COMMANDS, HISTORY_LIMIT, VISIBLE_LINES
from ui.controller import GRAY, YELLOW, GREEN, BLUE, RED, UP, DOWN, LEFT, RIGHT, CENTER, OK, CANCEL, HOME, PAGE
from ui.panel import frame


class ControllerTests(unittest.TestCase):
    def test_default_hiz_and_help(self):
        ui = Controller()
        self.assertEqual(ui.view.mode, Mode.HIZ)
        self.assertEqual(ui.view.backend, "STUB")
        self.assertFalse(ui.view.available)
        self.assertIn("HiZ> help", ui.history)

    def test_every_transition_uses_application_without_backend_request(self):
        ui = Controller()
        with patch.object(StubBackend, "request", side_effect=AssertionError("hardware request")):
            for before in Mode:
                for after in Mode:
                    ui.submit("mode " + before.value)
                    ui.submit("mode " + after.value)
                    self.assertEqual(ui.view.mode, after)
        self.assertFalse(ui.submit("mode invalid").success)
        self.assertEqual(ui.view.mode, Mode.GPIO)

    def test_menu_commands_delegate_to_core(self):
        ui = Controller()
        with patch.object(ui.application, "submit", wraps=ui.application.submit) as submit:
            for bit in (GRAY, YELLOW, GREEN, HOME):
                ui.buttons(bit)
            self.assertEqual([call.args[0] for call in submit.call_args_list],
                             ["help", "info", "mode uart", "mode hiz"])

    def test_mode_cycle_all_five(self):
        ui = Controller()
        for mode in (Mode.UART, Mode.I2C, Mode.SPI, Mode.GPIO, Mode.HIZ):
            ui.buttons(GREEN)
            self.assertEqual(ui.view.mode, mode)

    def test_navigation_and_run_buttons(self):
        for run in (BLUE, CENTER, OK):
            ui = Controller()
            ui.buttons(DOWN)
            self.assertEqual(ui.selected, "info")
            ui.buttons(run)
            self.assertIn("HiZ> info", ui.history)
            ui.buttons(UP)
            self.assertEqual(ui.selected, "help")
            ui.buttons(UP)
            self.assertEqual(ui.selected, COMMANDS[-1])

    def test_exit_has_priority_and_later_events_are_ignored(self):
        for exit_bit in (RED, CANCEL):
            ui = Controller()
            ui.buttons(exit_bit | GREEN | BLUE)
            self.assertTrue(ui.view.closed)
            before = ui.history
            ui.buttons(GREEN)
            self.assertIsNone(ui.submit("info"))
            ui.close()
            self.assertEqual(ui.history, before)

    def test_ambiguous_and_invalid_masks_do_nothing(self):
        ui = Controller()
        before = ui.history
        for mask in (0, -1, 1 << 14, True, "help", GRAY | GREEN):
            ui.buttons(mask)
        self.assertEqual(ui.history, before)
        self.assertEqual(ui.view.mode, Mode.HIZ)

    def test_all_mode_requests_reach_only_stubs(self):
        ui = Controller()
        for mode, command in (("uart", "write AA"), ("i2c", "scan"),
                              ("spi", "transfer FF"), ("gpio", "read")):
            ui.submit("mode " + mode)
            reply = ui.submit(command)
            self.assertFalse(reply.success)
            self.assertEqual(reply.text, f"{ui.view.mode.value} hardware backend not enabled.")
        self.assertFalse(ui.submit("power on").success)

    def test_bounded_history_and_scroll(self):
        ui = Controller()
        for _ in range(200):
            ui.submit("info")
        self.assertEqual(len(ui.history), HISTORY_LIMIT)
        self.assertTrue(all(len(line) <= 60 for line in ui.history))
        ui.buttons(LEFT)
        self.assertEqual(ui.offset, VISIBLE_LINES)
        ui.buttons(RIGHT)
        self.assertEqual(ui.offset, 0)
        ui.scroll(9999)
        self.assertEqual(ui.visible, ui.history[:VISIBLE_LINES])
        ui.buttons(PAGE)
        self.assertEqual(ui.offset, HISTORY_LIMIT - 2 * VISIBLE_LINES)
        ui.submit("info")
        self.assertEqual(ui.offset, 0)


class PanelTests(unittest.TestCase):
    def test_layout_dimensions_ascii_and_caption_limit(self):
        ui = Controller()
        ui._append("#`" * 100 + "\u2603")
        snapshot = frame(ui)
        self.assertEqual(len(snapshot.cells), 68)
        self.assertEqual([c.index for c in snapshot.cells], list(range(68)))
        self.assertEqual(snapshot.menu, ("Help", "Info", "Mode", "Run", "Exit"))
        for cell in snapshot.cells:
            self.assertEqual(len(cell.text), 15)
            self.assertLessEqual(len(cell.caption.encode("ascii")), 30)
            self.assertGreaterEqual(cell.x, 30)
            self.assertLessEqual(cell.x + 15 * 7, 450)
            self.assertLessEqual(cell.y + 17, 291)
        text = "".join(c.text for c in snapshot.cells)
        self.assertIn("STUB ONLY", text)
        self.assertIn("HW OFF", text)
        self.assertIn("?", text)

    def test_render_is_pure_and_closed_frame(self):
        ui = Controller()
        before = ui.history
        with patch.object(ui.application, "submit", side_effect=AssertionError("renderer dispatched")):
            self.assertEqual(frame(ui), frame(ui))
        self.assertEqual(before, ui.history)
        ui.close()
        self.assertTrue(frame(ui).closed)
        self.assertIn("Session closed", "".join(c.text for c in frame(ui).cells))

    def test_tk_event_routes_through_controller_and_exit_closes_window(self):
        from ui.tk_preview import Preview
        from unittest.mock import Mock
        preview = Preview.__new__(Preview)
        preview.controller = Controller()
        preview.finished = False
        preview.root = Mock()
        preview.canvas = Mock()
        preview.press(GREEN)
        self.assertEqual(preview.controller.view.mode, Mode.UART)
        preview.press(CANCEL)
        self.assertTrue(preview.controller.view.closed)
        preview.root.destroy.assert_called_once()
        preview.finish()
        preview.root.destroy.assert_called_once()

    def test_ui_import_surface_and_no_transport_calls(self):
        allowed = {"dataclasses", "tkinter", "wilipirate.application", "wilipirate.model"}
        forbidden = {"open", "eval", "exec", "compile", "__import__", "getattr", "setattr",
                     "connect", "connect_cm0", "system", "Popen", "read_buttons"}
        for path in (ROOT / "ui").glob("*.py"):
            source = path.read_text(encoding="utf-8")
            for node in ast.walk(ast.parse(source)):
                if isinstance(node, ast.Import):
                    self.assertTrue(all(item.name in allowed for item in node.names))
                if isinstance(node, ast.ImportFrom):
                    self.assertIn(node.module, {"controller", "panel"} if node.level else allowed)
                if isinstance(node, ast.Call):
                    name = node.func.id if isinstance(node.func, ast.Name) else (
                        node.func.attr if isinstance(node.func, ast.Attribute) else "")
                    self.assertNotIn(name, forbidden)
            for marker in ("fwcm0", "onewili_cm0", "spidev", "serial", "socket"):
                self.assertNotIn(marker, source.lower())

    def test_ui_runtime_audit(self):
        result = subprocess.run([sys.executable, "-B", str(ROOT / "tests/ui_no_hardware_runner.py")],
                                capture_output=True, text=True, timeout=10)
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn("PASS", result.stdout)


if __name__ == "__main__":
    unittest.main()
