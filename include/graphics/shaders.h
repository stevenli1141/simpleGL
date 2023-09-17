#ifndef GUARD_SHADERS_H
#define GUARD_SHADERS_H

#include "types.h"

#define DEFAULT_SHADER_PROGRAM          0
#define COLOR_SHADER_PROGRAM            1
#define UNLIT_SHADER_PROGRAM            2
#define LIT_SHADER_PROGRAM              3
#define HDR_FILTER_SHADER_PROGRAM       4
#define FB_SHADER_PROGRAM               5
#define GAUSSIAN_SHADER_PROGRAM         6
#define BLOOM_SHADER_PROGRAM            7
#define TEXT_SHADER_PROGRAM             8
#define SPRITE_SHADER_PROGRAM           9
#define PARTICLE_SHADER_PROGRAM         10
#define PARTICLE_HDR_SHADER_PROGRAM     11
#define DYNAMIC_OBJ_SHADER_PROGRAM      12
#define DYNAMIC_OBJ_HDR_SHADER_PROGRAM  13
#define NUM_SHADER_PROGRAMS             14

#define NO_SHADER                       0
#define DEFAULT_VERTEX_SHADER           1
#define COLOR_VERTEX_SHADER             2
#define UNLIT_VERTEX_SHADER             3
#define LIT_VERTEX_SHADER               4
#define FB_VERTEX_SHADER                5
#define SPRITE_VERTEX_SHADER            6
#define PARTICLE_VERTEX_SHADER          7
#define DYNAMIC_OBJ_VERTEX_SHADER       8
#define DEFAULT_FRAG_SHADER             9
#define COLOR_FRAG_SHADER               10
#define UNLIT_FRAG_SHADER               11
#define LIT_FRAG_SHADER                 12
#define HDR_FILTER_FRAG_SHADER          13
#define FB_FRAG_SHADER                  14
#define GAUSSIAN_FRAG_SHADER            15
#define BLOOM_FRAG_SHADER               16
#define TEXT_FRAG_SHADER                17
#define SPRITE_FRAG_SHADER              18
#define PARTICLE_FRAG_SHADER            19
#define PARTICLE_HDR_FRAG_SHADER        20
#define DYNAMIC_OBJ_FRAG_SHADER         21
#define DYNAMIC_OBJ_HDR_FRAG_SHADER     22
#define NUM_SHADERS                     23

#define SHADER_UNLIT_TRANSFORM                  0
#define SHADER_UNLIT_CAMERA_INVERSE_TRANSFORM   1
#define SHADER_UNLIT_TEXTURE                    2
#define SHADER_LIT_CAMERA_INVERSE_TRANSFORM     3
#define SHADER_HDR_FILTER_CAMERA                4
#define SHADER_LIT_CAMERA_DIR                   5 // unused
#define SHADER_LIT_TEXTURE_DIFF                 6
#define SHADER_LIT_TEXTURE_NORM                 7
#define SHADER_LIT_TEXTURE_SPEC                 8
#define SHADER_LIT_TEXTURE_AO                   9
#define SHADER_LIT_TEXTURE_EMIT                 10
#define SHADER_LIT_EMISSIVE_INTENSITY           11
#define SHADER_LIT_LIGHTMAP                     12
#define SHADER_LIT_AMBIENT_LIGHT                13
#define SHADER_LIT_DIRECTION_VEC                14
#define SHADER_LIT_DIRECTION_LIGHT              15
#define SHADER_LIT_POINT_LOCS                   16
#define SHADER_LIT_POINT_LIGHTS                 17
#define SHADER_LIT_SPOT_LOCS                    18
#define SHADER_LIT_SPOT_LIGHTS                  19
#define SHADER_LIT_SPOT_DIRS                    20
#define SHADER_LIT_SPOT_BOUNDS                  21
#define SHADER_HDR_FILTER_EMIT_INTENSITY        22
#define SHADER_HDR_FILTER_EMIT_TEXTURE          23
#define SHADER_GAUSSIAN_DIR                     24
#define SHADER_BLOOM_EXPOSURE                   25
#define SHADER_BLOOM_SCENE                      26
#define SHADER_BLOOM_HDR_MAP                    27
#define SHADER_TEXT_COLOR                       28
#define SHADER_SPRITE_TEXTURE                   29
#define SHADER_SPRITE_CAMERA                    30
#define SHADER_SPRITE_LIGHT                     31
#define SHADER_SPRITE_FADE_COLOR                32
#define SHADER_SPRITE_FADE_RATE                 33
#define SHADER_PARTICLE_CAMERA_TRANSFORM        34
#define SHADER_PARTICLE_TEXTURE                 35
#define SHADER_PARTICLE_HDR_CAMERA_TRANSFORM    36
#define SHADER_PARTICLE_HDR_TEXTURE_EMIT        37
#define SHADER_PARTICLE_HDR_EMISSIVE_INTENSITY  38
#define SHADER_DYNAMIC_OBJ_CAMERA               39
#define SHADER_DYNAMIC_OBJ_TRANSFORM            40
#define SHADER_DYNAMIC_OBJ_RGBA                 41
#define SHADER_DYNAMIC_OBJ_TEXTURE_DIFF         42
#define SHADER_DYNAMIC_OBJ_TEXTURE_NORM         43
#define SHADER_DYNAMIC_OBJ_TEXTURE_SPEC         44
#define SHADER_DYNAMIC_OBJ_TEXTURE_EMIT         45
#define SHADER_DYNAMIC_OBJ_EMISSIVE_INTENSITY   46
#define SHADER_DYNAMIC_OBJ_AMBIENT_LIGHT        47
#define SHADER_DYNAMIC_OBJ_DIRECTION_LIGHT      48
#define SHADER_DYNAMIC_OBJ_DIRECTION_VEC        49
#define SHADER_DYNAMIC_OBJ_HDR_CAMERA           50
#define SHADER_DYNAMIC_OBJ_HDR_TRANSFORM        51
#define SHADER_DYNAMIC_OBJ_HDR_RGBA             52
#define SHADER_DYNAMIC_OBJ_HDR_TEXTURE_EMIT     53
#define SHADER_DYNAMIC_OBJ_HDR_EMIT_INTENSITY   54
#define NUM_SHADER_UNIFORMS                     55

#define MAX_POINT_LIGHTS 8
#define MAX_SPOT_LIGHTS 8
#define TEXTURE_DIFF_LEVEL GL_TEXTURE0
#define TEXTURE_NORM_LEVEL GL_TEXTURE1
#define TEXTURE_SPEC_LEVEL GL_TEXTURE2
#define TEXTURE_EMIT_LEVEL GL_TEXTURE3
#define LIGHTMAP_LEVEL GL_TEXTURE4

#define SHADER_CAMERA_UNIFORM             0
#define SHADER_DIFF_UNIFORM               1
#define SHADER_NORM_UNIFORM               2
#define SHADER_SPEC_UNIFORM               3
#define SHADER_AO_UNIFORM                 4
#define SHADER_EMIT_UNIFORM               5
#define SHADER_PARAM1_UNIFORM             6
#define SHADER_PARAM2_UNIFORM             7
#define SHADER_PARAM3_UNIFORM             8
#define SHADER_AMBIENT_LIGHT_UNIFORM      9
#define SHADER_DIRECTION_LIGHT_UNIFORM    10
#define SHADER_DIRECTION_DIR_UNIFORM      11
#define SHADER_POINT_LOCS_UNIFORM         12
#define SHADER_POINT_LIGHTS_UNIFORM       13
#define SHADER_SPOT_LOCS_UNIFORM          14
#define SHADER_SPOT_LIGHTS_UNIFORM        15
#define NUM_SHADER_OPTIONS                16

extern const char g_shader_default_vertex_src[];
extern const char g_shader_default_frag_src[];
extern const char g_shader_color_vertex_src[];
extern const char g_shader_color_frag_src[];
extern const char g_shader_unlit_vertex_src[];
extern const char g_shader_unlit_frag_src[];
extern const char g_shader_lit_vertex_src[];
extern const char g_shader_lit_frag_src[];
extern const char g_shader_hdr_filter_frag_src[];
extern const char g_shader_fb_vertex_src[];
extern const char g_shader_fb_frag_src[];
extern const char g_shader_gaussian_frag_src[];
extern const char g_shader_bloom_frag_src[];
extern const char g_shader_text_frag_src[];
extern const char g_shader_sprite_vertex_src[];
extern const char g_shader_sprite_frag_src[];
extern const char g_shader_particle_vertex_src[];
extern const char g_shader_particle_frag_src[];
extern const char g_shader_particle_hdr_frag_src[];
extern const char g_shader_dynamic_obj_vertex_src[];
extern const char g_shader_dynamic_obj_frag_src[];
extern const char g_shader_dynamic_obj_hdr_frag_src[];

extern u32 g_shader_uniforms[];

extern void init_shaders(void);
extern void use_shader(u32 id);
extern const u32 get_shader_uniform(u8 shader, u8 option);
extern void set_uniform_v2_value(u32 var, float v0, float v1);
extern void set_uniform_v3_value(u32 var, float v0, float v1, float v2);
extern void set_uniform_v4_value(u32 var, float v0, float v1, float v2, float v3);
extern void set_uniform_m4_value(u32 var, bool8 transpose, float *matrix);
extern void set_uniform_1i_value(u32 var, s32 value);
extern void set_uniform_1f_value(u32 var, float value);
extern void set_uniform_1fv_values(u32 var, u8 n, float *values);
extern void set_uniform_2fv_values(u32 var, u8 n, float *values);
extern void set_uniform_3fv_values(u32 var, u8 n, float *values);
// unused
extern void set_program_uniform_v4(u8 program, u32 var, float v0, float v1, float v2, float v3);


#endif
