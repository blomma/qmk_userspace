#pragma once

// Settings shared by the fourth keymaps. QMK reads a module's config.h before
// the keyboard's and the keymap's, so a keymap can still override one of these
// with #undef and #define.

#define RGB_MATRIX_TIMEOUT 180000

// Each half only sees its own key presses, so without this the half not
// plugged into USB runs its timeouts (RGB matrix, Halcyon display and
// backlight) on its own and turns off while you type on the other half. The
// half plugged into USB now sends its activity to the other half.
#define SPLIT_ACTIVITY_ENABLE

// Enable rapid switch from tap to hold, disables double tap hold auto-repeat.
#define QUICK_TAP_TERM 0

// Configure the global tapping term (default: 200ms)
#define TAPPING_TERM 200

// Recommended for Chordal hold
// #define PERMISSIVE_HOLD

// #define CHORDAL_HOLD

// #define FLOW_TAP_TERM 150
