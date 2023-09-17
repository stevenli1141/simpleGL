#include <GL/glew.h>
#include <math.h>
#include <string.h>
#include "constants/vars.h"
#include "graphics/graphics.h"
#include "graphics/lightmaps.h"
#include "graphics/materials.h"
#include "graphics/linalg.h"
#include "graphics/particles.h"
#include "graphics/postprocess.h"
#include "graphics/shaders.h"
#include "graphics/textures.h"
#include "camera.h"
#include "config.h"
#include "screen_effects.h"
#include "system.h"
#include "log.h"
#include "utils.h"

// Collection of all functions related to graphics rendering

// loc(3) norm(3) uv0(2) uv1(2)
#define VERTEX_STRIDE (10 * sizeof(float))
#define VERTEX_NORM_OFFSET (3 * sizeof(float))
#define VERTEX_UV_OFFSET (6 * sizeof(float))
#define VERTEX_LIGHTMAP_UV_OFFSET (8 * sizeof(float))

#define DYN_OBJ_VERTEX_STRIDE (8 * sizeof(float))

#define PARTICLE_VERTEX_STRIDE (9 * sizeof(float))
#define PARTICLE_UV_OFFSET (3 * sizeof(float))
#define PARTICLE_RGBA_OFFSET (5 * sizeof(float))

#define NUM_OVERLAYS 1

// 3d scene buffer
static u32 s_vao;
static u32 s_vertex_buffer;
static u32 s_index_buffer;
// 3d dynamic object buffer
static u32 s_dyn_obj_vao;
static u32 s_dyn_obj_vertex_buffer;
static u32 s_dyn_obj_index_buffer;
// 2d buffer
static u32 s_ui_vao;
static u32 s_ui_vertex_buffer;

static void prepare_shader(u8 shader, const struct scene_lighting_data *lighting);
static void render_single(const struct scene_data *scene, const struct scene_static_object_data *data);
static void render_single_vertices_only(const struct scene_static_object_data *data);
static void add_overlay(const u32 texture);\
static void copy_corners_to_particle_vertex_buffer(float *vertices, const float *gpos, const s16 u, const s16 v, const float *rgba);

// framebuffer
static u32 s_fbo;
static u32 s_fb_tex;
static u32 s_fbo2;
static u32 s_fb_resolved_tex;
static u32 s_rbo;
static u32 s_hdr_fbo; // HDR framebuffer
static u32 s_hdr_tex;
static u32 s_hdr_rbo;
static u32 s_bloom_postprocess_tex;
// overlaying textures e.g. bloom

// Particles
static u32 s_particle_vao;
static u32 s_particle_vertex_buffer;
// 2d camera facing objects in 3d space
static u32 s_sprite_vao;
static u32 s_sprite_vertex_buffer;
static float s_sprite_vertices[30]; // only need to draw one rect at a time
// used for drawing fade
static u32 s_color_vao;
static u32 s_color_vertex_buffer;

// screen rect
static u32 s_fb_vao;
static u32 s_fb_vertex_buffer;

static float s_fb_tris[] = {
    -1.f,  1.f, 0.f, 0.f, 1.f,
    -1.f, -1.f, 0.f, 0.f, 0.f,
     1.f,  1.f, 0.f, 1.f, 1.f,
     1.f,  1.f, 0.f, 1.f, 1.f,
    -1.f, -1.f, 0.f, 0.f, 0.f,
     1.f, -1.f, 0.f, 1.f, 0.f
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// Init methods
////////////////////////////////////////////////////////////////////////////////////////////////////

void init_gl(void) {
    // Init GLEW
    glewInit();

    // glClearColor(0.f, 0.f, 0.f, 1.f);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glEnable(GL_MULTISAMPLE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glEnable(GL_FRAMEBUFFER_SRGB); gamma correction
    glPointSize(2.f);
    // glEnable(GL_LINE_SMOOTH);
}

void init_buffers(void) {
    // Bind VAO index 1
    glGenVertexArrays(1, &s_vao);
    glGenBuffers(1, &s_vertex_buffer);
    glGenBuffers(1, &s_index_buffer);

    // Linking vertex attributes
    // layout size type normalized(int) stride offset
    // layout (location = 0) in vec3 aPos; => location is attribute 0, first arg to glVertexAttribPointer
    // arg2: size, 3 pos 3 norm 2 uv
    // arg3: type of data GL_FLOAT
    // arg4: normalized, meant to convert integers to 0..1 range. not relevant here
    // arg5: stride
    // arg6: offset of where position data begins in buffer
    glBindVertexArray(s_vao);
    glBindBuffer(GL_ARRAY_BUFFER, s_vertex_buffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_STRIDE, NULL);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, VERTEX_STRIDE, (void *)VERTEX_NORM_OFFSET);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, VERTEX_STRIDE, (void *)VERTEX_UV_OFFSET);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, VERTEX_STRIDE, (void *)VERTEX_LIGHTMAP_UV_OFFSET);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_index_buffer);

    // init dynamic object buffer
    glGenVertexArrays(1, &s_dyn_obj_vao);
    glGenBuffers(1, &s_dyn_obj_vertex_buffer);
    glGenBuffers(1, &s_dyn_obj_index_buffer);
    glBindVertexArray(s_dyn_obj_vao);
    glBindBuffer(GL_ARRAY_BUFFER, s_dyn_obj_vertex_buffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, DYN_OBJ_VERTEX_STRIDE, NULL);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, DYN_OBJ_VERTEX_STRIDE, (void *)VERTEX_NORM_OFFSET);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, DYN_OBJ_VERTEX_STRIDE, (void *)VERTEX_UV_OFFSET);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_dyn_obj_index_buffer);

    // init particle vao
    glGenVertexArrays(1, &s_particle_vao);
    glGenBuffers(1, &s_particle_vertex_buffer);
    glBindVertexArray(s_particle_vao);
    glBindBuffer(GL_ARRAY_BUFFER, s_particle_vertex_buffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, PARTICLE_VERTEX_STRIDE, NULL);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, PARTICLE_VERTEX_STRIDE, (void *)PARTICLE_UV_OFFSET);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, PARTICLE_VERTEX_STRIDE, (void *)PARTICLE_RGBA_OFFSET);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    // init sprite vao
    glGenVertexArrays(1, &s_sprite_vao);
    glGenBuffers(1, &s_sprite_vertex_buffer);
    glBindVertexArray(s_sprite_vao);
    glBindBuffer(GL_ARRAY_BUFFER, s_sprite_vertex_buffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), NULL);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // init the UI vertex array
    glGenVertexArrays(1, &s_ui_vao);
    glGenBuffers(1, &s_ui_vertex_buffer);
    glBindVertexArray(s_ui_vao);
    glBindBuffer(GL_ARRAY_BUFFER, s_ui_vertex_buffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), NULL);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // init the screen color vertex array
    glGenVertexArrays(1, &s_color_vao);
    glGenBuffers(1, &s_color_vertex_buffer);
    glBindVertexArray(s_color_vao);
    glBindBuffer(GL_ARRAY_BUFFER, s_color_vertex_buffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), NULL);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // init framebuffer and MS texture
    glGenFramebuffers(1, &s_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, s_fbo);
    glGenTextures(1, &s_fb_tex);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, s_fb_tex); // bind fb tex. multisample to enable MSAA
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, MSAA_SAMPLES, GL_RGBA, SCREEN_WIDTH, SCREEN_HEIGHT, GL_TRUE);
    glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, s_fb_tex, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        LOG_WARN("Failed to generate multisample framebuffer!\n")
    }

    glGenRenderbuffers(1, &s_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, s_rbo);
    // glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCREEN_WIDTH, SCREEN_HEIGHT);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, MSAA_SAMPLES, GL_DEPTH24_STENCIL8, SCREEN_WIDTH, SCREEN_HEIGHT);  
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, s_rbo);

    // resolved 2D texture
    glGenFramebuffers(1, &s_fbo2);
    glBindFramebuffer(GL_FRAMEBUFFER, s_fbo2);
    glGenTextures(1, &s_fb_resolved_tex);
    glBindTexture(GL_TEXTURE_2D, s_fb_resolved_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, s_fb_resolved_tex, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        LOG_WARN("Failed to generate resolved framebuffer!\n")
    }

    // HDR filter framebuffer
    glGenFramebuffers(1, &s_hdr_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, s_hdr_fbo);
    glGenTextures(1, &s_hdr_tex);
    glBindTexture(GL_TEXTURE_2D, s_hdr_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, s_hdr_tex, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        LOG_WARN("Failed to generate HDR filter framebuffer!\n")
    }

    glGenRenderbuffers(1, &s_hdr_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, s_hdr_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCREEN_WIDTH, SCREEN_HEIGHT);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, s_hdr_rbo);


    // init the framebuffer vertex array
    glGenVertexArrays(1, &s_fb_vao);
    glGenBuffers(1, &s_fb_vertex_buffer);
    glBindVertexArray(s_fb_vao);
    glBindBuffer(GL_ARRAY_BUFFER, s_fb_vertex_buffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), NULL);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glBufferData(GL_ARRAY_BUFFER, 30 * sizeof(float), s_fb_tris, GL_STATIC_DRAW);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Frees all resources called by glGen*
// TODO clean up all generated textures
void clean_gl(void) {
    glDeleteTextures(1, &s_fb_tex);
    glDeleteTextures(1, &s_fb_resolved_tex);
    glDeleteTextures(1, &s_hdr_tex);
    glDeleteFramebuffers(1, &s_fbo);
    glDeleteFramebuffers(1, &s_fbo2);
    glDeleteFramebuffers(1, &s_hdr_fbo);
    glDeleteVertexArrays(1, &s_vao);
    glDeleteVertexArrays(1, &s_dyn_obj_vao);
    glDeleteVertexArrays(1, &s_ui_vao);
    glDeleteVertexArrays(1, &s_color_vao);
    glDeleteVertexArrays(1, &s_sprite_vao);
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// Render cycle
////////////////////////////////////////////////////////////////////////////////////////////////////

// Sets the viewport to the standard size
// Sets the framebuffer to 0 to allow free rendering to the screen
// Note this does not clear the buffer. To do that call render_clear.
void start_render_cycle(void) {
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
}

// Clears the current buffer
// Note that it does not need to be called since we may want to keep the previous screen around for more than 1 frame
void render_clear(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.f, 0.f, 0.f, 1.0f);
}

// Call after rendering all scene objects to post process
// This must be called before render_to_screen. Pass 0 (empty flags) to not run any post processing
void combine_and_post_process(const u32 post_process_flags) {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, s_fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, s_fbo2);
    glBlitFramebuffer(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    s_bloom_postprocess_tex = s_hdr_tex;
    if (post_process_flags & POST_PROCESS_BLOOM) {
        s_bloom_postprocess_tex = bloom(s_hdr_tex);
    }
}

// Unbinds framebuffer and renders framebuffer to screen
// TODO apply post processing effects here
void render_buffers_to_screen(void) {
    // This is the final call in the render lifecycle which jumps to screen
    // Currently bloom is the final step in this process
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.f, 0.f, 0.f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    // Blend hdr and base together for bloom effect
    use_shader(BLOOM_SHADER_PROGRAM);
    set_uniform_1f_value(SHADER_BLOOM_EXPOSURE, 0.5f);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, s_fb_resolved_tex);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, s_bloom_postprocess_tex);
    set_uniform_1i_value(SHADER_BLOOM_SCENE, 0);
    set_uniform_1i_value(SHADER_BLOOM_HDR_MAP, 1);

    render_screen_rect();
}

void preload_scene_textures(const struct scene_data *scene) {
    for (u8 i = 0; i < *(scene->num_static_objects); ++i) {
        load_material_textures(scene->static_data[i].material);
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// Specific render functions
////////////////////////////////////////////////////////////////////////////////////////////////////


// Renders rectangle the size of a screen to the frame buffer
void render_screen_rect(void) {
    glBindVertexArray(s_fb_vao);
    glBindBuffer(GL_ARRAY_BUFFER, s_fb_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, 30 * sizeof(float), s_fb_tris, GL_STATIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

// Renders a solid color rectangle using the color shader. Uses one of the fade colors plus an alpha
void render_color_rect(const float x, const float y, const float z, const float x2, const float y2, const u8 color, const float alpha) {
    const float r = g_fade_colors[color][0], g = g_fade_colors[color][1], b = g_fade_colors[color][2], a = alpha;
    const float vertices[] = {
        x, y, z, r, g, b, a,
        x, y2, z, r, g, b, a,
        x2, y, z, r, g, b, a,
        x2, y, z, r, g, b, a,
        x, y2, z, r, g, b, a,
        x2, y2, z, r, g, b, a,
    };
    use_shader(COLOR_SHADER_PROGRAM);
    glBindVertexArray(s_color_vao);
    glBindBuffer(GL_ARRAY_BUFFER, s_color_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, 42 * sizeof(float), vertices, GL_STATIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

// Renders a single rect with the rect specified by top left and bottom right with specified UV coordinates
void render_2d_rect(const float x, const float y, float z, const float x2, const float y2, const float u, const float v, const float u2, const float v2, const u8 texture) {
    const float verts[] = { x, y, z, u, v, x, y2, z, u, v2, x2, y, z, u2, v, x2, y, z, u2, v, x, y2, z, u, v2, x2, y2, z, u2, v2 };
    use_shader(FB_SHADER_PROGRAM);
    use_texture(TEXTURE_TYPE_INTERFACE, GL_TEXTURE0, texture);
    glBindVertexArray(s_ui_vao);
    glBindBuffer(GL_ARRAY_BUFFER, s_ui_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, 30 * sizeof(float), verts, GL_STATIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

// Renders a 9x9 rect with the specified border width having UVs that correspond to the edges of the texture specified by src_border_width
void render_2d_border_rect(const float x, const float y, const float w, const float h, const float src_border_width, const float border_width, const u8 texture) {
    const float u = 0.f, v = 0.f, u2 = src_border_width, u3 = 1.f - src_border_width, v2 = src_border_width, v3 = 1.f - src_border_width, u4 = 1.f, v4 = 1.f;
    const float x2 = x + border_width, x3 = x + w - border_width, x4 = x + w;
    const float y2 = y - border_width * SCREEN_WIDTH / SCREEN_HEIGHT, y3 = y - h + border_width * SCREEN_WIDTH / SCREEN_HEIGHT, y4 = y - h;
    const float verts[] = {
        x, y, 0.f, u, v, x, y2, 0.f, u, v2, x2, y, 0.f, u2, v, x2, y, 0.f, u2, v, x, y2, 0.f, u, v2, x2, y2, 0.f, u2, v2,
        x2, y, 0.f, u2, v, x2, y2, 0.f, u2, v2, x3, y, 0.f, u3, v, x3, y, 0.f, u3, v, x2, y2, 0.f, u2, v2, x3, y2, 0.f, u3, v2,
        x3, y, 0.f, u3, v, x3, y2, 0.f, u3, v2, x4, y, 0.f, u4, v, x4, y, 0.f, u4, v, x3, y2, 0.f, u3, v2, x4, y2, 0.f, u4, v2,
        x, y2, 0.f, u, v2, x, y3, 0.f, u, v3, x2, y2, 0.f, u2, v2, x2, y2, 0.f, u2, v2, x, y3, 0.f, u, v3, x2, y3, 0.f, u2, v3,
        x2, y2, 0.f, u2, v2, x2, y3, 0.f, u2, v3, x3, y2, 0.f, u3, v2, x3, y2, 0.f, u3, v2, x2, y3, 0.f, u2, v3, x3, y3, 0.f, u3, v3,
        x3, y2, 0.f, u3, v2, x3, y3, 0.f, u3, v3, x4, y2, 0.f, u4, v2, x4, y2, 0.f, u4, v2, x3, y3, 0.f, u3, v3, x4, y3, 0.f, u4, v3,
        x, y3, 0.f, u, v3, x, y4, 0.f, u, v4, x2, y3, 0.f, u2, v3, x2, y3, 0.f, u2, v3, x, y4, 0.f, u, v4, x2, y4, 0.f, u2, v4,
        x2, y3, 0.f, u2, v3, x2, y4, 0.f, u2, v4, x3, y3, 0.f, u3, v3, x3, y3, 0.f, u3, v3, x2, y4, 0.f, u2, v4, x3, y4, 0.f, u3, v4,
        x3, y3, 0.f, u3, v3, x3, y4, 0.f, u3, v4, x4, y3, 0.f, u4, v3, x4, y3, 0.f, u4, v3, x3, y4, 0.f, u3, v4, x4, y4, 0.f, u4, v4
    };
    use_shader(FB_SHADER_PROGRAM);
    use_texture(TEXTURE_TYPE_INTERFACE, GL_TEXTURE0, texture);
    glBindVertexArray(s_ui_vao);
    glBindBuffer(GL_ARRAY_BUFFER, s_ui_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, 270 * sizeof(float), verts, GL_STATIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, 54);
}

// Renders static objects in a scene to the multisampled frame buffer and single sample HDR filter framebuffer
void render_static_objects(const struct scene_data *scene) {
    glBindFramebuffer(GL_FRAMEBUFFER, s_fbo);
    // glClearColor(0.f, 0.f, 0.f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);

    u16 shader = 0;
    for (int i = 0; i < *(scene->num_static_objects); ++i) {
        const struct scene_static_object_data *data = &scene->static_data[i];
        const u16 mat_shader = get_material_shader(data->material);
        if (is_translucent(data->material)) {
            continue;
        }
        if (shader != mat_shader) {
            prepare_shader(mat_shader, &scene->lighting);
            shader = mat_shader;
        }
        render_single(scene, data);
    }

    // Render translucent materials after everything else
    for (int i = 0; i < *(scene->num_static_objects); ++i) {
        const struct scene_static_object_data *data = &scene->static_data[i];
        const u16 mat_shader = get_material_shader(data->material);
        if (!is_translucent(data->material)) {
            continue;
        }

        if (shader != mat_shader) {
            prepare_shader(mat_shader, &scene->lighting);
            shader = mat_shader;
        }
        u32 indices[data->n_indices];
        render_single(scene, data);
    }

    // Render to buffer HDR
    glDisable(GL_BLEND); // Turn off blend so (0, 0, 0, 0) can still cover more distant emits

    glBindFramebuffer(GL_FRAMEBUFFER, s_hdr_fbo);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    use_shader(HDR_FILTER_SHADER_PROGRAM);
    set_uniform_m4_value(SHADER_HDR_FILTER_CAMERA, GL_TRUE, g_camera_inverse_transform);
    for (int i = 0; i < *(scene->num_static_objects); ++i) {
        const struct scene_static_object_data *data = &scene->static_data[i];
        if (is_translucent(data->material)) {
            continue; // translucent objects should not block emit
        }
        const u32 tex = get_material_emit_texture(data->material);
        const float emit = get_material_emit_intensity(data->material);
        set_uniform_1f_value(SHADER_HDR_FILTER_EMIT_INTENSITY, emit);
        use_texture(TEXTURE_TYPE_EMIT, GL_TEXTURE0, tex);
        render_single_vertices_only(data);
    }
    glEnable(GL_BLEND);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Renders a dynamic object at the transform specified
// gtransform consists of x y z theta phi
void render_scene_dynamic_object(const float *gtransform, const float *ltransform, const struct dynamic_object_data *object, const struct scene_data *scene) {
    float vertices[object->n_vertices];
    memcpy(vertices, object->vertices, object->n_vertices * sizeof(float));
    for (u32 i = 0; i < object->n_vertices; i += 10) {
        float local[6];
        // apply local transform followed by global transform
        // phi precedes theta as always
        local[0] = object->vertices[i]*cos(ltransform[3]) - (object->vertices[i+1]*cos(ltransform[4]) - object->vertices[i+2]*sin(ltransform[4]))*sin(ltransform[3]);
        local[1] = object->vertices[i]*sin(ltransform[3]) + (object->vertices[i+1]*cos(ltransform[4]) - object->vertices[i+2]*sin(ltransform[4]))*cos(ltransform[3]);
        local[2] = object->vertices[i+1]*sin(ltransform[4]) + object->vertices[i+2]*cos(ltransform[4]); // local[2] = sin(ltransform[4])*object->vertices[i+2];
        local[3] = object->vertices[i+3]*cos(ltransform[3]) - object->vertices[i+3]*sin(ltransform[3]);
        local[4] = object->vertices[i+4]*sin(ltransform[3]) + object->vertices[i+4]*cos(ltransform[3]);
        local[0] += ltransform[0];
        local[1] += ltransform[1];
        local[2] += ltransform[2];

        vertices[i] = local[0]*cos(gtransform[3]) - local[1]*sin(gtransform[3]);
        vertices[i+1] = local[0]*sin(gtransform[3]) + local[1]*cos(gtransform[3]);
        vertices[i+2] = local[2]; // vertices[i+2] = sin(gtransform[4])*local[2];
        vertices[i+3] = local[3]*cos(gtransform[3]) - local[3]*sin(gtransform[3]);
        vertices[i+4] = local[4]*sin(gtransform[3]) + local[4]*cos(gtransform[3]);
        vertices[i] += gtransform[0];
        vertices[i+1] += gtransform[1];
        vertices[i+2] += gtransform[2];
    }
    glBindFramebuffer(GL_FRAMEBUFFER, s_fbo);
    const u16 shader = get_material_shader(object->material);
    prepare_shader(shader, &scene->lighting);
    use_material(object->material);
    use_lightmap(LIGHTMAP_LEVEL, 0);
    glBindVertexArray(s_vao);
    glBindBuffer(GL_ARRAY_BUFFER, s_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, object->n_vertices * sizeof(float), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, object->n_indices * sizeof(u32), object->indices, GL_STATIC_DRAW);
    glDrawElements(GL_TRIANGLES, object->n_indices, GL_UNSIGNED_INT, 0);

    // Render to HDR since dynamic objects can have emission
    glDisable(GL_BLEND);
    glBindFramebuffer(GL_FRAMEBUFFER, s_hdr_fbo);
    use_shader(HDR_FILTER_SHADER_PROGRAM);
    set_uniform_m4_value(SHADER_HDR_FILTER_CAMERA, GL_TRUE, g_camera_inverse_transform);
    const u32 tex = get_material_emit_texture(object->material);
    const float emit = get_material_emit_intensity(object->material);
    set_uniform_1f_value(SHADER_HDR_FILTER_EMIT_INTENSITY, emit);
    use_texture(TEXTURE_TYPE_EMIT, GL_TEXTURE0, tex);
    glBindBuffer(GL_ARRAY_BUFFER, s_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, object->n_vertices * sizeof(float), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, object->n_indices * sizeof(u32), object->indices, GL_STATIC_DRAW);
    glDrawElements(GL_TRIANGLES, object->n_indices, GL_UNSIGNED_INT, 0);
    glEnable(GL_BLEND);

    glBindFramebuffer(GL_FRAMEBUFFER, 0); // return to normal framebuffer
}

void render_dynamic_object(struct dynamic_object *dynamic_object, const struct scene_data *scene) {
    const struct dynamic_object_data *data = g_dynamic_object_data[dynamic_object->id];

    glBindFramebuffer(GL_FRAMEBUFFER, s_fbo);
    use_material(data->material);
    if (is_translucent(data->material)) {
        glDisable(GL_DEPTH_TEST);
    }
    float mat[16];
    use_shader(DYNAMIC_OBJ_SHADER_PROGRAM);
    calc_dynamic_obj_transform(dynamic_object, mat);
    set_uniform_m4_value(SHADER_DYNAMIC_OBJ_TRANSFORM, GL_TRUE, mat);
    set_uniform_v4_value(SHADER_DYNAMIC_OBJ_RGBA, dynamic_object->rgba[0], dynamic_object->rgba[1], dynamic_object->rgba[2], dynamic_object->rgba[3]);
    set_uniform_1i_value(SHADER_DYNAMIC_OBJ_TEXTURE_DIFF, 0);
    set_uniform_1i_value(SHADER_DYNAMIC_OBJ_TEXTURE_NORM, 1);
    set_uniform_1i_value(SHADER_DYNAMIC_OBJ_TEXTURE_SPEC, 2);
    set_uniform_1i_value(SHADER_DYNAMIC_OBJ_TEXTURE_EMIT, 3);
    set_uniform_1f_value(SHADER_DYNAMIC_OBJ_EMISSIVE_INTENSITY, get_material_emit_intensity(data->material));

    glBindVertexArray(s_dyn_obj_vao);

    glBindBuffer(GL_ARRAY_BUFFER, s_dyn_obj_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, data->n_vertices * sizeof(float), data->vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_dyn_obj_index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, data->n_indices * sizeof(u32), data->indices, GL_STATIC_DRAW);
    glDrawElements(GL_TRIANGLES, data->n_indices, GL_UNSIGNED_INT, 0);

    // Render to HDR since dynamic objects can have emission
    glDisable(GL_BLEND);
    glBindFramebuffer(GL_FRAMEBUFFER, s_hdr_fbo);
    use_shader(DYNAMIC_OBJ_HDR_SHADER_PROGRAM);
    set_uniform_v4_value(SHADER_DYNAMIC_OBJ_HDR_RGBA, dynamic_object->rgba[0], dynamic_object->rgba[1], dynamic_object->rgba[2], dynamic_object->rgba[3]);
    set_uniform_m4_value(SHADER_DYNAMIC_OBJ_HDR_TRANSFORM, GL_TRUE, mat);
    set_uniform_m4_value(SHADER_DYNAMIC_OBJ_HDR_CAMERA, GL_TRUE, g_camera_inverse_transform);
    set_uniform_1f_value(SHADER_DYNAMIC_OBJ_HDR_EMIT_INTENSITY, get_material_emit_intensity(data->material));
    use_texture(TEXTURE_TYPE_EMIT, GL_TEXTURE0, get_material_emit_texture(data->material));
    glBindBuffer(GL_ARRAY_BUFFER, s_dyn_obj_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, data->n_vertices * sizeof(float), data->vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_dyn_obj_index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, data->n_indices * sizeof(u32), data->indices, GL_STATIC_DRAW);
    glDrawElements(GL_TRIANGLES, data->n_indices, GL_UNSIGNED_INT, 0);
    glEnable(GL_BLEND);

    glEnable(GL_DEPTH_TEST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // return to normal framebuffer
}


// Particles

void render_point_particles(struct particle_system *ps, const u16 num_particles, const u32 material) {
    struct particle *particles = ps->particles;
    float vertices[num_particles * 9];
    const u8 pstype = g_particle_system_data[ps->id].acc_type;
    const float sx = (pstype & PARTICLE_ACC_NO_FOLLOW) ? 0.f : ps->position[0];
    const float sy = (pstype & PARTICLE_ACC_NO_FOLLOW) ? 0.f : ps->position[1];
    const float sz = (pstype & PARTICLE_ACC_NO_FOLLOW) ? 0.f : ps->position[2];
    const float ct = (pstype & PARTICLE_ACC_NO_FOLLOW) ? 1.f : cos(ps->rotation[0]);
    const float st = (pstype & PARTICLE_ACC_NO_FOLLOW) ? 0.f : sin(ps->rotation[0]);
    const float cp = (pstype & PARTICLE_ACC_NO_FOLLOW) ? 1.f : cos(ps->rotation[1]);
    const float sp = (pstype & PARTICLE_ACC_NO_FOLLOW) ? 0.f : sin(ps->rotation[1]);
    u16 n = 0;
    for (u16 i = 0; i < num_particles; ++i) {
        if (!particles[i].id) {
            continue;
        }
        const float x = sx + ct*particles[i].transform[0] - st*(cp*particles[i].transform[1] - sp*particles[i].transform[2]);
        const float y = sy + st*particles[i].transform[0] + ct*(cp*particles[i].transform[1] - sp*particles[i].transform[2]);
        const float z = sz + sp*particles[i].transform[1] + cp*particles[i].transform[2];
        vertices[n * 9] = x;
        vertices[n * 9 + 1] = y;
        vertices[n * 9 + 2] = z;
        vertices[n * 9 + 3] = particles[i].uv[0] / 128.f;
        vertices[n * 9 + 4] = particles[i].uv[1] / 128.f;
        vertices[n * 9 + 5] = particles[i].transform[4];
        vertices[n * 9 + 6] = particles[i].transform[5];
        vertices[n * 9 + 7] = particles[i].transform[6];
        vertices[n * 9 + 8] = particles[i].transform[7];
        ++n;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, s_fbo);
    use_shader(PARTICLE_HDR_SHADER_PROGRAM);
    set_uniform_m4_value(SHADER_PARTICLE_CAMERA_TRANSFORM, GL_TRUE, g_camera_inverse_transform);
    set_uniform_1f_value(SHADER_PARTICLE_HDR_EMISSIVE_INTENSITY, get_material_emit_intensity(material));
    use_texture(TEXTURE_TYPE_EMIT, GL_TEXTURE0, get_material_emit_texture(material));
    glBindVertexArray(s_particle_vao);
    glBindBuffer(GL_ARRAY_BUFFER, s_particle_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, n * PARTICLE_VERTEX_STRIDE, vertices, GL_STATIC_DRAW);
    glDrawArrays(GL_POINTS, 0, n);

    // Render to HDR
    glDisable(GL_BLEND);
    glBindFramebuffer(GL_FRAMEBUFFER, s_hdr_fbo);
    use_shader(PARTICLE_HDR_SHADER_PROGRAM);
    set_uniform_m4_value(SHADER_PARTICLE_CAMERA_TRANSFORM, GL_TRUE, g_camera_inverse_transform);
    set_uniform_1f_value(SHADER_PARTICLE_HDR_EMISSIVE_INTENSITY, 4 * get_material_emit_intensity(material));
    use_texture(TEXTURE_TYPE_EMIT, GL_TEXTURE0, get_material_emit_texture(material));
    glBindVertexArray(s_particle_vao);
    glBindBuffer(GL_ARRAY_BUFFER, s_particle_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, n * PARTICLE_VERTEX_STRIDE, vertices, GL_STATIC_DRAW);
    glDrawArrays(GL_POINTS, 0, n);
    glEnable(GL_BLEND);

    glBindFramebuffer(GL_FRAMEBUFFER, 0); // return to normal framebuffer
}

void render_line_particles(struct particle_system *ps, const u16 num_particles, const u32 material) {
    struct particle *particles = ps->particles;
    const u8 pstype = g_particle_system_data[ps->id].acc_type;
    const float sx = (pstype & PARTICLE_ACC_NO_FOLLOW) ? 0.f : ps->position[0];
    const float sy = (pstype & PARTICLE_ACC_NO_FOLLOW) ? 0.f : ps->position[1];
    const float sz = (pstype & PARTICLE_ACC_NO_FOLLOW) ? 0.f : ps->position[2];
    const float ct = (pstype & PARTICLE_ACC_NO_FOLLOW) ? 1.f : cos(ps->rotation[0]);
    const float st = (pstype & PARTICLE_ACC_NO_FOLLOW) ? 0.f : sin(ps->rotation[0]);
    const float cp = (pstype & PARTICLE_ACC_NO_FOLLOW) ? 1.f : cos(ps->rotation[1]);
    const float sp = (pstype & PARTICLE_ACC_NO_FOLLOW) ? 0.f : sin(ps->rotation[1]);
    float vertices[num_particles * 18];
    u16 n = 0;
    for (u16 i = 0; i < num_particles; ++i) {
        if (!particles[i].id) {
            continue;
        }
        const float x = sx + ct*particles[i].transform[0] - st*(cp*particles[i].transform[1] - sp*particles[i].transform[2]);
        const float y = sy + st*particles[i].transform[0] + ct*(cp*particles[i].transform[1] - sp*particles[i].transform[2]);
        const float z = sz + sp*particles[i].transform[1] + cp*particles[i].transform[2];
        vertices[n * 18] = x;
        vertices[n * 18 + 1] = y;
        vertices[n * 18 + 2] = z;
        vertices[n * 18 + 3] = particles[i].uv[0] / 128.f;
        vertices[n * 18 + 4] = particles[i].uv[1] / 128.f;
        vertices[n * 18 + 5] = particles[i].transform[4];
        vertices[n * 18 + 6] = particles[i].transform[5];
        vertices[n * 18 + 7] = particles[i].transform[6];
        vertices[n * 18 + 8] = particles[i].transform[7];
        vertices[n * 18 + 9] = x + cp*(ct*particles[i].velocity[0] - st*particles[i].velocity[1]);
        vertices[n * 18 + 10] = y + cp*(st*particles[i].velocity[0] + ct*particles[i].velocity[1]);
        vertices[n * 18 + 11] = z + sp*particles[i].velocity[2];
        vertices[n * 18 + 12] = particles[i].uv[0] / 128.f;
        vertices[n * 18 + 13] = particles[i].uv[1] / 128.f;
        vertices[n * 18 + 14] = particles[i].transform[4];
        vertices[n * 18 + 15] = particles[i].transform[5];
        vertices[n * 18 + 16] = particles[i].transform[6];
        vertices[n * 18 + 17] = particles[i].transform[7];
        ++n;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, s_fbo);
    use_shader(PARTICLE_HDR_SHADER_PROGRAM);
    

    set_uniform_m4_value(SHADER_PARTICLE_CAMERA_TRANSFORM, GL_TRUE, g_camera_inverse_transform);
    set_uniform_1f_value(SHADER_PARTICLE_HDR_EMISSIVE_INTENSITY, get_material_emit_intensity(material));
    use_texture(TEXTURE_TYPE_EMIT, GL_TEXTURE0, get_material_emit_texture(material));
    glBindVertexArray(s_particle_vao);
    glBindBuffer(GL_ARRAY_BUFFER, s_particle_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, 2 * n * PARTICLE_VERTEX_STRIDE, vertices, GL_STATIC_DRAW);
    glDrawArrays(GL_POINTS, 0, 2 * n);

    // Render to HDR
    glDisable(GL_BLEND);
    glBindFramebuffer(GL_FRAMEBUFFER, s_hdr_fbo);
    use_shader(PARTICLE_HDR_SHADER_PROGRAM);
    set_uniform_m4_value(SHADER_PARTICLE_CAMERA_TRANSFORM, GL_TRUE, g_camera_inverse_transform);
    set_uniform_1f_value(SHADER_PARTICLE_HDR_EMISSIVE_INTENSITY, 4 * get_material_emit_intensity(material));
    use_texture(TEXTURE_TYPE_EMIT, GL_TEXTURE0, get_material_emit_texture(material));
    glBindVertexArray(s_particle_vao);
    glBindBuffer(GL_ARRAY_BUFFER, s_particle_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, 2 * n * PARTICLE_VERTEX_STRIDE, vertices, GL_STATIC_DRAW);
    glDrawArrays(GL_LINES, 0, 2 * n);
    glEnable(GL_BLEND);

    glBindFramebuffer(GL_FRAMEBUFFER, 0); // return to normal framebuffer
}

void render_quad_particles(struct particle_system *ps, const u16 num_particles, const u32 material) {
    struct particle *particles = ps->particles;
    const u8 pstype = g_particle_system_data[ps->id].acc_type;
    const float sx = (pstype & PARTICLE_ACC_NO_FOLLOW) ? 0.f : ps->position[0];
    const float sy = (pstype & PARTICLE_ACC_NO_FOLLOW) ? 0.f : ps->position[1];
    const float sz = (pstype & PARTICLE_ACC_NO_FOLLOW) ? 0.f : ps->position[2];
    const float ct = (pstype & PARTICLE_ACC_NO_FOLLOW) ? 1.f : cos(ps->rotation[0]);
    const float st = (pstype & PARTICLE_ACC_NO_FOLLOW) ? 0.f : sin(ps->rotation[0]);
    const float cp = (pstype & PARTICLE_ACC_NO_FOLLOW) ? 1.f : cos(ps->rotation[1]);
    const float sp = (pstype & PARTICLE_ACC_NO_FOLLOW) ? 0.f : sin(ps->rotation[1]);
    float vertices[num_particles * 54];
    u16 n = 0;
    for (u16 i = 0; i < num_particles; ++i) {
        if (!particles[i].id) {
            continue;
        }
        float x = sx + particles[i].transform[0];
        float y = sy + particles[i].transform[1];
        float z = sz + particles[i].transform[2];
        if (!(pstype & PARTICLE_ACC_NO_FOLLOW)) {
            x = sx + ct*particles[i].transform[0] - st*(cp*particles[i].transform[1] - sp*particles[i].transform[2]);
            y = sy + st*particles[i].transform[0] + ct*(cp*particles[i].transform[1] - sp*particles[i].transform[2]);
            z = sz + sp*particles[i].transform[1] + cp*particles[i].transform[2];
        }
        const u8 uv_w = particles[i].uv[2];
        const u8 uv_h = particles[i].uv[3];
        const float w = particles[i].transform[3] * 0.5f;
        const float corners[12] = {
            -w, w * cos(g_camera.phi), w * sin(g_camera.phi),
            w, w * cos(g_camera.phi), w * sin(g_camera.phi),
            -w, -w * cos(g_camera.phi), -w * sin(g_camera.phi),
            w, -w * cos(g_camera.phi), -w * sin(g_camera.phi)
        };
        const float ct = cos(g_camera.theta);
        const float st = sin(g_camera.theta);
        const float gpos[12] = {
            x + ct*corners[0] - st*corners[1],
            y + st*corners[0] + ct*corners[1],
            z + corners[2],
            x + ct*corners[3] - st*corners[4],
            y + st*corners[3] + ct*corners[4],
            z + corners[5],
            x + ct*corners[6] - st*corners[7],
            y + st*corners[6] + ct*corners[7],
            z + corners[8],
            x + ct*corners[9] - st*corners[10],
            y + st*corners[9] + ct*corners[10],
            z + corners[11],
        };
        copy_corners_to_particle_vertex_buffer(vertices + n*54, gpos, particles[i].uv[0], particles[i].uv[1], particles[i].transform + 4);
        copy_corners_to_particle_vertex_buffer(vertices + n*54 + 9, gpos + 6, particles[i].uv[0], particles[i].uv[1] - uv_h, particles[i].transform + 4);
        copy_corners_to_particle_vertex_buffer(vertices + n*54 + 18, gpos + 3, particles[i].uv[0] + uv_w, particles[i].uv[1], particles[i].transform + 4);
        copy_corners_to_particle_vertex_buffer(vertices + n*54 + 27, gpos + 3, particles[i].uv[0] + uv_w, particles[i].uv[1], particles[i].transform + 4);
        copy_corners_to_particle_vertex_buffer(vertices + n*54 + 36, gpos + 6, particles[i].uv[0], particles[i].uv[1] - uv_h, particles[i].transform + 4);
        copy_corners_to_particle_vertex_buffer(vertices + n*54 + 45, gpos + 9, particles[i].uv[0] + uv_w, particles[i].uv[1] - uv_h, particles[i].transform + 4); 
        ++n;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, s_fbo);
    glDisable(GL_DEPTH_TEST);
    use_shader(PARTICLE_SHADER_PROGRAM);
    set_uniform_m4_value(SHADER_PARTICLE_CAMERA_TRANSFORM, GL_TRUE, g_camera_inverse_transform);
    use_texture(TEXTURE_TYPE_DIFF, GL_TEXTURE0, get_material_diff_texture(material));
    glBindVertexArray(s_particle_vao);
    glBindBuffer(GL_ARRAY_BUFFER, s_particle_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, 6 * n * PARTICLE_VERTEX_STRIDE, vertices, GL_STATIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, 6 * n);
    glEnable(GL_DEPTH_TEST);

    // Render to HDR
    glDisable(GL_BLEND);
    glBindFramebuffer(GL_FRAMEBUFFER, s_hdr_fbo);
    use_shader(PARTICLE_HDR_SHADER_PROGRAM);
    set_uniform_m4_value(SHADER_PARTICLE_HDR_CAMERA_TRANSFORM, GL_TRUE, g_camera_inverse_transform);
    set_uniform_1f_value(SHADER_PARTICLE_HDR_EMISSIVE_INTENSITY, get_material_emit_intensity(material));
    use_texture(TEXTURE_TYPE_EMIT, GL_TEXTURE0, get_material_emit_texture(material));
    glBindVertexArray(s_particle_vao);
    glBindBuffer(GL_ARRAY_BUFFER, s_particle_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, 6 * n * PARTICLE_VERTEX_STRIDE, vertices, GL_STATIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, 6 * n);
    glEnable(GL_BLEND);
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // return to normal framebuffer

}


// Sprites

// Render 2d/paper sprite objects in 3d space. This sprite will face the camera on the x-y plane (though not the Z due to looking out of place)
// Can rotate the sprite on the screen by angle psi
// Will render to the internal buffer but not the HDR buffer, currently does not support emissive sprites
void render_2d_image(const float *gpos, const float w, const float h, const float psi, const float u1, const float v1, const float u2, const float v2, const u16 texture) {
    const float corners[12] = {
        -cos(psi)*w/2 - sin(psi)*h, 0.f, -sin(psi)*w/2 + cos(psi)*h,
        cos(psi)*w/2 - sin(psi)*h, 0.f, sin(psi)*w/2 + cos(psi)*h,
        -cos(psi)*w/2, 0.f, -sin(psi)*w/2,
        cos(psi)*w/2, 0.f, sin(psi)*w/2,
    };

    const float gcorners[12] = {
        gpos[0] + cos(g_camera.theta)*corners[0] - sin(g_camera.theta)*corners[1], gpos[1] + sin(g_camera.theta)*corners[0] + cos(g_camera.theta)*corners[1], gpos[2] + corners[2],
        gpos[0] + cos(g_camera.theta)*corners[3] - sin(g_camera.theta)*corners[4], gpos[1] + sin(g_camera.theta)*corners[3] + cos(g_camera.theta)*corners[4], gpos[2] + corners[5],
        gpos[0] + cos(g_camera.theta)*corners[6] - sin(g_camera.theta)*corners[7], gpos[1] + sin(g_camera.theta)*corners[6] + cos(g_camera.theta)*corners[7], gpos[2] + corners[8],
        gpos[0] + cos(g_camera.theta)*corners[9] - sin(g_camera.theta)*corners[10], gpos[1] + sin(g_camera.theta)*corners[9] + cos(g_camera.theta)*corners[10], gpos[2] + corners[11]
    };

    // float cpos[12];
    // matrix4_loc3_multiply(g_camera_inverse_transform, gcorners, cpos);
    // matrix4_loc3_multiply(g_camera_inverse_transform, gcorners + 3, cpos + 3);
    // matrix4_loc3_multiply(g_camera_inverse_transform, gcorners + 6, cpos + 6);
    // matrix4_loc3_multiply(g_camera_inverse_transform, gcorners + 9, cpos + 9);
    // const float scale =  1.0 / (1.0 + distance_scale * sqrt(cpos[0]*cpos[0] + cpos[1]*cpos[1] + cpos[2]*cpos[2]));
    set_uniform_m4_value(SHADER_SPRITE_CAMERA, GL_TRUE, g_camera_inverse_transform);

    glBindFramebuffer(GL_FRAMEBUFFER, s_fbo);
    use_shader(SPRITE_SHADER_PROGRAM);
    use_texture(TEXTURE_TYPE_INTERFACE, GL_TEXTURE0, texture);
    set_uniform_v4_value(SHADER_SPRITE_LIGHT, 1.f, 1.f, 1.f, 1.f); // TODO get sprite lighting

    const float vertices[30] = {
        gcorners[0], gcorners[1], gcorners[2], u1, v1,
        gcorners[6], gcorners[7], gcorners[8], u1, v2,
        gcorners[3], gcorners[4], gcorners[5], u2, v1,
        gcorners[3], gcorners[4], gcorners[5], u2, v1,
        gcorners[6], gcorners[7], gcorners[8], u1, v2,
        gcorners[9], gcorners[10], gcorners[11], u2, v2,
    };

    glBindVertexArray(s_sprite_vao);
    glBindBuffer(GL_ARRAY_BUFFER, s_sprite_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, 30 * sizeof(float), vertices, GL_STATIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // return to normal framebuffer
}

// simple fade implementation where we just render a rect. Uses the color shader to do so
void render_fade(void) {
    if (!g_fade || !g_fade_amount) {
        return;
    }
    // (1 + cos t) / 2
    const float fade_amount = (float)g_fade_amount * (g_fade_period == 0 ? 1.0 : (0.5 * (1 - cos(6.28 * (g_system_vars[SYSVAR_FRAMES] - g_fade_start) / g_fade_period)))) / 128.0;
    const float r = g_fade_colors[g_fade][0], g = g_fade_colors[g_fade][1], b = g_fade_colors[g_fade][2], a = fade_amount;
    const float vertices[] = {
        -1.f, 1.f, 0.f, r, g, b, a,
        -1.f, -1.f, 0.f, r, g, b, a,
        1.f, 1.f, 0.f, r, g, b, a,
        1.f, 1.f, 0.f, r, g, b, a,
        -1.f, -1.f, 0.f, r, g, b, a,
        1.f, -1.f, 0.f, r, g, b, a,
    };
    use_shader(COLOR_SHADER_PROGRAM);
    glBindVertexArray(s_color_vao);
    glBindBuffer(GL_ARRAY_BUFFER, s_color_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, 42 * sizeof(float), vertices, GL_STATIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Static
////////////////////////////////////////////////////////////////////////////////////////////////////

// Renders a single object
// Calls glDrawElements. This means it is an expensive call. To optimize performance for static objects combine as many under the same material as possible
// For instanced meshes call render_instanced
// This is useful for complex standalone dynamic objects e.g. deformable ones
// TODO this function is doing a lot of repeated work calculating and sending the light data
// Move this to another function and only ever rewrite this information if we switch shaders
static void render_single(const struct scene_data *scene, const struct scene_static_object_data *data) {
    use_material(data->material);
    use_lightmap(LIGHTMAP_LEVEL, data->lightmap);
    glBindVertexArray(s_vao);

    glBindBuffer(GL_ARRAY_BUFFER, s_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, VERTEX_STRIDE * data->n_vertices, data->vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32) * data->n_indices, data->indices, GL_STATIC_DRAW);
    glDrawElements(GL_TRIANGLES, data->n_indices, GL_UNSIGNED_INT, 0);
}

static void render_single_vertices_only(const struct scene_static_object_data *data) {
    glBindVertexArray(s_vao);
    glBindBuffer(GL_ARRAY_BUFFER, s_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, VERTEX_STRIDE * data->n_vertices, data->vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32) * data->n_indices, data->indices, GL_STATIC_DRAW);
    glDrawElements(GL_TRIANGLES, data->n_indices, GL_UNSIGNED_INT, 0);
}

static void prepare_shader(u8 shader, const struct scene_lighting_data *lighting) {
    use_shader(shader);

    // This should only support 3D scene shaders. Lighting data is only set for lit shaders where uniform macros are hard coded
    if (shader == UNLIT_SHADER_PROGRAM) {
        set_uniform_m4_value(SHADER_UNLIT_CAMERA_INVERSE_TRANSFORM, GL_TRUE, g_camera_inverse_transform);
        return;
    }

    if (shader == DYNAMIC_OBJ_SHADER_PROGRAM) {
        set_uniform_m4_value(SHADER_DYNAMIC_OBJ_CAMERA, GL_TRUE, g_camera_inverse_transform);
        return;
    }

    set_uniform_m4_value(SHADER_LIT_CAMERA_INVERSE_TRANSFORM, GL_TRUE, g_camera_inverse_transform);

    set_uniform_v4_value(SHADER_LIT_AMBIENT_LIGHT, lighting->ambient_light[0], lighting->ambient_light[1], lighting->ambient_light[2], 1.f);
    set_uniform_v3_value(SHADER_LIT_DIRECTION_LIGHT, lighting->direction_light[0], lighting->direction_light[1], lighting->direction_light[2]);

    float dir[3];
    matrix4_dir3_multiply(g_camera_inverse_transform, lighting->direction_vec, dir);
    set_uniform_v3_value(SHADER_LIT_DIRECTION_VEC, dir[0], dir[1], dir[2]);

    float point_locs[3 * MAX_POINT_LIGHTS] = { 0 };
    float point_colors[3 * MAX_POINT_LIGHTS] = { 0 };
    for (u8 i = 0; i < *(lighting->num_point_lights); ++i) {
        matrix4_loc3_multiply(g_camera_inverse_transform, lighting->point_lights[i].location, point_locs + 3*i);
        point_colors[i*3] = lighting->point_lights[i].color[0];
        point_colors[i*3 + 1] = lighting->point_lights[i].color[1];
        point_colors[i*3 + 2] = lighting->point_lights[i].color[2];
    }
    set_uniform_3fv_values(SHADER_LIT_POINT_LOCS, *(lighting->num_point_lights) * 3, point_locs);
    set_uniform_3fv_values(SHADER_LIT_POINT_LIGHTS, *(lighting->num_point_lights) * 3, point_colors);

    float spot_locs[3 * MAX_SPOT_LIGHTS] = { 0 };
    float spot_colors[3 * MAX_SPOT_LIGHTS] = { 0 };
    float spot_dirs[3 * MAX_SPOT_LIGHTS] = { 0 };
    float spot_bounds[2 * MAX_SPOT_LIGHTS] = { 0 };
    for (u8 i = 0; i < *(lighting->num_spot_lights); ++i) {
        matrix4_loc3_multiply(g_camera_inverse_transform, lighting->spot_lights[i].location, spot_locs + 3*i);
        matrix4_dir3_multiply(g_camera_inverse_transform, lighting->spot_lights[i].direction, spot_dirs + 3*i);
        spot_colors[i*3] = lighting->spot_lights[i].color[0];
        spot_colors[i*3 + 1] = lighting->spot_lights[i].color[1];
        spot_colors[i*3 + 2] = lighting->spot_lights[i].color[2];
        spot_bounds[i*2] = lighting->spot_lights[i].inner;
        spot_bounds[i*2 + 1] = lighting->spot_lights[i].outer;
    }
    set_uniform_3fv_values(SHADER_LIT_SPOT_LOCS, *(lighting->num_spot_lights) * 3, spot_locs);
    set_uniform_3fv_values(SHADER_LIT_SPOT_LIGHTS, *(lighting->num_spot_lights) * 3, spot_colors);
    set_uniform_3fv_values(SHADER_LIT_SPOT_DIRS, *(lighting->num_spot_lights) * 3, spot_dirs);
    set_uniform_2fv_values(SHADER_LIT_SPOT_BOUNDS, *(lighting->num_spot_lights) * 2, spot_bounds);
}

static void copy_corners_to_particle_vertex_buffer(float *vertices, const float *gpos, const s16 u, const s16 v, const float *rgba) {
    vertices[0] = gpos[0];
    vertices[1] = gpos[1];
    vertices[2] = gpos[2];
    vertices[3] = u / 128.f;
    vertices[4] = v / 128.f;
    vertices[5] = rgba[0];
    vertices[6] = rgba[1];
    vertices[7] = rgba[2];
    vertices[8] = rgba[3];
}
