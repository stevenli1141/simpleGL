#ifndef GUARD_SCREEN_EFFECTS_H
#define GUARD_SCREEN_EFFECTS_H

#include "types.h"

#define COLOR_NONE          0x0
#define COLOR_BLACK         0x1
#define COLOR_WHITE         0x2
#define COLOR_RED           0x3
#define COLOR_GREEN         0x4
#define COLOR_BLUE          0x5
#define COLOR_YELLOW        0x6
#define COLOR_MAGENTA       0x7
#define COLOR_CYAN          0x8
#define COLOR_DARKRED       0x9
#define COLOR_ICE           0xA
#define COLOR_TEAL          0xB
#define COLOR_PINK          0xC
#define COLOR_ORANGE        0xD

extern const float g_fade_colors[][3];
extern u8 g_fade;
extern u8 g_fade_amount;
extern u8 g_fade_period;
extern u8 g_fade_start;

#endif
