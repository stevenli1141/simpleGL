#include <GL/glew.h>
#include "graphics/graphics.h"
#include "graphics/shaders.h"
#include "types.h"
#include "config.h"

// Post processing function library

////////////////////////////////////////////////////////////////////////////////////////////////////
// Bloom
////////////////////////////////////////////////////////////////////////////////////////////////////

// Idea is bloom is very expensive for a high resolution.
// Using a cheap 2-directional independent gaussian is an often cited option, but I found its results were disappointing.
// Downsampling and upsampling with custom kernels is probably the easiest option without getting too complicated. It also leaves plenty of room for tuning.
// Below are the preset ways of doing it which can be easily tuned

#define BLOOM_GAUSSIAN_BLUR_N_ITERATIONS 7

static u32 s_blur_fbos[BLOOM_GAUSSIAN_BLUR_N_ITERATIONS];
static u32 s_blur_tex[BLOOM_GAUSSIAN_BLUR_N_ITERATIONS];
static const u8 factors[] =  {   2,   4,    8,   16,    16,    8,    4,   2 }; // Power of 2 scaling (inverse), 1 = no change, 2 = 1/2 width
static const float shape[] = { 2.f, 1.6f, 1.2f, 1.2f,  1.1f, 1.6f,  1.5f, 1.5f }; // Factor to multiply at each stage, changing these can yield interesting results

void init_postprocessing(void) {
    // Setup 2-pass Gaussian blur buffers
    glGenFramebuffers(BLOOM_GAUSSIAN_BLUR_N_ITERATIONS, s_blur_fbos);
    glGenTextures(BLOOM_GAUSSIAN_BLUR_N_ITERATIONS, s_blur_tex);
    for (unsigned int i = 0; i < BLOOM_GAUSSIAN_BLUR_N_ITERATIONS; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, s_blur_fbos[i]);
        glBindTexture(GL_TEXTURE_2D, s_blur_tex[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCREEN_WIDTH / factors[i], SCREEN_HEIGHT / factors[i], 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, s_blur_tex[i], 0);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void clean_postprocessing(void) {
    glDeleteFramebuffers(BLOOM_GAUSSIAN_BLUR_N_ITERATIONS, s_blur_fbos);
    glDeleteTextures(BLOOM_GAUSSIAN_BLUR_N_ITERATIONS, s_blur_tex);
}

// Renders bloom by repeatedly rendering from one buffer texture onto the other. Returns final texture in iteration
const u32 bloom(const u32 hdr_tex) {
    use_shader(GAUSSIAN_SHADER_PROGRAM);
    // cycle
    for (u8 i = 0; i < BLOOM_GAUSSIAN_BLUR_N_ITERATIONS; ++i) {
        glViewport(0, 0, SCREEN_WIDTH / factors[i], SCREEN_HEIGHT / factors[i]);
        glBindFramebuffer(GL_FRAMEBUFFER, s_blur_fbos[i]);
        glClearColor(0.f, 0.f, 0.f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, i == 0 ? hdr_tex : s_blur_tex[i-1]);
        const u8 factor = i == 0 ? 1 : factors[i-1];
        const float s = i == 0 ? 1.f : shape[i-1];
        set_uniform_v3_value(SHADER_GAUSSIAN_DIR, factor * 1.f / SCREEN_WIDTH, factor * 1.f / SCREEN_HEIGHT, s);

        render_screen_rect();

        // x = 1.f - x;
        // y = 1.f - y;
    }

    return s_blur_tex[BLOOM_GAUSSIAN_BLUR_N_ITERATIONS - 1];
}


