#ifndef GUARD_LIGHTMAPS_H
#define GUARD_LIGHTMAPS_H

#include "types.h"

#define LIGHTMAP_NONE                       0
#define MAX_LIGHTMAP_ID                     32

#define LIGHTMAP_WIDTH                      1024

#define MAX_LIGHTMAP_CAPACITY               8

struct lightmap_data {
    const u8 *bytes;
};

extern void init_lightmaps(void);
extern void clean_lightmaps(void);
extern void load_lightmaps(const u8 n, const u32 *lightmaps);
extern void use_lightmap(const u32 level, const u32 cur_lightmap_index);

#endif
