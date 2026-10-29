#pragma once

// Myriad boilerplate
#undef MYRIAD_ENABLE

#undef OLED_TIMEOUT
#define OLED_TIMEOUT 60000

// Each half only sees its own key presses, so without this the half not
// plugged into USB times out its display while you type on the other half.
// The half plugged into USB now turns both displays on and off.
#define SPLIT_OLED_ENABLE

// Sends the host selected with XP_MODE to the half showing the layer screen
#define SPLIT_TRANSACTION_IDS_USER XP_HOST_SYNC

#undef RGB_MATRIX_TIMEOUT
#define RGB_MATRIX_TIMEOUT 180000

// Enable rapid switch from tap to hold, disables double tap hold auto-repeat.
#undef QUICK_TAP_TERM
#define QUICK_TAP_TERM 0

// Configure the global tapping term (default: 200ms)
#undef TAPPING_TERM
#define TAPPING_TERM 200

// Recommended for Chordal hold
#undef PERMISSIVE_HOLD
// #define PERMISSIVE_HOLD

#undef CHORDAL_HOLD
// #define CHORDAL_HOLD

#undef FLOW_TAP_TERM
// #define FLOW_TAP_TERM 150

#define LAYER_STATE_8BIT

// Sets the default brightness value
#undef RGB_MATRIX_DEFAULT_VAL
#define RGB_MATRIX_DEFAULT_VAL 70

// Sets the default animation speed
#undef RGB_MATRIX_DEFAULT_SPD
#define RGB_MATRIX_DEFAULT_SPD 10

#define SPLIT_LAYER_STATE_ENABLE
