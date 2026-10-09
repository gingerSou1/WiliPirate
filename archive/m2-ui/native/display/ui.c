#include "ui.h"
#include "display/st7796.h"
#include <string.h>

_Static_assert(ST7796_W == WP_WIDTH && ST7796_H == WP_HEIGHT,
               "The M2 layout targets the official 480x320 DISPLAY");

#define BE16(c) ((uint16_t)(((c) >> 8) | ((c) << 8)))
#define BG BE16(0x0841)
#define FG BE16(0xFFFF)
#define MUTED BE16(0xBDF7)
#define TILE BE16(0x1949)
#define ACCENT BE16(0x07FF)

static void centered(int y, int scale, uint16_t color, const char *text) {
    int width = (int)strlen(text) * 6 * scale;
    st7796_draw_text((WP_WIDTH - width) / 2, y, scale, color, BG, text);
}

static void button(struct wp_rect r, const char *label) {
    st7796_fill_rect(r.x, r.y, r.w, r.h, ACCENT);
    st7796_fill_rect(r.x + 2, r.y + 2, r.w - 4, r.h - 4, TILE);
    int width = (int)strlen(label) * 12;
    st7796_draw_text(r.x + (r.w - width) / 2, r.y + (r.h - 16) / 2,
                     2, FG, TILE, label);
}

void wp_render(enum wp_page page, bool touch_available) {
    /* A complete clear also removes inherited stock pixels on first launch. */
    st7796_fill_screen(BG);
    centered(16, 3, FG, wp_page_name(page));
    if (page == WP_MENU) {
        centered(46, 2, MUTED, "Hardware Interface Tool");
        for (unsigned i = 0; i < WP_TILE_COUNT; ++i)
            button(wp_tile_rect(i), wp_page_name((enum wp_page)(WP_GPIO + i)));
        button(wp_back_rect(), "EXIT / HELP");
    } else if (page == WP_EXIT_HELP) {
        centered(104, 2, FG, "Hold physical HOME for 5 seconds");
        centered(144, 1, MUTED, "DISPLAY reboots to the stock application.");
        centered(174, 1, MUTED, "M2 provides navigation only.");
        button(wp_back_rect(), "BACK");
    } else {
        centered(106, 2, FG, "Not implemented - M2 UI prototype");
        centered(144, 1, MUTED, "No interface operations available.");
        button(wp_back_rect(), "BACK");
    }
    centered(302, 1, MUTED, touch_available
        ? "Hold HOME 5s: exit | Hold PAGE 5s: About"
        : "Touch unavailable | Hold HOME 5s to exit");
}
