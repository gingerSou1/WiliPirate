/* SPDX-License-Identifier: MIT; Copyright (c) 2026 gingerSou1 */
#include "../capture.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static struct diag_capture c;
static void start(uint32_t now) {
    diag_init(&c);
    assert(diag_claim(&c));
    assert(!diag_claim(&c));
    diag_receive_begin(&c, now);
}
static void add(const char *s, uint32_t now) {
    diag_append(&c, (const uint8_t *)s, strlen(s), now);
}

int main(void) {
    start(100);
    assert(diag_remaining(&c, 100) == 5000);
    assert(diag_remaining(&c, 5099) == 1);
    assert(diag_remaining(&c, 5100) == 0);
    add("[i\\i\\", 101);
    add("p ABCD 42 unknown payload 1]\r\n", 103);
    diag_finish(&c, 5100);
    assert(!c.errors && c.frames == 1 && c.firmware_ok == 1);
    assert(!strcmp(c.timestamp, "ABCD") && !strcmp(c.sequence, "42"));
    assert(c.chunks == 2 && c.timing[1].elapsed_ms == 3);
    assert(!diag_claim(&c));
    size_t frozen = c.length;
    add("ignored", 5101);
    assert(c.length == frozen);

    start(UINT32_MAX - 10);
    assert(diag_remaining(&c, 9) == 4980); /* uptime wrap */
    diag_finish(&c, 4990);
    assert(c.errors & DIAG_TIMEOUT);
    assert(!diag_claim(&c));

    start(0);
    add("[*some_event FF]\n[i\\i\\p 00 1 payload\nwith a bare ]\n 1]\n", 7);
    diag_finish(&c, 5000);
    assert(!c.errors && c.frames == 1);

    start(0);
    add("[i\\i\\p 1 2 1]\n", 1); /* empty body, no invented address list */
    diag_finish(&c, 5000);
    assert(!c.errors && c.frames == 1);

    start(0);
    add("[i\\i\\p 1 2 EPOWERZONE 6 FPGA 0]\n", 1);
    diag_finish(&c, 5000);
    assert(c.errors == DIAG_FIRMWARE_FAILED && c.firmware_ok == 0);

    start(0);
    add("[i\\g\\u 1 2 FFFF 1]\n", 1);
    diag_finish(&c, 5000);
    assert((c.errors & DIAG_WRONG_PATH) && c.frames == 0);

    start(0);
    add("[i\\i\\p GG 2 ignored 1]\n", 1);
    diag_finish(&c, 5000);
    assert(c.errors & DIAG_MALFORMED);

    start(0);
    add("[i\\i\\p 1 nope ignored 1]\n", 1);
    diag_finish(&c, 5000);
    assert(c.errors & DIAG_MALFORMED);

    start(0);
    add("[i\\i\\p 1 1 body 1]\n[i\\i\\p 2 2 second 1]\n", 1);
    diag_finish(&c, 5000);
    assert(c.errors & DIAG_MULTIPLE);

    start(0);
    add("[i\\i\\p 1 1 unfinished", 1);
    diag_finish(&c, 5000);
    assert(c.errors & DIAG_INCOMPLETE);

    start(0);
    add("[i\\i\\p 1 1 ", 1);
    static uint8_t body[8192];
    memset(body, 'Z', sizeof body);
    diag_append(&c, body, sizeof body, 2);
    add(" 1]\n", 3);
    diag_finish(&c, 5000);
    assert(!c.errors && c.length > 4096 && c.frames == 1);
    assert(!memcmp(c.bytes + 11, body, sizeof body));

    start(0);
    static uint8_t too_large[DIAG_CAPACITY + 1];
    memset(too_large, 0, sizeof too_large);
    diag_append(&c, too_large, sizeof too_large, 2);
    assert(c.length == DIAG_CAPACITY && (c.errors & DIAG_TRUNCATED));
    diag_finish(&c, 5000);

    start(0);
    for (size_t i = 0; i <= DIAG_CHUNKS; ++i) add("X", (uint32_t)i);
    assert(c.errors & DIAG_TIMING_TRUNCATED);
    assert(c.chunks == DIAG_CHUNKS);
    diag_finish(&c, 5000);

    diag_init(&c);
    assert(diag_claim(&c));
    diag_fail(&c, DIAG_SEND_ERROR);
    assert(!diag_claim(&c));
    assert(strstr(diag_summary(&c), "SEND ERROR"));
    puts("PASS: 14 capture cases (synthetic frames; no hardware)");
    return 0;
}
