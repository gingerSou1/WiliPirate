# Milestone 0/1 validation

Date: 2026-10-03. Host: Windows, Python 3.12; shell: MSYS bash.
No physical FREE-WILi was accessed, no device packages were installed,
and no firmware, Linux image, bridge or boot configuration was changed.

## Executed checks

- `py -3.12 -B -m unittest discover -s tests -v`: 18 tests passed.
- Tests cover launch with forbidden stdin/file/process/network access;
  dependency-free runtime imports; help/info/mode; refusal of every hardware
  mode; invalid syntax; control characters; input length; batch stop-on-error;
  exit/EOF/Ctrl-C; CRLF; recovery after invalid and oversized input.
- Packaging tests run a relocated staged app with isolated Python (`-I`) from
  an unrelated directory, inspect package contents, and verify that an existing
  destination directory or file is not overwritten.
- `bash -n apps/wilipirate/run.sh`: passed.
- Actual `dist/apps/wilipirate/run.sh --command info` invocation under MSYS
  from the host temporary directory: passed, correct identification and safety
  message. This tests the shell entry point, not CM0 Linux deployment.
- `py -3.12 -B tools/stage.py --output dist/apps`: passed; generated output
  is Git-ignored. No OneWili package is required or bundled for M1.
- Source-file citation audit against the pinned research checkouts identified
  two stale paths (Bus Pirate LICENSE.TXT and src/binmode/logicanalyzer.pio);
  both references were corrected before the final audit.
- `git diff --check` and staged whitespace checks: passed.
- Git executable mode for run.sh is explicitly recorded as 100755; LF-only
  shell content is enforced by .gitattributes and checked by tests.
- ControlLab remained clean at 32b1ea9b873049d817b622adb3c6c83cc9b9378f.

The first sandboxed downloads/Python launch failed due to host sandbox/network
restrictions. Authorized host execution completed the research and tests.
A git inspection accidentally run from the unrelated launcher-test directory
reported "not a git repository"; this was not an app failure and changed no repo.

## Not verified

Physical Linux Apps navigation, app log behavior on this specific device,
stock firmware/bridge compatibility, LCD/touch UI, or any bus/electrical behavior.
Python 3.10/3.11 and native Linux/macOS hosts were not executed here. No upstream
firmware builds, driver tests or hardware examples were run: those components
were not modified, and they are not dependencies of the no-I/O scaffold.

Milestone 2 is a proposal only. See ARCHITECTURE.md for the exact gated GPIO
snapshot test. The application does not enforce electrical HiZ or verify
existing target power. The original-code distribution license is undecided;
third-party reference licenses are documented in THIRD_PARTY.md.
