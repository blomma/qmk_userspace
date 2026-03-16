
// Copyright 2024 splitkb.com (support@splitkb.com)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <stdint.h>

// All values (including hue) are scaled to 0-255
#define HSV_CAPS_ON 17, 191, 245
#define HSV_LAYER_0 0, 0, 160
#define HSV_LAYER_1 23, 89, 255
#define HSV_LAYER_2 43, 71, 255
#define HSV_LAYER_3 0, 82, 255
#define HSV_LAYER_4 77, 64, 255
#define HSV_LAYER_UNDEF 0, 255, 255
#define HSV_LAYER_6 131, 99, 255
#define HSV_LAYER_7 154, 94, 255
#define HSV_LAYER_5 176, 77, 255

void    b_draw_grid(void);
void    b_update_grid(void);
void    b_init_grid(void);
void    b_add_cell_cluster(void);
uint8_t b_get_random_color_index(void);
