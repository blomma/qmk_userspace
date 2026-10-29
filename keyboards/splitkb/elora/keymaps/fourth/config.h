#pragma once

// Myriad boilerplate
#undef MYRIAD_ENABLE

#undef OLED_TIMEOUT
#define OLED_TIMEOUT 60000

// Each half only sees its own key presses, so without this the half not
// plugged into USB times out its display while you type on the other half.
// The half plugged into USB now turns both displays on and off.
#define SPLIT_OLED_ENABLE

#define LAYER_STATE_8BIT

// Sets the default brightness value
#undef RGB_MATRIX_DEFAULT_VAL
#define RGB_MATRIX_DEFAULT_VAL 70

// Sets the default animation speed
#undef RGB_MATRIX_DEFAULT_SPD
#define RGB_MATRIX_DEFAULT_SPD 10

#define SPLIT_LAYER_STATE_ENABLE
