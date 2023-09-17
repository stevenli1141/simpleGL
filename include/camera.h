#ifndef GUARD_CAMERA_H
#define GUARD_CAMERA_H

struct camera {
    float theta; // degrees
    float phi; // degrees
    float x;
    float y;
    float z;
};

extern struct camera g_camera;
extern float g_camera_inverse_transform[16];
extern void calc_camera_inverse_transform(void);
extern void get_camera_direction(float *x, float *y, float *z);
extern void move_camera_toward(const float rate, const float x, const float y, const float z, const float theta, const float phi);

#endif
