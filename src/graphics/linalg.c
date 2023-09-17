#include <math.h>
#include "graphics/linalg.h"
#include "log.h"
#include "utils.h"

const float square_distance(const float *vector3) {
    return vector3[0]*vector3[0] + vector3[1]*vector3[1] + vector3[2]*vector3[2];
}

const float square_distance_between(const float *p1, const float *p2) {
    return (p1[0]-p2[0])*(p1[0]-p2[0]) + (p1[1]-p2[1])*(p1[1]-p2[1]) + (p1[2]-p2[2])*(p1[2]-p2[2]);
}

void normalize(float *vector3) {
    const float length = sqrt(square_distance(vector3));
    if (length > 0.f) {
        vector3[0] = vector3[0] / length;
        vector3[1] = vector3[1] / length;
        vector3[2] = vector3[2] / length;
    }
}

const float get_phi(const float *vector3) {
    const float dxy = sqrt(vector3[0]*vector3[0] + vector3[1]*vector3[1]);
    if (dxy <= 0.f) {
        return vector3[2] > 0.f ? 1.57f : -1.57f;
    }
    return atan(vector3[2] / dxy);
}

void matrix4_multiply(const float *left, const float *right, float *result) {
    for (u8 r = 0; r < 4; ++r) {
        for (u8 c = 0; c < 4; ++c) {
            result[r*4+c] = left[r*4]*right[c] + left[r*4+1]*right[4+c] + left[r*4+2]*right[8+c] + left[r*4+3]*right[12+c];
        }
    }
}

void matrix4_vector4_multiply(const float *matrix4, const float *vector4, float *result) {
    for (u8 r = 0; r < 4; ++r) {
        result[r] = matrix4[r*4]*vector4[0] + matrix4[r*4+1]*vector4[1] + matrix4[r*4+2]*vector4[2] + matrix4[r*4+3]*vector4[3];
    }
}

// Takes a vec3 representing position, so assumes 4th element is 1. outputs vector3
void matrix4_loc3_multiply(const float *matrix4, const float *vector3, float *result) {
    for (u8 r = 0; r < 3; ++r) {
        result[r] = matrix4[r*4]*vector3[0] + matrix4[r*4+1]*vector3[1] + matrix4[r*4+2]*vector3[2] + matrix4[r*4+3];
    }
}

// Takes a vec3 representing direction, so assumes 4th element is 0
void matrix4_dir3_multiply(const float *matrix4, const float *vector3, float *result) {
    for (u8 r = 0; r < 3; ++r) {
        result[r] = matrix4[r*4]*vector3[0] + matrix4[r*4+1]*vector3[1] + matrix4[r*4+2]*vector3[2];
    }
}

const bool8 test_box_collision(const float *inverse_transform, const float *scale, const float *position, const float tolerance) {
    float local[4];
    matrix4_loc3_multiply(inverse_transform, position, local);
    // translation and rotation have a constant jacobian, so only take scaling into account
    if (fabs(local[0]) > 1 + tolerance / scale[0]) return false;
    if (fabs(local[1]) > 1 + tolerance / scale[1]) return false;
    if (fabs(local[2]) > 1 + tolerance / scale[2]) return false;
    return true;
}

const float get_box_collision_dist(const float *inverse_transform, const float *scale, const float *position, const float tolerance) {
    float local[4];
    matrix4_loc3_multiply(inverse_transform, position, local);
    float min_dist = fabs(local[0]), r = scale[0];
    if (fabs(local[1] < min_dist)) {
        min_dist = fabs(local[1]);
        r = scale[1];
    }
    if (fabs(local[2] < min_dist)) {
        min_dist = fabs(local[2]);
        r = scale[2];
    }
    return min_dist / r;
}

const bool8 test_plane_collision(const float *inverse_transform, const float *scale, const float *position, const float tolerance) {
    float local[4];
    matrix4_loc3_multiply(inverse_transform, position, local);
    if (fabs(local[0]) > 1 + tolerance / scale[0]) return false;
    if (fabs(local[1]) > 1 + tolerance / scale[1]) return false;
    return fabs(local[2]) < 0.1 + tolerance; // need at least some buffer for z to prevent things from slipping through
}

const bool8 test_sphere_collision(const float *inverse_transform, const float *scale, const float *position, const float tolerance) {
    float local[4];
    matrix4_loc3_multiply(inverse_transform, position, local);
    // translation and rotation have a constant jacobian, so only take scaling into account
    float rescale = scale[0]*scale[1]*scale[2];
    float distance = sqrt(square_distance(local));

    return distance < 1 + tolerance / rescale;
}

const bool8 test_cylinder_collision(const float *inverse_transform, const float *scale, const float *position, const float tolerance) {
    float local[4];
    matrix4_loc3_multiply(inverse_transform, position, local);
    // translation and rotation have a constant jacobian, so only take scaling into account
    float min_distance = sqrt(local[0]*local[0] + local[1]*local[1]);

    return min_distance < 1.0 && fabs(local[2]) < 1.0;
}
