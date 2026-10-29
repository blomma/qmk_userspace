#pragma once

#include <stdint.h>

// Layers shared by the fourth keymaps
enum layers {
    BASE = 0,
    SYM,
    NAV,
    GAME,
    SETTING,
    LAYER_COUNT,
};

// Name of a layer, such as "Base", that fits on a display. NULL for a layer
// past the end of the list.
const char *layer_name(uint8_t layer);
