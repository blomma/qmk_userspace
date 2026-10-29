#pragma once

// Registers the split transaction that sends the selected host to the half
// showing the layer screen. Call from keyboard_post_init_user.
void display_init(void);

// Counts a key press towards growing the plant. Call on every key press.
void display_count_keypress(void);
