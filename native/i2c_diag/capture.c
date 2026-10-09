/* SPDX-License-Identifier: MIT; Copyright (c) 2026 gingerSou1 */
#include "capture.h"
#include <string.h>

void diag_init(struct diag_capture *c) {
    memset(c, 0, sizeof *c);
    c->firmware_ok = -1;
}

bool diag_claim(struct diag_capture *c) {
    if (c->phase != DIAG_READY) return false;
    c->phase = DIAG_PREPARING; /* Consume the attempt before opening or sending. */
    return true;
}

void diag_fail(struct diag_capture *c, uint32_t error) {
    c->errors |= error;
    c->phase = DIAG_FINISHED;
}

void diag_receive_begin(struct diag_capture *c, uint32_t now_ms) {
    if (c->phase != DIAG_PREPARING) return;
    c->receive_started_ms = now_ms;
    c->phase = DIAG_RECEIVING;
}

uint32_t diag_remaining(const struct diag_capture *c, uint32_t now_ms) {
    if (c->phase != DIAG_RECEIVING) return 0;
    uint32_t elapsed = now_ms - c->receive_started_ms;
    return elapsed >= DIAG_RECEIVE_MS ? 0 : DIAG_RECEIVE_MS - elapsed;
}

void diag_append(struct diag_capture *c, const uint8_t *bytes, size_t n,
                 uint32_t now_ms) {
    if (c->phase != DIAG_RECEIVING || n == 0) return;
    /* The read was started within the budget. Retain returned bytes even if
     * that final short read reached the deadline; no subsequent read is made. */
    size_t available = DIAG_CAPACITY - c->length;
    size_t saved = n < available ? n : available;
    if (saved != n) c->errors |= DIAG_TRUNCATED;
    if (saved) {
        uint32_t elapsed = now_ms - c->receive_started_ms;
        if (c->length == 0) c->first_byte_ms = elapsed;
        c->last_byte_ms = elapsed;
        if (c->chunks < DIAG_CHUNKS) {
            c->timing[c->chunks++] = (struct diag_chunk){
                (uint32_t)c->length, (uint32_t)saved, elapsed};
        } else {
            c->errors |= DIAG_TIMING_TRUNCATED;
        }
        memcpy(c->bytes + c->length, bytes, saved);
        c->length += saved;
    }
}

static bool field(const uint8_t *b, size_t end, size_t *pos,
                  char *out, size_t capacity, unsigned base) {
    size_t start = *pos;
    while (*pos < end && b[*pos] != ' ') {
        uint8_t ch = b[*pos];
        bool digit = ch >= '0' && ch <= '9';
        bool hex = (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F');
        if (ch < 33 || ch > 126 || (base == 10 && !digit) ||
            (base == 16 && !digit && !hex)) return false;
        ++*pos;
    }
    size_t n = *pos - start;
    if (n == 0 || n >= capacity || *pos >= end) return false;
    memcpy(out, b + start, n);
    out[n] = 0;
    ++*pos;
    return true;
}

static void envelope(struct diag_capture *c, size_t start, size_t end) {
    char path[64], timestamp[32], sequence[32];
    size_t pos = start + 1;
    if (!field(c->bytes, end, &pos, path, sizeof path, 0) ||
        !field(c->bytes, end, &pos, timestamp, sizeof timestamp, 16) ||
        !field(c->bytes, end, &pos, sequence, sizeof sequence, 10) ||
        pos > end - 1) {
        c->errors |= DIAG_MALFORMED;
        return;
    }
    if (strcmp(path, "i\\i\\p") != 0) {
        c->errors |= DIAG_WRONG_PATH;
        return;
    }
    if (++c->frames != 1) c->errors |= DIAG_MULTIPLE;
    if (c->frames == 1) {
        memcpy(c->path, path, strlen(path) + 1);
        memcpy(c->timestamp, timestamp, strlen(timestamp) + 1);
        memcpy(c->sequence, sequence, strlen(sequence) + 1);
        c->firmware_ok = c->bytes[end - 1] == '1' ? 1 : 0;
    }
    if (c->bytes[end - 1] == '0') c->errors |= DIAG_FIRMWARE_FAILED;
}

void diag_finish(struct diag_capture *c, uint32_t now_ms) {
    if (c->phase != DIAG_RECEIVING) return;
    c->receive_finished_ms = now_ms;
    /* Recognize only the documented envelope. Body bytes are never decoded.
     * Payload newlines are retained until the complete trailing flag arrives. */
    size_t frame = SIZE_MAX, line = 0;
    for (size_t i = 0; i <= c->length; ++i) {
        if (i != c->length && c->bytes[i] != '\n') continue;
        size_t end = i;
        if (end > line && c->bytes[end - 1] == '\r') --end;
        if (end > line + 1 && c->bytes[line] == '[' && c->bytes[line + 1] != '*') {
            if (frame != SIZE_MAX) c->errors |= DIAG_MALFORMED;
            frame = line;
        }
        if (frame != SIZE_MAX && end >= frame + 3 && c->bytes[end - 1] == ']' &&
            (c->bytes[end - 2] == '0' || c->bytes[end - 2] == '1') &&
            (c->bytes[end - 3] == ' ' || c->bytes[end - 3] == '\t' ||
             c->bytes[end - 3] == '\n')) {
            envelope(c, frame, end - 1);
            frame = SIZE_MAX;
        }
        line = i + 1;
    }
    if (frame != SIZE_MAX) c->errors |= DIAG_INCOMPLETE;
    if (c->frames == 0) c->errors |= c->length ? DIAG_INCOMPLETE : DIAG_TIMEOUT;
    c->phase = DIAG_FINISHED;
}

const char *diag_summary(const struct diag_capture *c) {
    if (c->phase == DIAG_READY) return "READY: GREEN sends one Poll";
    if (c->phase == DIAG_PREPARING) return "Opening / checking pending input";
    if (c->phase == DIAG_RECEIVING) return "Capturing raw text for 5 seconds";
    if (c->errors & DIAG_OPEN_ERROR) return "OPEN FAILED - Poll not sent";
    if (c->errors & DIAG_PENDING_INPUT) return "PENDING INPUT - Poll not sent";
    if (c->errors & DIAG_TIMER_ERROR) return "RECOVERY TIMER FAILED - not sent";
    if (c->errors & DIAG_SEND_ERROR) return "SEND ERROR - no retry";
    if (c->errors & DIAG_IO_ERROR) return "RECEIVE I/O ERROR";
    if (c->errors & (DIAG_TRUNCATED | DIAG_TIMING_TRUNCATED)) return "TRUNCATED / INCOMPLETE EVIDENCE";
    if (c->errors & DIAG_TRANSPORT_LOSS) return "TRANSPORT LOSS - inconclusive";
    if (c->errors & DIAG_TIMEOUT) return "TIMEOUT - no response captured";
    if (c->errors & (DIAG_WRONG_PATH | DIAG_MALFORMED | DIAG_MULTIPLE | DIAG_INCOMPLETE))
        return "UNQUALIFIED / INCOMPLETE FRAME";
    if (c->errors & DIAG_FIRMWARE_FAILED) return "COMPLETE FRAME: firmware failed";
    return "COMPLETE FRAME - schema unknown";
}
