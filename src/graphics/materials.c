#include "GL/glew.h"
#include "graphics/materials.h"
#include "graphics/shaders.h"
#include "graphics/textures.h"

const bool8 is_translucent(const u16 material_id) {
    return g_mat_data[material_id].translucent;
}

const u16 get_material_shader(const u16 material_id) {
    return g_mat_data[material_id].shader;
}

const u32 get_material_diff_texture(const u16 material_id) {
    return g_mat_data[material_id].texture_diff;
}

const u32 get_material_emit_texture(const u16 material_id) {
    return g_mat_data[material_id].texture_emit;
}

const float get_material_emit_intensity(const u16 material_id) {
    return g_mat_data[material_id].emissive_intensity;
}

void use_material(const u16 material_id) {
    // TODO loop through the possible texture options (diff, norm, spec/ao, emit) to see which ones are supported by the material's shader
    // Right now materials only supports lit shader
    use_texture(TEXTURE_TYPE_DIFF, TEXTURE_DIFF_LEVEL, g_mat_data[material_id].texture_diff);
    set_uniform_1i_value(SHADER_LIT_TEXTURE_DIFF, 0);
    use_texture(TEXTURE_TYPE_NORM, TEXTURE_NORM_LEVEL, g_mat_data[material_id].texture_norm);
    set_uniform_1i_value(SHADER_LIT_TEXTURE_NORM, 1);
    use_texture(TEXTURE_TYPE_SPEC, TEXTURE_SPEC_LEVEL, g_mat_data[material_id].texture_spec);
    set_uniform_1i_value(SHADER_LIT_TEXTURE_SPEC, 2);
    use_texture(TEXTURE_TYPE_EMIT, TEXTURE_EMIT_LEVEL, g_mat_data[material_id].texture_emit);
    set_uniform_1i_value(SHADER_LIT_TEXTURE_EMIT, 3);
    set_uniform_1f_value(SHADER_LIT_EMISSIVE_INTENSITY, g_mat_data[material_id].emissive_intensity);
}

void load_material_textures(const u16 material_id) {
    load_texture(TEXTURE_TYPE_DIFF, g_mat_data[material_id].texture_diff);
    load_texture(TEXTURE_TYPE_NORM, g_mat_data[material_id].texture_norm);
    load_texture(TEXTURE_TYPE_SPEC, g_mat_data[material_id].texture_spec);
    load_texture(TEXTURE_TYPE_EMIT, g_mat_data[material_id].texture_emit);
}
