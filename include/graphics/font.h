#ifndef GUARD_FONT_H
#define GUARD_FONT_H

#define FONT_DEFAULT        0
#define FONT_LUCIDAS        1
#define FONT_LUCIDAS_B      2
#define FONT_LUDICAS_I      3
#define FONT_OPTIMA         4
#define FONT_OPTIMA_M       5
#define FONT_OPTIMA_B       6
#define FONT_OPTIMA_I       7
#define NUM_FONTS           8

#define MAX_TEXT_LENGTH     512

#define TEXT_COLOR_NONE     0
#define TEXT_COLOR_WHITE    1
#define TEXT_COLOR_BLACK    2
#define TEXT_COLOR_GRAY     3
#define TEXT_COLOR_RED      4
#define TEXT_COLOR_GREEN    5
#define TEXT_COLOR_BLUE     6
#define TEXT_COLOR_YELLOW   7
#define TEXT_COLOR_CYAN     8
#define TEXT_COLOR_MAGENTA  9
#define TEXT_COLOR_ORANGE   10
#define TEXT_COLOR_ICE      11
#define TEXT_COLOR_TEAL     12
#define TEXT_COLOR_BROWN    13
#define TEXT_COLOR_DARKGRAY 14
#define NUM_TEXT_COLORS     16

#define TEXT_VALIGN_CENTER  1
#define TEXT_HALIGN_CENTER  2
#define TEXT_VALIGN_BOTTOM  4
#define TEXT_HALIGN_RIGHT   8

// Font size 32 is defined as taking up 1/10 of the screen height and 1/25 of the screen width (screen w and h is 2 in openGL)
// Base texture source width is 40px
#define SIZE32_WIDTH 0.08
#define SIZE32_HEIGHT 0.2
#define FONT_TEXTURE_WIDTH 512.0
#define TEXT_HORIZONTAL_SPACING 0.1

#define LINE_DISTANCE(fontsize) (fontsize * SIZE32_HEIGHT / 32.0)

#endif
