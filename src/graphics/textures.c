#include <GL/glew.h>
#include "graphics/images.h"
#include "graphics/textures.h"
#include "log.h"

#include <stdio.h>

static u32 textures[MAX_TEXTURE_ID];
static u32 textures_norm[MAX_TEXTURE_NORM_ID];
static u32 textures_spec[MAX_TEXTURE_SPEC_ID];
static u32 textures_emit[MAX_TEXTURE_EMIT_ID];
static u32 textures_interface[MAX_TEXTURE_INTERFACE_ID];
static u32 textures_font[MAX_TEXTURE_FONT_ID];

static u32 *s_tex_gl_ptrs[NUM_TEXTURE_TYPES] = {
    [TEXTURE_TYPE_DIFF] = textures,
    [TEXTURE_TYPE_NORM] = textures_norm,
    [TEXTURE_TYPE_SPEC] = textures_spec,
    [TEXTURE_TYPE_EMIT] = textures_emit,
    [TEXTURE_TYPE_INTERFACE] = textures_interface,
    [TEXTURE_TYPE_FONT] = textures_font
};

static const u16 s_tex_max[NUM_TEXTURE_TYPES] = {
    [TEXTURE_TYPE_DIFF] = MAX_TEXTURE_ID,
    [TEXTURE_TYPE_NORM] = MAX_TEXTURE_NORM_ID,
    [TEXTURE_TYPE_SPEC] = MAX_TEXTURE_SPEC_ID,
    [TEXTURE_TYPE_EMIT] = MAX_TEXTURE_EMIT_ID,
    [TEXTURE_TYPE_INTERFACE] = MAX_TEXTURE_INTERFACE_ID,
    [TEXTURE_TYPE_FONT] = MAX_TEXTURE_FONT_ID
};


static const u8 s_texture_white[] = { 0xFF, 0xFF, 0xFF, 0xFF };
static const u32 s_texture_white_len = 1;

extern const struct texture_data g_texture_byte_data[];

extern const struct texture_data g_texture_norm_byte_data[];

extern const struct texture_data g_texture_spec_byte_data[];

extern const struct texture_data g_texture_emit_byte_data[];

extern const struct texture_data g_texture_interface_byte_data[];

const struct texture_data g_texture_font_byte_data[MAX_TEXTURE_FONT_ID] = {
    [TEXTURE_FONT_LUCIDAS] = { g_img_fonts_lucidas, 512 },
};

static const struct texture_data *s_tex_data_ptrs[NUM_TEXTURE_TYPES] = {
    [TEXTURE_TYPE_DIFF] = g_texture_byte_data,
    [TEXTURE_TYPE_NORM] = g_texture_norm_byte_data,
    [TEXTURE_TYPE_SPEC] = g_texture_spec_byte_data,
    [TEXTURE_TYPE_EMIT] = g_texture_emit_byte_data,
    [TEXTURE_TYPE_INTERFACE] = g_texture_interface_byte_data,
    [TEXTURE_TYPE_FONT] = g_texture_font_byte_data
};

// Load texture on demand
void use_texture(const u8 type, const u32 level, const u32 texture) {
    if (!s_tex_gl_ptrs[type][texture]) {
        load_texture(type, texture);
    }
    glActiveTexture(level);
    glBindTexture(GL_TEXTURE_2D, s_tex_gl_ptrs[type][texture]);
}

void init_textures(void) {
}

void clean_textures(void) {
    glDeleteTextures(MAX_TEXTURE_ID, textures);
    glDeleteTextures(MAX_TEXTURE_NORM_ID, textures_norm);
    glDeleteTextures(MAX_TEXTURE_SPEC_ID, textures_spec);
    glDeleteTextures(MAX_TEXTURE_EMIT_ID, textures_emit);
    glDeleteTextures(MAX_TEXTURE_INTERFACE_ID, textures_interface);
    glDeleteTextures(MAX_TEXTURE_FONT_ID, textures_font);
}

#define SUPPORTS_MIPMAP(type) (type <= TEXTURE_TYPE_EMIT || type == TEXTURE_TYPE_FONT || type == TEXTURE_TYPE_PARTICLE)
#define TEXTURE_WRAP_TYPE(type) (type <= TEXTURE_TYPE_EMIT ? GL_REPEAT : GL_CLAMP)
#define TEXTURE_FILTER_TYPE(type) (SUPPORTS_MIPMAP(type) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR)

void load_texture(const u8 type, const u32 id) {
    if (s_tex_data_ptrs[type][id].bytes == NULL) {
        return;
    }
    LOG_INFO("Loading texture type: %d id: %d\n", type, id)
    const u32 width = s_tex_data_ptrs[type][id].width;
    const u32 height = type == TEXTURE_TYPE_INTERFACE ? s_tex_data_ptrs[type][id].height : s_tex_data_ptrs[type][id].width;
    glGenTextures(1, s_tex_gl_ptrs[type] + id);
    glBindTexture(GL_TEXTURE_2D, s_tex_gl_ptrs[type][id]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, TEXTURE_WRAP_TYPE(type));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, TEXTURE_WRAP_TYPE(type));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, TEXTURE_FILTER_TYPE(type));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, TEXTURE_FILTER_TYPE(type));
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, s_tex_data_ptrs[type][id].bytes);
    if (SUPPORTS_MIPMAP(type)) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }
}

void unload_textures(const u8 type) {
    glDeleteTextures(s_tex_max[type], s_tex_gl_ptrs[type]);
    for (u8 i = 0; i < s_tex_max[type]; ++i) {
        s_tex_gl_ptrs[type][i] = 0;
    }
}
