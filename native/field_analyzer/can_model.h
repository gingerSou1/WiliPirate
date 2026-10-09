/* SPDX-License-Identifier: MIT; Copyright (c) 2026 gingerSou1 */
#ifndef WILIPIRATE_CAN_MODEL_H
#define WILIPIRATE_CAN_MODEL_H
#include <stdbool.h>
#include <stdint.h>
enum { FA_CAN_CAPACITY=8 };
/* Normalized fields from documented receive_canfd, not a wire struct.
 * timestamp_us is MAIN drain-time uptime, not bus arrival time. */
struct fa_can_frame {
 uint32_t id, timestamp_us; uint8_t length, data[64]; bool extended, fd;
};
struct fa_can_session {
 struct fa_can_frame frames[FA_CAN_CAPACITY], capture[FA_CAN_CAPACITY];
 unsigned count, captured, next_demo;
 uint32_t received, overwritten, rejected, upstream_dropped;
};
void fa_can_init(struct fa_can_session *s);
bool fa_can_accept(struct fa_can_session *s,const struct fa_can_frame *f);
/* Decode only the documented Receive CAN(FD) result body, never framing,
 * command responses with unknown prefixes, or arbitrary text events. */
bool fa_can_decode_queue(struct fa_can_session *s,const char *body);
void fa_can_demo(struct fa_can_session *s);
void fa_can_capture(struct fa_can_session *s);
/* No runtime flag or configuration can enable a live backend. */
bool fa_can_live_available(void);
#endif
