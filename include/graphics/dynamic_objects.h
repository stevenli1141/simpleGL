#ifndef GUARD_DYNAMIC_OBJECTS_H
#define GUARD_DYNAMIC_OBJECTS_H

#include "scene.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// Dynamically allocated dynamic objects
////////////////////////////////////////////////////////////////////////////////////////////////////

#define DYNAMIC_OBJ_NONE            0

#define NUM_DYNAMIC_OBJS            32

extern const struct dynamic_object_data *g_dynamic_object_data[];

// Active dynamic objects

#define MAX_ACTIVE_DYNAMIC_OBJECTS  32

struct dynamic_object {
    u8 id;
    float transform[9];
    float rgba[4];
};

extern struct dynamic_object g_dynamic_objects[];

extern void render_active_dynamic_objects(const struct scene_data *scene);
extern const u8 create_dynamic_object(const u8 dynamic_object_type, const float *transform);
extern void calc_dynamic_obj_transform(struct dynamic_object *obj, float *matrix);

#endif
