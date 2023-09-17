#ifndef GUARD_CONTROLLER_H
#define GUARD_CONTROLLER_H

#include "types.h"

// Handles all controller input from SDL

#define INPUT_NONE 0
#define INPUT_CONFIRM 1
#define INPUT_CANCEL 2
#define INPUT_MENU 3
#define INPUT_PLAYCARDS 4
#define INPUT_PAGEUP 5
#define INPUT_PAGEDOWN 6
#define INPUT_UP 7
#define INPUT_DOWN 8
#define INPUT_LEFT 9
#define INPUT_RIGHT 10
#define INPUT_PAUSE 11
#define INPUT_QUIT 12
#define INPUT_DEBUG 13
#define INPUT_SPECIAL 14
#define NUM_CONTROLLER_INPUTS 16

extern bool8 g_key_pressed[];
extern bool8 g_controller_quit;

// this is used to clear a key input optionally setting it into negative to get a longer delay
// the counter for each key is incremented every cycle if it is negative to clear the delay
// at or above 0, it will increment if it is pressed and below max value, if it isn't pressed it resets to 0
// this offers a way to optionally reset of keys, set delays for next time, and get press duration
// e.g. to implement initial delay, test that the key is either 1 or is above a certain threshold (within 127)
extern void set_key_counter(const u8 key, const s8 count);
extern const s8 get_key_counter(const u8 key);

extern void controller_input_update(void);

#endif
