/* SPDX-License-Identifier: MIT; Copyright (c) 2026 gingerSou1 */
#ifndef WILIPIRATE_I2C_CAPTURE_H
#define WILIPIRATE_I2C_CAPTURE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

enum { DIAG_CAPACITY = 32768, DIAG_CHUNKS = 1024,
       DIAG_RECEIVE_MS = 5000, DIAG_READ_MS = 10 };
enum diag_phase { DIAG_READY, DIAG_PREPARING, DIAG_RECEIVING, DIAG_FINISHED };
enum diag_error {
    DIAG_OPEN_ERROR = 1u << 0, DIAG_PENDING_INPUT = 1u << 1,
    DIAG_SEND_ERROR = 1u << 2, DIAG_IO_ERROR = 1u << 3,
    DIAG_TRUNCATED = 1u << 4, DIAG_TIMING_TRUNCATED = 1u << 5,
    DIAG_TIMEOUT = 1u << 6, DIAG_INCOMPLETE = 1u << 7,
    DIAG_WRONG_PATH = 1u << 8, DIAG_MALFORMED = 1u << 9,
    DIAG_MULTIPLE = 1u << 10, DIAG_TRANSPORT_LOSS = 1u << 11,
    DIAG_FIRMWARE_FAILED = 1u << 12, DIAG_TIMER_ERROR = 1u << 13
};
struct diag_chunk { uint32_t offset, length, elapsed_ms; };
struct diag_capture {
    enum diag_phase phase;
    uint32_t errors, opened_ms, send_started_ms, send_finished_ms;
    uint32_t receive_started_ms, receive_finished_ms;
    uint32_t first_byte_ms, last_byte_ms;
    size_t length, chunks, frames;
    char path[64], timestamp[32], sequence[32];
    int firmware_ok;
    uint8_t bytes[DIAG_CAPACITY];
    struct diag_chunk timing[DIAG_CHUNKS];
};

void diag_init(struct diag_capture *capture);
bool diag_claim(struct diag_capture *capture);
void diag_fail(struct diag_capture *capture, uint32_t error);
void diag_receive_begin(struct diag_capture *capture, uint32_t now_ms);
uint32_t diag_remaining(const struct diag_capture *capture, uint32_t now_ms);
void diag_append(struct diag_capture *capture, const uint8_t *bytes,
                 size_t length, uint32_t now_ms);
void diag_finish(struct diag_capture *capture, uint32_t now_ms);
const char *diag_summary(const struct diag_capture *capture);

#endif
