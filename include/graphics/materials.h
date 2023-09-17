#ifndef GUARD_MATERIALS_H
#define GUARD_MATERIALS_H

#include "types.h"

#define MAT_EXAMPLEMMETALPAINT      1
#define MAT_EXAMPLEMMETALBRUSHED    2
#define MAT_EXAMPLEMMETALPAINTCHIP  3
#define MAT_EXAMPLEMPOSTER          4
#define MAT_EXAMPLEMWOODPARQUET     5
#define MAT_EXAMPLEMLEATHERGREEN    6
#define MAT_EXAMPLEMLIGHT           7
#define MAT_EXAMPLEMMONITORS        8
#define MAT_POINTPARTICLE           128
#define NUM_MATERIALS               256

struct material_data {
    u8 shader;
    bool8 translucent;
    u16 texture_diff;
    u16 texture_norm;
    u16 texture_spec;
    u16 texture_ao;
    u16 texture_emit;
    float emissive_intensity;
    // const struct shader_params *shader_params;
};

extern const struct material_data g_mat_data[];

extern const u16 get_material_shader(const u16 material_id);
extern const bool8 is_translucent(const u16 material_id);
extern const u32 get_material_diff_texture(const u16 material_id);
extern const u32 get_material_emit_texture(const u16 material_id);
extern const float get_material_emit_intensity(const u16 material_id);
extern void use_material(u16 material_id);
extern void load_material_textures(const u16 material_id);

#endif
