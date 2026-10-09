"""Static boundaries for the separate diagnostic; no device operations."""
from pathlib import Path
import re
import unittest

ROOT = Path(__file__).resolve().parents[1]
DIAG = ROOT / "native/i2c_diag"


class I2CDiagnosticPreparationTests(unittest.TestCase):
    def test_single_manual_stock_command_and_no_configuration(self):
        main = (DIAG / "main.c").read_text(encoding="utf-8")
        self.assertEqual(len(re.findall(r"\bow_raw_send\s*\(", main)), 1)
        self.assertIn('ow_raw_send(&device, "i\\\\i\\\\p")', main)
        self.assertIn("if (!diag_claim(&capture)) return;", main)
        self.assertIn("event.btn == UARTKBD_BTN_GREEN && green_released", main)
        calls = set(re.findall(r"\b(ow_\w+|ioexp_\w+|picpwr_\w+|board_\w+)\s*\(", main))
        self.assertEqual(calls, {"ow_raw_send", "ow_fwgui_get_stats", "board_init", "board_backlight_set"})
        self.assertNotRegex(main, r"\b(?:gpio_|i2c_|spi_|can_|cc1101_|ow_io_)\w*\s*\(")

    def test_bounded_receive_and_recovery_on_error_paths(self):
        main = (DIAG / "main.c").read_text(encoding="utf-8")
        self.assertIn("fw2_app_recovery_open_onewili(&device)", main)
        self.assertIn("device.t.read(device.t.ctx, bytes, sizeof bytes, slice)", main)
        self.assertIn("diag_remaining(&capture, now_ms())", main)
        self.assertIn("add_repeating_timer_ms(-10, send_recovery", main)
        self.assertIn("cancel_repeating_timer(&recovery_timer)", main)
        self.assertIn("fw2_app_recovery_sleep_ms(10)", main)
        self.assertNotIn("ow_raw_next_response(", main)
        self.assertNotIn("ow_poll_text_line(", main)
        self.assertNotRegex(main, r"\b(?:malloc|calloc|realloc|ow_sd_\w+)\s*\(")

    def test_standalone_sram_target_without_m2_subdirectory(self):
        cmake = (DIAG / "CMakeLists.txt").read_text(encoding="utf-8")
        self.assertIn("fw2_display_app(WiliPirateI2CDiag", cmake)
        self.assertIn("POWER_ZONES DISPLAY)", cmake)
        self.assertIn('set(FW2_AGENTIO OFF', cmake)
        self.assertNotIn("native/display", cmake)
        self.assertNotIn("copy_to_ram", cmake)
        self.assertIn("onewili_fwgui", cmake)


if __name__ == "__main__":
    unittest.main()
