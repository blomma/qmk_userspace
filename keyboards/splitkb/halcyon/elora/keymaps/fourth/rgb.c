#include "color.h"
#include "rgb_matrix.h"

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    hsv_t hsv = {HSV_GOLDENROD};
    if (hsv.v > rgb_matrix_get_val()) {
        hsv.v = rgb_matrix_get_val();
    }
    rgb_t rgb = hsv_to_rgb(hsv);
    for (uint8_t i = led_min; i < led_max; i++) {
        if (HAS_FLAGS(g_led_config.flags[i], LED_FLAG_KEYLIGHT)) {
            rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
        }
    }

    return false;
}
