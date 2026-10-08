#include "navigation.h"
#include <assert.h>
#include <stdio.h>

static bool tap(struct wp_navigation *nav, unsigned x, unsigned y) {
    (void)wp_touch(nav, false, 0, 0);
    return wp_touch(nav, true, x, y);
}

int main(void) {
    struct wp_navigation nav = {WP_MENU, false};
    /* Every selectable interface and its Back route. Held contact must never
     * leak into a new page or trigger another control as the finger moves. */
    for (unsigned i = 0; i < WP_TILE_COUNT; ++i) {
        struct wp_rect rect = wp_tile_rect(i);
        assert(tap(&nav, (unsigned)(rect.x + rect.w / 2),
                   (unsigned)(rect.y + rect.h / 2)));
        assert(nav.page == (enum wp_page)(WP_GPIO + i));
        assert(!wp_touch(&nav, true, 240, 270));
        assert(nav.page == (enum wp_page)(WP_GPIO + i));
        assert(tap(&nav, 240, 270));
        assert(nav.page == WP_MENU);
    }
    assert(!tap(&nav, 0, 0));
    assert(!tap(&nav, 480, 100));
    assert(!tap(&nav, 100, 320));
    assert(!tap(&nav, 240, 100)); /* grid gap */
    assert(!tap(&nav, 32, 127));  /* row gap */

    /* Inclusive top/left and exclusive bottom/right limits prevent overlap. */
    assert(tap(&nav, 16, 76));
    assert(nav.page == WP_GPIO);
    assert(wp_back(&nav));
    assert(!wp_back(&nav));
    assert(!tap(&nav, 232, 76));
    assert(!tap(&nav, 16, 126));
    assert(tap(&nav, 231, 125));
    assert(nav.page == WP_GPIO);
    assert(wp_back(&nav));
    assert(tap(&nav, 240, 270));
    assert(nav.page == WP_EXIT_HELP);
    assert(!wp_touch(&nav, true, 240, 270));
    assert(tap(&nav, 240, 270));
    assert(nav.page == WP_MENU);
    puts("navigation: all six pages, Back, exit help, contact latch and bounds OK");
    return 0;
}
