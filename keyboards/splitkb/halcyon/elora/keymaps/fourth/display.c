#include "qp.h"
#include "qp_surface.h"
#include "hardware/structs/rosc.h"
#include "display.h"

// Fonts mono2
#include "../../../../../../users/halcyon_modules/splitkb/hlc_tft_display/graphics/fonts/Retron2000-27.qff.h"

// Numbers mono2
#include "../../../../../../users/halcyon_modules/splitkb/hlc_tft_display/graphics/numbers/0.qgf.h"
#include "../../../../../../users/halcyon_modules/splitkb/hlc_tft_display/graphics/numbers/1.qgf.h"
#include "../../../../../../users/halcyon_modules/splitkb/hlc_tft_display/graphics/numbers/2.qgf.h"
#include "../../../../../../users/halcyon_modules/splitkb/hlc_tft_display/graphics/numbers/3.qgf.h"
#include "../../../../../../users/halcyon_modules/splitkb/hlc_tft_display/graphics/numbers/4.qgf.h"
#include "../../../../../../users/halcyon_modules/splitkb/hlc_tft_display/graphics/numbers/undef.qgf.h"

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

int b_color_value = 0;
#define GRID_WIDTH 27
#define GRID_HEIGHT 48
#define CELL_SIZE 4 // Cell size excluding outline
#define OUTLINE_SIZE 1

// Define the probability factor for initial alive cells
#define INITIAL_ALIVE_PROBABILITY 0.2 // 20% chance of being alive

bool grid[GRID_HEIGHT][GRID_WIDTH];         // Current state
bool new_grid[GRID_HEIGHT][GRID_WIDTH];     // Next state
bool changed_grid[GRID_HEIGHT][GRID_WIDTH]; // Tracks changed cells

uint32_t b_get_random_32bit(void) {
    uint32_t random_value = 0;
    for (int i = 0; i < 32; i++) {
        wait_ms(1);
        random_value = (random_value << 1) | (rosc_hw->randombit & 1);
    }
    return random_value;
}

void b_init_grid() {
    // Initialize grid with alive cells
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            grid[y][x]         = (rand() < INITIAL_ALIVE_PROBABILITY *
                                       RAND_MAX); // Use probability factor
            changed_grid[y][x] = true; // Mark all as changed initially
        }
    }
}

void b_draw_grid() {
    uint8_t hue      = 0; // Hue for alive cells
    uint8_t sat      = 0; // Saturation for alive cells
    uint8_t val_dead = 0; // Brightness for dead cells

    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            if (changed_grid[y][x]) { // Only update changed cells
                uint16_t left   = x * (CELL_SIZE + OUTLINE_SIZE);
                uint16_t top    = y * (CELL_SIZE + OUTLINE_SIZE);
                uint16_t right  = left + CELL_SIZE + OUTLINE_SIZE;
                uint16_t bottom = top + CELL_SIZE + OUTLINE_SIZE;

                // Draw the outline
                qp_rect(lcd_surface, left, top, right, bottom, hue, sat,
                        val_dead, true);

                // Draw the filled cell inside the outline if it's alive
                if (grid[y][x]) {
                    switch (b_color_value) {
                        case 0:
                            qp_rect(lcd_surface, left + OUTLINE_SIZE,
                                    top + OUTLINE_SIZE, right - OUTLINE_SIZE,
                                    bottom - OUTLINE_SIZE, HSV_LAYER_0, true);
                            break;
                        case 1:
                            qp_rect(lcd_surface, left + OUTLINE_SIZE,
                                    top + OUTLINE_SIZE, right - OUTLINE_SIZE,
                                    bottom - OUTLINE_SIZE, HSV_LAYER_1, true);
                            break;
                        case 2:
                            qp_rect(lcd_surface, left + OUTLINE_SIZE,
                                    top + OUTLINE_SIZE, right - OUTLINE_SIZE,
                                    bottom - OUTLINE_SIZE, HSV_LAYER_2, true);
                            break;
                        case 3:
                            qp_rect(lcd_surface, left + OUTLINE_SIZE,
                                    top + OUTLINE_SIZE, right - OUTLINE_SIZE,
                                    bottom - OUTLINE_SIZE, HSV_LAYER_3, true);
                            break;
                        case 4:
                            qp_rect(lcd_surface, left + OUTLINE_SIZE,
                                    top + OUTLINE_SIZE, right - OUTLINE_SIZE,
                                    bottom - OUTLINE_SIZE, HSV_LAYER_4, true);
                            break;
                        case 5:
                            qp_rect(lcd_surface, left + OUTLINE_SIZE,
                                    top + OUTLINE_SIZE, right - OUTLINE_SIZE,
                                    bottom - OUTLINE_SIZE, HSV_LAYER_5, true);
                            break;
                        case 6:
                            qp_rect(lcd_surface, left + OUTLINE_SIZE,
                                    top + OUTLINE_SIZE, right - OUTLINE_SIZE,
                                    bottom - OUTLINE_SIZE, HSV_LAYER_6, true);
                            break;
                        case 7:
                            qp_rect(lcd_surface, left + OUTLINE_SIZE,
                                    top + OUTLINE_SIZE, right - OUTLINE_SIZE,
                                    bottom - OUTLINE_SIZE, HSV_LAYER_7, true);
                            break;
                        default:
                            qp_rect(lcd_surface, left + OUTLINE_SIZE,
                                    top + OUTLINE_SIZE, right - OUTLINE_SIZE,
                                    bottom - OUTLINE_SIZE, HSV_LAYER_UNDEF,
                                    true);
                    }
                }
            }
        }
    }
}

void b_update_grid() {
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            int alive_neighbors = 0;

            // Count alive neighbors
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    if (dy == 0 && dx == 0) continue; // Skip the current cell
                    int ny = y + dy;
                    int nx = x + dx;
                    if (ny >= 0 && ny < GRID_HEIGHT && nx >= 0 &&
                        nx < GRID_WIDTH) {
                        alive_neighbors += grid[ny][nx];
                    }
                }
            }

            // Apply the rules of the Game of Life
            if (grid[y][x]) {
                // Any live cell with two or three live neighbours survives.
                new_grid[y][x] = (alive_neighbors == 2 || alive_neighbors == 3);
            } else {
                // Any dead cell with exactly three live neighbours becomes a
                // live cell.
                new_grid[y][x] = (alive_neighbors == 3);
            }

            // Track changed cells
            changed_grid[y][x] = (grid[y][x] != new_grid[y][x]);
        }
    }

    // Copy new grid state to current grid
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            grid[y][x] = new_grid[y][x];
        }
    }
}

// Function to add a cluster of cells at a random position
void b_add_cell_cluster() {
    int cluster_size = 3; // Size of the cluster (3x3)
    int x            = rand() % (GRID_WIDTH - cluster_size);
    int y            = rand() % (GRID_HEIGHT - cluster_size);

    for (int dy = 0; dy < cluster_size; dy++) {
        for (int dx = 0; dx < cluster_size; dx++) {
            bool is_alive = rand() % 2; // Randomly choose between 0 and 1
            grid[y + dy][x + dx]         = is_alive; // Set the cell to be alive
            changed_grid[y + dy][x + dx] = true;     // Mark the cell as changed
        }
    }
}

bool display_module_housekeeping_task_user(const bool second_display) {
    if (second_display) {
        static uint32_t last_draw                     = 0;
        static bool     second_display_set            = false;
        static uint32_t previous_matrix_activity_time = 0;

        if (!second_display_set) {
            srand(b_get_random_32bit());
            b_init_grid();
            b_color_value      = rand() % 8;
            second_display_set = true;
        }

        if (timer_elapsed32(last_draw) >= 100) { // Throttle to 10 fps
            b_draw_grid();
            b_update_grid();

            if (previous_matrix_activity_time != last_matrix_activity_time()) {
                b_color_value = rand() % 8;
                b_add_cell_cluster();
                previous_matrix_activity_time = last_matrix_activity_time();
            }

            last_draw = timer_read32();
        }
    }

    if (!second_display) {
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
                                LCD_HEIGHT - Retron27->line_height - 5,
                                Retron27, buf, HSV_CAPS_ON, HSV_BLACK);
        }

        if (previous_layer_state != layer_state || first_run_layer == false) {
            qp_clear(lcd_surface);

            switch (get_highest_layer(layer_state | default_layer_state)) {
                case 0:
                    qp_drawtext_recolor(
                        lcd_surface, 5,
                        LCD_HEIGHT - Retron27->line_height * 3 - 10, Retron27,
                        base, HSV_CAPS_ON, HSV_BLACK);
                    layer_number = qp_load_image_mem(gfx_0);
                    qp_drawimage_recolor(lcd_surface, 5, 5, layer_number,
                                         HSV_LAYER_0, HSV_BLACK);
                    break;
                case 1:
                    qp_drawtext_recolor(
                        lcd_surface, 5,
                        LCD_HEIGHT - Retron27->line_height * 3 - 10, Retron27,
                        sym, HSV_CAPS_ON, HSV_BLACK);
                    layer_number = qp_load_image_mem(gfx_1);
                    qp_drawimage_recolor(lcd_surface, 5, 5, layer_number,
                                         HSV_LAYER_1, HSV_BLACK);
                    break;
                case 2:
                    qp_drawtext_recolor(
                        lcd_surface, 5,
                        LCD_HEIGHT - Retron27->line_height * 3 - 10, Retron27,
                        nav, HSV_CAPS_ON, HSV_BLACK);
                    layer_number = qp_load_image_mem(gfx_2);
                    qp_drawimage_recolor(lcd_surface, 5, 5, layer_number,
                                         HSV_LAYER_2, HSV_BLACK);
                    break;
                case 3:
                    qp_drawtext_recolor(
                        lcd_surface, 5,
                        LCD_HEIGHT - Retron27->line_height * 3 - 10, Retron27,
                        game, HSV_CAPS_ON, HSV_BLACK);
                    layer_number = qp_load_image_mem(gfx_3);
                    qp_drawimage_recolor(lcd_surface, 5, 5, layer_number,
                                         HSV_LAYER_3, HSV_BLACK);
                    break;
                case 4:
                    qp_drawtext_recolor(
                        lcd_surface, 5,
                        LCD_HEIGHT - Retron27->line_height * 3 - 10, Retron27,
                        setting, HSV_CAPS_ON, HSV_BLACK);
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
    }

    // Move surface to lcd
    qp_surface_draw(lcd_surface, lcd, 0, 0, 0);
    qp_flush(lcd);

    return false;
}
