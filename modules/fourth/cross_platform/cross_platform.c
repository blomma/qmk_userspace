#include "cross_platform.h"
#include <string.h>
#include "keymap_swedish.h"
#ifdef SPLIT_KEYBOARD
#    include "transactions.h"
#endif

ASSERT_COMMUNITY_MODULES_MIN_API_VERSION(1, 1, 3);

// xp_resolve and the chord table index the symbols by their offset from
// XP_LCBR, so they must stay first and in this order in qmk_module.json
_Static_assert(XP_RCBR == XP_LCBR + 1 && XP_BSLS == XP_LCBR + 2 &&
                   XP_PIPE == XP_LCBR + 3,
               "XP_ symbols must be contiguous in qmk_module.json");
#define XP_SYMBOL_COUNT (XP_PIPE - XP_LCBR + 1)

#define XP_HOST_DEFAULT XP_HOST_MACOS

typedef struct {
    uint8_t host; // xp_host_t, stored as one byte
} cross_platform_config_t;

_Static_assert(sizeof(cross_platform_config_t) <=
                   EECONFIG_MODULE_CROSS_PLATFORM_DATA_SIZE,
               "EECONFIG_MODULE_CROSS_PLATFORM_DATA_SIZE is too small");

// Persisted in the module's EEPROM datablock
static cross_platform_config_t config;

// Writes the default host to a datablock that is missing, from an older
// layout, or being reset
void eeconfig_init_cross_platform_datablock(void) {
    const cross_platform_config_t defaults = {.host = XP_HOST_DEFAULT};
    eeconfig_update_cross_platform_datablock(&defaults, 0, sizeof(defaults));
}

xp_host_t xp_host(void) {
    return config.host;
}

const char *xp_host_name(xp_host_t host) {
    static const char *names[XP_HOST_COUNT] = {
        [XP_HOST_MACOS]   = "Mac",
        [XP_HOST_WINDOWS] = "Win",
    };

    if (host >= XP_HOST_COUNT) {
        return "?";
    }
    return names[host];
}

// Windows chords come from keymap_swedish.h. The macOS chords match
// keymap_swedish_mac_iso.h, which can't be included alongside it since both
// define the same SE_ names.
// clang-format off
static const uint16_t chords[XP_SYMBOL_COUNT][XP_HOST_COUNT] = {
    [XP_LCBR - XP_LCBR] = {[XP_HOST_WINDOWS] = SE_LCBR, [XP_HOST_MACOS] = S(A(SE_8))},
    [XP_RCBR - XP_LCBR] = {[XP_HOST_WINDOWS] = SE_RCBR, [XP_HOST_MACOS] = S(A(SE_9))},
    [XP_BSLS - XP_LCBR] = {[XP_HOST_WINDOWS] = SE_BSLS, [XP_HOST_MACOS] = S(A(SE_7))},
    [XP_PIPE - XP_LCBR] = {[XP_HOST_WINDOWS] = SE_PIPE, [XP_HOST_MACOS] = A(SE_7)},
};
// clang-format on

static bool is_xp_symbol(uint16_t keycode) {
    return keycode >= XP_LCBR && keycode <= XP_PIPE;
}

uint16_t xp_resolve(uint16_t keycode) {
    if (!is_xp_symbol(keycode)) {
        return keycode;
    }
    return chords[keycode - XP_LCBR][xp_host()];
}

// Takes a config read from EEPROM or received from the other half, falling
// back to the default host if it holds a host this firmware doesn't know
static void set_config(const cross_platform_config_t *new_config) {
    config = *new_config;
    if (config.host >= XP_HOST_COUNT) {
        config.host = XP_HOST_DEFAULT;
    }
}

#ifdef SPLIT_KEYBOARD
// The other half only receives the selection, it never saves it. Its own
// EEPROM keeps whatever was selected while it was plugged into USB.
static void host_sync_handler(uint8_t in_buflen, const void *in_data,
                              uint8_t out_buflen, void *out_data) {
    if (in_buflen == sizeof(cross_platform_config_t)) {
        cross_platform_config_t received;
        memcpy(&received, in_data, sizeof(received));
        set_config(&received);
    }
}
#endif

void keyboard_post_init_cross_platform(void) {
    cross_platform_config_t stored;
    eeconfig_read_cross_platform_datablock(&stored, 0, sizeof(stored));
    set_config(&stored);
#ifdef SPLIT_KEYBOARD
    transaction_register_rpc(XP_HOST_SYNC, host_sync_handler);
#endif

    keyboard_post_init_cross_platform_kb();
}

void housekeeping_task_cross_platform(void) {
#ifdef SPLIT_KEYBOARD
    if (is_keyboard_master()) {
        // Sent on every change, and every half second on top of that so a
        // half that reconnects gets it again
        static bool     synced      = false;
        static uint8_t  synced_host = XP_HOST_DEFAULT;
        static uint32_t last_sync   = 0;

        if (!synced || config.host != synced_host ||
            timer_elapsed32(last_sync) >= 500) {
            if (transaction_rpc_send(XP_HOST_SYNC, sizeof(config), &config)) {
                synced      = true;
                synced_host = config.host;
                last_sync   = timer_read32();
            }
        }
    }
#endif

    housekeeping_task_cross_platform_kb();
}

bool process_record_cross_platform(uint16_t keycode, keyrecord_t *record) {
    if (!process_record_cross_platform_kb(keycode, record)) {
        return false;
    }

    // The chord registered on press, so release lifts the same keys even if
    // the mode was toggled while the key was held
    static uint16_t held[XP_SYMBOL_COUNT];

    if (is_xp_symbol(keycode)) {
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
            config.host =
                config.host == XP_HOST_MACOS ? XP_HOST_WINDOWS : XP_HOST_MACOS;
            eeconfig_update_cross_platform_datablock(&config, 0,
                                                     sizeof(config));
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
