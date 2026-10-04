"""Architectural guards against accidental hardware integration or fallback."""
import ast
import os
from pathlib import Path
import subprocess
import sys
import unittest

ROOT = Path(__file__).resolve().parents[1]
APP = ROOT / "apps/wilipirate"


class SafetyTests(unittest.TestCase):
    def test_all_runtime_modules_have_closed_import_and_call_surface(self):
        allowed = {"__future__", "abc", "argparse", "dataclasses", "enum", "sys"}
        local = {"model", "parser", "backends", "state", "application", "console"}
        forbidden_calls = {"open", "eval", "exec", "compile", "__import__", "getattr",
                           "setattr", "import_module", "load_module", "system", "popen", "Popen"}
        for source in APP.rglob("*.py"):
            with self.subTest(source=source.name):
                text = source.read_text(encoding="utf-8")
                tree = ast.parse(text)
                for node in ast.walk(tree):
                    if isinstance(node, ast.Import):
                        for item in node.names:
                            self.assertIn(item.name, allowed)
                    elif isinstance(node, ast.ImportFrom):
                        if node.level:
                            self.assertEqual(node.level, 1)
                            self.assertIn(node.module, local | {None})
                        else:
                            self.assertIn(node.module, allowed | {"wilipirate.console"})
                    elif isinstance(node, ast.Call):
                        name = node.func.id if isinstance(node.func, ast.Name) else (
                            node.func.attr if isinstance(node.func, ast.Attribute) else "")
                        self.assertNotIn(name, forbidden_calls)
                    elif isinstance(node, ast.Attribute) and isinstance(node.value, ast.Name) and node.value.id == "sys":
                        self.assertIn(node.attr, {"stdin", "stdout"})
                for marker in ("fwcm0", "onewili", "/dev/", "COM1", "bridge.sock"):
                    self.assertNotIn(marker, text.lower() if marker.islower() else text)

    def test_runtime_audit_denies_hardware_even_with_backend_environment(self):
        environment = dict(os.environ, WILIPIRATE_BACKEND="real", FWCM0_SOCKET="/not/a/bridge")
        result = subprocess.run([sys.executable, "-B", str(ROOT / "tests/no_hardware_runner.py"), str(APP)],
                                env=environment, capture_output=True, text=True, timeout=10)
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn("PASS: runtime import", result.stdout)

    def test_hardware_validation_finding_is_preserved(self):
        import hashlib
        content = (ROOT / "docs/HARDWARE_VALIDATION.md").read_bytes().replace(b"\r\n", b"\n")
        # Git blob identity of the immutable M1A finding; no hardware or git process needed.
        digest = hashlib.sha1(b"blob " + str(len(content)).encode() + b"\0" + content).hexdigest()
        self.assertEqual(digest, "1015df81d011e0a8c9940651114b4eea12af74ae")


if __name__ == "__main__":
    unittest.main()
