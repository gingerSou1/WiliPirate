/* SPDX-License-Identifier: MIT; Copyright (c) 2026 gingerSou1 */
#include "fw2.h"
#include "input/app_recovery_onewili.h"
#include "platform/diag.h"
#include "capture.h"
#include <stdio.h>
#include <string.h>

static struct diag_capture capture;
static ow_device device;
static ow_fwgui_stats before, after;
static bool info, green_released;
static size_t page;

static uint32_t now_ms(void) { return (uint32_t)(time_us_64() / 1000u); }
static uint16_t be16(uint16_t c) { return (uint16_t)((c >> 8) | (c << 8)); }
static void text(int y, const char *value) {
    st7796_draw_text(8, y, 1, be16(0xffff), be16(0x0841), value);
}

static size_t pages(void) {
    if (info) return 2 + (capture.chunks + 9) / 10;
    return capture.length ? (capture.length + 79) / 80 : 1;
}

static void render(void) {
    char line[80];
    st7796_fill_screen(be16(0x0841));
    text(8, "WiliPirate I2C diagnostic v001 - NOT device discovery");
    text(26, diag_summary(&capture));
    snprintf(line, sizeof line, "bytes=%u chunks=%u Poll frames=%u errors=0x%04x",
             (unsigned)capture.length, (unsigned)capture.chunks,
             (unsigned)capture.frames, (unsigned)capture.errors);
    text(44, line);
    snprintf(line, sizeof line, "%s page %u/%u - full bytes retained in RAM",
             info ? "INFO" : "RAW HEX", (unsigned)page + 1, (unsigned)pages());
    text(62, line);
    if (capture.phase == DIAG_READY) {
        text(92, green_released ? "ARMED: press GREEN once to send stock Poll"
                               : "Press and release GREEN to arm; press again to Poll");
        text(114, "Disconnect targets until a physical test is approved.");
        text(136, "Standard startup changes VREF/power. No SAFE/HiZ claim.");
        text(158, "No retry or automatic configuration; one attempt per run.");
    } else if (info && page == 0) {
        snprintf(line, sizeof line, "path: %s", capture.path[0] ? capture.path : "(not established)");
        text(92, line);
        snprintf(line, sizeof line, "MAIN timestamp(hex): %s", capture.timestamp);
        text(110, line);
        snprintf(line, sizeof line, "MAIN sequence(decimal): %s  flag=%d", capture.sequence, capture.firmware_ok);
        text(128, line);
        snprintf(line, sizeof line, "open(ms uptime)=%u send start=%u end=%u",
                 (unsigned)capture.opened_ms, (unsigned)capture.send_started_ms,
                 (unsigned)capture.send_finished_ms);
        text(146, line);
        snprintf(line, sizeof line, "receive start=%u end=%u first=%u last=%u ms",
                 (unsigned)capture.receive_started_ms, (unsigned)capture.receive_finished_ms,
                 (unsigned)capture.first_byte_ms, (unsigned)capture.last_byte_ms);
        text(164, line);
        text(182, "MAIN identity is observed, not a proven request token.");
        text(200, "Byte/chunk times are read-completion times, not wire times.");
        text(218, "No response-body schema or device addresses are decoded.");
    } else if (info && page == 1) {
#define COUNTER(label, member, row) \
        snprintf(line, sizeof line, label " before=%u after=%u delta=%u", \
                 (unsigned)before.member, (unsigned)after.member, \
                 (unsigned)(after.member - before.member)); text(row, line)
        COUNTER("IRQ overruns", ring_overrun_bytes, 92);
        COUNTER("HW overruns", hw_overruns, 110);
        COUNTER("Dropped frames", dropped_frames, 128);
        COUNTER("Bad checksum", checksum_errors, 146);
        COUNTER("Bad length", length_errors, 164);
        COUNTER("Text frames", frames_text, 182);
        COUNTER("Text high water", text_max_fill, 200);
        COUNTER("IRQ high water", ring_max_fill, 218);
#undef COUNTER
    } else if (info) {
        size_t start = (page - 2) * 10;
        for (size_t i = 0; i < 10 && start + i < capture.chunks; ++i) {
            struct diag_chunk chunk = capture.timing[start + i];
            snprintf(line, sizeof line, "chunk %u off=%u len=%u elapsed=%u ms",
                     (unsigned)(start + i), (unsigned)chunk.offset,
                     (unsigned)chunk.length, (unsigned)chunk.elapsed_ms);
            text(92 + (int)i * 16, line);
        }
    } else {
        for (size_t row = 0; row < 10; ++row) {
            size_t offset = page * 80 + row * 8;
            if (offset >= capture.length) break;
            int used = snprintf(line, sizeof line, "%05u:", (unsigned)offset);
            for (size_t col = 0; col < 8 && offset + col < capture.length; ++col)
                used += snprintf(line + used, sizeof line - (size_t)used,
                                 " %02X", capture.bytes[offset + col]);
            text(92 + (int)row * 16, line);
        }
    }
    const char *labels[] = {"PREV", "NEXT", "POLL", "INFO/RAW"};
    const uint16_t colors[] = {0x9ad6, 0x06ff, 0x0012, 0xf800};
    for (int i = 0; i < 4; ++i) {
        st7796_fill_rect(i * 96, 264, 93, 24, colors[i]);
        st7796_draw_text(i * 96 + 6, 271, 1, i < 2 ? 0 : be16(0xffff),
                         colors[i], labels[i]);
    }
    text(300, "HOME 5s: stock recovery | PAGE 5s: About | no second Poll");
}

static void loss_snapshot(void) {
    ow_fwgui_get_stats(&after);
    if (after.ring_overrun_bytes != before.ring_overrun_bytes ||
        after.hw_overruns != before.hw_overruns ||
        after.dropped_frames != before.dropped_frames ||
        after.checksum_errors != before.checksum_errors ||
        after.length_errors != before.length_errors)
        capture.errors |= DIAG_TRANSPORT_LOSS;
}

static bool send_recovery(struct repeating_timer *timer) {
    (void)timer;
    /* Same recovery service pattern as the official OneWili opener. While
     * sending, the main thread does not concurrently service recovery. */
    fw2_app_recovery_task();
    return true;
}

static void run_once(void) {
    if (!diag_claim(&capture)) return;
    page = 0;
    render();
    capture.opened_ms = now_ms();
    if (fw2_app_recovery_open_onewili(&device) != OW_OK) {
        diag_fail(&capture, DIAG_OPEN_ERROR);
        render();
        return;
    }
    ow_fwgui_get_stats(&before);
    /* Reject pre-existing text instead of attributing it to our Poll. No
     * other text consumer runs, and we never retry an ambiguous session. */
    static uint8_t bytes[512]; /* Keep raw_send's command buffer off our caller stack. */
    uint32_t quiet_start = now_ms();
    while ((uint32_t)(now_ms() - quiet_start) < 100u) {
        int n = device.t.read(device.t.ctx, bytes, sizeof bytes, DIAG_READ_MS);
        if (n != 0) {
            if (n > 0 && n <= (int)sizeof bytes) {
                memcpy(capture.bytes, bytes, (size_t)n);
                capture.length = (size_t)n; /* Clearly labelled pending input. */
            }
            diag_fail(&capture, n < 0 ? DIAG_IO_ERROR : DIAG_PENDING_INPUT);
            loss_snapshot();
            render();
            return;
        }
    }
    loss_snapshot();
    if (capture.errors & DIAG_TRANSPORT_LOSS) {
        diag_fail(&capture, DIAG_TRANSPORT_LOSS);
        render();
        return;
    }
    /* Snapshot only after the quiet preflight, immediately before sending. */
    ow_fwgui_get_stats(&before);
    struct repeating_timer recovery_timer;
    if (!add_repeating_timer_ms(-10, send_recovery, NULL, &recovery_timer)) {
        diag_fail(&capture, DIAG_TIMER_ERROR);
        loss_snapshot();
        render();
        return;
    }
    capture.send_started_ms = now_ms();
    int submitted = ow_raw_send(&device, "i\\i\\p"); /* The sole Poll call site. */
    capture.send_finished_ms = now_ms();
    cancel_repeating_timer(&recovery_timer);
    fw2_app_recovery_task();
    if (submitted != 7) { /* reset byte + five command bytes + newline */
        diag_fail(&capture, DIAG_SEND_ERROR);
        loss_snapshot();
        render();
        return;
    }
    diag_receive_begin(&capture, now_ms());
    for (;;) {
        uint32_t remaining = diag_remaining(&capture, now_ms());
        if (!remaining) break;
        uint32_t slice = remaining < DIAG_READ_MS ? remaining : DIAG_READ_MS;
        int n = device.t.read(device.t.ctx, bytes, sizeof bytes, slice);
        if (n < 0 || n > (int)sizeof bytes) {
            capture.errors |= DIAG_IO_ERROR;
            break;
        }
        diag_append(&capture, bytes, (size_t)n, now_ms());
        if (capture.length == DIAG_CAPACITY) capture.errors |= DIAG_TRUNCATED;
        if (capture.errors & (DIAG_TRUNCATED | DIAG_TIMING_TRUNCATED)) break;
    }
    diag_finish(&capture, now_ms());
    loss_snapshot();
    render();
    /* No raw dumping during capture: printing cannot delay receive servicing.
     * Every byte remains available on the paginated LCD; no SD writes or RTT
     * export action requiring a debugger is hidden in the capture path. */
    DIAG("I2C diagnostic: bytes=%u flags=%x frames=%u; no retry\n",
         (unsigned)capture.length, (unsigned)capture.errors, (unsigned)capture.frames);
}

int main(void) {
    diag_init(&capture);
    board_init();
    fw2_app_recovery_init();
    st7796_init();
    fw2_app_about_use_lcd_restore(render);
    render();
    board_backlight_set(1);
    /* Deliberately no release_unused, VREF setter, rail reporting/request,
     * target GPIO calls, I2C settings, CAN or interface initialization. */
    for (;;) {
        fw2_app_recovery_task();
        uartkbd_event_t event;
        while (uartkbd_next_event(&event)) {
            if (event.btn == UARTKBD_BTN_GREEN && !event.pressed &&
                capture.phase == DIAG_READY) {
                green_released = true;
                render();
            } else if (event.pressed) {
                if (event.btn == UARTKBD_BTN_GREEN && green_released) run_once();
                if (event.btn == UARTKBD_BTN_GREY && page) { --page; render(); }
                if (event.btn == UARTKBD_BTN_YELLOW && page + 1 < pages()) { ++page; render(); }
                if (event.btn == UARTKBD_BTN_BLUE) { info = !info; page = 0; render(); }
            }
        }
        fw2_app_recovery_sleep_ms(10);
    }
}
