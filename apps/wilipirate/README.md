# WiliPirate 0.1.0-m1

A FREE-WILi 2 CM0 Linux application. Requires existing Python 3.10+ only.
No packages, services, firmware changes or OneWili connection are needed for M1.

When later approved for deployment, this folder belongs at
/home/apps/wilipirate/ on the CM0 Linux filesystem. run.sh must be executable.
Choose Linux > Apps > wilipirate > run.sh. It prints identification, safety
status, help, info and the modes, then exits. The launcher records stdout/stderr
under ~/.local/state/freewili/apps/. It does not read disconnected stdin.
This initial UI is log/terminal-based; no LCD or touch UI is included.

From a terminal, use ./run.sh --console for the HiZ> prompt, or:

    ./run.sh --command help --command info --command mode

Commands: help, info, mode, mode hiz, exit. Names are case-insensitive.
I2C/SPI/UART/GPIO are listed but disabled. Other commands fail without I/O.
A failed --command returns 2 and stops the sequence; EOF/exit return 0,
and Ctrl-C in the console returns 130. Long/control-character input is rejected.
No Bus Pirate transaction syntax or arbitrary Python/shell execution is offered.

HiZ means this app performs no hardware operations. It does not measure or
force electrical high impedance, or turn off previously enabled target power.
External pin/power state is unknown. No pin mapping or bus results are supplied.
No persistent data is written. The folder can be relocated without the source
checkout or a virtual environment. No hardware has been tested for this release.
