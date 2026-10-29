#include "cross_platform.h"
#include "keymap_swedish.h"

typedef union {
    uint32_t raw;
    struct {
        bool windows : 1;
    };
} user_config_t;

// Persisted in EEPROM. A cleared EEPROM reads as all zeroes, which makes macOS
// the default.
static user_config_t user_config;

void cross_platform_init(void) {
    user_config.raw = eeconfig_read_user();
}

bool xp_is_macos(void) {
    return !user_config.windows;
}

// Windows chords come from keymap_swedish.h. The macOS chords match
// keymap_swedish_mac_iso.h, which can't be included alongside it since both
// define the same SE_ names.
static const uint16_t chords[][2] = {
    //                    Windows  macOS
    [XP_LCBR - XP_LCBR] = {SE_LCBR, S(A(SE_8))},
    [XP_RCBR - XP_LCBR] = {SE_RCBR, S(A(SE_9))},
    [XP_BSLS - XP_LCBR] = {SE_BSLS, S(A(SE_7))},
    [XP_PIPE - XP_LCBR] = {SE_PIPE, A(SE_7)},
};

uint16_t xp_resolve(uint16_t keycode) {
    if (keycode < XP_LCBR || keycode >= XP_SYMBOLS_END) {
        return keycode;
    }
    return chords[keycode - XP_LCBR][xp_is_macos()];
}

bool process_record_cross_platform(uint16_t keycode, keyrecord_t *record) {
    // The chord registered on press, so release lifts the same keys even if
    // the mode was toggled while the key was held
    static uint16_t held[XP_SYMBOLS_END - XP_LCBR];

    if (keycode >= XP_LCBR && keycode < XP_SYMBOLS_END) {
        uint8_t i = keycode - XP_LCBR;
        if (record->event.pressed) {
            held[i] = xp_resolve(keycode);
            register_code16(held[i]);
        } else {
            unregister_code16(held[i]);
        }
        return false;
    }

    if (keycode == XP_MODE) {
        if (record->event.pressed) {
            user_config.windows = !user_config.windows;
            eeconfig_update_user(user_config.raw);
        }
        return false;
    }

    return true;
}

void tap_dance_xp_pair_on_each_tap(tap_dance_state_t *state, void *user_data) {
    tap_dance_xp_pair_t *pair = (tap_dance_xp_pair_t *)user_data;

    if (state->count == 2) {
        pair->registered = xp_resolve(pair->kc2);
        register_code16(pair->registered);
        state->finished = true;
    }
}

void tap_dance_xp_pair_finished(tap_dance_state_t *state, void *user_data) {
    tap_dance_xp_pair_t *pair = (tap_dance_xp_pair_t *)user_data;

    pair->registered = xp_resolve(pair->kc1);
    register_code16(pair->registered);
}

void tap_dance_xp_pair_reset(tap_dance_state_t *state, void *user_data) {
    tap_dance_xp_pair_t *pair = (tap_dance_xp_pair_t *)user_data;

    if (state->count == 1) {
        wait_ms(TAP_CODE_DELAY);
    }
    unregister_code16(pair->registered);
    pair->registered = KC_NO;
}
