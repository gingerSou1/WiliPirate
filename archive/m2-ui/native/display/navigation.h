#ifndef WILIPIRATE_NAVIGATION_H
#define WILIPIRATE_NAVIGATION_H

#include <stdbool.h>
#include <stdint.h>

enum wp_page {
    WP_MENU, WP_GPIO, WP_UART, WP_I2C, WP_SPI, WP_CAN, WP_LOGIC, WP_EXIT_HELP
};

struct wp_rect { int x, y, w, h; };
struct wp_navigation { enum wp_page page; bool touch_down; };

enum { WP_WIDTH = 480, WP_HEIGHT = 320, WP_TILE_COUNT = 6 };

static inline struct wp_rect wp_tile_rect(unsigned index) {
    return (struct wp_rect){16 + (int)(index % 2) * 232,
                           76 + (int)(index / 2) * 58, 216, 50};
}

static inline struct wp_rect wp_back_rect(void) {
    return (struct wp_rect){16, 254, 448, 38};
}

static inline bool wp_contains(struct wp_rect rect, unsigned x, unsigned y) {
    return x >= (unsigned)rect.x && y >= (unsigned)rect.y &&
           x < (unsigned)(rect.x + rect.w) && y < (unsigned)(rect.y + rect.h);
}

/* One selection per contact: a held finger never activates a newly drawn page.
 * Release re-arms the next tap. Blank space and out-of-panel samples do nothing. */
static inline bool wp_touch(struct wp_navigation *nav, bool down,
                            unsigned x, unsigned y) {
    bool rising = down && !nav->touch_down;
    nav->touch_down = down;
    if (!rising || x >= WP_WIDTH || y >= WP_HEIGHT) return false;
    enum wp_page next = nav->page;
    if (wp_contains(wp_back_rect(), x, y)) {
        next = nav->page == WP_MENU ? WP_EXIT_HELP : WP_MENU;
    } else if (nav->page == WP_MENU) {
        for (unsigned i = 0; i < WP_TILE_COUNT; ++i) {
            if (wp_contains(wp_tile_rect(i), x, y)) {
                next = (enum wp_page)(WP_GPIO + i);
                break;
            }
        }
    }
    if (next == nav->page) return false;
    nav->page = next;
    return true;
}

/* CANCEL is an additional physical Back action, never an interface operation. */
static inline bool wp_back(struct wp_navigation *nav) {
    if (nav->page == WP_MENU) return false;
    nav->page = WP_MENU;
    return true;
}

static inline const char *wp_page_name(enum wp_page page) {
    switch (page) {
        case WP_GPIO: return "GPIO";
        case WP_UART: return "UART";
        case WP_I2C: return "I2C";
        case WP_SPI: return "SPI";
        case WP_CAN: return "CAN";
        case WP_LOGIC: return "LOGIC";
        case WP_EXIT_HELP: return "Exit WiliPirate";
        default: return "WiliPirate";
    }
}
#endif
