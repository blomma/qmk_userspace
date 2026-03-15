#include "qp.h"
#include "qp_surface.h"

// Fonts mono2
#include "../../../../../../users/halcyon_modules/splitkb/hlc_tft_display/graphics/fonts/Retron2000-27.qff.h"

// Numbers mono2
#include "../../../../../../users/halcyon_modules/splitkb/hlc_tft_display/graphics/numbers/0.qgf.h"
#include "../../../../../../users/halcyon_modules/splitkb/hlc_tft_display/graphics/numbers/1.qgf.h"
#include "../../../../../../users/halcyon_modules/splitkb/hlc_tft_display/graphics/numbers/2.qgf.h"
#include "../../../../../../users/halcyon_modules/splitkb/hlc_tft_display/graphics/numbers/3.qgf.h"
#include "../../../../../../users/halcyon_modules/splitkb/hlc_tft_display/graphics/numbers/4.qgf.h"
#include "../../../../../../users/halcyon_modules/splitkb/hlc_tft_display/graphics/numbers/undef.qgf.h"

// All values (including hue) are scaled to 0-255
#define HSV_CAPS_ON 17, 191, 245
#define HSV_LAYER_0 0, 0, 160
#define HSV_LAYER_1 23, 89, 255
#define HSV_LAYER_2 43, 71, 255
#define HSV_LAYER_3 0, 82, 255
#define HSV_LAYER_4 77, 64, 255
#define HSV_LAYER_UNDEF 0, 255, 255

static const char *base    = "Base    ";
static const char *sym     = "Sym     ";
static const char *nav     = "Nav     ";
static const char *game    = "Game    ";
static const char *setting = "Setting";

static painter_font_handle_t  Retron27;
static painter_image_handle_t layer_number;

painter_device_t lcd;
painter_device_t lcd_surface;

layer_state_t previous_layer_state = {0};

char buf[64] = {0};

bool display_module_housekeeping_task_user(const bool second_display) {
    if (second_display) {
        return true;
    }

    static bool first_run_layer = false;
    if (first_run_layer == false) {
        Retron27 = qp_load_font_mem(font_Retron2000_27);
    }

    if (get_highest_layer(layer_state | default_layer_state) == 4) {
        snprintf(buf, sizeof(buf), "%i,%i", rgb_matrix_get_mode(),
                 rgb_matrix_get_val());
        qp_drawtext_recolor(lcd_surface, 5,
                            LCD_HEIGHT - Retron27->line_height * 2 - 10,
                            Retron27, buf, HSV_CAPS_ON, HSV_BLACK);
        snprintf(buf, sizeof(buf), "%i", rgb_matrix_get_speed());
        qp_drawtext_recolor(lcd_surface, 5,
                            LCD_HEIGHT - Retron27->line_height - 5, Retron27,
                            buf, HSV_CAPS_ON, HSV_BLACK);
    }

    if (previous_layer_state != layer_state || first_run_layer == false) {
        qp_clear(lcd_surface);

        switch (get_highest_layer(layer_state | default_layer_state)) {
            case 0:
                qp_drawtext_recolor(lcd_surface, 5,
                                    LCD_HEIGHT - Retron27->line_height * 3 - 10,
                                    Retron27, base, HSV_CAPS_ON, HSV_BLACK);
                layer_number = qp_load_image_mem(gfx_0);
                qp_drawimage_recolor(lcd_surface, 5, 5, layer_number,
                                     HSV_LAYER_0, HSV_BLACK);
                break;
            case 1:
                qp_drawtext_recolor(lcd_surface, 5,
                                    LCD_HEIGHT - Retron27->line_height * 3 - 10,
                                    Retron27, sym, HSV_CAPS_ON, HSV_BLACK);
                layer_number = qp_load_image_mem(gfx_1);
                qp_drawimage_recolor(lcd_surface, 5, 5, layer_number,
                                     HSV_LAYER_1, HSV_BLACK);
                break;
            case 2:
                qp_drawtext_recolor(lcd_surface, 5,
                                    LCD_HEIGHT - Retron27->line_height * 3 - 10,
                                    Retron27, nav, HSV_CAPS_ON, HSV_BLACK);
                layer_number = qp_load_image_mem(gfx_2);
                qp_drawimage_recolor(lcd_surface, 5, 5, layer_number,
                                     HSV_LAYER_2, HSV_BLACK);
                break;
            case 3:
                qp_drawtext_recolor(lcd_surface, 5,
                                    LCD_HEIGHT - Retron27->line_height * 3 - 10,
                                    Retron27, game, HSV_CAPS_ON, HSV_BLACK);
                layer_number = qp_load_image_mem(gfx_3);
                qp_drawimage_recolor(lcd_surface, 5, 5, layer_number,
                                     HSV_LAYER_3, HSV_BLACK);
                break;
            case 4:
                qp_drawtext_recolor(lcd_surface, 5,
                                    LCD_HEIGHT - Retron27->line_height * 3 - 10,
                                    Retron27, setting, HSV_CAPS_ON, HSV_BLACK);
                layer_number = qp_load_image_mem(gfx_4);
                qp_drawimage_recolor(lcd_surface, 5, 5, layer_number,
                                     HSV_LAYER_4, HSV_BLACK);
                break;
            default:
                layer_number = qp_load_image_mem(gfx_undef);
                qp_drawimage_recolor(lcd_surface, 5, 5, layer_number,
                                     HSV_LAYER_UNDEF, HSV_BLACK);
        }

        qp_close_image(layer_number);
        previous_layer_state = layer_state;
        first_run_layer      = true;
    }

    // Move surface to lcd
    qp_surface_draw(lcd_surface, lcd, 0, 0, 0);
    qp_flush(lcd);

    return false;
}
