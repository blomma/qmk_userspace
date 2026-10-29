#include <stddef.h>
#include "layers.h"
#include "community_modules.h"

ASSERT_COMMUNITY_MODULES_MIN_API_VERSION(1, 0, 0);

// clang-format off
const char *layer_name(uint8_t layer) {
    static const char *names[LAYER_COUNT] = {
        [BASE]    = "Base",
        [SYM]     = "Sym",
        [NAV]     = "Nav",
        [GAME]    = "Game",
        [SETTING] = "Setting",
    };

    if (layer >= LAYER_COUNT) {
        return NULL;
    }
    return names[layer];
}
// clang-format on
