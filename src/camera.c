#include <math.h>
#include <SDL.h>
#include "camera.h"
#include "graphics/linalg.h"

struct camera g_camera = { 0 };

static float sintheta;
static float costheta;
static float sinphi;
static float cosphi;
float g_camera_inverse_transform[16] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };

// Call this at the start of every frame before doing anything
// Calculating sin, cos and the transform matrix may be a bit expensive so we do this once here
void calc_camera_inverse_transform(void) {
    sintheta = -sin(g_camera.theta);
    costheta = cos(g_camera.theta);
    sinphi = -sin(g_camera.phi);
    cosphi = cos(g_camera.phi);

    float rotate_x[16] = { 1, 0, 0, 0, 0, cosphi, -sinphi, 0, 0, sinphi, cosphi, 0, 0, 0, 0, 1 };
    float rotate_z[16] = { costheta, -sintheta, 0, 0, sintheta, costheta, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
    float translate[16] = { 1, 0, 0, -g_camera.x, 0, 1, 0, -g_camera.y, 0, 0, 1, -g_camera.z, 0, 0, 0, 1 };

    float all_rotations[16];
    matrix4_multiply(rotate_x, rotate_z, all_rotations);
    matrix4_multiply(all_rotations, translate, g_camera_inverse_transform);

/*
    g_camera_inverse_transform[0] = costheta;
    g_camera_inverse_transform[1] = -sintheta*cosphi;
    g_camera_inverse_transform[2] = sintheta*sinphi;
    g_camera_inverse_transform[3] = -costheta*g_camera.x + sintheta*cosphi*g_camera.y + sintheta*sinphi*g_camera.z;
    g_camera_inverse_transform[4] = sintheta;
    g_camera_inverse_transform[5] = costheta*cosphi;
    g_camera_inverse_transform[6] = -costheta*sinphi;
    g_camera_inverse_transform[7] = -sintheta*g_camera.x - costheta*cosphi*g_camera.y + costheta*sinphi*g_camera.z,
    g_camera_inverse_transform[8] = 0;
    g_camera_inverse_transform[9] = sinphi;
    g_camera_inverse_transform[10] = cosphi;
    g_camera_inverse_transform[11] = -sinphi*g_camera.y - cosphi*g_camera.z;
    g_camera_inverse_transform[12] = 0;
    g_camera_inverse_transform[13] = 0;
    g_camera_inverse_transform[14] = 0;
    g_camera_inverse_transform[15] = 1;
*/
}

// Must be called after calc_camera_inverse_transform so that values are updated
void get_camera_direction(float *x, float *y, float *z) {
    *y = sinphi; // sinphi is actually -sin(phi)
    *z = cosphi;
}

void move_camera_toward(const float rate, const float x, const float y, const float z, const float theta, const float phi) {
    g_camera.x = g_camera.x * (1.f - rate) + x * rate;
    g_camera.y = g_camera.y * (1.f - rate) + y * rate;
    g_camera.z = g_camera.z * (1.f - rate) + z * rate;
    g_camera.theta = g_camera.theta * (1.f - rate) + theta * rate;
    g_camera.phi = g_camera.phi * (1.f - rate) + phi * rate;
}
