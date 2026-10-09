"""Source/artifact boundaries for the separate M2 DISPLAY application."""
from pathlib import Path
import re
import unittest

ROOT = Path(__file__).resolve().parents[1]


class DisplayPreparationTests(unittest.TestCase):
    def test_ui_has_only_standard_lifecycle_and_display_input_calls(self):
        main = (ROOT / "native/display/main.c").read_text()
        calls = set(re.findall(r"\b(ow_\w+|ioexp_\w+|picpwr_\w+|board_\w+)\s*\(", main))
        self.assertEqual(calls, {"board_init", "board_backlight_set", "picpwr_release_unused"})
        self.assertIn("fw2_app_recovery_init();", main)
        self.assertIn("fw2_app_recovery_task();", main)
        self.assertIn("fw2_app_recovery_sleep_ms(10);", main)
        for file in ("main.c", "ui.c", "navigation.h"):
            source = (ROOT / "native/display" / file).read_text()
            self.assertNotRegex(source, r"\b(?:ow_|gpio_|i2c_|spi_|uart_|can_|cc1101_|ir_tx_|pdm_)\w*\s*\(")
        ui = (ROOT / "native/display/ui.c").read_text()
        self.assertNotIn("HiZ", ui)
        self.assertNotIn('"SAFE', ui)

    def test_supported_ram_target_and_no_onewili_link(self):
        cmake = (ROOT / "native/display/CMakeLists.txt").read_text()
        self.assertIn("fw2_display_app(WiliPirate", cmake)
        self.assertIn("POWER_ZONES DISPLAY)", cmake)
        self.assertIn("VERSION 001", cmake)
        self.assertNotIn("onewili", cmake)
        root = (ROOT / "CMakeLists.txt").read_text()
        self.assertIn("add_subdirectory(wilibsp/bsp)", root)
        self.assertNotIn("add_subdirectory(wilibsp)", root)
        self.assertIn("option(FW2_AGENTIO", root)
