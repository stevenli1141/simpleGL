#ifndef GUARD_SCENE_H
#define GUARD_SCENE_H

#include "types.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// Map definitions
////////////////////////////////////////////////////////////////////////////////////////////////////

#define NUM_LIGHTSCENES 3

#define COLLIDER_RECT 0
#define COLLIDER_PLANE 1
#define COLLIDER_SPHERE 2
#define COLLIDER_CYLINDER 3

#define TRANSFORM_X             1
#define TRANSFORM_Y             2
#define TRANSFORM_Z             4
#define TRANSFORM_THETA         8
#define TRANSFORM_PHI           16

#define CAMERA_DEFAULT          0
#define CAMERA_OVERRIDE_THETA   1
#define CAMERA_OVERRIDE_PHI     2
#define CAMERA_LOOKAT           3

#define MAX_SCENE_NAME_LEN      32

////////////////////////////////////////////////////////////////////////////////////////////////////
// Scene data
////////////////////////////////////////////////////////////////////////////////////////////////////

// Static mesh object
struct scene_static_object_data {
    const float *vertices;
    const u32 *indices;
    u16 n_vertices;
    u16 n_indices;
    u32 material;
    u8 lightmap;
};

// Dynamic mesh object. Can be have used outside scenes
struct dynamic_object_data {
    const float *vertices;
    const u32 *indices;
    u16 n_vertices;
    u16 n_indices;
    u32 material;
};

// Container for dynamic objects in a scene
struct scene_dynamic_object_data {
    const struct dynamic_object_data *dynamic_objects[8]; // max 8 dynamic objects
    float dynamic_transforms[40];
};

struct scene_point_light_data {
    float location[3];
    float color[3];
};

struct scene_spot_light_data {
    float location[3];
    float direction[3];
    float color[3];
    float inner;
    float outer;
};

// struct scene_light_data
struct scene_lighting_data {
    float ambient_light[3];
    float direction_vec[3];
    float direction_light[3];
    const struct scene_point_light_data *point_lights;
    const struct scene_spot_light_data *spot_lights;
    const u8 *num_point_lights;
    const u8 *num_spot_lights;
};

struct scene_collider_data {
    float inverse_transform[16];
    float scale[3];
    bool8 trigger;
    const u8 *script;
    u8 type;
};

typedef void (*scene_startup_func)(void);

// Main scene data
struct scene_data {
    const struct scene_static_object_data *static_data;
    const u32 *num_static_objects;
    const struct scene_lighting_data lighting;
    const u32 lightmaps[NUM_LIGHTSCENES+1];
    const struct scene_collider_data *colliders;
    const u8 *num_colliders;
    // const struct scene_particle_system_data particle_systems;
    // const u8 num_particle_systems;
    // u32 type;
    // u32 id;
    const float scene_scale;
    const scene_startup_func startup_cb;
};



#define INIT_SCENE(SCENE) \
extern const struct scene_static_object_data g_##SCENE##_static_objects[]; \
extern const u32 g_##SCENE##_num_static_objects; \
extern const struct scene_point_light_data g_##SCENE##_point_lights[]; \
extern const u8 g_##SCENE##_num_point_lights; \
extern const struct scene_spot_light_data g_##SCENE##_spot_lights[]; \
extern const u8 g_##SCENE##_num_spot_lights; \
extern const struct scene_collider_data g_##SCENE##_colliders[]; \
extern const u8 g_##SCENE##_num_colliders; \

INIT_SCENE(testscene)

extern const struct scene_data const g_scene_data[];

////////////////////////////////////////////////////////////////////////////////////////////////////
// Map control
////////////////////////////////////////////////////////////////////////////////////////////////////

extern u32 g_current_scene;

// Callable by scripts
extern void set_scene_locked(const bool8 locked);
extern const u8 load_and_set_scene(const u16 scene_id);
extern struct scene_event_data *get_scene_entity(const u8 id);
extern void set_entity_transform(struct scene_event_data *entity, const float x, const float y, const float z, const float theta, const u8 mask);
extern void set_entity_property_value(struct scene_event_data *entity, const u8 prop, const u8 value);
extern void move_entity_to(struct scene_event_data *entity, const float rate, const float x, const float y, const float z, const float theta, const u8 mask);
extern void move_entity_jump(struct scene_event_data *entity, const u8 t, const u8 delta_t, const float h, const float x, const float y, const float z, const float theta, const u8 mask);
extern void set_dynamic_subobject_transform(struct scene_event_data *entity, const u8 subindex, const float x, const float y, const float z, const float theta, const float phi);
extern void move_dynamic_subobject_to(struct scene_event_data *entity, const u8 subindex, const float rate, const float x, const float y, const float z, const float theta, const float phi);
extern void reset_scene_camera(void);

const u8 *g_scene_names[];

#endif

