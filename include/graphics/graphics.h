#ifndef GUARD_GRAPHICS_H
#define GUARD_GRAPHICS_H

#include "graphics/dynamic_objects.h"
#include "graphics/particles.h"
#include "scene.h"
#include "types.h"

// Headers for all graphics related functions, anything to do with modeling and rendering
extern void init_gl(void);
extern void init_buffers(void);
extern void clean_gl(void);
extern void init_text(void);
extern void clean_text(void);

// Render lifecycle
extern void start_render_cycle(void);
extern void render_clear(void);
extern void render_static_objects(const struct scene_data *scene);
extern void render_scene_dynamic_object(const float *gtransform, const float *ltransform, const struct dynamic_object_data *object, const struct scene_data *scene);
extern void render_dynamic_object(struct dynamic_object *dynamic_object, const struct scene_data *scene);
extern void render_point_particles(struct particle_system *ps, const u16 num_particles, const u32 material);
extern void render_line_particles(struct particle_system *ps, const u16 num_particles, const u32 material);
extern void render_quad_particles(struct particle_system *ps, const u16 num_particles, const u32 material);
extern void render_2d_image(const float *gpos, const float w, const float h, const float psi, const float u1, const float v1, const float u2, const float v2, const u16 texture);
extern void combine_and_post_process(const u32 post_process_flags);
extern void render_buffers_to_screen(void);
extern void preload_scene_textures(const struct scene_data *scene);

// Utilities
extern void render_screen_rect(void);
extern void render_color_rect(const float x, const float y, const float z, const float x2, const float y2, const u8 color, const float alpha);
extern void render_2d_rect(const float x, const float y, float z, const float x2, const float y2, const float u, const float v, const float u2, const float v2, const u8 texture);
extern void render_2d_border_rect(const float x, const float y, const float w, const float h, const float src_border_width, const float border_width, const u8 texture);
extern void render_text(const u8 *text, const u16 len, const float x, const float y, const float z, const u8 font, const u8 size, const u8 color);
extern void render_text_wrapped(const u8 *text, const u16 len, const float x, const float y, const float z, const float width, const float line_height, const u8 font, const u8 size, const u8 alignment, const u8 color);
extern void render_text_options_wrapped(const u8 *text, const u16 len, const u8 **options, const u8 cursor, const float x, const float y, const float z, const float width, const float line_height, const u8 font, const u8 size, const u8 alignment, const u8 color);
extern void render_fade(void);

// For testing only
extern void test_gl_draw_scene(const u8 start_scene_index);

#endif
