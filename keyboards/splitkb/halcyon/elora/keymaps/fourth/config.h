#pragma once

// Turns the TFT display and its backlight off after 60 seconds without input.
// The Halcyon module config.h is read after this file and only sets its
// default of 120 seconds when this is not already defined.
#define HLC_BACKLIGHT_TIMEOUT 60000
