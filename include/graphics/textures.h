#ifndef GUARD_TEXTURE_H
#define GUARD_TEXTURE_H

#include "types.h"

#define TEXTURE_NONE                    0
#define MAX_TEXTURE_ID                  128

#define TEXTURE_NORM_NONE               0
#define MAX_TEXTURE_NORM_ID             128

#define TEXTURE_SPEC_NONE               0
#define MAX_TEXTURE_SPEC_ID             128

#define TEXTURE_EMIT_NONE               0
#define MAX_TEXTURE_EMIT_ID             32

#define TEXTURE_INTERFACE_NONE          0
#define MAX_TEXTURE_INTERFACE_ID        128

#define TEXTURE_FONT_NONE               0
#define TEXTURE_FONT_LUCIDAS            1
#define MAX_TEXTURE_FONT_ID             2

#define TEXTURE_TYPE_DIFF           0
#define TEXTURE_TYPE_NORM           1
#define TEXTURE_TYPE_SPEC           2
#define TEXTURE_TYPE_EMIT           3
#define TEXTURE_TYPE_INTERFACE      4
#define TEXTURE_TYPE_FONT           5
#define TEXTURE_TYPE_PARTICLE       6
#define NUM_TEXTURE_TYPES           8

struct texture_data {
    const u8* bytes;
    const u32 width;
    const u32 height;
};

extern void use_texture(const u8 type, const u32 level, const u32 texture);

extern void init_textures(void);
extern void load_texture(const u8 type, const u32 id);
extern void unload_textures(const u8 type);

extern void clean_textures(void);

#endif
