// SPDX-License-Identifier: MIT; Copyright (c) 2026 gingerSou1
// Offline-prepared bench source. Deployment/operation requires separate approval.
#include <Arduino.h>
#include <Arduino_CAN.h>

static_assert(PIN_CAN0_TX == 4 && PIN_CAN0_RX == 5,
              "This sketch is specifically for UNO R4 Minima D4/D5");
constexpr uint32_t FRAME_ID = 0x321;
constexpr uint32_t PERIOD_MS = 100;
constexpr uint32_t MAX_ATTEMPTS = 100; // bounded 10-second run; no automatic restart
static bool running = false;
static uint32_t counter = 0, lastSend = 0;

static void stopRun(const char *reason) {
  if (running) CAN.end();
  running = false;
  Serial.println(reason);
}

void setup() {
  Serial.begin(115200); // never wait for a host; CAN stays closed until 's'
  Serial.println("Minima CAN counter: s=start, x=stop; 500 kbps, ID 0x321");
  Serial.println("Requires level-shifted Waveshare SN65HVD230 and an ACK node.");
}

void loop() {
  // Bound command work so serial traffic cannot starve the CAN/error service.
  for (unsigned i = 0; i < 8 && Serial.available(); ++i) {
    const int command = Serial.read();
    if (command == 'x') stopRun("Stopped manually");
    if (command == 's' && !running) {
      if (!CAN.begin(CanBitRate::BR_500k)) {
        CAN.end(); // release any partially initialized controller
        Serial.println("STOP: CAN initialization failed; no automatic retry");
      } else {
        CAN.clearError();
        counter = 0;
        lastSend = millis();
        running = true;
        Serial.println("Started bounded run; queued does not mean acknowledged");
      }
    }
  }
  if (!running) return;
  int error = 0;
  if (CAN.isError(error)) {
    stopRun("STOP: asynchronous CAN error");
    Serial.print("Driver event: "); Serial.println(error);
    return;
  }
  const uint32_t now = millis();
  if (static_cast<uint32_t>(now - lastSend) < PERIOD_MS) return;
  lastSend = now; // no catch-up bursts after a delayed loop
  uint8_t data[8];
  for (unsigned i = 0; i < 4; ++i) {
    data[i] = static_cast<uint8_t>(counter >> (8 * i));
    data[i + 4] = static_cast<uint8_t>(now >> (8 * i));
  }
  const CanMsg frame(CanStandardId(FRAME_ID), sizeof data, data);
  const int result = CAN.write(frame);
  if (result < 0) {
    stopRun("STOP: CAN write refused; no automatic retry");
    Serial.print("Driver result: "); Serial.println(result);
    return;
  }
  Serial.print("Queued counter "); Serial.println(counter);
  if (++counter >= MAX_ATTEMPTS) stopRun("Finished bounded run");
}
