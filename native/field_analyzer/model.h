/* SPDX-License-Identifier: MIT; Copyright (c) 2026 gingerSou1 */
#ifndef WILIPIRATE_FIELD_MODEL_H
#define WILIPIRATE_FIELD_MODEL_H
#include <stdbool.h>
#include <stdint.h>
#include "can_model.h"
enum fa_page { FA_HOME, FA_PROTOCOLS, FA_GUIDE, FA_PREVIEW, FA_TOOLS, FA_CAPTURES, FA_SETTINGS, FA_GLITCH, FA_GLITCH_GUIDE, FA_CAN_CONFIG, FA_CAN_LISTEN, FA_CAN_MONITOR, FA_CAN_REVIEW };
enum fa_instrument { FA_SCOPE, FA_LOGIC, FA_PROTOCOL, FA_GENERATOR, FA_CAN, FA_UART, FA_I2C, FA_SPI, FA_COUNT };
struct fa_rect { int x, y, w, h; };
struct fa_state { enum fa_page page; enum fa_instrument instrument; bool down; struct fa_can_session can; unsigned review; enum fa_page review_return; };
struct fa_guide { const char *connector, *signals, *ground, *limits, *accessory, *stock, *desktop, *precaution; };
enum { FA_WIDTH=480, FA_HEIGHT=320 };
void fa_init(struct fa_state *s);
bool fa_touch(struct fa_state *s, bool down, unsigned x, unsigned y);
bool fa_back(struct fa_state *s);
bool fa_home(struct fa_state *s);
struct fa_rect fa_tile(unsigned i);
const char *fa_name(enum fa_instrument i);
const struct fa_guide *fa_connection(enum fa_instrument i);
#endif
