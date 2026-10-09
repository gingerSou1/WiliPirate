# M3 diagnostic: manual empty-bus verification

**Preparation only. Deployment and device access require separate approval.**
These instructions describe the exact offline-validated artifact; they are not
authorization to install or launch it. Do not connect a PN532 or any target yet.

## Artifact and precautions

File: `build/m3-diag/WiliPirateI2CDiag.uf2` (66,560 bytes).
SHA-256:

```text
95719e2cec6415dc974297973d5938f192a2a431c18f8899063d2f385e804a0e
```

The official validator confirms a DISPLAY SRAM app, not firmware. Keep the
separate M2 `WiliPirate.uf2` unchanged. Disconnect all external targets,
GPIO/header wiring, external VREF sources, accessories and probes. A USB host
connection used for the approved official installer is separate from target
wiring. Confirm the installed stock MAIN/DISPLAY/loader versions and supported
SD Apps mechanism; no firmware update is part of this procedure.

Startup changes core voltage/clocks, PSRAM timing, internal buses, expander
outputs/directions, radio routing and display settings. It selects external
VREF for VIO, requests/maintains normal DISPLAY power, and does not deliberately
release inherited rails. Do not assume it supplies target voltage or establishes
HiZ. Manual link opening initializes the internal OneWili UART and SD client.
The app adds no VREF, external GPIO, bus-setting or extra rail configuration;
stock MAIN Poll's own electrical behavior is still unverified.

## Install after approval

1. From the repository root, verify the exact file:

   ```powershell
   Get-FileHash -LiteralPath .\build\m3-diag\WiliPirateI2CDiag.uf2 -Algorithm SHA256
   ```

2. Use official WiliBSP host tooling with its `pyfwfinder`/`pyserial` dependencies
   already configured. This workspace retains the pinned unmodified source copy.
   The corresponding command is:

   ```powershell
   py -3.12 -B build/m3-prerequisites/wilibsp-be4bdd63d31a80f95410e583710cf4e43a7be7fa/tools/fw.py install-app build/m3-diag/WiliPirateI2CDiag.uf2 --device YOUR_DEVICE_SERIAL
   ```

   Replace `YOUR_DEVICE_SERIAL` with your own device identifier. A separately
   configured official checkout can supply the same installer instead. Check
   its exit/result before proceeding. It validates the UF2, transfers to SD
   `/apps/WiliPirateI2CDiag.uf2`, flushes the copy and returns SD ownership to
   MAIN. Do not interrupt/eject during handoff. Do not use BOOTSEL, `flash`,
   a firmware updater or a debugger loading procedure.

## First launch: display and HOME, without Poll

1. With all external wiring still disconnected, open the stock **Apps** menu
   and select **WiliPirateI2CDiag.uf2**.
2. Confirm the diagnostic title, READY state, zero captured bytes and the four
   labels above grey/yellow/green/blue buttons. No Poll is sent on launch.
3. Optionally verify PAGE held five seconds opens About with version 001 and
   repository URL; release to return.
4. **Do not arm or press GREEN for this launch.** Hold physical **HOME for five
   seconds** and confirm stock DISPLAY operation returns. If display/recovery
   fails, stop and report it; no target connection or firmware workaround.

## Fresh launch: one empty-bus Poll

After the first launch/recovery check passes and the empty-bus run is approved:

1. Launch the diagnostic again, with all external wiring disconnected.
2. Press and release **GREEN** to arm. Confirm ARMED. Press GREEN once more to
   initiate the sole attempt. Do not use PAGE/About during capture.
3. Wait for a terminal status. The app opens the link, checks for pending input,
   submits at most one stock Poll, and captures for a five-second receive window.
   An opening/preflight/send error may end the attempt without sending Poll.
4. Record the displayed byte count, chunk count, Poll-frame count, status and
   complete error mask. No second Poll is permitted during this run, including
   after errors. Do not change rails, VREF or I2C settings to bypass a refusal.

## Retrieve all captured bytes and metadata before HOME

The raw result is retained in RAM, with **every byte**, not only a preview:

1. Select **RAW HEX** using BLUE if necessary. Record or photograph every page
   from `1/N` through `N/N`, using YELLOW NEXT and GREY PREV. Each page contains
   up to 80 bytes, eight per row, with decimal offsets. Keep pages in order.
2. Reconstruct the captured stream by concatenating only the hexadecimal byte
   pairs in offset order. Exclude page titles and decimal offset labels. Verify
   contiguous offsets and exactly the displayed byte count. Preserve control
   bytes, newlines and non-text values; do not convert it to an address list.
3. Use BLUE to select **INFO**. Record its identity/timing page, transport-counter
   page and every remaining chunk-timing page. These preserve observed command
   path, MAIN timestamp/sequence/flag, submission/receive times and loss evidence.
4. For pre-existing-input errors, the retained bytes are labelled pending input,
   not Poll output. Keep that distinction when reporting the capture.
5. Save all records before HOME: exiting clears RAM. There is no SD log or
   automatic machine-readable export. Large captures require many photographs;
   missing pages make reconstruction incomplete. Approve any future automated
   export separately rather than treating a partial screen as complete output.

Even a complete successful envelope does not prove a device count, address list,
electrically valid bus, or absence of stale same-path replies. Do not report
"no devices" from an empty body, timeout or unavailable result. On truncation,
timing overflow, loss, malformed/multiple/unrelated frames, retain the evidence
and report the run as inconclusive. A firmware error such as `EPOWERZONE` is a
refusal to investigate later, not a reason to enable a rail or retry now.

| Mask bit | Meaning |
| --- | --- |
| `0x0001` | Link opening failed |
| `0x0002` | Pre-existing input; Poll not sent |
| `0x0004` | Submission error; no retry |
| `0x0008` | Receive I/O error |
| `0x0010` | Raw buffer full/truncated |
| `0x0020` | Chunk-timing records exhausted |
| `0x0040` | No reply captured before receive deadline |
| `0x0080` | Incomplete response evidence |
| `0x0100` | Unrelated command path |
| `0x0200` | Malformed envelope/identity |
| `0x0400` | Multiple matching frames |
| `0x0800` | Transport loss/checksum/length error |
| `0x1000` | Firmware returned failure flag |
| `0x2000` | Recovery timer could not start; Poll not sent |

Flags can be combined. Their absence only qualifies the observed envelope,
not the unknown response body's semantics.

## Exit and stop

After preserving the capture, hold HOME for five seconds and confirm stock
operation returns. Report the artifact hash, component versions if known,
full raw reconstruction, all INFO data and recovery outcome. Redact personal
paths and serials from public reports.

If HOME fails or submission appears stuck, stop following this test sequence
and use official FreeWili support/recovery guidance; do not flash firmware.
The source services HOME during receive and attempts timer-based service during
the blocking write, but those physical failure paths are not yet qualified.
No PN532 wiring, known-peripheral test, automatic retry or further interface
work is authorized by this empty-bus procedure.
