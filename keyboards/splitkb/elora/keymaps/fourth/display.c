#include QMK_KEYBOARD_H
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "cross_platform.h"
#include "display.h"
#include "frames.h"
#include "big_font.h"
#include "layers.h"

// The 6x8 font the OLED driver writes text with. The driver keeps its copy to
// itself, so the layer screen includes its own to draw text at any pixel.
#include OLED_FONT_H

// The half plugged into USB shows a plant that grows as you type, the other
// half shows the layer screen.

// Frames are written straight into the OLED buffer
_Static_assert(sizeof(plant_anim[0]) == OLED_MATRIX_SIZE, "frame size");

// The display stands on its side, so the screen is as wide as the panel is
// tall
#define SCREEN_WIDTH OLED_DISPLAY_HEIGHT
#define SCREEN_HEIGHT OLED_DISPLAY_WIDTH
_Static_assert((SCREEN_WIDTH * SCREEN_HEIGHT) / 8 == OLED_MATRIX_SIZE,
               "the screen covers the whole OLED buffer");

// The layer name is stacked a letter per row in a column down the left side,
// centred on STACK_X. Every name starts STACK_MARGIN pixels from the top and
// has to end at least as far from the bottom to use the big font. This is the
// same margin the text in the bottom right corner keeps to the edges.
#define STACK_X 13
#define STACK_MARGIN 3

// Text in the 6x8 font sits in the bottom right corner, the RGB readout
// centred down the right side. TEXT_MARGIN is the space to the edges: the
// font leaves its own last column and row blank, which adds one more pixel.
#define TEXT_MARGIN 2
#define RGB_LINES 3
#define RGB_CHARS 6
#define RGB_LINE_PITCH 12
#define RGB_X (SCREEN_WIDTH - TEXT_MARGIN - RGB_CHARS * OLED_FONT_WIDTH)
#define RGB_TOP \
    ((SCREEN_HEIGHT - (RGB_LINES - 1) * RGB_LINE_PITCH - OLED_FONT_HEIGHT) / 2)
#define HOST_TOP (SCREEN_HEIGHT - TEXT_MARGIN - OLED_FONT_HEIGHT)
_Static_assert(STACK_X + (BIG_FONT_MAX_WIDTH + 1) / 2 < RGB_X,
               "the layer name overlaps the RGB readout");
_Static_assert(RGB_TOP + (RGB_LINES - 1) * RGB_LINE_PITCH + OLED_FONT_HEIGHT <=
                   HOST_TOP,
               "the RGB readout overlaps the host");

static const char *boot = "Boot";

static uint8_t current_frame = 0;

void display_count_keypress(void) {
    static uint8_t keypresses = 0;

    if (++keypresses == GROW_RATE) {
        keypresses    = 0;
        current_frame = (current_frame + 1) % ANIM_FRAMES;
    }
}

// Writes str centred on the given line and blanks the rest of the line
static void write_centered(uint8_t line, const char *str) {
    const uint8_t width = oled_max_chars();
    const size_t  len   = strlen(str);

    oled_set_cursor(0, line);
    for (uint8_t i = 0; len < width && i < (width - len) / 2; i++) {
        oled_write_char(' ', false);
    }
    oled_write_ln(str, false);
}

// The layer screen in the layout of the OLED buffer
static uint8_t layer_screen[OLED_MATRIX_SIZE];

static void set_pixel(int16_t x, int16_t y) {
    if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) {
        return;
    }
    layer_screen[x + (y / 8) * SCREEN_WIDTH] |= 1 << (y % 8);
}

// Lights a column of scale by scale blocks, bit n of bits at block row n
static void draw_column(int16_t x, int16_t top, uint32_t bits, uint8_t scale) {
    for (int16_t y = top; bits != 0; y += scale, bits >>= 1) {
        if (bits & 1) {
            for (uint8_t i = 0; i < scale * scale; i++) {
                set_pixel(x + i % scale, y + i / scale);
            }
        }
    }
}

// The 6x8 font glyph for c, NULL for a character the font doesn't have
static const uint8_t *small_glyph(char c) {
    const uint8_t code = (uint8_t)c;
    if (code < OLED_FONT_START || code > OLED_FONT_END) {
        return NULL;
    }
    return &font[(code - OLED_FONT_START) * OLED_FONT_WIDTH];
}

// Draws str in the 6x8 font with its top left corner at (x, top). Like the
// OLED driver, characters the font doesn't have are left blank.
static void draw_text(int16_t x, int16_t top, const char *str) {
    for (const char *c = str; *c != '\0'; c++, x += OLED_FONT_WIDTH) {
        const uint8_t *glyph = small_glyph(*c);
        for (uint8_t i = 0; glyph != NULL && i < OLED_FONT_WIDTH; i++) {
            draw_column(x + i, top, pgm_read_byte(&glyph[i]), 1);
        }
    }
}

// A font the layer name can be stacked in. Its letters are capitals of one
// height, each centred on the stack on its own, so a glyph's width is that of
// its ink.
typedef struct {
    uint8_t height;
    // Blank rows between two letters
    uint8_t gap;
    uint8_t (*width)(char c);
    void (*draw)(char c, int16_t left, int16_t top);
} stack_font_t;

// Retron2000, the font on the Halcyon displays. Characters it doesn't have
// show as '?'.
static uint8_t big_glyph(char c) {
    c = toupper((unsigned char)c);
    if (c < BIG_FONT_FIRST || c > BIG_FONT_LAST) {
        c = '?';
    }
    return c - BIG_FONT_FIRST;
}

static uint8_t big_width(char c) {
    const uint8_t glyph = big_glyph(c);
    return pgm_read_word(&big_font_offsets[glyph + 1]) -
           pgm_read_word(&big_font_offsets[glyph]);
}

static void big_draw(char c, int16_t left, int16_t top) {
    const uint8_t  glyph = big_glyph(c);
    const uint16_t end   = pgm_read_word(&big_font_offsets[glyph + 1]);
    for (uint16_t i = pgm_read_word(&big_font_offsets[glyph]); i < end; i++) {
        draw_column(left++, top, pgm_read_dword(&big_font_columns[i]), 1);
    }
}

static const stack_font_t big_font = {
    .height = BIG_FONT_HEIGHT,
    .gap    = 4,
    .width  = big_width,
    .draw   = big_draw,
};

// The 6x8 font at twice the size, for names too long to stack in the big
// font. Its capitals leave the bottom row blank.
#define DOUBLE_SCALE 2

// First and last inked column of the 6x8 glyph for c. Returns false for a
// blank glyph or a character the font doesn't have.
static bool small_glyph_ink(char c, const uint8_t **glyph, uint8_t *first,
                            uint8_t *last) {
    *glyph = small_glyph(toupper((unsigned char)c));
    if (*glyph == NULL) {
        return false;
    }
    bool inked = false;
    for (uint8_t i = 0; i < OLED_FONT_WIDTH; i++) {
        if (pgm_read_byte(&(*glyph)[i]) != 0) {
            if (!inked) {
                *first = i;
            }
            *last = i;
            inked = true;
        }
    }
    return inked;
}

static uint8_t double_width(char c) {
    const uint8_t *glyph;
    uint8_t        first, last;
    if (!small_glyph_ink(c, &glyph, &first, &last)) {
        return 0;
    }
    return (last - first + 1) * DOUBLE_SCALE;
}

static void double_draw(char c, int16_t left, int16_t top) {
    const uint8_t *glyph;
    uint8_t        first, last;
    if (!small_glyph_ink(c, &glyph, &first, &last)) {
        return;
    }
    for (uint8_t i = first; i <= last; i++, left += DOUBLE_SCALE) {
        draw_column(left, top, pgm_read_byte(&glyph[i]), DOUBLE_SCALE);
    }
}

static const stack_font_t double_font = {
    .height = (OLED_FONT_HEIGHT - 1) * DOUBLE_SCALE,
    .gap    = 3,
    .width  = double_width,
    .draw   = double_draw,
};

static uint16_t stack_height(const stack_font_t *font, size_t letters) {
    if (letters == 0) {
        return 0;
    }
    return letters * font->height + (letters - 1) * font->gap;
}

// Stacks name down the left side from the top, in the big font if it fits
// and at double size otherwise. A name too long even for that is cut off at
// the bottom.
static void draw_stacked(const char *name) {
    const size_t        letters = strlen(name);
    const stack_font_t *font    = &big_font;
    if (stack_height(font, letters) > SCREEN_HEIGHT - 2 * STACK_MARGIN) {
        font = &double_font;
    }

    int16_t top = STACK_MARGIN;
    for (const char *c = name; *c != '\0' && top < SCREEN_HEIGHT; c++) {
        font->draw(*c, STACK_X - font->width(*c) / 2, top);
        top += font->height + font->gap;
    }
}

// The screen is drawn from scratch on every pass, then copied into the OLED
// buffer. The copy only marks the bytes that actually changed as dirty, so
// an unchanged screen costs no I2C traffic.
static void render_layer_screen(void) {
    const uint8_t layer = get_highest_layer(layer_state | default_layer_state);

    memset(layer_screen, 0, sizeof(layer_screen));

    draw_stacked(layer < LAYER_COUNT ? layer_name(layer) : "?");

    if (layer == SETTING) {
        const char   *labels[RGB_LINES] = {"MM:", "MV:", "MS:"};
        const uint8_t values[RGB_LINES] = {rgb_matrix_get_mode(),
                                           rgb_matrix_get_val(),
                                           rgb_matrix_get_speed()};
        for (uint8_t i = 0; i < RGB_LINES; i++) {
            char line[RGB_CHARS + 1];
            snprintf(line, sizeof(line), "%s%3u", labels[i], values[i]);
            draw_text(RGB_X, RGB_TOP + i * RGB_LINE_PITCH, line);
        }
    }

    const char *host = xp_host_name(xp_host());
    draw_text(SCREEN_WIDTH - TEXT_MARGIN - strlen(host) * OLED_FONT_WIDTH,
              HOST_TOP, host);

    oled_set_cursor(0, 0);
    oled_write_raw((const char *)layer_screen, sizeof(layer_screen));
}

bool oled_task_user(void) {
    if (is_keyboard_master()) {
        oled_write_raw_P(plant_anim[current_frame], OLED_MATRIX_SIZE);
    } else {
        render_layer_screen();
    }

    return false;
}

// QK_BOOT: replace the plant with a boot message before the jump to the
// bootloader. The display keeps showing it while in the bootloader. QK_BOOT
// runs on the half plugged into USB, which is the half showing the plant.
bool shutdown_user(bool jump_to_bootloader) {
    if (jump_to_bootloader) {
        oled_clear();
        write_centered(oled_max_lines() / 2, boot);
        // There is no next OLED pass once we jump, so render everything now.
        // This also turns on a display that has timed out.
        oled_render_dirty(true);
    }

    return true;
}
