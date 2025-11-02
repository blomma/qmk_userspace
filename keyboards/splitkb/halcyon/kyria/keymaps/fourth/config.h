#pragma once

#undef RGB_MATRIX_TIMEOUT
#define RGB_MATRIX_TIMEOUT 180000

// Enable rapid switch from tap to hold, disables double tap hold auto-repeat.
#undef QUICK_TAP_TERM
#define QUICK_TAP_TERM 0

// Configure the global tapping term (default: 200ms)
#undef TAPPING_TERM
#define TAPPING_TERM 150

// Recommended for Chordal hold
#undef PERMISSIVE_HOLD
#define PERMISSIVE_HOLD

#undef CHORDAL_HOLD
#define CHORDAL_HOLD

#undef FLOW_TAP_TERM
// #define FLOW_TAP_TERM 150
