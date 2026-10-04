# Milestone 1B host validation

Date: 2026-10-04. Platform: original Windows desktop, Python 3.12.
Branch: feature/wilipirate. No device connection, enumeration, installation,
firmware operation, physical validation or Milestone 2 work was performed.

## Results

- `py -3.12 -B -m unittest discover -s tests -v`: **32 tests passed**.
- All 25 logical transitions across HiZ/UART/I2C/SPI/GPIO pass with backend
  request calls prohibited during transitions. Each new session starts in HiZ.
- Invalid modes preserve state; parser grammar/arity/length/control validation,
  per-mode help/info, isolated sessions, exit/EOF/Ctrl-C and batch errors pass.
- Five explicit stubs return unavailable without fabricated bus results.
  A missing backend fails closed. No real backend selection flag exists.
- Runtime AST import/call allowlists pass. A subprocess audit exercises imports,
  every mode and default/interactive/batch execution while denying process,
  socket, device/file access and hardware imports. Adversarial backend environment
  variables do not enable hardware. Standard-library imports are preloaded;
  application import reads are limited to Python source/bytecode. These tests
  guard accidental integration, not arbitrary hostile Python execution.
- Packaging tests verify the explicit seven-module package, dependency-free
  relocated execution, refusal to overwrite destinations and portable launcher.
- `py -3.12 -B tools/stage.py --output dist/m1b/apps`: passed. Staged locally
  into ignored dist/m1b/apps/wilipirate; the previous M1 artifact was not replaced.
- MSYS2 `bash -n` on run.sh: passed. A first attempt at a guessed Git Bash path
  failed because that executable was absent; the available MSYS2 shell was used.
- Staged app run from the unrelated host temporary directory with commands
  `mode uart`, `info`, `exit`: passed, status 0, Mode UART, Backend STUB.
  The shell launcher was syntax-checked; a CM0/Linux launcher run is unverified.
- `git diff --check`: passed.
- `docs/HARDWARE_VALIDATION.md`: unchanged from ea864b8, Git blob identity
  1015df81d011e0a8c9940651114b4eea12af74ae. A regression test checks this content.
  Commit ea864b8 is preserved in branch history; no M1A findings were weakened.

An initial runtime-audit test exposed argparse's lazy standard-library locale
import after file access was denied. Preloading argparse help/locale before
activating the audit resolved the test setup issue without relaxing runtime
hardware/file/process/network denial. The full suite then passed.

## Limits and next prerequisites

This is host application validation, not FREE-WILi compatibility or electrical
safety validation. HiZ is a logical mode and does not configure physical pins.
M1A and all physical validation remain deferred. No fwcm0/OneWili transport or
fallback workaround is included. The mandatory fail-closed rule is recorded in
ARCHITECTURE.md and AGENTS.md.

UI_RESEARCH.md records official application/display/input source research.
The requested htop-style source was not located and needs an exact reference.
The delivered UI is a terminal/log console. An on-device panel renderer needs
separate implementation, a supported bridge-only connection proven not to
fall back, version-qualified display/input behavior, cleanup and restoration,
and separately authorized framework validation/deployment. No hardware bus
backend is necessary merely to exercise those future UI functions.
