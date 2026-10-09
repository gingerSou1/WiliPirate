/* Offline draw sink: executes the production renderer with the BSP font.
 * This validates layout bounds and creates previews; it emulates no device. */
#include "ui.h"
#include "display/st7796.h"
#include "display/font5x7.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static unsigned char pixels[WP_HEIGHT][WP_WIDTH][3];
static unsigned clear_count;
static unsigned placeholder_count;

static void pixel(int x, int y, uint16_t wire) {
    uint16_t color = (uint16_t)((wire >> 8) | (wire << 8));
    pixels[y][x][0] = (unsigned char)(((color >> 11) & 31) * 255 / 31);
    pixels[y][x][1] = (unsigned char)(((color >> 5) & 63) * 255 / 63);
    pixels[y][x][2] = (unsigned char)((color & 31) * 255 / 31);
}

void st7796_fill_rect(int x, int y, int w, int h, uint16_t color) {
    assert(x >= 0 && y >= 0 && w > 0 && h > 0);
    assert(x + w <= WP_WIDTH && y + h <= WP_HEIGHT);
    for (int row = y; row < y + h; ++row)
        for (int col = x; col < x + w; ++col) pixel(col, row, color);
}

void st7796_fill_screen(uint16_t color) {
    ++clear_count;
    st7796_fill_rect(0, 0, WP_WIDTH, WP_HEIGHT, color);
}

void st7796_draw_text(int x, int y, int scale, uint16_t fg, uint16_t bg,
                      const char *text) {
    assert(x >= 0 && y >= 0 && scale >= 1 && scale <= 4);
    assert(x + (int)strlen(text) * 6 * scale <= WP_WIDTH);
    assert(y + 8 * scale <= WP_HEIGHT);
    if (strcmp(text, "Not implemented - M2 UI prototype") == 0)
        ++placeholder_count;
    for (; *text; ++text, x += 6 * scale) {
        assert(*text >= FONT5X7_FIRST && *text <= FONT5X7_LAST);
        const uint8_t *glyph = font5x7[(unsigned char)*text - FONT5X7_FIRST];
        for (int gy = 0; gy < 8 * scale; ++gy) {
            for (int gx = 0; gx < 6 * scale; ++gx) {
                int col = gx / scale, row = gy / scale;
                bool on = col < 5 && row < 7 && ((glyph[col] >> row) & 1);
                pixel(x + gx, y + gy, on ? fg : bg);
            }
        }
    }
}

static void preview(const char *path) {
    FILE *file = fopen(path, "wb");
    assert(file);
    fprintf(file, "P6\n%d %d\n255\n", WP_WIDTH, WP_HEIGHT);
    assert(fwrite(pixels, sizeof pixels, 1, file) == 1);
    assert(fclose(file) == 0);
}

int main(int argc, char **argv) {
    for (int page = WP_MENU; page <= WP_EXIT_HELP; ++page) {
        unsigned before = clear_count;
        wp_render((enum wp_page)page, true);
        assert(clear_count == before + 1);
        if (argc == 2) {
            char path[512];
            int n = snprintf(path, sizeof path, "%s/page-%d.ppm", argv[1], page);
            assert(n > 0 && (size_t)n < sizeof path);
            preview(path);
        }
        wp_render((enum wp_page)page, false); /* missing-touch error footer */
    }
    assert(placeholder_count == WP_TILE_COUNT * 2);
    puts("render: all pages/fallbacks fit, complete clears and six placeholders OK");
    return 0;
}
