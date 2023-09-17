#ifndef GUARD_LINALG_H
#define GUARD_LINALG_H

#include "types.h"

extern const float square_distance(const float *vector3);
extern const float square_distance_between(const float *p1, const float *p2);
extern void normalize(float *vector3);
extern const float get_phi(const float *vector3);

// multiples left onto right
// result must not be related to left or right
// matrices are in row major form
extern void matrix4_multiply(const float *left, const float *right, float *result);

extern void matrix4_vector4_multiply(const float *matrix4, const float *vector4, float *result);
extern void matrix4_loc3_multiply(const float *matrix4, const float *vector3, float *result);
extern void matrix4_dir3_multiply(const float *matrix4, const float *vector3, float *result);

extern const bool8 test_box_collision(const float *inverse_transform, const float *scale, const float *position, const float tolerance);
extern const bool8 test_plane_collision(const float *inverse_transform, const float *scale, const float *position, const float tolerance);
extern const bool8 test_sphere_collision(const float *inverse_transform, const float *scale, const float *position, const float tolerance);
extern const bool8 test_cylinder_collision(const float *inverse_transform, const float *scale, const float *position, const float tolerance);

#endif
