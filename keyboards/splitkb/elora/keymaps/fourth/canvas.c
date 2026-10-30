#include QMK_KEYBOARD_H
#include <string.h>
#include "canvas.h"

_Static_assert((SCREEN_WIDTH * SCREEN_HEIGHT) / 8 == OLED_MATRIX_SIZE,
               "the screen covers the whole OLED buffer");

// The screen in the layout of the OLED buffer
static uint8_t screen[OLED_MATRIX_SIZE];

void canvas_clear(void) {
    memset(screen, 0, sizeof(screen));
}

void canvas_set_pixel(int16_t x, int16_t y, bool on) {
    if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) {
        return;
    }
    uint8_t *byte = &screen[x + (y / 8) * SCREEN_WIDTH];
    if (on) {
        *byte |= 1 << (y % 8);
    } else {
        *byte &= ~(1 << (y % 8));
    }
}

void canvas_show(void) {
    oled_set_cursor(0, 0);
    oled_write_raw((const char *)screen, sizeof(screen));
}
