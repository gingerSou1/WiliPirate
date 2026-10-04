# WiliPirate 0.1.0-m1b

A stub-only FREE-WILi 2 CM0 Linux application. Requires Python 3.10+ only.
The app.py entry point and wilipirate/ package must remain together.

Run ./run.sh --console from a terminal for a Bus Pirate-style prompt.
Modes: HiZ (default), UART, I2C, SPI, GPIO. Commands:

    help
    info
    mode
    mode hiz
    mode uart
    mode i2c
    mode spi
    mode gpio
    exit

Mode selection changes only application state. The prompt follows the current
mode. Every info response identifies Backend: STUB and Hardware: not enabled.
Invalid mode names do not change state. The help response lists stub requests:
I2C scan, UART/I2C/GPIO read or write <arguments...>, SPI transfer <arguments...>.
These requests fail with '<mode> hardware backend not enabled.' No physical
operation, payload encoding, fake response or power change is implemented.

For a batch session:

    ./run.sh --command 'mode uart' --command info --command exit

Batch commands share state and stop on failure (status 2). Exit and EOF return
0; Ctrl-C in the console returns 130. Each new session starts in HiZ. Overlong
input, control characters, command chaining and unsupported commands are rejected.

The eventual supported location is /home/apps/wilipirate/run.sh on CM0 Linux,
with an executable launcher selected in Linux > Apps. Default invocation logs
help/info/modes and exits without reading disconnected stdin. Launcher logs
belong under ~/.local/state/freewili/apps/. M1B includes no LCD/touch renderer.
Physical deployment and validation are deferred; do not install this milestone
on a device without a separate approval.

HiZ is logical, not measured/enforced electrical isolation. External pin and
power state remain unknown. No device connection, dependencies, plugin loader,
hardware selection flags or persistent data are used. The folder is relocatable.
There is no supported-bridge-to-direct-hardware fallback path in this application.
