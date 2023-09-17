#include <SDL.h>
#include "controller.h"

static SDL_Event e;

static bool8 quit = false;

bool8 g_key_pressed[NUM_CONTROLLER_INPUTS] = { 0 };
bool8 g_controller_quit = false;

// stores metrics for 
static s8 s_key_counters[NUM_CONTROLLER_INPUTS];

// TODO handle this in opconfig instead so user can edit keymappings
static SDL_Scancode key_mappings[] = {
    [INPUT_NONE] = 0,
    [INPUT_CONFIRM] = SDL_SCANCODE_X,
    [INPUT_CANCEL] = SDL_SCANCODE_Z,
    [INPUT_MENU] = SDL_SCANCODE_S,
    [INPUT_PLAYCARDS] = SDL_SCANCODE_A,
    [INPUT_PAGEUP] = SDL_SCANCODE_Q,
    [INPUT_PAGEDOWN] = SDL_SCANCODE_W,
    [INPUT_UP] = SDL_SCANCODE_UP,
    [INPUT_DOWN] = SDL_SCANCODE_DOWN,
    [INPUT_LEFT] = SDL_SCANCODE_LEFT,
    [INPUT_RIGHT] = SDL_SCANCODE_RIGHT,
    [INPUT_PAUSE] = SDL_SCANCODE_SPACE,
    [INPUT_SPECIAL] = SDL_SCANCODE_T,
};

static void handle_key_input(const u8 *SDL_keystates);

void set_key_counter(const u8 key, const s8 count) {
    s_key_counters[key] = count;
}

const s8 get_key_counter(const u8 key) {
    return s_key_counters[key];
}

void controller_input_update(void) {
    for (u8 i = INPUT_CONFIRM; i < NUM_CONTROLLER_INPUTS; ++i) {
        g_key_pressed[i] = false;
        if (s_key_counters[i] < 0) {
            s_key_counters[i]++;
        }
    }

    while(SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            g_controller_quit = true;
            return;
        }
    }

    handle_key_input(SDL_GetKeyboardState(NULL));
}

// To do map opconfig
static void handle_key_input(const u8 *keystates) {
    for (u8 i = INPUT_CONFIRM; i < NUM_CONTROLLER_INPUTS; ++i) {
        if (keystates[key_mappings[i]]) {
            g_key_pressed[i] = true;
            if (s_key_counters[i] >= 0 && s_key_counters[i] < 127) {
                s_key_counters[i]++;
            }
        }
        // clear input if it isn't pressed this frame
        if (!g_key_pressed[i] && s_key_counters[i] > 0) {
            s_key_counters[i] = 0;
        }
    }
}
