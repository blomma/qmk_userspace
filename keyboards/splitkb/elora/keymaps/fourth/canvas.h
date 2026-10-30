#pragma once

#include <stdbool.h>
#include <stdint.h>

// A screen drawn a pixel at a time, then shown on the OLED in one go. Both
// halves draw their screen from scratch on every pass: showing it copies it
// into the OLED buffer, which only marks the bytes that actually changed as
// dirty, so an unchanged screen costs no I2C traffic.

// The display stands on its side, so the screen is as wide as the panel is
// tall. x runs from the left, y from the top.
#define SCREEN_WIDTH OLED_DISPLAY_HEIGHT
#define SCREEN_HEIGHT OLED_DISPLAY_WIDTH

void canvas_clear(void);

// Lights or blanks a pixel. Pixels off the screen are ignored.
void canvas_set_pixel(int16_t x, int16_t y, bool on);

// Copies the screen into the OLED buffer
void canvas_show(void);
