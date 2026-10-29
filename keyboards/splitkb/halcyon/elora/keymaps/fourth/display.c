#include QMK_KEYBOARD_H
#include "hardware/structs/rosc.h"
#include "hlc_tft_display/hlc_tft_display.h"
#include "cross_platform.h"
#include "layers.h"

// Fonts mono2
#include "hlc_tft_display/graphics/fonts/Retron2000-27.qff.h"

// Numbers mono2
#include "hlc_tft_display/graphics/numbers/0.qgf.h"
#include "hlc_tft_display/graphics/numbers/1.qgf.h"
#include "hlc_tft_display/graphics/numbers/2.qgf.h"
#include "hlc_tft_display/graphics/numbers/3.qgf.h"
#include "hlc_tft_display/graphics/numbers/4.qgf.h"
#include "hlc_tft_display/graphics/numbers/undef.qgf.h"

// Layer colours, indexed by layer number and also used for the Game of Life
// cells. All values (including hue) are scaled to 0-255.
static const hsv_t layer_colors[] = {
    {0, 0, 255},    // 0
    {23, 89, 255},  // 1
    {43, 71, 255},  // 2
    {0, 82, 255},   // 3
    {77, 64, 255},  // 4
    {176, 77, 255}, // 5
    {131, 99, 255}, // 6
    {154, 94, 255}, // 7
};
static const hsv_t undefined_layer_color = {0, 255, 255};
_Static_assert(ARRAY_SIZE(layer_colors) >= LAYER_COUNT,
               "layer_colors needs a colour for every layer");

// Number image shown for each layer
// clang-format off
static const uint8_t *const layer_numbers[] = {
    [BASE]    = gfx_0,
    [SYM]     = gfx_1,
    [NAV]     = gfx_2,
    [GAME]    = gfx_3,
    [SETTING] = gfx_4,
};
// clang-format on
_Static_assert(ARRAY_SIZE(layer_numbers) == LAYER_COUNT,
               "layer_numbers needs an entry for every layer");

static const char *boot = "Boot";

static painter_font_handle_t Retron27;

static char buf[64] = {0};

static uint8_t cell_color_value = 0;
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

// Seed built from 32 ROSC (Ring Oscillator) random bits. Each bit is sampled
// 10us apart, the minimum spacing pico-sdk's pico_rand uses, so consecutive
// reads don't return the same oscillator state. Costs ~320us, once.
static uint32_t get_random_seed(void) {
    uint32_t seed = 0;
    for (int i = 0; i < 32; i++) {
        wait_us(10);
        seed = (seed << 1) | (rosc_hw->randombit & 1);
    }
    return seed;
}

static void init_game_of_life_grid(void) {
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

static void draw_game_of_life_grid(void) {
    // Colour is looked up once per frame instead of per cell
    const hsv_t color = layer_colors[cell_color_value];

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
                            bottom - OUTLINE_SIZE, color.h, color.s, color.v,
                            true);
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

static void update_game_of_life_grid(void) {
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
static void add_cell_cluster_to_game_of_life_grid(void) {
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

// Text rows stacked up from the bottom edge: the host on the bottom row, the
// RGB readout above it and the layer name on top, just under the layer number
// image.
enum text_rows {
    HOST_ROW,
    RGB_SPEED_ROW,
    RGB_MODE_ROW,
    LAYER_NAME_ROW,
};

static uint16_t text_row_y(uint8_t row) {
    return LCD_HEIGHT - 5 - Retron27->line_height * (row + 1);
}

// Blanks one full-width line of text starting at y
static void clear_text_row(uint16_t y) {
    qp_rect(lcd_surface, 0, y, LCD_WIDTH - 1, y + Retron27->line_height - 1,
            HSV_BLACK, true);
}

// Draws the layer number image and, for known layers, the layer name.
// Only the layer name row is cleared, not the whole surface: a full clear
// marks every pixel dirty and pushes the entire framebuffer over SPI on each
// layer change. The layer number images share one size and are drawn with a
// black background, so they overwrite each other.
static void draw_layer(uint8_t layer, uint16_t name_y) {
    const uint8_t *number = gfx_undef;
    hsv_t          color  = undefined_layer_color;

    clear_text_row(name_y);

    if (layer < LAYER_COUNT) {
        number = layer_numbers[layer];
        color  = layer_colors[layer];
        qp_drawtext_recolor(lcd_surface, 5, name_y, Retron27, layer_name(layer),
                            HSV_CAPS_ON, HSV_BLACK);
    }

    painter_image_handle_t image = qp_load_image_mem(number);
    qp_drawimage_recolor(lcd_surface, 5, 5, image, color.h, color.s, color.v,
                         HSV_BLACK);
    qp_close_image(image);
}

bool display_module_housekeeping_task_user(const bool second_display) {
    if (second_display) {
        static uint32_t last_draw                     = 0;
        static bool     second_display_set            = false;
        static uint32_t previous_matrix_activity_time = 0;

        if (!second_display_set) {
            srand(get_random_seed());
            init_game_of_life_grid();
            cell_color_value   = rand() % ARRAY_SIZE(layer_colors);
            second_display_set = true;
        }

        if (timer_elapsed32(last_draw) >= 200) { // Throttle to 5 fps
            draw_game_of_life_grid();
            update_game_of_life_grid();

            if (previous_matrix_activity_time != last_matrix_activity_time()) {
                cell_color_value = rand() % ARRAY_SIZE(layer_colors);
                add_cell_cluster_to_game_of_life_grid();
                previous_matrix_activity_time = last_matrix_activity_time();
            }

            last_draw = timer_read32();
        }
    }

    if (!second_display) {
        if (Retron27 == NULL) {
            Retron27 = qp_load_font_mem(font_Retron2000_27);
        }

        const uint16_t host_y       = text_row_y(HOST_ROW);
        const uint16_t rgb_speed_y  = text_row_y(RGB_SPEED_ROW);
        const uint16_t rgb_mode_y   = text_row_y(RGB_MODE_ROW);
        const uint16_t layer_name_y = text_row_y(LAYER_NAME_ROW);

        // Redraw only when the shown layer changes. Starts out of range so the
        // first call always draws.
        static uint8_t last_layer = UINT8_MAX;
        const uint8_t  layer =
            get_highest_layer(layer_state | default_layer_state);

        if (layer != last_layer) {
            draw_layer(layer, layer_name_y);
            last_layer = layer;
        }

        // Host selected with XP_MODE. The cross_platform module keeps both
        // halves in sync, so whichever half shows this screen can show it.
        static bool      host_drawn = false;
        static xp_host_t last_host  = XP_HOST_MACOS;
        const xp_host_t  host       = xp_host();

        if (!host_drawn || host != last_host) {
            clear_text_row(host_y);
            qp_drawtext_recolor(lcd_surface, 5, host_y, Retron27,
                                xp_host_name(host), HSV_CAPS_ON, HSV_BLACK);
            last_host  = host;
            host_drawn = true;
        }

        // RGB readout on the setting layer. Rows are cleared before redrawing
        // so a shorter value doesn't leave digits from the previous one behind,
        // and they are only redrawn when a value actually changes.
        static bool    rgb_info_drawn = false;
        static uint8_t last_rgb_mode  = 0;
        static uint8_t last_rgb_val   = 0;
        static uint8_t last_rgb_speed = 0;

        if (layer == SETTING) {
            const uint8_t mode  = rgb_matrix_get_mode();
            const uint8_t val   = rgb_matrix_get_val();
            const uint8_t speed = rgb_matrix_get_speed();

            if (!rgb_info_drawn || mode != last_rgb_mode ||
                val != last_rgb_val) {
                clear_text_row(rgb_mode_y);
                snprintf(buf, sizeof(buf), "%u,%u", mode, val);
                qp_drawtext_recolor(lcd_surface, 5, rgb_mode_y, Retron27, buf,
                                    HSV_CAPS_ON, HSV_BLACK);
            }

            if (!rgb_info_drawn || speed != last_rgb_speed) {
                clear_text_row(rgb_speed_y);
                snprintf(buf, sizeof(buf), "%u", speed);
                qp_drawtext_recolor(lcd_surface, 5, rgb_speed_y, Retron27, buf,
                                    HSV_CAPS_ON, HSV_BLACK);
            }

            last_rgb_mode  = mode;
            last_rgb_val   = val;
            last_rgb_speed = speed;
            rgb_info_drawn = true;
        } else if (rgb_info_drawn) {
            clear_text_row(rgb_mode_y);
            clear_text_row(rgb_speed_y);
            rgb_info_drawn = false;
        }
    }

    // Move surface to lcd
    qp_surface_draw(lcd_surface, lcd, 0, 0, 0);
    qp_flush(lcd);

    return false;
}

// QK_BOOT: replace the layer name with a boot message before the jump to the
// bootloader. The display keeps showing it while in the bootloader. QK_BOOT
// runs on the half plugged into USB, which is the half showing the layer
// screen, so the font is already loaded there.
bool shutdown_user(bool jump_to_bootloader) {
    if (jump_to_bootloader && Retron27 != NULL) {
        // The key press would normally wake a timed-out display on the next
        // housekeeping pass, which never comes once we jump
        backlight_wakeup();
        qp_power(lcd, true);

        const uint16_t name_y = text_row_y(LAYER_NAME_ROW);
        clear_text_row(name_y);
        qp_drawtext_recolor(lcd_surface, 5, name_y, Retron27, boot, HSV_RED,
                            HSV_BLACK);
        qp_surface_draw(lcd_surface, lcd, 0, 0, 0);
        qp_flush(lcd);
    }

    return true;
}
