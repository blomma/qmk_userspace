#pragma once

// Myriad boilerplate
#undef MYRIAD_ENABLE

#undef RGB_MATRIX_TIMEOUT
#define RGB_MATRIX_TIMEOUT 180000

// Enable rapid switch from tap to hold, disables double tap hold auto-repeat.
#undef QUICK_TAP_TERM
#define QUICK_TAP_TERM 0

// Configure the global tapping term (default: 200ms)
#undef TAPPING_TERM
#define TAPPING_TERM 150

// Recomended for Chordal hold
#undef PERMISSIVE_HOLD
#define PERMISSIVE_HOLD

#undef CHORDAL_HOLD
#define CHORDAL_HOLD

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

// #define MK_3_SPEED
// #undef MK_C_OFFSET_UNMOD
// #define MK_C_OFFSET_UNMOD 32
//
// #undef MK_C_INTERVAL_UNMOD
// #define MK_C_INTERVAL_UNMOD 16
