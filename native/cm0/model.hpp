#pragma once
// Pure logical model; no driver, transport or peripheral dependencies.
namespace wilipirate {
enum class Mode { HiZ, UART, I2C, SPI, GPIO };
struct State { Mode mode = Mode::HiZ; bool closed = false; bool help = false; };
constexpr const char* mode_name(Mode mode) {
    switch (mode) {
    case Mode::HiZ: return "HiZ";
    case Mode::UART: return "UART";
    case Mode::I2C: return "I2C";
    case Mode::SPI: return "SPI";
    case Mode::GPIO: return "GPIO";
    }
    return "UNAVAILABLE";
}
constexpr bool select(State& state, Mode mode) {
    if (state.closed || static_cast<int>(mode) < 0 || static_cast<int>(mode) > 4) return false;
    state.mode = mode;
    state.help = false;
    return true;
}
constexpr bool backend_available(Mode) { return false; }
constexpr const char* backend_name(Mode) { return "STUB"; }
constexpr void close(State& state) { state.mode = Mode::HiZ; state.closed = true; }
constexpr void buttons(State& state, unsigned mask) {
    if (state.closed || mask >= (1u << 14)) return;
    // Gray=Help, Yellow=Mode, Red or X=Exit. Cancel wins combined presses.
    if (mask & ((1u << 4) | (1u << 11))) { close(state); return; }
    if (!mask || (mask & (mask - 1u))) return;
    if (mask == 1u) state.help = !state.help;
    if (mask == 2u) select(state, static_cast<Mode>((static_cast<int>(state.mode) + 1) % 5));
}
}
