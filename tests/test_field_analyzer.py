"""Field UI boundary checks; operations are intentionally absent."""
from pathlib import Path
import re
import unittest

ROOT = Path(__file__).resolve().parents[1]
FIELD = ROOT / "native/field_analyzer"


class FieldAnalyzerTests(unittest.TestCase):
    def test_v002_rollback_is_byte_identical(self):
        import hashlib
        import json
        folder = ROOT / "releases/field-v002"
        manifest = json.loads((folder / "manifest.json").read_text())
        data = (folder / "WiliPirate.uf2").read_bytes()
        self.assertEqual(len(data), 61440)
        self.assertEqual(hashlib.sha256(data).hexdigest(), "53ca890ce882f8cc088657abd8f7cc4482292e150e5544a60044604e9a3c9a2f")
        self.assertEqual(manifest["sha256"], hashlib.sha256(data).hexdigest())

    def test_can_and_glitching_have_no_live_calls(self):
        for file in FIELD.glob("*.c"):
            self.assertNotRegex(file.read_text(), r"\b(?:ow_\w+|picpwr_\w+|ioexp_\w+|gpio_\w+|can_\w+)\s*\(")
        source = (FIELD / "can_model.c").read_text()
        self.assertIn("bool fa_can_live_available(void){return false;}", source)
        self.assertIn("SIMULATION", (FIELD / "ui.c").read_text())

    def test_native_adapter_has_only_display_input_recovery_calls(self):
        source = (FIELD / "main.c").read_text(encoding="utf-8")
        self.assertNotRegex(source, r"\b(?:ow_|picpwr_|ioexp_|gpio_|i2c_|spi_|can_|ow_sd_|cc1101_)\w*\s*\(")
        self.assertIn("fw2_app_recovery_task();", source)
        self.assertIn("fw2_app_recovery_sleep_ms(10);", source)
        self.assertIn("FIELD_STANDARD_STARTUP_APPROVED", source)
        self.assertNotIn("onewili", (FIELD / "CMakeLists.txt").read_text())

    def test_model_and_renderer_are_pure_and_all_features_unavailable(self):
        for file in ("model.c", "ui.c", "can_model.c"):
            source = (FIELD / file).read_text(encoding="utf-8")
            self.assertNotRegex(source, r"\b(?:fopen|malloc|system|ow_\w+|picpwr_\w+)\s*\(")
        source = (FIELD / "ui.c").read_text()
        self.assertIn("Not implemented", source)
        self.assertIn("Pinout or electrical compatibility not yet verified.", source)
        self.assertIn("No instrument operations.", source)

    def test_active_build_cannot_build_retired_apps(self):
        source = (ROOT / "CMakeLists.txt").read_text()
        self.assertNotRegex(source, r"add_subdirectory\(native/(display|i2c_diag)\)")
        self.assertIn("FIELD_BUILD_DEVICE", source)
        self.assertIn("if(NOT FIELD_STANDARD_STARTUP_APPROVED)", source)

    def test_device_candidate_replaces_existing_wilipirate_identity(self):
        source = (FIELD / "CMakeLists.txt").read_text()
        self.assertIn("add_executable(WiliPirate main.c)", source)
        self.assertIn("fw2_display_app(WiliPirate", source)
        self.assertIn("NAME WiliPirate VERSION 003", source)
        self.assertNotIn("WiliPirateFieldAnalyzer", source)

    def test_archived_snapshot_hashes_are_preserved(self):
        import hashlib
        import json
        manifest = json.loads((ROOT / "archive/manifest.json").read_text())
        for entry in manifest["entries"]:
            self.assertEqual(hashlib.sha256((ROOT / entry["archive_path"]).read_bytes()).hexdigest(), entry["sha256"])


if __name__ == "__main__":
    unittest.main()
