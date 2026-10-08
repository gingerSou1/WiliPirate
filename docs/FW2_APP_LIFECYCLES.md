# Existing FW2 application lifecycle investigation

M2 clarification: standard BSP initialization is explicitly allowed for the
new UI-only application. The lifecycle/side-effect findings below remain valid;
their earlier preservation gate is historical. See [M2 DISPLAY](M2_DISPLAY.md).

Date: 2026-10-07. Branch: `feature/wilipirate-panel`; baseline `592824b`.
Source/artifact inspection only. No builds, installed packages, device access,
application execution, BSP edits, pushes or merges.

## Findings

Actual full-screen touch applications have public **app-contract branches**,
distinct from their older default branches. They use the same RAM-app framework
as the BSP examples, initialize/own the DISPLAY peripherals directly, and return
via physical HOME recovery and DISPLAY watchdog reboot. They demonstrate that
custom LCD/touch apps are feasible while MAIN stays separate, but **do not
demonstrate unchanged VREF/power**. Their exact pinned startup sources reach
expander initialization, often before `main`.

The earlier reports' independent-app search was incomplete: reading only default
branches missed these loadable variants. This investigation corrects that source
selection. It does not establish a no-expander startup composition or change
the preservation decision. No examined example satisfies the requested
display/touch + stock MAIN OneWili + zero VREF/power-change composition.

[Structured per-application traces](FW2_APP_LIFECYCLES.json) record all fourteen
requested lifecycle fields for all 19 BSP app directories and seven official
app-contract variants, plus five additional/mapping-limited applications.
The tables below explain the common reset path, full
touch-UI cases, independent applications, SD-image provenance and evidence gaps.

## Source selection and memory models

Rechecked WiliBSP master `be4bdd63d31a80f95410e583710cf4e43a7be7fa`,
subghz master `a79634b76e9de9f9c2df2f97a6914addb056bc3e` and official SD image
`5652b0510377d3f58a130dc1032a80bda362a0c6`. Enumerated official organization
repositories and queried application branches. Current public branch heads:

| Application | Default HEAD inspected | `feat/wilibsp-app-contract` HEAD | Exact BSP gitlink |
| --- | --- | --- | --- |
| subghz | a79634b76e9de9f9c2df2f97a6914addb056bc3e | 6105ca4861a51335c5d44751b9c98e28f1eda6d3 | c40cf9dd628ac66f694e797d60973c137bcb252f |
| usbcamfw | 16400e107fe3bbb7ee58b95f62d1e8567f9fbc2a | 18e54ccf76f5b3cad78e2eb82c06907a739073b1 | 279b4f59e84bf3e3f9af31a9777a356c9f11687f |
| WiliIR | 59669d89232e637d409eb8fbbfcb6dde18623bac | 8454cd04b5bf09380f8e0744e5663ee718253c38 | c38b2594704b3ab427af3ae0bacb46f16d87d4c5 |
| guitarman | ebcda5168a9dc876a4c37c19be2da4ae220c8683 | c95895f9aa4c51d61dab77ea0702b73c3c8f31fa | a89c476fe0892db2816832be89166adf3c6a0396 |
| wilidoro | e43484a0e2f20bbb3a813b916627beb8439918cf | cd88eac7b83e46f5d0e445a1598fc64d4cafa7f2 | c38b2594704b3ab427af3ae0bacb46f16d87d4c5 |
| orca-field-notes | 9ad4a738995f7c020ffb0e708fc3842c7abc071b | 8aa1c49ec9da57e68be66fdfda477a9143868a25 | a89c476fe0892db2816832be89166adf3c6a0396 |
| sensorview | 9e316663d48967b77b2dc4b0ca204b217cdb1968 | e312d386350695a7cece72021fcfc4ce2d92ef86 | c40cf9dd628ac66f694e797d60973c137bcb252f |

The four pinned BSP revisions above were downloaded/inspected, not substituted
with the current BSP by assumption. All have `board_init_psram`, a bootstrap
flag causing subsequent `board_init` to use `board_init_inherited`, and reachable
`ioexp_init` writing external VREF selection. Their recovery code reboots DISPLAY.
Older pins differ in runtime-service initialization/recovery power enforcement;
none supplies an expander-preserving attach initializer.

The current default branches of subghz, WiliIR, usbcamfw and orca-browser use
`copy_to_ram`; guitarman and sensorview use `default`. Those are flash-stored
images, not proof of an SD RAM application. Wiliplayer's current source is
explicitly flash-resident. Their RAM execution or polished UI must not be mistaken
for nonpersistent loading. App-contract branches use `fw2_display_app` or
`fw2_psram_app` instead. Comments still mentioning flash/copy-to-RAM in a migrated
file do not override its active CMake helper.

## Reset, ownership and rendering

### SRAM app path

```text
stock SD/App Explorer -> DISPLAY loader loads SRAM UF2
 -> application vector / Pico SDK _reset_handler
 -> data/BSS/runtime initialization -> main
 -> board_init -> board_init_clk -> board_init_peripherals -> ioexp_init
 -> recovery keyboard/power policy
 -> ST7796 + optional touch + app drawing
```

[fw2_display_app](https://github.com/freewili/wilibsp/blob/be4bdd63d31a80f95410e583710cf4e43a7be7fa/bsp/CMakeLists.txt)
sets `no_flash`, emits metadata and generates/checks the app UF2. The inspected
[SDK 2.3.0 crt0.S](https://github.com/raspberrypi/pico-sdk/blob/2.3.0/src/rp2_common/pico_crt0/crt0.S)
calls `runtime_init` then `main`; [runtime.c](https://github.com/raspberrypi/pico-sdk/blob/2.3.0/src/rp2_common/pico_runtime/runtime.c)
walks the SDK initializers. This is an SDK reset/runtime path, not a call from
stock DISPLAY with its application context retained. No target compilation
was performed to inspect a resulting SRAM ELF's symbols.

### PSRAM app path

```text
DISPLAY loader establishes/fills PSRAM
 -> _entry_point/_reset_handler in PSRAM
 -> copies bootstrap to SRAM -> fw2_psram_bootstrap
 -> copy data, zero BSS, set VTOR; reset/recreate runtime services
 -> board_init_psram -> clock/QMI/peripheral work -> ioexp_init
 -> enable interrupts; language constructors -> main
 -> board_init (in most apps) -> board_init_inherited -> ioexp_init again
 -> recovery + LCD/touch + first frame
```

Sources: [psram_startup.S](https://github.com/freewili/wilibsp/blob/be4bdd63d31a80f95410e583710cf4e43a7be7fa/bsp/app/psram_startup.S),
[psram_bootstrap.c](https://github.com/freewili/wilibsp/blob/be4bdd63d31a80f95410e583710cf4e43a7be7fa/bsp/app/psram_bootstrap.c),
[linker overrides](https://github.com/freewili/wilibsp/tree/be4bdd63d31a80f95410e583710cf4e43a7be7fa/bsp/app/psram_link),
and each app's exact gitlink in the table above. The bootstrap deliberately avoids
the normal cold-boot preinit array to protect the executing PSRAM bus. It still
calls board initialization; bypassing the usual CRT does not bypass VREF writes.
`hello_psram_exec` is a real exception to a literal `board_init()` call in main,
but its bootstrap has already performed those operations.

Apps replace the **executing** DISPLAY program. MAIN continues as the separate
stock service processor; this is not a stock DISPLAY plugin/overlay. The apps
claim their own SPI/DMA/PIO/IRQ resources. LCD initialization configures internal
SPI1 and GPIO8/9/10/11, issues panel SWRESET/configuration and claims DMA/IRQ.
Touch uses the DISPLAY's internal I2C1 and FT6336 controller; LVGL ports install
pointer polling callbacks. Their partial pixel buffers flush through
`st7796_flush_async`; completion ISR releases LVGL's flush. Direct-render apps
draw/flush their own buffers instead. UI ownership is concrete peripheral
ownership, not merely calling stock MAIN to draw pixels.

The expander also controls display reset/buffer directions and unrelated VREF,
mic/IR/USB/radio controls. Initializing the display infrastructure currently
writes these complete output/direction registers; the app does not inherit only
the display bits. See [board.c](https://github.com/freewili/wilibsp/blob/be4bdd63d31a80f95410e583710cf4e43a7be7fa/bsp/platform/board.c)
and [ioexp.c](https://github.com/freewili/wilibsp/blob/be4bdd63d31a80f95410e583710cf4e43a7be7fa/bsp/platform/ioexp.c).
Internal DISPLAY initialization must not be conflated with external target bus
I/O. Nevertheless the shared expander's VREF/power writes violate preservation.

## Actual full-screen app traces

All seven app-contract variants run on DISPLAY, use `main`, call `board_init`,
reach `ioexp_init`, select external VREF via their BSP, and service recovery.
The six PSRAM apps additionally invoke `board_init_inherited` indirectly after
bootstrap. None of these seven opens OneWili for its UI; each renders locally.
Their existence alone does not prove the complete proposed UI + MAIN-link
minimal composition. No explicit programmable target-Vout enable was found in
the traced entry sequences, but full expander/app power masks are not unchanged
target-power evidence.

| Application/source entry | First-screen sequence | Touch | Other hardware/power |
| --- | --- | --- | --- |
| [subghz 6105ca48](https://github.com/freewili/subghz/blob/6105ca4861a51335c5d44751b9c98e28f1eda6d3/src/main.c) | PSRAM bootstrap; board/recovery; PSRAM sizing; `st7796_init`, full black fill, `lv_init`, `lvgl_port_init`, `screen_home_create`; radio/capture/LED setup; backlight; first `lvgl_port_run` renders widgets | `ft6336_init` inside LVGL port; pointer callback | CC1101 probe/modulation, antenna mux, GDO capture, LED UI. No declared zones in helper; relies on inherited availability. |
| [WiliIR 8454cd04](https://github.com/freewili/WiliIR/blob/8454cd04b5bf09380f8e0744e5663ee718253c38/src/main.c) | Bootstrap; board/recovery; buffer checks; LCD/AgentIO; black fill; LVGL; home-screen objects; first LVGL loop flush | FT6336 in LVGL port | Declares SENSORS/DISPLAY/USB_HUB/RGB_LEDS; IR capture/TX, USB, LEDs; retained-UI redraw on About dismissal |
| [GuitarMan c95895f9](https://github.com/freewili/guitarman/blob/c95895f9aa4c51d61dab77ea0702b73c3c8f31fa/src/main.c) | Bootstrap; board/recovery; LCD/fill/LVGL; touch; `song_store_boot`; `ui_nav_create`; `lv_timer_handler`; backlight | Explicit FT6336/register-touch | USB host power asserted and MSC/catalog queried before first selector screen; no zones declared in helper |
| [Wilidoro cd88eac7](https://github.com/freewili/wilidoro/blob/cd88eac7b83e46f5d0e445a1598fc64d4cafa7f2/src/target/main.c) | Bootstrap; board/recovery; LCD/AgentIO/fill; LVGL/touch; HAL; `app_init`; `lv_timer_handler` | Explicit FT6336/register-touch | Declares SENSORS/DISPLAY/AUDIO/SUBGHZ/RGB_LEDS; HAL keyboard, LED, backlight, audio/tilt helpers |
| [Orca browser 8aa1c49e](https://github.com/freewili/freewili2-orca-field-notes/blob/8aa1c49ec9da57e68be66fdfda477a9143868a25/apps/orca_browser/main.c) | Bootstrap; board/recovery; AUDIO request; LCD fill/boot banner; LVGL port; `screen_home`; `lv_refr_now` | FT6336 in LVGL port | Audio rail request and codec low-power setup; no generated zones |
| [SensorView e312d386](https://github.com/freewili/sensorview/blob/e312d386350695a7cece72021fcfc4ce2d92ef86/src/main.c) | Bootstrap; board/recovery; LCD/fill; LVGL/touch; sensor/AHRS/LED/PDM setup; `ui_app_shell_create`; `lv_timer_handler`; backlight | Explicit FT6336/register-touch | Sensor hub, PDM/mic power, LEDs; no generated zones |
| [Webcam 18e54ccf](https://github.com/freewili/usbcamfw/blob/18e54ccf76f5b3cad78e2eb82c06907a739073b1/src/main.c) | SRAM reset; board/PSRAM checks; recovery/USB power wait; explicit additional `ioexp_init`; LCD; `draw_boot_screen`; backlight | No touch initialization in this entry | USB_HUB keep-awake, later VBUS cycle/UVC initialization and core1 LCD streaming; no zones in helper |

The corresponding CMake files are linked by repository/commit and stored in the
structured record. Wilidoro's FW2 target is defined in
[cmake/board_fw2.cmake](https://github.com/freewili/wilidoro/blob/cd88eac7b83e46f5d0e445a1598fc64d4cafa7f2/cmake/board_fw2.cmake),
not an OG or simulator target. Subghz's [LVGL port](https://github.com/freewili/subghz/blob/6105ca4861a51335c5d44751b9c98e28f1eda6d3/src/ui/lvgl_port.c)
provides the actual display flush and FT6336 input composition.

## Current WiliBSP apps: complete coverage

At be4bdd63 there are 19 app directories, including `template` (not registered
by the root CMake). The other 18 are registered. All ordinary rows below are
SRAM `fw2_display_app` targets using SDK entry -> main -> board/recovery;
all reach expander defaults/external VREF. `hello_dvi` uses `board_init_clk`,
not a literal `board_init`; `hello_psram_exec` uses the PSRAM bootstrap described
above. No ordinary example calls `board_init_inherited` directly.

| App | First display output | Touch | OneWili | Additional ownership |
| --- | --- | --- | --- | --- |
| template | LCD clear/text | No | No | DISPLAY zone; releases unused inherited rails |
| hello_display | LCD clear/touch invitation | Yes | No | DISPLAY/RGB_LEDS; PIO LEDs |
| hello_agentio | Full clear, color bars/text | Yes | No | DISPLAY, PSRAM, AgentIO/keyboard |
| hello_keyboard | Clear, text/chord bars -> async framebuffer flush | Yes | No | DISPLAY, PSRAM, keyboard/AgentIO |
| hello_charger | Clear -> status screen -> async flush | No | No | DISPLAY, PSRAM, keyboard/charger telemetry |
| hello_audio | LCD title/status | No | No | DISPLAY/SENSORS/AUDIO; codec/I2S |
| hello_cc1101 | LCD title/status | No | No | DISPLAY/SENSORS/SUBGHZ; antenna/radio |
| hello_mics | LCD mic title | No | No | DISPLAY/SENSORS; mic enable, PDM/DMA |
| hello_sensors | LCD title/presence | No | No | DISPLAY/SENSORS; four internal sensors |
| hello_ir | LCD loopback instructions | No | No | DISPLAY/SENSORS; IR rail/capture/TX |
| hello_usbdrive | LCD USB title/status | No | No | DISPLAY/SENSORS/USB_HUB; host-power/MSC |
| hello_sdcard | LCD connecting status before link | No | Recovery-aware open + SD wrapper | DISPLAY/SDCARD; MAIN SD writes/reads |
| hello_dvi | HSTX test pattern; no LCD | No | No | 252 MHz, DVI; no declared zones |
| toggleled | LCD GPIO title before link | No | Recovery-aware open | DISPLAY/SENSORS; explicit VREF_3V3 + MAIN GPIO25 |
| hello_vref | LCD sweep title before link | No | Recovery-aware open | DISPLAY/SENSORS; VREF sweep/ADC/GPIO25 |
| retrochat | LCD clear -> chat UI | Yes | No | DISPLAY/AUDIO; codec/PDM/acoustic modem/core1 |
| canblast | `screen_static`, then rail/link status | No | Recovery-aware open, binary/fast API | CAN keep-awake/config/stream/TX; releases inherited rails by default |
| dualcpu | LCD clear/static buttons/status before link | Yes | Recovery-aware open, peer streams | DISPLAY/RGB_LEDS; WIFI_BT keep-awake, release unused, ESP32 mode change |
| hello_psram_exec | LCD clear/PSRAM text after rail cycle | No | No | DISPLAY/RGB_LEDS; RGB rail cycle; AgentIO |

Each row's immutable `apps/<name>/main.c` and CMake source links are in the JSON.
The strongest real composition for **touch UI + stock MAIN OneWili** is
[dualcpu/main.c](https://github.com/freewili/wilibsp/blob/be4bdd63d31a80f95410e583710cf4e43a7be7fa/apps/dualcpu/main.c#L879):
board -> recovery -> LCD/AgentIO -> FT6336/static UI -> ESP32 power policy ->
`fw2_app_recovery_open_onewili` -> MAIN commands/events. Its loop services both
recovery and touch/streams. It proves the composition is used, and also proves
that this instance changes VREF/rail policy. It is not a no-expander counterexample.

All examples use the common physical-HOME watchdog exit; none shows a direct
return into a saved stock DISPLAY call stack. No programmable target-Vout enable
was identified in these entry sequences. GPIO examples explicitly change VIO;
whole-mask power release/reassertion is not a target-state preservation guarantee.

## Other public application patterns

* [radio2tc 460f8691](https://github.com/freewili/radio2tc/blob/460f86916f0ec16c3503c2d7381dfcd86c991015/freewili/src/main.cpp)
  is a genuine additional SRAM touch UI + MAIN OneWili app. Its helper declares
  DISPLAY/USB_HUB/SDCARD; entry is board -> recovery -> ST7796/FT6336 ->
  AgentIO -> `screen::init`/backlight -> native USB link -> SD logger OneWili open.
  Its pinned BSP `5fa1e56cea29254badb9c8f71acd027cda0ea45a` has the same
  `board_init_peripherals -> ioexp_init` and watchdog recovery. Thus it proves
  another working ownership pattern, not preservation. No explicit target-Vout
  enable is identified; expander/rail defaults still change.
* [chordboard-invaders efd62691](https://github.com/freewili/chordboard-invaders/blob/efd62691069ca6b7d4e337a42bb7f7d1e6eb089c/apps/typing_invaders/main.c)
  calls board, PSRAM, LCD, FT6336, keyboard, audio/LED/haptic/IMU/high-score setup,
  then renders intro/game frames via DMA. No OneWili or common HOME recovery
  call is found in main; loader-compatible exit and unchanged state are unverified.
  Its source should not be presented as a qualified current FW2App lifecycle.
* [wiliplayer 6b3b0725](https://github.com/freewili/wiliplayer/blob/6b3b07256963d06a2068b4be9b8198cc90eb3d9b/src/main.c)
  uses its own board/expander initialization, codec/mic/IR, USB/PSRAM and LCD
  rendering, or an HDMI mode that deliberately skips LCD. It is flash-resident,
  not a supported preserved-stock RAM app in this source. Configuration reboot
  runs its firmware again; it is not proof of stock UI restoration. No touch or
  OneWili startup is identified in the inspected main.
* `whalestoyourroom` b356263eb472c7c3f519ff386703105835f6920f is content,
  not a source-backed DISPLAY reset/render implementation.
* `wili8jam` default 71104dda2ac448fe2eb6c0c62f988eb6154faa78 selects
  `adafruit_fruit_jam`, DVI/USB and GPIO11 host-power startup. It is **not** the
  source-proven FW2 LCD lifecycle of the SD `wili8.uf2`, despite that artifact's
  embedded repository URL. Its reset/return path cannot be mapped to FW2 by name.

## Official SD image: artifact/source matching limits

Inspected all 20 app UF2 files in the official
[SD image at 5652b051](https://github.com/freewili/FREE-WILi2-Defcon34SDCardImage/tree/5652b0510377d3f58a130dc1032a80bda362a0c6/apps),
read-only. [Inventory](FW2_SD_APP_INVENTORY.json) records exact file hashes,
sizes, min/max block targets, magic checks and embedded repository URLs.
This is address/metadata inspection, not deployment validation, code execution,
or proof that a particular artifact was built from a particular source commit.

| SD app with located public source | Observed image target | Source available for lifecycle |
| --- | --- | --- |
| subghz | PSRAM | App-contract branch above; exact SD build commit not embedded/proven |
| wiliir | PSRAM | App-contract branch above; exact build mapping unverified |
| guitarman | PSRAM | App-contract branch above; exact build mapping unverified |
| orca-field-notes | PSRAM | App-contract branch above; exact build mapping unverified |
| wilidoro | PSRAM | App-contract branch above; exact build mapping unverified |
| usbcam-viewer | SRAM | App-contract branch above; exact build mapping unverified |
| wili8 | SRAM | Embedded URL points to Fruit Jam source; FW2 lifecycle unmapped |
| meshtastic | PSRAM | Embedded URL points to Ytuf/firmware; public freewili-port branch inspected, exact RAM artifact build source unmapped |

The six named app-contract sources match the *memory model* of their SD artifacts,
unlike their default branches. A matching memory model is insufficient to claim
binary/source identity. SensorView has a public PSRAM branch but is not present
among the 20 SD app files at this revision.

The SD Meshtastic artifact points to `Ytuf/firmware`; its default develop
91f930d5c028f0690090e102935094049dc0b608 has no FreeWili variant, while public
`freewili-port` 58af2f8557255ce42952fa51e435a272d3ab1a4f does.
[variant.cpp](https://github.com/Ytuf/firmware/blob/58af2f8557255ce42952fa51e435a272d3ab1a4f/src/platform/extra_variants/freewili/variant.cpp)
uses `initVariant -> initIOExpanderPicoSDK`, not WiliBSP `board_init`. That routine
writes full expander outputs `04 DF F4 85` and directions `0C 00 00 02`, clearing
the four VREF-select bits and changing power/mux state. Thus different function
names do not imply preservation. The Arduino reset-to-variant invocation and
PSRAM loader build of the shipped artifact are unverified.

[setup](https://github.com/Ytuf/firmware/blob/58af2f8557255ce42952fa51e435a272d3ab1a4f/src/main.cpp)
calls `screen->setup`; [TFTDisplay.cpp](https://github.com/Ytuf/firmware/blob/58af2f8557255ce42952fa51e435a272d3ab1a4f/src/graphics/TFTDisplay.cpp)
has a FREEWILI SPI1 40 MHz panel init/configuration and FT5316 touch polling.
The Wi-Fi service uses its vendored `ow_open_fwgui` to reach MAIN. Late variant
initialization adds audio/USB/touch/LED/haptic work. A source-proven HOME exit to
stock for this exact SD artifact was not located. This is another full DISPLAY
ownership architecture, but not a preservation-compatible minimal sequence.

`startrek.uf2` embeds `freewili/startrek`, which returns HTTP 404 through the
public API. `wilidexed.uf2` embeds `freewili/wilibsp`, but the inspected current
apps/branches inventory does not expose its application source. Remaining
unmapped files: tone_poc, wilicankit, DOOM, flappywili, fw2nes, wili-snake,
wiliman, orcafood, wili_apps_galore and wedding_disco. No public matching source
was established by the bounded official repository/metadata/search inspection.
Their entry, expander/VREF behavior, rendering and exit remain UNVERIFIED;
they are not treated as minimal-startup evidence.

## HOME and restoration: what is actually proved

[app_recovery.c](https://github.com/freewili/wilibsp/blob/be4bdd63d31a80f95410e583710cf4e43a7be7fa/bsp/input/app_recovery.c)
initializes keyboard reception, tests a fresh HOME-down state (1100 ms maximum
frame age), tracks a five-second hold and calls `watchdog_reboot(0,0,0)`.
Every loop/retry/error path must keep servicing it. The resulting CPU reset
enters the DISPLAY recovery loader and is intended to resume preserved stock
flash firmware. MAIN is not reset by this app call. No C `return`, trampoline
or saved-context resume is used by these apps.

The upstream [2026-08-06 hardware finding](https://github.com/freewili/wilibsp/blob/be4bdd63d31a80f95410e583710cf4e43a7be7fa/docs/superpowers/findings/2026-08-06-fw2app-contract-e2e.md)
records stock App Explorer launch and physical HOME recovery for hello_agentio.
It also records LCD pixels surviving handoff, which prompted complete first-screen
clearing. This is concrete upstream evidence for the mechanism, not verification
of every app or this user's installed stack. The public stock loader/recovery
implementation was not located in the inspected release repositories, so its
internal selection/reset handoff is documented plus upstream observed behavior.
Original VIO/power restoration is not proved by returning to stock firmware.

Subghz/WiliIR on-screen HOME callbacks merely request their own `SCREEN_HOME`.
About-dismissal redraw callbacks restore the app's retained screen, not stock UI.
Those two UI events must not be reported as application exit mechanisms.

## Conclusion and validation

**PROVEN SUPPORTED:** source-backed SRAM/PSRAM custom DISPLAY applications,
local full-screen rendering/touch ownership, and examples combining that UI
with stock MAIN OneWili. **PROVEN UNSUITABLE for the preservation invariant:**
the traced board/expander initialization compositions. **UNVERIFIED:** an
existing composition that initializes only display/touch/MAIN link with zero
expander/VREF/power change, and artifact-to-exact-source lifecycle for unmapped
SD apps. No such composition was found, and none was invented from individual
driver APIs.

Only research documentation changed. Full Windows host regression: 50 tests
discovered, 49 passed, one native compiler check skipped because C++ compiler
is unavailable. Import/call guards, runtime audits and M1A identity passed.
`git diff --check` passed. No embedded build or physical test is implied.
Stop: no WiliPirate implementation or device interaction.
