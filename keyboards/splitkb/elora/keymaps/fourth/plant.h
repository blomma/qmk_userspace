#pragma once

// The sunflower on the half plugged into USB. It grows from a seed as you
// type, blooms, then drops its petals and starts over from a new seed. It
// sways in the wind, and every key press gives it a nudge.

// Counts a key press towards growing the plant. Call on every key press.
void plant_count_keypress(void);

// Moves the animation on and shows the plant. Call on every OLED pass.
void plant_render(void);
