#include QMK_KEYBOARD_H
#include <string.h>
#include "cross_platform.h"
#include "display.h"
#include "frames.h"
#include "layers.h"

// The half plugged into USB shows a plant that grows as you type, the other
// half shows the layer screen.

// Frames and logos are written straight into the OLED buffer
_Static_assert(sizeof(plant_anim[0]) == OLED_MATRIX_SIZE, "frame size");
_Static_assert(sizeof(base_logo) == OLED_MATRIX_SIZE, "logo size");
_Static_assert(sizeof(sym_logo) == OLED_MATRIX_SIZE, "logo size");
_Static_assert(sizeof(game_logo) == OLED_MATRIX_SIZE, "logo size");

// Logo shown for each layer, NULL for a text only screen. The logos only cover
// the middle eight of the sixteen text lines, which leaves room for the layer
// name and other text above and below them.
// clang-format off
static const char *const layer_logos[] = {
    [BASE]    = base_logo,
    [SYM]     = sym_logo,
    [NAV]     = sym_logo,
    [GAME]    = game_logo,
    [SETTING] = NULL,
};
// clang-format on
_Static_assert(ARRAY_SIZE(layer_logos) == LAYER_COUNT,
               "layer_logos needs an entry for every layer");

static const char *boot = "Boot";

// Text lines on the layer screen
enum text_lines {
    LAYER_NAME_LINE = 1,
    RGB_MODE_LINE   = 4,
    RGB_VAL_LINE,
    RGB_SPEED_LINE,
    HOST_LINE = 14,
};

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

// Writes a label and a right aligned value on the given line
static void write_value(uint8_t line, const char *label, uint8_t value) {
    oled_set_cursor(0, line);
    oled_write_P(label, false);
    oled_write_ln(get_u8_str(value, ' '), false);
}

// Everything is redrawn on every pass. The OLED driver only marks the parts of
// the screen that actually changed as dirty, so an unchanged screen costs no
// I2C traffic. The screen is only cleared on a layer change, since a clear
// marks the whole screen dirty.
static void render_layer_screen(void) {
    static uint8_t last_layer = UINT8_MAX;
    const uint8_t  layer = get_highest_layer(layer_state | default_layer_state);

    if (layer != last_layer) {
        oled_clear();
        last_layer = layer;
    }

    if (layer < LAYER_COUNT) {
        if (layer_logos[layer] != NULL) {
            oled_write_raw_P(layer_logos[layer], OLED_MATRIX_SIZE);
        }
        write_centered(LAYER_NAME_LINE, layer_name(layer));
    } else {
        write_centered(LAYER_NAME_LINE, "?");
    }

    if (layer == SETTING) {
        write_value(RGB_MODE_LINE, PSTR("MM: "), rgb_matrix_get_mode());
        write_value(RGB_VAL_LINE, PSTR("MV: "), rgb_matrix_get_val());
        write_value(RGB_SPEED_LINE, PSTR("MS: "), rgb_matrix_get_speed());
    }

    write_centered(HOST_LINE, xp_host_name(xp_host()));
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
