#pragma once

// Myriad boilerplate
#undef MYRIAD_ENABLE

// The keyboard slows I2C down to 100 kHz for the EEPROM on a Myriad card.
// Without a Myriad card only the OLED is on the bus, so run it at the RP2040
// default of 400 kHz.
#undef I2C1_CLOCK_SPEED
#define I2C1_CLOCK_SPEED 400000

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
