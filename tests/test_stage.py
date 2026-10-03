"""Validate self-contained staging without fetching dependencies or using a Wili."""
import importlib.util
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
SPEC = importlib.util.spec_from_file_location("wilipirate_stage", ROOT / "tools/stage.py")
staging = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(staging)


class StageTests(unittest.TestCase):
    def test_staged_app_is_relocatable_and_dependency_free(self):
        with tempfile.TemporaryDirectory(prefix="wilipirate test ") as directory:
            base = Path(directory)
            target = staging.stage(base / "first location")
            self.assertEqual({path.name for path in target.iterdir()},
                             {"app.py", "run.sh", "README.md", "THIRD_PARTY.md"})
            moved = base / "moved app"
            target.rename(moved)
            result = subprocess.run([sys.executable, "-I", "-B", str(moved / "app.py"),
                                     "--command", "info"], cwd=base, stdin=subprocess.DEVNULL,
                                    capture_output=True, text=True, timeout=5)
            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertIn("WiliPirate", result.stdout)
            self.assertIn("External pin/power state is unknown", result.stdout)
            self.assertEqual((moved / "app.py").read_bytes(), (ROOT / "apps/wilipirate/app.py").read_bytes())
            self.assertNotIn(b"\r", (moved / "run.sh").read_bytes())

    def test_existing_directory_is_never_overwritten(self):
        with tempfile.TemporaryDirectory() as directory:
            target = staging.stage(Path(directory))
            sentinel = target / "user-data.txt"
            sentinel.write_text("preserve me", encoding="utf-8")
            before = {p.name: p.read_bytes() for p in target.iterdir()}
            with self.assertRaises(FileExistsError):
                staging.stage(Path(directory))
            self.assertEqual(before, {p.name: p.read_bytes() for p in target.iterdir()})

    def test_existing_file_is_never_overwritten(self):
        with tempfile.TemporaryDirectory() as directory:
            target = Path(directory) / "wilipirate"
            target.write_text("preserve me", encoding="utf-8")
            with self.assertRaises(FileExistsError):
                staging.stage(Path(directory))
            self.assertEqual(target.read_text(encoding="utf-8"), "preserve me")

    def test_shell_launcher_has_portable_line_endings_and_no_checkout_paths(self):
        launcher = (ROOT / "apps/wilipirate/run.sh").read_bytes()
        self.assertTrue(launcher.startswith(b"#!/bin/sh\n"))
        self.assertNotIn(b"\r", launcher)
        self.assertNotIn(b"vendor/", launcher)
        self.assertNotIn(str(ROOT).encode(), launcher)


if __name__ == "__main__":
    unittest.main()
