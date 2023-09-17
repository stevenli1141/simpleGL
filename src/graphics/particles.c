#include <math.h>
#include "constants/vars.h"
#include "graphics/graphics.h"
#include "graphics/materials.h"
#include "graphics/particles.h"
#include "log.h"
#include "system.h"
#include "utils.h"

// examples of particles
const struct particle_system_data g_particle_system_data[NUM_PARTICLE_PRESETS] = {
    [PARTICLE_RED] = {
        .type = PARTICLE_TYPE_POINTS,
        .rate = 32,
        .material = MAT_POINTPARTICLE,
        .lifetime = 20,
        .avg_lifetime = 20,
        .lifetime_range = 10,
        .s0 = { 0.f, 0.f, 0.f, 1.f, 1.f, 1.f, 1.f, 1.f },
        .s0_variance = { 0.2f, 0.2f, 0.2f, 0.f, 0.f, 0.f, 0.f, 0.01f },
        .v0 = { 0.f, 0.f, 0.01f },
        .v0_variance = { 0.005f, 0.005f, 0.002f },
        .acc = { 0.f, 0.f, 0.f, 0.f, 0.f, -0.0002f },
        .alpha_decay = 10,
        .uvs = { 24, 0, 0, 0 }
    },
    [PARTICLE_GREEN] = {
        .type = PARTICLE_TYPE_POINTS,
        .rate = 32,
        .material = MAT_POINTPARTICLE,
        .lifetime = 20,
        .avg_lifetime = 20,
        .lifetime_range = 10,
        .s0 = { 0.f, 0.f, 0.f, 1.f, 0.8f, 1.f, 1.f, 1.f },
        .s0_variance = { 0.2f, 0.2f, 0.2f, 0.f, 0.f, 0.f, 0.f, 0.01f },
        .v0 = { 0.01f, 0.01f, 0.02f },
        .v0_variance = { 0.01f, 0.01f, 0.01f },
        .acc = { 0.f, 0.f, 0.f, 0.f, 0.f, 0.f },
        .alpha_decay = 10,
        .uvs = { 36, 0, 0, 0 }
    },
    [PARTICLE_BLUE] = {
        .type = PARTICLE_TYPE_POINTS,
        .rate = 32,
        .material = MAT_POINTPARTICLE,
        .lifetime = 20,
        .avg_lifetime = 20,
        .lifetime_range = 10,
        .s0 = { 0.f, 0.f, 0.f, 1.f, 0.6f, 0.7f, 1.6f, 1.f },
        .s0_variance = { 0.05f, 0.05f, 0.05f, 0.f, 0.f, 0.f, 0.f, 0.01f },
        .v0 = { 0.f, 0.f, 0.01f },
        .v0_variance = { 0.005f, 0.005f, 0.002f },
        .acc = { 0.f, 0.f, 0.f, 0.f, 0.f, -0.0002f },
        .alpha_decay = 10,
        .uvs = { 8, 0, 0, 0 }
    },
    [PARTICLE_RGB] = {
        .type = PARTICLE_TYPE_POINTS,
        .rate = 32,
        .material = MAT_POINTPARTICLE,
        .lifetime = 20,
        .avg_lifetime = 20,
        .lifetime_range = 10,
        .s0 = { 0.f, 0.f, 0.f, 1.f, 0.8f, 0.8f, 0.8f, 1.f },
        .s0_variance = { 0.05f, 0.05f, 0.05f, 0.f, 0.2f, 0.2f, 0.2f, 0.01f },
        .v0 = { 0.f, 0.f, 0.01f },
        .v0_variance = { 0.005f, 0.005f, 0.002f },
        .acc = { 0.f, 0.f, 0.f, 0.f, 0.f, -0.0002f },
        .alpha_decay = 10,
        .uvs = { 8, 0, 0, 0 }
    },
    [PARTICLE_BLUE_SWIRL] = {
        .type = PARTICLE_TYPE_LINES,
        .rate = 64,
        .material = MAT_POINTPARTICLE,
        .lifetime = 40,
        .avg_lifetime = 30,
        .lifetime_range = 20,
        .s0 = { 0.2f, 0.f, 0.1f, 1.f, 0.5f, 0.7f, 1.2f, 1.f },
        .s0_variance = { 0.02f, 0.02f, 0.1f, 0.f, 0.f, 0.f, 0.f, 0.01f },
        .v0 = { 0.01f, 0.01f, 0.f },
        .v0_variance = { 0.f, 0.01f, 0.001f },
        .acc = { -0.03f, 0.f, -0.03f, 0.f, 0.0002f, -0.0002f },
        .alpha_decay = 10,
        .uvs = { 8, 0, 0, 0 }
    }
};

static struct particle_system s_particle_systems[MAX_PARTICLE_SYSTEMS];

static const float generate_approx_gaussian(void);
static const bool8 can_evict_particle_system(struct particle_system *ps);
static void spawn_particle(struct particle_system *ps);
static void update_particle(struct particle *particle);

const u8 create_particle_system(const u8 type, const float x, const float y, const float z) {
    for (u8 i = 0; i < MAX_PARTICLE_SYSTEMS; ++i) {
        
        if (can_evict_particle_system(&s_particle_systems[i])) {
            s_particle_systems[i].id = type;
            s_particle_systems[i].timer = g_particle_system_data[type].lifetime;
            s_particle_systems[i].position[0] = x;
            s_particle_systems[i].position[1] = y;
            s_particle_systems[i].position[2] = z;
            s_particle_systems[i].rotation[0] = 0.f;
            s_particle_systems[i].rotation[1] = 0.f;
            s_particle_systems[i].rotation[2] = 0.f;
            g_system_vars[SYSVAR_LATEST_PARTICLE_SYSTEM] = i;
            return i;
        }
    }
    LOG_WARN("Unable to create new particle system, not enough space.\n")
    return MAX_PARTICLE_SYSTEMS;
}

void set_particle_system_position(const u8 i, const float x, const float y, const float z) {
    s_particle_systems[i].position[0] = x;
    s_particle_systems[i].position[1] = y;
    s_particle_systems[i].position[2] = z;
}

void set_particle_system_rotation(const u8 i, const float theta, const float phi, const float psi) {
    s_particle_systems[i].rotation[0] = theta;
    s_particle_systems[i].rotation[1] = phi;
    s_particle_systems[i].rotation[2] = psi; // not currently supported
}

void update_particles(void) {
    for (u8 i = 0; i < MAX_PARTICLE_SYSTEMS; ++i) {
        struct particle_system *ps = &s_particle_systems[i];
        if (can_evict_particle_system(ps)) {
            s_particle_systems[i].id = 0;
            continue;
        }
        if (!ps->id) {
            continue;
        }
        const u16 lifetime = g_particle_system_data[ps->id].lifetime;
        for (u16 j = 0; j < MAX_PARTICLES_PER_SYSTEM; ++j) {
            if (ps->particles[j].id == 0) {
                continue;
            }
            update_particle(&(ps->particles[j]));
        }
        if (ps->timer == 0) {
            continue;
        }
        if (ps->timer > 0) {
            ps->timer--;
        }

        const u8 rate = g_particle_system_data[ps->id].rate;
        u8 n = rate / 16;
        if (n == 0 && rate) {
            n = !(g_system_vars[SYSVAR_FRAMES] % (16 / rate)); // spawn 1 if we are on this frame
        }
        for (u8 j = 0; j < n; ++j) {
            spawn_particle(ps);
        }
    }
}

void render_particles(void) {
    for (u8 i = 0; i < MAX_PARTICLE_SYSTEMS; ++i) {
        if (!s_particle_systems[i].id) {
            continue;
        }
        const u8 type = g_particle_system_data[s_particle_systems[i].id].type;
        const u32 material = g_particle_system_data[s_particle_systems[i].id].material;
        switch (type) {
        case PARTICLE_TYPE_POINTS:
            render_point_particles(&s_particle_systems[i], MAX_PARTICLES_PER_SYSTEM, material);
            break;
        case PARTICLE_TYPE_LINES:
            render_line_particles(&s_particle_systems[i], MAX_PARTICLES_PER_SYSTEM, material);
            break;
        case PARTICLE_TYPE_TRIS:
            break;
        case PARTICLE_TYPE_QUADS:
            render_quad_particles(&s_particle_systems[i], MAX_PARTICLES_PER_SYSTEM, material);
            break;
        }
    }
}

static void spawn_particle(struct particle_system *ps) {
    const struct particle_system_data *data = &g_particle_system_data[ps->id];
    for (u16 i = 0; i < MAX_PARTICLES_PER_SYSTEM; ++i) {
        if (ps->particles[i].id == 0) {
            // generate 3 independent gaussians so we can have completely uncorrelated for 3 dimensions, after that can reuse
            const float r1 = generate_approx_gaussian();
            const float r2 = generate_approx_gaussian();
            const float r3 = generate_approx_gaussian();
            ps->particles[i].id = ps->id;
            ps->particles[i].timer = data->avg_lifetime;
            if (data->lifetime_range > 0) {
                ps->particles[i].timer += RAND(data->lifetime_range) - data->lifetime_range / 2;
            }
            const float x = data->s0[0] + r1 * data->s0_variance[0];
            const float y = data->s0[1] + r2 * data->s0_variance[1];
            const float z = data->s0[2] + r3 * data->s0_variance[2];
            const float vx = data->v0[0] + r1 * data->v0_variance[0];
            const float vy = data->v0[1] + r2 * data->v0_variance[1];
            const float vz = data->v0[2] + r3 * data->v0_variance[2];
            ps->particles[i].transform[0] = x;
            ps->particles[i].transform[1] = y;
            ps->particles[i].transform[2] = z;
            ps->particles[i].transform[3] = data->s0[3] + r1 * data->s0_variance[3];
            ps->particles[i].transform[4] = data->s0[4] + r2 * data->s0_variance[4];
            ps->particles[i].transform[5] = data->s0[5] + r3 * data->s0_variance[5];
            ps->particles[i].transform[6] = data->s0[6] + r1 * data->s0_variance[6];
            ps->particles[i].transform[7] = data->s0[7] + r2 * data->s0_variance[7];
            ps->particles[i].velocity[0] = vx;
            ps->particles[i].velocity[1] = vy;
            ps->particles[i].velocity[2] = vz;
            ps->particles[i].uv[0] = data->uvs[0];
            ps->particles[i].uv[1] = data->uvs[1];
            ps->particles[i].uv[2] = data->uvs[2];
            ps->particles[i].uv[3] = data->uvs[3];

            if (data->acc_type & PARTICLE_ACC_NO_FOLLOW) {
                const float ct = cos(ps->rotation[0]), st = sin(ps->rotation[0]);
                const float cp = cos(ps->rotation[1]), sp = sin(ps->rotation[1]);
                ps->particles[i].transform[0] = ct*x - st*(cp*y - sp*z) + ps->position[0];
                ps->particles[i].transform[1] = st*x + ct*(sp*y + cp*z) + ps->position[1];
                ps->particles[i].transform[2] = sp*y + cp*z + ps->position[2];
                ps->particles[i].velocity[0] = ct*vx - st*(cp*vy - sp*vz);
                ps->particles[i].velocity[1] = st*vx + ct*(sp*vy + cp*vz);
                ps->particles[i].velocity[2] = sp*vy + cp*vz;
            }

            break;
        }
    }
}

// probably the closest we can get to the inverse error function without doing anything more complicated. still requires a natural log
static const float generate_approx_gaussian() {
    return 0.4 * log(2.0 / ((RAND(256) - 128) / 129.0 + 1) - 1);
}

static const bool8 can_evict_particle_system(struct particle_system *ps) {
    if (ps->id == 0) {
        return true;
    }
    if (ps->timer != 0) {
        return false;
    }
    for (u16 i = 0; i < MAX_PARTICLES_PER_SYSTEM; ++i) {
        if (ps->particles[i].id) {
            return false;
        }
    }
    return true;
}

static void update_particle(struct particle *particle) {
    const struct particle_system_data *ps = &g_particle_system_data[particle->id];
    particle->timer--;
    if (particle->timer == 0) {
        particle->id = 0;
        return;
    }
    const float r = generate_approx_gaussian();
    // update velocity
    if (ps->acc_type & PARTICLE_ACC_INVERSE) {
        if (particle->transform[0] != 0.f) {
            particle->velocity[0] = particle->velocity[0] + ps->acc[0] / particle->transform[0] + ps->acc[1];
        }
        if (particle->transform[1] != 0.f) {
            particle->velocity[1] = particle->velocity[1] + ps->acc[2] / particle->transform[1] + ps->acc[3];
        }
        if (particle->transform[2] != 0.f) {
            particle->velocity[2] = particle->velocity[2] + ps->acc[4] / particle->transform[2] + ps->acc[5];
        }
    } else {
        // Note for NO_FOLLOW case the velocity and position are not affected, but acc is based on relative to ps
        particle->velocity[0] = particle->velocity[0] + ps->acc[0]*particle->transform[0] + ps->acc[1];
        particle->velocity[1] = particle->velocity[1] + ps->acc[2]*particle->transform[1] + ps->acc[3];
        particle->velocity[2] = particle->velocity[2] + ps->acc[4]*particle->transform[2] + ps->acc[5];
    }

    // update transform
    particle->transform[0] = particle->transform[0] + particle->velocity[0];
    particle->transform[1] = particle->transform[1] + particle->velocity[1];
    particle->transform[2] = particle->transform[2] + particle->velocity[2];

    // scale grwoth/decay
    particle->transform[3] = particle->transform[3] * ps->scale_rate / 128.0;
    // alpha change
    if (particle->timer < ps->alpha_decay && particle->transform[7] > 0.f) {
        particle->transform[7] -= 1.0 / ps->alpha_decay;
    } else if (ps->max_alpha) {
        particle->transform[7] = 0.75 * particle->transform[7] + 0.25 * ps->max_alpha / 128.0;
    }
}
