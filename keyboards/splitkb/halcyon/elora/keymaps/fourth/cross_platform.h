#pragma once

#include QMK_KEYBOARD_H

// Swedish symbols that need a different chord on Windows and macOS. Each one
// resolves to the chord for the selected host when it is pressed. Symbols that
// share a chord on both hosts use the plain SE_ keycodes from keymap_swedish.h.
enum cross_platform_keycodes {
    XP_LCBR = SAFE_RANGE, // {
    XP_RCBR,              // }
    XP_BSLS,              // (backslash)
    XP_PIPE,              // |
    XP_SYMBOLS_END,
    // Toggles between Windows and macOS chords, remembered across power cycles
    XP_MODE = XP_SYMBOLS_END,
};

// Loads the selected host from EEPROM. Call from keyboard_post_init_user.
void cross_platform_init(void);

bool xp_is_macos(void);

// Resolves an XP_ symbol to the chord for the selected host. Any other keycode
// is returned unchanged.
uint16_t xp_resolve(uint16_t keycode);

bool process_record_cross_platform(uint16_t keycode, keyrecord_t *record);

// Like ACTION_TAP_DANCE_DOUBLE, but kc1 and kc2 may be XP_ symbols.
typedef struct {
    uint16_t kc1;
    uint16_t kc2;
    uint16_t registered; // Resolved chord currently held, released on reset
} tap_dance_xp_pair_t;

void tap_dance_xp_pair_on_each_tap(tap_dance_state_t *state, void *user_data);
void tap_dance_xp_pair_finished(tap_dance_state_t *state, void *user_data);
void tap_dance_xp_pair_reset(tap_dance_state_t *state, void *user_data);

#define ACTION_TAP_DANCE_XP_DOUBLE(kc1, kc2)                                                                     \
    {                                                                                                            \
        .fn        = {tap_dance_xp_pair_on_each_tap, tap_dance_xp_pair_finished, tap_dance_xp_pair_reset, NULL}, \
        .user_data = (void *)&((tap_dance_xp_pair_t){kc1, kc2, KC_NO}),                                          \
    }
