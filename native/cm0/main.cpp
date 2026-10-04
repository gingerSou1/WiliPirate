#include "model.hpp"
#include "wilicm0/device.hpp"
#include <csignal>
#include <cerrno>
#include <ctime>
#include <cstdio>
#include <exception>
#include <stdexcept>
#include <string>
#include <unistd.h>

namespace {
volatile std::sig_atomic_t stopped = 0;
void stop(int) { stopped = 1; }
void checked(ow_status status, const char* operation) {
    if (status != OW_OK) throw std::runtime_error(std::string(operation) + " unavailable (" + std::to_string(status) + ")");
}
void text(ow_device* device, int id, int y, const char* value) {
    const std::string quoted = std::string("\"") + value + "\"";
    checked(ow_gui_controls_add_text(device, id, 20, y, 0, 1,
                                    "white", "black", quoted.c_str()), "panel text");
}
void draw(ow_device* device, const wilipirate::State& state) {
    // Static text fits the 31-byte caption limit. No free-form protocol payload.
    std::string mode = std::string("\"Mode: ") + wilipirate::mode_name(state.mode) + "\"";
    checked(ow_gui_control_properties_set_control_value_text(device, 1, mode.c_str()), "mode text");
    checked(ow_gui_control_properties_set_control_value_text(device, 3,
        state.help ? "\"Modes are logical only.\"" : "\"All bus backends are STUBS.\""), "help text");
}
}

int main(int argc, char**) {
    // No selectable transport, device, fallback, or hardware-operation flags.
    if (argc != 1) { std::fprintf(stderr, "No arguments supported.\n"); return 2; }
    std::signal(SIGINT, stop);
    std::signal(SIGTERM, stop);
    try {
        // Official WiliCM0BSP C++ adapter: socket only, no subprocess/direct access.
        // It performs its documented Device State probe before any UI operation.
        wilicm0::Device bridge;
        auto* device = bridge.get();
        wilipirate::State state;
        bool panel_attempted = false;
        int result = 0;
        try {
            panel_attempted = true; // even a lost create acknowledgement may have changed UI
            checked(ow_gui_panels_add_panel(device, false, 0, "black", true), "panel create");
            const char* labels[] = {"Help", "Mode", "\"\"", "\"\"", "Exit"};
            for (int i = 0; i < 5; ++i)
                checked(ow_gui_panels_set_menu_text(device, i, labels[i]), "menu label");
            text(device, 0, 25, "WiliPirate");
            text(device, 1, 65, "Mode: HiZ");
            text(device, 2, 105, "Hardware: DISABLED");
            text(device, 3, 155, "All bus backends are STUBS.");
            text(device, 4, 195, "Physical pin state unknown.");
            checked(ow_gui_panels_show_panel(device, 0), "panel show");
            std::puts("WiliPirate: HiZ; hardware disabled. Gray Help, Yellow Mode, Red/X Exit.");
            while (!stopped && !state.closed) {
                uint32_t pressed = 0;
                checked(ow_gui_panels_read_buttons(device, &pressed), "button read");
                const auto previous = state;
                wilipirate::buttons(state, pressed);
                if (!state.closed && (state.mode != previous.mode || state.help != previous.help))
                    draw(device, state);
                if (!state.closed && !stopped) {
                    const timespec delay{0, 50000000};
                    if (::nanosleep(&delay, nullptr) != 0 && errno != EINTR)
                        throw std::runtime_error("UI wait failed");
                }
            }
        } catch (const std::exception& error) {
            std::fprintf(stderr, "%s; stopping without retry or fallback.\n", error.what());
            result = 1;
        }
        wilipirate::close(state);
        // One documented display-only cleanup attempt, never a board reset/reinit.
        // Restores default display per docs, not a proven previous-panel snapshot.
        if (panel_attempted && ow_gui_clear_display(device) != OW_OK) {
            std::fprintf(stderr, "Display cleanup unavailable; stock UI restoration unverified.\n");
            result = 1;
        }
        // Official RAII close releases the bridge session with bounded teardown.
        return result;
    } catch (const std::exception& error) {
        std::fprintf(stderr, "Required bridge unavailable: %s. No fallback.\n", error.what());
        return 1;
    }
}
