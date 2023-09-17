#ifndef GUARD_PARTICLES_H
#define GUARD_PARTICLES_H

#include "types.h"

#define PARTICLE_NONE               0
#define PARTICLE_RED                1
#define PARTICLE_GREEN              2
#define PARTICLE_BLUE               3
#define PARTICLE_RGB                4
#define PARTICLE_RED_SPARKS         5
#define PARTICLE_GREEN_SPARKS       6
#define PARTICLE_BLUE_SPARKS        7
#define PARTICLE_BLUE_SWIRL         8
#define PARTICLE_BLUE_RED           9
#define PARTICLE_FIRE               10
#define PARTICLE_GAS                20
#define NUM_PARTICLE_PRESETS        256

#define PARTICLE_TYPE_POINTS        0
#define PARTICLE_TYPE_LINES         1
#define PARTICLE_TYPE_TRIS          2
#define PARTICLE_TYPE_QUADS         3
#define PARTICLE_TYPE_UNLIT_MESH    4
#define PARTICLE_TYPE_LIT_MESH      5

#define PARTICLE_ACC_INVERSE        1
#define PARTICLE_ACC_NO_FOLLOW      2

#define MAX_PARTICLE_SYSTEMS        16
#define MAX_PARTICLES_PER_SYSTEM    256


struct particle {
    u8 id;
    u16 timer;
    u8 uv[4];
    float transform[8];
    float velocity[8];
};

struct particle_system {
    u8 id;
    s16 timer;
    float position[3];
    float rotation[3];
    struct particle particles[MAX_PARTICLES_PER_SYSTEM];
};

struct particle_system_data {
    const u8 type;
    const u8 rate; // rate at which new particles are spawned. normalized so that 16 is 1 per frame
    const s16 lifetime; // 0 is forever
    const u8 avg_lifetime;
    const u8 lifetime_range;
    const u16 material;
    const float s0[8]; // xyz scale rgba
    const float s0_variance[8];
    const float v0[3]; // initial velocity mean
    const float v0_variance[3]; // initial velocity variance
    const float acc[6]; // acceleration function
    const u8 acc_type; // non-constant types can be used
    const u8 scale_rate; // grows/decays by scale_rate. 128 is 100%
    const u8 max_alpha; // upper threshold, alpha increases as we get here
    const u8 alpha_decay; // at what time in particle lifetime does alpha decrease
    const u8 uvs[4]; // 128 is max
    const u8 uv_change; // rate at which we shift UV offsets. UVs are shifted by width 0 means no change
};

extern const struct particle_system_data g_particle_system_data[];

extern const u8 create_particle_system(const u8 type, const float x, const float y, const float z);
extern void set_particle_system_position(const u8 i, const float x, const float y, const float z);
extern void set_particle_system_rotation(const u8 i, const float theta, const float phi, const float psi);
extern void update_particles(void);
extern void render_particles(void);

// Testing only
extern void test_particle_systems(void);

#endif
