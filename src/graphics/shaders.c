#include <GL/glew.h>
#include "graphics/shaders.h"
#include "log.h"
#include "types.h"

#define SHADER_TYPE(i) (i < DEFAULT_FRAG_SHADER) ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER

static u32 s_shaders[NUM_SHADERS];
static u32 s_shader_programs[NUM_SHADER_PROGRAMS];

u32 g_shader_uniforms[NUM_SHADER_UNIFORMS];

static const char *s_shader_sources[NUM_SHADERS] = {
    [NO_SHADER] = NULL,
    [DEFAULT_VERTEX_SHADER] = g_shader_default_vertex_src,
    [COLOR_VERTEX_SHADER] = g_shader_color_vertex_src,
    [UNLIT_VERTEX_SHADER] = g_shader_unlit_vertex_src,
    [LIT_VERTEX_SHADER] = g_shader_lit_vertex_src,
    [FB_VERTEX_SHADER] = g_shader_fb_vertex_src,
    [SPRITE_VERTEX_SHADER] = g_shader_sprite_vertex_src,
    [PARTICLE_VERTEX_SHADER] = g_shader_particle_vertex_src,
    [DYNAMIC_OBJ_VERTEX_SHADER] = g_shader_dynamic_obj_vertex_src,
    [DEFAULT_FRAG_SHADER] = g_shader_default_frag_src,
    [COLOR_FRAG_SHADER] = g_shader_color_frag_src,
    [UNLIT_FRAG_SHADER] = g_shader_unlit_frag_src,
    [LIT_FRAG_SHADER] = g_shader_lit_frag_src,
    [HDR_FILTER_FRAG_SHADER] = g_shader_hdr_filter_frag_src,
    [FB_FRAG_SHADER] = g_shader_fb_frag_src,    
    [GAUSSIAN_FRAG_SHADER] = g_shader_gaussian_frag_src,
    [BLOOM_FRAG_SHADER] = g_shader_bloom_frag_src,
    [TEXT_FRAG_SHADER] = g_shader_text_frag_src,
    [SPRITE_FRAG_SHADER] = g_shader_sprite_frag_src,
    [PARTICLE_FRAG_SHADER] = g_shader_particle_frag_src,
    [PARTICLE_HDR_FRAG_SHADER] = g_shader_particle_hdr_frag_src,
    [DYNAMIC_OBJ_FRAG_SHADER] = g_shader_dynamic_obj_frag_src,
    [DYNAMIC_OBJ_HDR_FRAG_SHADER] = g_shader_dynamic_obj_hdr_frag_src,
};

// Only support 2 shaders per program for now
static u32 s_shader_program_link_ids[NUM_SHADER_PROGRAMS][2] = {
    { DEFAULT_VERTEX_SHADER, DEFAULT_FRAG_SHADER },
    { COLOR_VERTEX_SHADER, COLOR_FRAG_SHADER },
    { UNLIT_VERTEX_SHADER, UNLIT_FRAG_SHADER },
    { LIT_VERTEX_SHADER, LIT_FRAG_SHADER },
    { LIT_VERTEX_SHADER, HDR_FILTER_FRAG_SHADER },
    { FB_VERTEX_SHADER, FB_FRAG_SHADER },
    { FB_VERTEX_SHADER, GAUSSIAN_FRAG_SHADER },
    { FB_VERTEX_SHADER, BLOOM_FRAG_SHADER },
    { FB_VERTEX_SHADER, TEXT_FRAG_SHADER },
    { SPRITE_VERTEX_SHADER, SPRITE_FRAG_SHADER },
    { PARTICLE_VERTEX_SHADER, PARTICLE_FRAG_SHADER },
    { PARTICLE_VERTEX_SHADER, PARTICLE_HDR_FRAG_SHADER },
    { DYNAMIC_OBJ_VERTEX_SHADER, DYNAMIC_OBJ_FRAG_SHADER },
    { DYNAMIC_OBJ_VERTEX_SHADER, DYNAMIC_OBJ_HDR_FRAG_SHADER }
};


// Stores some flags here
static u32 s_shader_parameters[NUM_SHADER_PROGRAMS][NUM_SHADER_OPTIONS] = {
    [NO_SHADER] = {},
    [COLOR_SHADER_PROGRAM] = {},
    [UNLIT_SHADER_PROGRAM] = {
        [SHADER_DIFF_UNIFORM] = SHADER_UNLIT_TEXTURE
    },
    [LIT_SHADER_PROGRAM] = {
        [SHADER_CAMERA_UNIFORM] = SHADER_LIT_CAMERA_INVERSE_TRANSFORM,
        [SHADER_DIFF_UNIFORM] = SHADER_LIT_TEXTURE_DIFF,
        [SHADER_NORM_UNIFORM] = SHADER_LIT_TEXTURE_NORM,
        [SHADER_AMBIENT_LIGHT_UNIFORM] = SHADER_LIT_AMBIENT_LIGHT,
        [SHADER_DIRECTION_LIGHT_UNIFORM] = SHADER_LIT_DIRECTION_LIGHT,
        [SHADER_DIRECTION_DIR_UNIFORM] = SHADER_LIT_DIRECTION_VEC,
        [SHADER_POINT_LOCS_UNIFORM] = SHADER_LIT_POINT_LOCS,
        [SHADER_POINT_LIGHTS_UNIFORM] = SHADER_LIT_POINT_LIGHTS,
        [SHADER_SPOT_LOCS_UNIFORM] = SHADER_LIT_SPOT_LOCS,
        [SHADER_SPOT_LIGHTS_UNIFORM] = SHADER_LIT_POINT_LIGHTS
    },
};

const u32 get_shader_uniform(u8 shader, u8 option) {
    if (shader > NUM_SHADER_PROGRAMS || option > NUM_SHADER_OPTIONS) {
        return 0;
    }
    return s_shader_parameters[shader][option];
}

// Compiles shader
static void create_shader(u32 *shader, const char *shader_src, GLenum shader_type) {
    s32 success;
    char log_info[512];
    *shader = glCreateShader(shader_type);
    glShaderSource(*shader, 1, &shader_src, NULL);
    glCompileShader(*shader);
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(*shader, 512, NULL, log_info);
        LOG_INFO("Failed to compile shader %d\n", *shader)
        LOG_INFO(log_info)
    }
}

// Links shader program given list of shaders
// Shaders must be compiled beforehand before being passed here
static void create_shader_program(u32 *shader_program_id, u32 *shaders, u8 num_shaders_per_program) {
    s32 success;
    char log_info[512];
    *shader_program_id = glCreateProgram();
    for (u8 i = 0; i < num_shaders_per_program; ++i) {
        glAttachShader(*shader_program_id, s_shaders[shaders[i]]);
    }
    glLinkProgram(*shader_program_id);
    glGetProgramiv(*shader_program_id, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(*shader_program_id, 512, NULL, log_info);
        LOG_INFO("Failed to compile shader program %d\n", *shader_program_id)
        LOG_INFO(log_info)
    }
}

// compile all shaders and link them
void init_shaders(void) {
    for (u8 i = 1; i < NUM_SHADERS; ++i) {
        create_shader(s_shaders + i, s_shader_sources[i], SHADER_TYPE(i));
    }
    for (u8 i = 0; i < NUM_SHADER_PROGRAMS; ++i) {
        create_shader_program(s_shader_programs + i, s_shader_program_link_ids[i], 2);
    }
    // get location of all uniforms
    glUseProgram(s_shader_programs[UNLIT_SHADER_PROGRAM]);
    g_shader_uniforms[SHADER_UNLIT_TRANSFORM] = glGetUniformLocation(s_shader_programs[UNLIT_SHADER_PROGRAM], "u_transform");
    g_shader_uniforms[SHADER_UNLIT_CAMERA_INVERSE_TRANSFORM] = glGetUniformLocation(s_shader_programs[UNLIT_SHADER_PROGRAM], "u_camera_inverse_transform");
    g_shader_uniforms[SHADER_UNLIT_TEXTURE] = glGetUniformLocation(s_shader_programs[UNLIT_SHADER_PROGRAM], "u_texture");
    g_shader_uniforms[SHADER_LIT_CAMERA_INVERSE_TRANSFORM] = glGetUniformLocation(s_shader_programs[LIT_SHADER_PROGRAM], "u_camera_inverse_transform");
    g_shader_uniforms[SHADER_HDR_FILTER_CAMERA] = glGetUniformLocation(s_shader_programs[HDR_FILTER_SHADER_PROGRAM], "u_camera_inverse_transform");
    // g_shader_uniforms[SHADER_LIT_CAMERA_DIR] = glGetUniformLocation(s_shader_programs[LIT_SHADER_PROGRAM], "u_camera_dir");
    g_shader_uniforms[SHADER_LIT_TEXTURE_DIFF] = glGetUniformLocation(s_shader_programs[LIT_SHADER_PROGRAM], "u_texture_diff");
    g_shader_uniforms[SHADER_LIT_TEXTURE_NORM] = glGetUniformLocation(s_shader_programs[LIT_SHADER_PROGRAM], "u_texture_norm");
    g_shader_uniforms[SHADER_LIT_TEXTURE_SPEC] = glGetUniformLocation(s_shader_programs[LIT_SHADER_PROGRAM], "u_texture_spec");
    // g_shader_uniforms[SHADER_LIT_TEXTURE_AO] = glGetUniformLocation(s_shader_programs[LIT_SHADER_PROGRAM], "u_texture_ao");
    g_shader_uniforms[SHADER_LIT_TEXTURE_EMIT] = glGetUniformLocation(s_shader_programs[LIT_SHADER_PROGRAM], "u_texture_emit");
    g_shader_uniforms[SHADER_LIT_EMISSIVE_INTENSITY] = glGetUniformLocation(s_shader_programs[LIT_SHADER_PROGRAM], "u_emissive_intensity");
    g_shader_uniforms[SHADER_LIT_LIGHTMAP] = glGetUniformLocation(s_shader_programs[LIT_SHADER_PROGRAM], "u_lightmap");
    g_shader_uniforms[SHADER_LIT_AMBIENT_LIGHT] = glGetUniformLocation(s_shader_programs[LIT_SHADER_PROGRAM], "u_ambient_light");
    g_shader_uniforms[SHADER_LIT_DIRECTION_VEC] = glGetUniformLocation(s_shader_programs[LIT_SHADER_PROGRAM], "u_direction_vec");
    g_shader_uniforms[SHADER_LIT_DIRECTION_LIGHT] = glGetUniformLocation(s_shader_programs[LIT_SHADER_PROGRAM], "u_direction_light");
    g_shader_uniforms[SHADER_LIT_POINT_LOCS] = glGetUniformLocation(s_shader_programs[LIT_SHADER_PROGRAM], "u_point_locs");
    g_shader_uniforms[SHADER_LIT_POINT_LIGHTS] = glGetUniformLocation(s_shader_programs[LIT_SHADER_PROGRAM], "u_point_lights");
    g_shader_uniforms[SHADER_LIT_SPOT_LOCS] = glGetUniformLocation(s_shader_programs[LIT_SHADER_PROGRAM], "u_spot_locs");
    g_shader_uniforms[SHADER_LIT_SPOT_LIGHTS] = glGetUniformLocation(s_shader_programs[LIT_SHADER_PROGRAM], "u_spot_lights");
    g_shader_uniforms[SHADER_LIT_SPOT_DIRS] = glGetUniformLocation(s_shader_programs[LIT_SHADER_PROGRAM], "u_spot_dirs");
    g_shader_uniforms[SHADER_LIT_SPOT_BOUNDS] = glGetUniformLocation(s_shader_programs[LIT_SHADER_PROGRAM], "u_spot_bounds");
    g_shader_uniforms[SHADER_HDR_FILTER_EMIT_INTENSITY] = glGetUniformLocation(s_shader_programs[HDR_FILTER_SHADER_PROGRAM], "u_emissive_intensity");
    g_shader_uniforms[SHADER_HDR_FILTER_EMIT_TEXTURE] = glGetUniformLocation(s_shader_programs[HDR_FILTER_SHADER_PROGRAM], "u_texture_emit");
    g_shader_uniforms[SHADER_GAUSSIAN_DIR] = glGetUniformLocation(s_shader_programs[GAUSSIAN_SHADER_PROGRAM], "u_blur_params");
    g_shader_uniforms[SHADER_BLOOM_EXPOSURE] = glGetUniformLocation(s_shader_programs[BLOOM_SHADER_PROGRAM], "u_exposure");
    g_shader_uniforms[SHADER_BLOOM_SCENE] = glGetUniformLocation(s_shader_programs[BLOOM_SHADER_PROGRAM], "u_texture");
    g_shader_uniforms[SHADER_BLOOM_HDR_MAP] = glGetUniformLocation(s_shader_programs[BLOOM_SHADER_PROGRAM], "u_hdr_bloom");
    g_shader_uniforms[SHADER_TEXT_COLOR] = glGetUniformLocation(s_shader_programs[TEXT_SHADER_PROGRAM], "u_text_color");
    g_shader_uniforms[SHADER_SPRITE_TEXTURE] = glGetUniformLocation(s_shader_programs[SPRITE_SHADER_PROGRAM], "u_texture");
    g_shader_uniforms[SHADER_SPRITE_CAMERA] = glGetUniformLocation(s_shader_programs[SPRITE_SHADER_PROGRAM], "u_camera_inverse_transform");
    g_shader_uniforms[SHADER_SPRITE_LIGHT] = glGetUniformLocation(s_shader_programs[SPRITE_SHADER_PROGRAM], "u_light");
    g_shader_uniforms[SHADER_SPRITE_FADE_COLOR] = glGetUniformLocation(s_shader_programs[SPRITE_SHADER_PROGRAM], "u_fade_color");
    g_shader_uniforms[SHADER_SPRITE_FADE_RATE] = glGetUniformLocation(s_shader_programs[SPRITE_SHADER_PROGRAM], "u_fade_rate");
    g_shader_uniforms[SHADER_PARTICLE_CAMERA_TRANSFORM] = glGetUniformLocation(s_shader_programs[PARTICLE_SHADER_PROGRAM], "u_camera_inverse_transform");
    g_shader_uniforms[SHADER_PARTICLE_TEXTURE] = glGetUniformLocation(s_shader_programs[PARTICLE_SHADER_PROGRAM], "u_texture");
    g_shader_uniforms[SHADER_PARTICLE_HDR_CAMERA_TRANSFORM] = glGetUniformLocation(s_shader_programs[PARTICLE_HDR_SHADER_PROGRAM], "u_camera_inverse_transform");
    g_shader_uniforms[SHADER_PARTICLE_HDR_TEXTURE_EMIT] = glGetUniformLocation(s_shader_programs[PARTICLE_HDR_SHADER_PROGRAM], "u_texture_emit");
    g_shader_uniforms[SHADER_PARTICLE_HDR_EMISSIVE_INTENSITY] = glGetUniformLocation(s_shader_programs[PARTICLE_HDR_SHADER_PROGRAM], "u_emissive_intensity");
    g_shader_uniforms[SHADER_DYNAMIC_OBJ_CAMERA] = glGetUniformLocation(s_shader_programs[DYNAMIC_OBJ_SHADER_PROGRAM], "u_camera_inverse_transform");
    g_shader_uniforms[SHADER_DYNAMIC_OBJ_TRANSFORM] = glGetUniformLocation(s_shader_programs[DYNAMIC_OBJ_SHADER_PROGRAM], "u_transform");
    g_shader_uniforms[SHADER_DYNAMIC_OBJ_RGBA] = glGetUniformLocation(s_shader_programs[DYNAMIC_OBJ_SHADER_PROGRAM], "u_rgba_multiplier");
    g_shader_uniforms[SHADER_DYNAMIC_OBJ_TEXTURE_DIFF] = glGetUniformLocation(s_shader_programs[DYNAMIC_OBJ_SHADER_PROGRAM], "u_texture_diff");
    g_shader_uniforms[SHADER_DYNAMIC_OBJ_TEXTURE_NORM] = glGetUniformLocation(s_shader_programs[DYNAMIC_OBJ_SHADER_PROGRAM], "u_texture_norm");
    g_shader_uniforms[SHADER_DYNAMIC_OBJ_TEXTURE_SPEC] = glGetUniformLocation(s_shader_programs[DYNAMIC_OBJ_SHADER_PROGRAM], "u_texture_spec");
    g_shader_uniforms[SHADER_DYNAMIC_OBJ_TEXTURE_EMIT]  = glGetUniformLocation(s_shader_programs[DYNAMIC_OBJ_SHADER_PROGRAM], "u_texture_emit");
    g_shader_uniforms[SHADER_DYNAMIC_OBJ_EMISSIVE_INTENSITY] = glGetUniformLocation(s_shader_programs[DYNAMIC_OBJ_SHADER_PROGRAM], "u_emissive_intensity");
    g_shader_uniforms[SHADER_DYNAMIC_OBJ_AMBIENT_LIGHT] = glGetUniformLocation(s_shader_programs[DYNAMIC_OBJ_SHADER_PROGRAM], "u_ambient_light");
    g_shader_uniforms[SHADER_DYNAMIC_OBJ_DIRECTION_LIGHT] = glGetUniformLocation(s_shader_programs[DYNAMIC_OBJ_SHADER_PROGRAM], "u_direction_light");
    g_shader_uniforms[SHADER_DYNAMIC_OBJ_DIRECTION_VEC] = glGetUniformLocation(s_shader_programs[DYNAMIC_OBJ_SHADER_PROGRAM], "u_direction_vec");
    g_shader_uniforms[SHADER_DYNAMIC_OBJ_HDR_CAMERA] = glGetUniformLocation(s_shader_programs[DYNAMIC_OBJ_HDR_SHADER_PROGRAM], "u_camera_inverse_transform");
    g_shader_uniforms[SHADER_DYNAMIC_OBJ_HDR_TRANSFORM] = glGetUniformLocation(s_shader_programs[DYNAMIC_OBJ_HDR_SHADER_PROGRAM], "u_transform");
    g_shader_uniforms[SHADER_DYNAMIC_OBJ_HDR_RGBA] = glGetUniformLocation(s_shader_programs[DYNAMIC_OBJ_HDR_SHADER_PROGRAM], "u_rgba_multiplier");
    g_shader_uniforms[SHADER_DYNAMIC_OBJ_HDR_TEXTURE_EMIT] = glGetUniformLocation(s_shader_programs[DYNAMIC_OBJ_HDR_SHADER_PROGRAM], "u_texture_emit");
    g_shader_uniforms[SHADER_DYNAMIC_OBJ_HDR_EMIT_INTENSITY] = glGetUniformLocation(s_shader_programs[DYNAMIC_OBJ_HDR_SHADER_PROGRAM], "u_emissive_intensity");
}

// Use shader given internal id. maps the internal id to opengl id
void use_shader(u32 program_id) {
    glUseProgram(s_shader_programs[program_id]);
}

void set_uniform_v2_value(u32 var, float v0, float v1) {
    glUniform2f(g_shader_uniforms[var], v0, v1);
}

void set_uniform_v3_value(u32 var, float v0, float v1, float v2) {
    glUniform3f(g_shader_uniforms[var], v0, v1, v2);
}

void set_uniform_v4_value(u32 var, float v0, float v1, float v2, float v3) {
    glUniform4f(g_shader_uniforms[var], v0, v1, v2, v3);
}

void set_program_uniform_v4(u8 program, u32 var, float v0, float v1, float v2, float v3) {
    glProgramUniform4f(s_shader_programs[program], g_shader_uniforms[var], v0, v1, v2, v3);
}

void set_uniform_m4_value(u32 var, bool8 transpose, float *matrix) {
    glUniformMatrix4fv(g_shader_uniforms[var], 1, transpose, matrix);
}

void set_uniform_1i_value(u32 var, s32 value) {
    glUniform1i(g_shader_uniforms[var], value);
}

void set_uniform_1f_value(u32 var, float value) {
    glUniform1f(g_shader_uniforms[var], value);
}

void set_uniform_1fv_values(u32 var, u8 n, float *values) {
    glUniform1fv(g_shader_uniforms[var], n, values);
}

void set_uniform_2fv_values(u32 var, u8 n, float *values) {
    glUniform2fv(g_shader_uniforms[var], n, values);
}

void set_uniform_3fv_values(u32 var, u8 n, float *values) {
    glUniform3fv(g_shader_uniforms[var], n, values);
}
