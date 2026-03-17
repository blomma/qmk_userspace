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

int cell_color_value = 0;
#define GRID_WIDTH 27
#define GRID_HEIGHT 48
#define CELL_SIZE 4 // Cell size excluding outline
#define OUTLINE_SIZE 1

// Define the probability factor for initial alive cells
#define INITIAL_ALIVE_PROBABILITY 0.2 // 20% chance of being alive

// Game of Life State: Bit-packed for Memory and Speed
// Each uint32_t represents a row where each bit (0-26) is a cell (1=Alive,
// 0=Dead). This reduces memory from 3888 bytes (bool[48][27]) to 576 bytes
// (uint32_t[48] * 3).
//
// Technical Note on Bit-Masking (1UL << x):
// 1. (1UL << x) creates a bitmask with only the x-th bit set.
// 2. We use '1UL' (Unsigned Long) to ensure the literal is at least 32 bits
// wide.
//    Using a standard '1' (signed int) can lead to Undefined Behavior if
//    shifting into the sign bit (bit 31), though our GRID_WIDTH is only 27.
// 3. Bitwise operations (&, |, ^, ~) allow us to check, set, flip, or clear
// cells
//    without the overhead of array indexing and boolean branching.
static uint32_t grid1[GRID_HEIGHT]; // Grid state A
static uint32_t grid2[GRID_HEIGHT]; // Grid state B

// Pointer Swapping Optimization: Instead of copying new_grid to grid every
// frame, we simply swap these pointers. This eliminates 1,296 memory
// assignments per frame.
static uint32_t *p_grid     = grid1; // Pointer to current state
static uint32_t *p_new_grid = grid2; // Pointer to next state

// Change Detection Optimization: A bit-packed mask of cells that changed state.
// If a bit is 1, the cell flipped (Alive<->Dead). If 0, it stayed the same.
static uint32_t changed_grid[GRID_HEIGHT]; // Tracks changed cells (bit-packed)

// Seeding Optimization: Instead of waiting 32ms (32 * wait_ms(1)) to harvest
// entropy from the ROsc (Ring Oscillator), we use the system timer.
// timer_read32() provides the milliseconds since boot, which is highly variable
// depending on exactly when the user powers on or interacts with the board.
uint32_t get_random_seed(void) {
    return timer_read32() ^ rosc_hw->randombit;
}

void init_game_of_life_grid(void) {
    // Initialize grid with alive cells
    for (int y = 0; y < GRID_HEIGHT; y++) {
        p_grid[y] = 0;
        for (int x = 0; x < GRID_WIDTH; x++) {
            if (rand() < INITIAL_ALIVE_PROBABILITY * RAND_MAX) {
                p_grid[y] |= (1UL << x);
            }
        }
        // Force all cells to 'changed' so the first draw fills the screen
        changed_grid[y] = 0xFFFFFFFF; // Mark all as changed initially
    }
}

/**
 * Maps the color index to specific HSV values defined in display.h.
 * Moving this to a helper allows the draw loop to calculate color ONCE per
 * frame instead of per-cell.
 */
static void get_hsv_color(int index, uint8_t *h, uint8_t *s, uint8_t *v) {
    switch (index) {
        case 0:
            *h = 0;
            *s = 0;
            *v = 160;
            break; // HSV_LAYER_0
        case 1:
            *h = 23;
            *s = 89;
            *v = 255;
            break; // HSV_LAYER_1
        case 2:
            *h = 43;
            *s = 71;
            *v = 255;
            break; // HSV_LAYER_2
        case 3:
            *h = 0;
            *s = 82;
            *v = 255;
            break; // HSV_LAYER_3
        case 4:
            *h = 77;
            *s = 64;
            *v = 255;
            break; // HSV_LAYER_4
        case 5:
            *h = 176;
            *s = 77;
            *v = 255;
            break; // HSV_LAYER_5
        case 6:
            *h = 131;
            *s = 99;
            *v = 255;
            break; // HSV_LAYER_6
        case 7:
            *h = 154;
            *s = 94;
            *v = 255;
            break; // HSV_LAYER_7
        default:
            *h = 0;
            *s = 255;
            *v = 255;
            break; // HSV_LAYER_UNDEF
    }
}

void draw_game_of_life_grid(void) {
    uint8_t h, s, v;
    get_hsv_color(cell_color_value, &h, &s, &v);

    for (int y = 0; y < GRID_HEIGHT; y++) {
        uint32_t changed = changed_grid[y];

        // Row-Skipping Optimization: If no cells in this row changed since the
        // last generation, we skip the entire row. This is the single biggest
        // draw-time saver.
        if (changed == 0) continue; // Skip unchanged rows

        uint32_t current_row = p_grid[y];
        uint16_t top         = y * (CELL_SIZE + OUTLINE_SIZE);
        uint16_t bottom      = top + CELL_SIZE + OUTLINE_SIZE;

        for (int x = 0; x < GRID_WIDTH; x++) {
            // Only draw cells that have actually flipped state
            if (changed & (1UL << x)) {
                uint16_t left  = x * (CELL_SIZE + OUTLINE_SIZE);
                uint16_t right = left + CELL_SIZE + OUTLINE_SIZE;

                if (current_row & (1UL << x)) {
                    // Cell became ALIVE:
                    // 1. Draw black background (outline)
                    // 2. Draw the colored 'alive' square
                    qp_rect(lcd_surface, left, top, right, bottom, 0, 0, 0,
                            true);
                    qp_rect(lcd_surface, left + OUTLINE_SIZE,
                            top + OUTLINE_SIZE, right - OUTLINE_SIZE,
                            bottom - OUTLINE_SIZE, h, s, v, true);
                } else {
                    // Cell became DEAD:
                    // Just draw the black background to "erase" the previous
                    // alive color.
                    qp_rect(lcd_surface, left, top, right, bottom, 0, 0, 0,
                            true);
                }
            }
        }
    }
}

void update_game_of_life_grid(void) {
    for (int y = 0; y < GRID_HEIGHT; y++) {
        uint32_t next_row = 0;

        // Row Caching: Get row pointers once per line to avoid repeated array
        // access
        uint32_t row_above = (y > 0) ? p_grid[y - 1] : 0;
        uint32_t row_curr  = p_grid[y];
        uint32_t row_below = (y < GRID_HEIGHT - 1) ? p_grid[y + 1] : 0;

        for (int x = 0; x < GRID_WIDTH; x++) {
            int alive_neighbors = 0;

            // Neighbor Counting via Bit-Shifting:
            // Instead of nested loops with 'if' boundary checks, we use direct
            // bit-masking on the cached rows. This is significantly faster on
            // MCU.

            // Neighbors above
            if (row_above) {
                if (x > 0 && (row_above & (1UL << (x - 1)))) alive_neighbors++;
                if (row_above & (1UL << x)) alive_neighbors++;
                if (x < GRID_WIDTH - 1 && (row_above & (1UL << (x + 1))))
                    alive_neighbors++;
            }

            // Neighbors same row
            if (x > 0 && (row_curr & (1UL << (x - 1)))) alive_neighbors++;
            if (x < GRID_WIDTH - 1 && (row_curr & (1UL << (x + 1))))
                alive_neighbors++;

            // Neighbors below
            if (row_below) {
                if (x > 0 && (row_below & (1UL << (x - 1)))) alive_neighbors++;
                if (row_below & (1UL << x)) alive_neighbors++;
                if (x < GRID_WIDTH - 1 && (row_below & (1UL << (x + 1))))
                    alive_neighbors++;
            }

            // Conway's Rules
            bool current = (row_curr & (1UL << x));
            if (current) {
                // Survival: 2 or 3 neighbors
                if (alive_neighbors == 2 || alive_neighbors == 3)
                    next_row |= (1UL << x);
            } else {
                // Birth: exactly 3 neighbors
                if (alive_neighbors == 3) next_row |= (1UL << x);
            }
        }

        p_new_grid[y] = next_row;

        // Bitwise XOR Optimization:
        // Identifying which cells changed state (flipped bits) is a single XOR
        // operation. This 'changed_grid' is then used by the draw function to
        // skip static cells.
        changed_grid[y] = row_curr ^ next_row;
    }

    // Pointer Swap: Move to the next generation instantly
    uint32_t *tmp = p_grid;
    p_grid        = p_new_grid;
    p_new_grid    = tmp;
}

// Function to add a cluster of cells at a random position
void add_cell_cluster_to_game_of_life_grid(void) {
    int cluster_size = 3;
    int x            = rand() % (GRID_WIDTH - cluster_size);
    int y            = rand() % (GRID_HEIGHT - cluster_size);

    for (int dy = 0; dy < cluster_size; dy++) {
        for (int dx = 0; dx < cluster_size; dx++) {
            if (rand() % 2) {
                p_grid[y + dy] |= (1UL << (x + dx));
            } else {
                p_grid[y + dy] &= ~(1UL << (x + dx));
            }
            changed_grid[y + dy] |= (1UL << (x + dx));
        }
    }
}

bool display_module_housekeeping_task_user(const bool second_display) {
    if (second_display) {
        static uint32_t last_draw                     = 0;
        static bool     second_display_set            = false;
        static uint32_t previous_matrix_activity_time = 0;

        if (!second_display_set) {
            srand(get_random_seed());
            init_game_of_life_grid();
            cell_color_value   = rand() % 8;
            second_display_set = true;
        }

        if (timer_elapsed32(last_draw) >= 200) { // Throttle to 10 fps
            draw_game_of_life_grid();
            update_game_of_life_grid();

            if (previous_matrix_activity_time != last_matrix_activity_time()) {
                cell_color_value = rand() % 8;
                add_cell_cluster_to_game_of_life_grid();
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
