#include <GL/glew.h>
#include <string.h>
#include "graphics/font.h"
#include "graphics/graphics.h"
#include "graphics/shaders.h"
#include "graphics/textures.h"
#include "log.h"
#include "utils.h"

// Index is c - 32 to avoid the first 32 ASCII chars
static const u16 s_font_data[NUM_FONTS][96][4] = {
    [FONT_LUCIDAS] = {
        [0] = { 0, 0, 20, 1 }, // space
        [1] = { 9, 347, 15, 403 }, // !
        [2] = { 26, 347, 41, 403 }, // double quote
        [3] = { 46, 347, 81, 403 }, // #
        [4] = { 89, 347, 112, 403 }, // $
        [5] = { 120, 347, 157, 403 }, // %
        [6] = { 161, 347, 196, 403 }, // &
        [7] = { 203, 347, 210, 403 }, // single quote
        [8] = { 218, 347, 232, 403 }, // (
        [9] = { 237, 347, 250, 403 }, // )
        [10] = { 258, 347, 279, 403 }, // *
        [11] = { 286, 347, 317, 403 }, // +
        [12] = { 473, 347, 481, 403 }, // ,
        [13] = { 324, 347, 337, 403 }, // -
        [14] = { 346, 347, 354, 403 }, // .
        [15] = { 361, 347, 388, 403 }, // /
        [16] = { 6, 13, 35, 69 }, // 0
        [17] = { 48, 13, 60, 69 }, // 1
        [18] = { 80, 13, 106, 69 }, // 2
        [19] = { 119, 13, 144, 69 }, // 3
        [20] = { 154, 13, 183, 69 }, // 4
        [21] = { 194, 13, 218, 69 }, // 5
        [22] = { 228, 13, 258, 69 }, // 6
        [23] = { 268, 13, 294, 69 }, // 7
        [24] = { 304, 13, 331, 69 }, // 8
        [25] = { 340, 13, 368, 69 }, // 9
        [26] = { 397, 347, 403, 403 }, // :
        [27] = { 416, 347, 429, 403 }, // ;
        [28] = { 434, 347, 464, 403 }, // <
        [29] = { 6, 414, 36, 470 }, // =
        [30] = { 42, 414, 73, 470 }, // >
        [31] = { 78, 414, 100, 470 }, // ?
        [32] = { 105, 414, 149, 470 }, // @
        [33] = { 3, 80, 42, 136 }, // A
        [34] = { 49, 80, 74, 136 }, // B
        [35] = { 80, 80, 114, 136 }, // C
        [36] = { 124, 80, 158, 136 }, // D
        [37] = { 164, 80, 192, 136 }, // E
        [38] = { 200, 80, 223, 136 }, // F
        [39] = { 230, 80, 263, 136 }, // G
        [40] = { 274, 80, 305, 136 }, // H
        [41] = { 317, 80, 323, 136 }, // I
        [42] = { 325, 80, 343, 136 }, // J
        [43] = { 354, 80, 385, 136 }, // K
        [44] = { 393, 80, 418, 136 }, // L
        [45] = { 425, 80, 463, 136 }, // M
        [46] = { 475, 80, 507, 136 }, // N
        [47] = { 6, 147, 45, 203 }, // O
        [48] = { 54, 147, 79, 203 }, // P
        [49] = { 85, 147, 127, 203 }, // Q
        [50] = { 133, 147, 163, 203 }, // R
        [51] = { 167, 147, 193, 203 }, // S
        [52] = { 197, 147, 232, 203 }, // T
        [53] = { 239, 147, 268, 203 }, // U
        [54] = { 276, 147, 311, 203 }, // V
        [55] = { 314, 147, 362, 203 }, // W
        [56] = { 364, 147, 398, 203 }, // X
        [57] = { 401, 147, 435, 203 }, // Y
        [58] = { 440, 147, 470, 203 }, // Z
        [59] = { 158, 414, 170, 470 }, // [
        [60] = { 173, 414, 200, 470 }, // backslash
        [61] = { 205, 414, 216, 470 }, // ]
        [62] = { 226, 414, 256, 470 }, // ^
        [63] = { 262, 414, 286, 470 }, // _
        [64] = { 300, 414, 312, 470 }, // `
        [65] = { 6, 214, 33, 270 }, // a
        [66] = { 41, 214, 68, 270 }, // b
        [67] = { 76, 214, 99, 270 }, // c
        [68] = { 107, 214, 134, 270 }, // d
        [69] = { 144, 214, 169, 270 }, // e
        [70] = { 176, 214, 197, 270 }, // f
        [71] = { 200, 214, 227, 270 }, // g
        [72] = { 239, 214, 264, 270 }, // h
        [73] = { 276, 214, 282, 270 }, // i
        [74] = { 284, 214, 301, 270 }, // j
        [75] = { 313, 214, 340, 270 }, // k
        [76] = { 348, 214, 354, 270 }, // l
        [77] = { 366, 214, 409, 270 }, // m
        [78] = { 420, 214, 445, 270 }, // n
        [79] = { 6, 281, 35, 337 }, // o
        [80] = { 44, 281, 71, 337 }, // p
        [81] = { 79, 281, 106, 337}, // q
        [82] = { 118, 281, 135, 337 }, // r
        [83] = { 143, 281, 163, 337 }, // s
        [84] = { 171, 281, 188, 337 }, // t
        [85] = { 197, 281, 222, 337 }, // u
        [86] = { 229, 281, 258, 337 }, // v
        [87] = { 260, 281, 303, 337 }, // w
        [88] = { 307, 281, 337, 337 }, // x
        [89] = { 341, 281, 370, 337 }, // y
        [90] = { 375, 281, 401, 337 }, // z
    }
};

static u8 s_kerning[][3] = {
    { 'A', 'T', 2 },
    { 'A', 'V', 2 },
    { 'A', 'W', 2 },
    { 'A', 'Y', 1 },
    { 'F', 'A', 1 },
    { 'L', 'T', 2 },
    { 'L', 'V', 2 },
    { 'L', 'W', 1 },
    { 'L', 'Y', 2 },
    { 'P', 'A', 1 },
    { 'T', 'A', 2 },
    { 'V', 'A', 2 },
    { 'W', 'A', 1 },
};

static float s_text_colors[NUM_TEXT_COLORS][4] = {
    [TEXT_COLOR_NONE] = { 0.f, 0.f, 0.f, 0.f },
    [TEXT_COLOR_WHITE] = { 1.f, 1.f, 1.f, 1.f },
    [TEXT_COLOR_BLACK] = { 0.f, 0.f, 0.f, 1.f },
    [TEXT_COLOR_GRAY] = { 0.5f, 0.5f, 0.5f, 1.f },
    [TEXT_COLOR_RED] = { 1.f, 0.f, 0.f, 1.f },
    [TEXT_COLOR_GREEN] = { 0.f, 1.f, 0.f, 1.f },
    [TEXT_COLOR_BLUE] = { 0.f, 0.f, 1.f, 1.f },
    [TEXT_COLOR_YELLOW] = { 1.f, 1.f, 0.f, 1.f },
    [TEXT_COLOR_CYAN] = { 0.f, 1.f, 1.f, 1.f },
    [TEXT_COLOR_MAGENTA] = { 1.f, 0.f, 1.f, 1.f },
    [TEXT_COLOR_ORANGE] = { 1.f, 0.5f, 0.f, 1.f },
    [TEXT_COLOR_ICE] = { 0.5f, 1.f, 1.f, 1.f },
    [TEXT_COLOR_TEAL] = { 0.f, 1.f, 0.75f, 1.f },
    [TEXT_COLOR_BROWN] = { 0.4f, 0.25f, 0.f, 1.f },
    [TEXT_COLOR_DARKGRAY] = { 0.25f, 0.25f, 0.25, 1.f }
};

static const u32 s_font_textures[NUM_FONTS] = { TEXTURE_FONT_NONE, TEXTURE_FONT_LUCIDAS };

static u32 s_text_vao;
static u32 s_text_vertex_buffer;
static float s_text_vertices[MAX_TEXT_LENGTH * 30];

static float add_char_to_vertex_buffer(float *vertices, const u8 c, const u8 font, const u8 size, const float cx, const float cy, const float z);

void init_text(void) {
    glGenVertexArrays(1, &s_text_vao);
    glGenBuffers(1, &s_text_vertex_buffer);
    glBindVertexArray(s_text_vao);
    glBindBuffer(GL_ARRAY_BUFFER, s_text_vertex_buffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), NULL);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
}

void clean_text(void) {
    glDeleteVertexArrays(1, &s_text_vao);
    glDeleteBuffers(1, &s_text_vertex_buffer);
}

#define FONT_TEXTURE_BASE_WIDTH 40.0

void render_text(const u8 *text, const u16 len, const float x, const float y, const float z, const u8 font, const u8 size, const u8 color) {
    if (text == NULL) {
        return;
    }
    if (s_font_data[font] == NULL) {
        LOG_ERROR("Unsupported font %d\n", font)
        return;
    }
    float cx = x;
    const float h = size * SIZE32_HEIGHT / 32.f;
    u32 v_index = 0;
    for (u8 i = 0; i < len && i < MAX_TEXT_LENGTH; ++i) {
        if (text[i] == 0) {
            break;
        }
        const float w = add_char_to_vertex_buffer(s_text_vertices + v_index, text[i], font, size, cx, y, z);
        v_index += 30;
        cx += w + TEXT_HORIZONTAL_SPACING * size * SIZE32_WIDTH / 32.f;
    }
    if (!v_index) {
        return;
    }
    use_shader(TEXT_SHADER_PROGRAM);
    use_texture(TEXTURE_TYPE_FONT, GL_TEXTURE0, s_font_textures[font]);
    set_uniform_v4_value(SHADER_TEXT_COLOR, s_text_colors[color][0], s_text_colors[color][1], s_text_colors[color][2], s_text_colors[color][3]);
    glBindVertexArray(s_text_vao);
    glBindBuffer(GL_ARRAY_BUFFER, s_text_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, v_index * sizeof(float), s_text_vertices, GL_STATIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, v_index / 5);
}

static void line_break_and_align(float *vertices, const u32 last_line_start, const u32 v_index, const float x, const float y, const float h, const float line_height, const u8 alignment, float *cx, float *cy) {
    const float w = *cx - x;
    // Last line needs to be centered
    if (alignment & TEXT_HALIGN_CENTER) {
        for (u32 i = last_line_start; i < v_index; i += 5) {
            vertices[i] -= w/2;
        }
    } else if (alignment & TEXT_HALIGN_RIGHT) {
        for (u32 i = last_line_start; i < v_index; i += 5) {
            vertices[i] -= w;
        }
    }
    // All previous lines need to be shifted up
    if (alignment & TEXT_VALIGN_CENTER) {
        for (u32 i = 0; i < v_index; i += 5) {
            vertices[i+1] += h/2;
        }
    }
    *cx = x;
    *cy -= (alignment & TEXT_VALIGN_CENTER) ? (line_height * h / 2) : (line_height * h);
}

// This will center the last line horizontally while avoiding vertical movement
static void align_last_line(float *vertices, const float len, const float x, const u8 alignment, const float cx) {
    const float w = cx - x;
    if (alignment & TEXT_HALIGN_CENTER) {
        for (u32 i = 0; i < len; i += 5) {
            vertices[i] -= w/2;
        }
    } else if (alignment & TEXT_HALIGN_RIGHT) {
        for (u32 i = 0; i < len; i += 5) {
            vertices[i] -= w;
        }
    }
}


// Renders text wrapped to at most the width provided. Line height is a multiplier of calculated height, e.g. line height of 2.0 leaves a gap equal to text height
// If valign is true, the text is centered around the y given. If halign is true, the text is centered around the x given
void render_text_wrapped(const u8 *text, const u16 len, const float x, const float y, const float z, const float width, const float line_height, const u8 font, const u8 size, const u8 alignment, const u8 color) {
    if (s_font_data[font] == NULL) {
        LOG_ERROR("Unsupported font %d\n", font)
        return;
    }
    const float h = size * SIZE32_HEIGHT / 32.f;
    float cx = x;
    float cy = (alignment & TEXT_VALIGN_CENTER) ? y + h/2 : y;
    
    u32 v_index = 0;
    u32 visible_v_index = 0;
    u32 last_line_index = 0;
    for (u8 i = 0; i < MAX_TEXT_LENGTH; ++i) {
        if (text[i] == 0 || text[i] > 127) {
            break;
        }
        if (text[i] == '\n') {
            line_break_and_align(s_text_vertices, last_line_index, v_index, x, y, h, line_height, alignment, &cx, &cy);
            last_line_index = v_index;
            continue;
        }
        if (text[i] == ' ') {
            if (cx > x + width) {
                line_break_and_align(s_text_vertices, last_line_index, v_index, x, y, h, line_height, alignment, &cx, &cy);
                last_line_index = v_index;
            } else if (alignment & TEXT_HALIGN_RIGHT) {
                cx -= 0.5 * size * SIZE32_WIDTH / 32.f;
            } else {
                cx += 0.5 * size * SIZE32_WIDTH / 32.f;
            }
            continue;
        }

        const float w = add_char_to_vertex_buffer(s_text_vertices + v_index, text[i], font, size, cx, cy, z);
        cx += w + TEXT_HORIZONTAL_SPACING * size * SIZE32_WIDTH / 32.f;
        v_index += 30;
        if (i < len) {
            // we have to compute the width and height of everything to center properly, so we use this to hide everything past the specified length
            visible_v_index += 30;
        }
    }
    if (!visible_v_index) {
        return;
    }
    // center the last line
    align_last_line(s_text_vertices + last_line_index, v_index - last_line_index, x, alignment, cx);
    use_shader(TEXT_SHADER_PROGRAM);
    use_texture(TEXTURE_TYPE_FONT, GL_TEXTURE0, s_font_textures[font]);
    set_uniform_v4_value(SHADER_TEXT_COLOR, s_text_colors[color][0], s_text_colors[color][1], s_text_colors[color][2], s_text_colors[color][3]);
    glBindVertexArray(s_text_vao);
    glBindBuffer(GL_ARRAY_BUFFER, s_text_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, visible_v_index * sizeof(float), s_text_vertices, GL_STATIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, visible_v_index / 5);
}

static float add_char_to_vertex_buffer(float *vertices, const u8 c, const u8 font, const u8 size, const float cx, const float cy, const float z) {
    const float u1 = s_font_data[font][c - 32][0] / FONT_TEXTURE_WIDTH;
    const float v1 = s_font_data[font][c - 32][1] / FONT_TEXTURE_WIDTH;
    const float u2 = s_font_data[font][c - 32][2] / FONT_TEXTURE_WIDTH;
    const float v2 = s_font_data[font][c - 32][3] / FONT_TEXTURE_WIDTH;
    const float w = (size * SIZE32_WIDTH / 32.f) * (u2 - u1) * FONT_TEXTURE_WIDTH / FONT_TEXTURE_BASE_WIDTH;
    const float h = size * SIZE32_HEIGHT / 32.f;
    const float v[30] = {
        cx, cy, z, u1, v1,
        cx, cy-h, z, u1, v2,
        cx+w, cy, z, u2, v1,
        cx+w, cy, z, u2, v1,
        cx, cy-h, z, u1, v2,
        cx+w, cy-h, z, u2, v2
    };
    memcpy(vertices, v, 30 * sizeof(float));
    return w;
}
