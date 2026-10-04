"""Static/native-model checks only. Never launch the prepared CM0 executable."""
from pathlib import Path
import os
import re
import shutil
import subprocess
import sys
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "apps/wilipirate"))
from wilipirate.application import Application
from wilipirate.model import Mode


class NativePreparationTests(unittest.TestCase):
    def test_only_documented_ui_api_calls(self):
        source = (ROOT / "native/cm0/main.cpp").read_text()
        calls = set(re.findall(r"\b(ow_\w+)\s*\(", source))
        self.assertEqual(calls, {
            "ow_gui_controls_add_text", "ow_gui_control_properties_set_control_value_text",
            "ow_gui_panels_add_panel", "ow_gui_panels_set_menu_text",
            "ow_gui_panels_show_panel", "ow_gui_panels_read_buttons", "ow_gui_clear_display",
        })
        self.assertIn("wilicm0::Device bridge;", source)
        for forbidden in ("system(", "popen(", "exec(", "connect_cm0", "LinuxTransport", "fwcm0 api", "/dev/", "ow_io_"):
            self.assertNotIn(forbidden, source)
        self.assertIn("if (argc != 1)", source)
        self.assertIn("std::signal(SIGTERM, stop)", source)
        self.assertIn("Display cleanup unavailable", source)

    def test_supported_adapter_has_no_direct_fallback(self):
        source = (ROOT / "vendor/wilicm0bsp/bsp/src/device.cpp").read_text()
        self.assertIn("::socket(AF_UNIX", source)
        self.assertIn("::connect(", source)
        self.assertIn("catch (...) { close(); throw; }", source)
        self.assertIn("ow_hardware_system_device_state", source)
        for forbidden in ("LinuxTransport", "run_direct", "execl", "popen", "system(", "/dev/", "ioctl("):
            self.assertNotIn(forbidden, source)
        # This source guard is not a live bridge test or proof about firmware effects.

    def test_build_and_launcher_are_explicit_linux_apps_only(self):
        cmake = (ROOT / "native/cm0/CMakeLists.txt").read_text()
        self.assertIn('CMAKE_SYSTEM_NAME STREQUAL "Linux"', cmake)
        self.assertIn('CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64)$"', cmake)
        self.assertIn('WILICM0_BUILD_DRIVER OFF', cmake)
        self.assertIn('wilicm0::wilicm0', cmake)
        launcher = (ROOT / "native/cm0/run.sh").read_bytes()
        self.assertNotIn(b"\r", launcher)
        self.assertIn(b'exec "$APP_DIR/WiliPirate"', launcher)
        self.assertNotIn(b"sudo", launcher)
        self.assertNotIn(b"fwcm0", launcher)

    def test_native_constexpr_matches_python_modes(self):
        compiler = os.environ.get("WILIPIRATE_SYNTAX_CXX") or shutil.which("arm-none-eabi-g++") or shutil.which("g++")
        if not compiler:
            self.skipTest("C++ compiler unavailable; native constexpr validation not performed")
        modes = tuple(Mode)
        code = ['#include "model.hpp"', 'using namespace wilipirate;', 'constexpr bool check() {', 'State s;']
        code += ['if (s.mode != Mode::HiZ || s.closed) return false;']
        for before in modes:
            for after in modes:
                app = Application()
                app.submit("mode " + before.value)
                reply = app.submit("mode " + after.value)
                self.assertTrue(reply.success)
                expected = modes.index(app.view.mode)
                code += [f'select(s, static_cast<Mode>({modes.index(before)}));',
                         f'if (!select(s, static_cast<Mode>({modes.index(after)}))) return false;',
                         f'if (static_cast<int>(s.mode) != {expected}) return false;']
        code += [
            'auto old = s.mode;',
            'if (select(s, static_cast<Mode>(-1)) || select(s, static_cast<Mode>(5)) || s.mode != old) return false;',
            'for (int i=0;i<5;++i) if (backend_available(static_cast<Mode>(i))) return false;',
            'select(s, Mode::HiZ);',
            'for (int i=1;i<=5;++i) { buttons(s, 2u); if (static_cast<int>(s.mode) != i%5) return false; }',
            'buttons(s, 1u); if (!s.help) return false;',
            'buttons(s, 3u); if (s.mode != Mode::HiZ || !s.help) return false;',
            'buttons(s, (1u<<11)|2u); if (!s.closed || s.mode != Mode::HiZ) return false;',
            'if (select(s, Mode::UART)) return false;',
            'buttons(s, 2u); return s.mode == Mode::HiZ;',
            '}', 'static_assert(check(), "Python/native logical mode parity failed");',
        ]
        with tempfile.TemporaryDirectory() as temp:
            source = Path(temp) / "model_check.cpp"
            source.write_text("\n".join(code))
            result = subprocess.run([compiler, "-std=c++17", "-Wall", "-Wextra", "-Werror", "-fsyntax-only",
                                     "-I", str(ROOT / "native/cm0"), str(source)],
                                    capture_output=True, text=True, timeout=30)
        self.assertEqual(result.returncode, 0, result.stderr)


if __name__ == "__main__":
    unittest.main()
