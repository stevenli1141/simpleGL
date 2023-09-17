#include <GL/glew.h>
#include "graphics/images.h"
#include "graphics/lightmaps.h"
#include "graphics/shaders.h"

static u32 s_lightmaps[MAX_LIGHTMAP_CAPACITY];

static const u8 s_no_lightmap[] = { 0, 0, 0, 0 };
static const u32 s_no_lightmap_len = 1;

static struct lightmap_data s_lightmap_data[MAX_LIGHTMAP_ID] = {
    [0] = { s_no_lightmap  }
};

void init_lightmaps() {
    glGenTextures(MAX_LIGHTMAP_CAPACITY, s_lightmaps);
    // bind empty lightmap
    glBindTexture(GL_TEXTURE_2D, s_lightmaps[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, s_lightmap_data[0].bytes);
    glGenerateMipmap(GL_TEXTURE_2D);
}

void clean_lightmaps() {
    glDeleteTextures(MAX_LIGHTMAP_CAPACITY, s_lightmaps);
}

void load_lightmaps(const u8 n, const u32 *lightmaps) {
    clean_lightmaps();
    init_lightmaps();
    for (u8 i = 1; i < n; ++i) {
        const u32 lightmap = lightmaps[i];
        if (!lightmap || s_lightmap_data[lightmap].bytes == NULL) {
            s_lightmaps[i] = 0;
            continue;
        }
        glBindTexture(GL_TEXTURE_2D, s_lightmaps[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, LIGHTMAP_WIDTH, LIGHTMAP_WIDTH, 0, GL_RGBA, GL_UNSIGNED_BYTE, s_lightmap_data[lightmap].bytes);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
}

// Use the lightmap loaded at the current index specified
void use_lightmap(const u32 level, const u32 cur_lightmap_index) {
    glActiveTexture(level);
    glBindTexture(GL_TEXTURE_2D, s_lightmaps[cur_lightmap_index]);
    set_uniform_1i_value(SHADER_LIT_LIGHTMAP, 4);
}
