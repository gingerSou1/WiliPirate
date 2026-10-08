#include "fw2.h"
#include "platform/diag.h"
#include "navigation.h"
#include "ui.h"

static struct wp_navigation navigation = {WP_MENU, false};
static bool touch_available;

/* Redraw the current page after the BSP's modal About screen is dismissed. */
static void restore_ui(void) {
    wp_render(navigation.page, touch_available);
}

int main(void) {
    board_init();
    fw2_app_recovery_init();
    st7796_init();
    touch_available = ft6336_init();
    fw2_app_about_use_lcd_restore(restore_ui);
    wp_render(navigation.page, touch_available);
    board_backlight_set(1);
    /* Standard template power lifecycle; no additional rail requests. */
    picpwr_release_unused();
    DIAG("WiliPirate M2: UI only; touch=%d; HOME 5s exits\n",
         (int)touch_available);

    for (;;) {
        fw2_app_recovery_task();
        uartkbd_event_t event;
        while (uartkbd_next_event(&event)) {
            if (event.pressed && event.btn == UARTKBD_BTN_CANCEL &&
                wp_back(&navigation)) restore_ui();
        }
        uint16_t x = 0, y = 0;
        bool down = touch_available && ft6336_poll(&x, &y);
        if (wp_touch(&navigation, down, x, y)) restore_ui();
        fw2_app_recovery_sleep_ms(10);
    }
}
