#pragma once

#include QMK_KEYBOARD_H

// Swedish symbols that need a different chord on Windows and macOS. Each XP_
// symbol keycode (XP_LCBR, XP_RCBR, XP_BSLS, XP_PIPE) resolves to the chord
// for the selected host when it is pressed, and XP_MODE toggles between
// Windows and macOS. The keycodes are declared in qmk_module.json. Symbols
// that share a chord on both hosts use the plain SE_ keycodes from
// keymap_swedish.h.

typedef enum {
    XP_HOST_MACOS,
    XP_HOST_WINDOWS,
    XP_HOST_COUNT,
} xp_host_t;

// The selected host. On a split keyboard both halves agree, as the half
// plugged into USB sends its selection to the other half.
xp_host_t xp_host(void);

// Short name for a host, such as "Mac", that fits on a display
const char *xp_host_name(xp_host_t host);

// Resolves an XP_ symbol to the chord for the selected host. Any other keycode
// is returned unchanged.
uint16_t xp_resolve(uint16_t keycode);

// Like ACTION_TAP_DANCE_DOUBLE, but kc1 and kc2 may be XP_ symbols.
typedef struct {
    uint16_t kc1;
    uint16_t kc2;
    uint16_t registered; // Resolved chord currently held, released on reset
} tap_dance_xp_pair_t;

void tap_dance_xp_pair_on_each_tap(tap_dance_state_t *state, void *user_data);
void tap_dance_xp_pair_finished(tap_dance_state_t *state, void *user_data);
void tap_dance_xp_pair_reset(tap_dance_state_t *state, void *user_data);

#define ACTION_TAP_DANCE_XP_DOUBLE(kc1, kc2)                                     \
    {                                                                            \
        .fn        = {tap_dance_xp_pair_on_each_tap, tap_dance_xp_pair_finished, \
                      tap_dance_xp_pair_reset, NULL},                            \
        .user_data = (void *)&((tap_dance_xp_pair_t){kc1, kc2, KC_NO}),          \
    }
