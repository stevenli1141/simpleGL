#include <GL/glew.h>
#include <math.h>
#include <string.h>
#include "constants/vars.h"
#include "graphics/dynamic_objects.h"
#include "graphics/graphics.h"
#include "graphics/linalg.h"
#include "graphics/shaders.h"
#include "camera.h"
#include "log.h"
#include "system.h"

const struct dynamic_object_data *g_dynamic_object_data[NUM_DYNAMIC_OBJS] = {
};

struct dynamic_object g_dynamic_objects[MAX_ACTIVE_DYNAMIC_OBJECTS];

void render_active_dynamic_objects(const struct scene_data *scene) {
    use_shader(DYNAMIC_OBJ_SHADER_PROGRAM);
    set_uniform_v4_value(SHADER_DYNAMIC_OBJ_AMBIENT_LIGHT, scene->lighting.ambient_light[0], scene->lighting.ambient_light[1], scene->lighting.ambient_light[2], scene->lighting.ambient_light[3]);
    float dir[3];
    matrix4_dir3_multiply(g_camera_inverse_transform, scene->lighting.direction_vec, dir);
    set_uniform_v3_value(SHADER_DYNAMIC_OBJ_DIRECTION_VEC, dir[0], dir[1], dir[2]);
    set_uniform_v3_value(SHADER_DYNAMIC_OBJ_DIRECTION_LIGHT, scene->lighting.direction_light[0], scene->lighting.direction_light[1], scene->lighting.direction_light[2]);
    set_uniform_m4_value(SHADER_DYNAMIC_OBJ_CAMERA, GL_TRUE, g_camera_inverse_transform);

    for (u8 i = 0; i < MAX_ACTIVE_DYNAMIC_OBJECTS; ++i) {
        if (!g_dynamic_objects[i].id) {
            continue;
        }
        render_dynamic_object(&g_dynamic_objects[i], scene);
    }
}

const u8 create_dynamic_object(const u8 dynamic_object_type, const float *transform) {
    for (u8 i = 0; i < MAX_ACTIVE_DYNAMIC_OBJECTS; ++i) {
        if (!g_dynamic_objects[i].id) {
            g_dynamic_objects[i].id = dynamic_object_type;
            memcpy(g_dynamic_objects[i].transform, transform, sizeof(float) * 9);
            g_dynamic_objects[i].rgba[0] = 1.f;
            g_dynamic_objects[i].rgba[1] = 1.f;
            g_dynamic_objects[i].rgba[2] = 1.f;
            g_dynamic_objects[i].rgba[3] = 1.f;
            g_system_vars[SYSVAR_LATEST_DYNAMIC_OBJECT] = i;
            return i;
        }
    }
    return MAX_ACTIVE_DYNAMIC_OBJECTS;
}

void calc_dynamic_obj_transform(struct dynamic_object *dynamic_object, float *mat) {
    const float st = sin(dynamic_object->transform[3]);
    const float ct = cos(dynamic_object->transform[3]);
    const float sf = sin(dynamic_object->transform[4]);
    const float cf = cos(dynamic_object->transform[4]);
    const float sps = sin(dynamic_object->transform[5]);
    const float cps = cos(dynamic_object->transform[5]);

    const float scale_translate[16] = {
        dynamic_object->transform[6], 0, 0, dynamic_object->transform[0],
        0, dynamic_object->transform[7], 0, dynamic_object->transform[1],
        0, 0, dynamic_object->transform[8], dynamic_object->transform[2],
        0, 0, 0, 1
    };
    const float ry[16] = { cps, 0, -sps, 0, 0, 1, 0, 0, sps, 0, cps, 0, 0, 0, 0, 1 };
    const float rx[16] = { 1, 0, 0, 0, 0, cf, -sf, 0, 0, sf, cf, 0, 0, 0, 0, 1 };
    const float rz[16] = { ct, -st, 0, 0, st, ct, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };

    float tmp1[16];
    float tmp2[16];
    matrix4_multiply(ry, rx, tmp1);
    matrix4_multiply(rz, tmp1, tmp2);
    matrix4_multiply(scale_translate, tmp2, mat);
}
