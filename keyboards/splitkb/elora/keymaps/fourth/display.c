#include QMK_KEYBOARD_H
#include <string.h>
#include "transactions.h"
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

// Logo and name shown for each layer. The logos only cover the middle eight
// of the sixteen text lines, which leaves room for text above and below them.
// clang-format off
static const struct {
    const char *name;
    const char *logo; // NULL for a text only screen
} layer_display[] = {
    [BASE]    = {"Base",    base_logo},
    [SYM]     = {"Sym",     sym_logo},
    [NAV]     = {"Nav",     sym_logo},
    [GAME]    = {"Game",    game_logo},
    [SETTING] = {"Setting", NULL},
};
// clang-format on

static const char *windows = "Win";
static const char *macos   = "Mac";
static const char *boot    = "Boot";

// Text lines on the layer screen
enum text_lines {
    LAYER_NAME_LINE = 1,
    RGB_MODE_LINE   = 4,
    RGB_VAL_LINE,
    RGB_SPEED_LINE,
    HOST_LINE = 14,
};

static uint8_t current_frame = 0;

// Host selected with XP_MODE. That state lives on the half plugged into USB,
// which shows the plant, so it is sent over to the half showing the layer
// screen. Nothing is shown until the first value arrives.
static bool host_received = false;
static bool host_macos    = false;

static void host_sync_handler(uint8_t in_buflen, const void *in_data,
                              uint8_t out_buflen, void *out_data) {
    if (in_buflen == sizeof(host_macos)) {
        memcpy(&host_macos, in_data, sizeof(host_macos));
        host_received = true;
    }
}

void display_init(void) {
    transaction_register_rpc(XP_HOST_SYNC, host_sync_handler);
}

void housekeeping_task_user(void) {
    if (!is_keyboard_master()) {
        return;
    }

    // Sent on every change, and every half second on top of that so a half
    // that reconnects gets it again
    static bool     synced       = false;
    static bool     synced_macos = false;
    static uint32_t last_sync    = 0;
    const bool      macos        = xp_is_macos();

    if (!synced || macos != synced_macos || timer_elapsed32(last_sync) >= 500) {
        if (transaction_rpc_send(XP_HOST_SYNC, sizeof(macos), &macos)) {
            synced       = true;
            synced_macos = macos;
            last_sync    = timer_read32();
        }
    }
}

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
    const uint8_t  layer =
        get_highest_layer(layer_state | default_layer_state);

    if (layer != last_layer) {
        oled_clear();
        last_layer = layer;
    }

    if (layer < ARRAY_SIZE(layer_display)) {
        if (layer_display[layer].logo != NULL) {
            oled_write_raw_P(layer_display[layer].logo, OLED_MATRIX_SIZE);
        }
        write_centered(LAYER_NAME_LINE, layer_display[layer].name);
    } else {
        write_centered(LAYER_NAME_LINE, "?");
    }

    if (layer == SETTING) {
        write_value(RGB_MODE_LINE, PSTR("MM: "), rgb_matrix_get_mode());
        write_value(RGB_VAL_LINE, PSTR("MV: "), rgb_matrix_get_val());
        write_value(RGB_SPEED_LINE, PSTR("MS: "), rgb_matrix_get_speed());
    }

    if (host_received) {
        write_centered(HOST_LINE, host_macos ? macos : windows);
    }
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
