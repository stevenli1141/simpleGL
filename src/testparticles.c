#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <math.h>
#include "constants/scenes.h"
#include "constants/vars.h"
#include "graphics/graphics.h"
#include "graphics/lightmaps.h"
#include "graphics/particles.h"
#include "graphics/postprocess.h"
#include "graphics/shaders.h"
#include "camera.h"
#include "config.h"
#include "controller.h"
#include "log.h"
#include "scene.h"
#include "system.h"

// create_particle_system
void test_particle_systems(void) {
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        LOG_ERROR("SDL could not initialize! SDL_Error: %s\n", SDL_GetError())
        return;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#if ENABLE_MSAA
    if (SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1) != 0) {
        LOG_WARN("Unable to set multisample buffers! SDL Error: %s\n", SDL_GetError())
    }
    if (SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 2) != 0) {
        LOG_WARN("Unable to set multisample samples! SDL Error: %s\n", SDL_GetError())
    }
#endif
    SDL_Window *window = SDL_CreateWindow("FF72D", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (window == NULL) {
        return;
    }
    SDL_GLContext *gl_context = SDL_GL_CreateContext(window);
    if (gl_context == NULL) {
        LOG_ERROR("Could not create OpenGL context: %s\n", SDL_GetError())
        return;
    }
    // Use Vsync
    if (SDL_GL_SetSwapInterval(1) < 0) {
        LOG_WARN("Unable to set VSync! SDL Error: %s\n", SDL_GetError())
    }

    init_gl(); LOG_INFO("Init GL success\n")
    init_shaders(); LOG_INFO("Loaded shaders\n")
    init_buffers(); LOG_INFO("Initialized buffers\n")
    init_lightmaps(); LOG_INFO("Initialized lightmaps\n")
    init_postprocessing(); LOG_INFO("Initialized post processing resources\n")
    init_text(); LOG_INFO("Initialized text renderer\n")

    g_camera.z = 1.f;

    // TODO create a load scene function
    const struct scene_data *scene = &g_scene_data[TEST_SCENE_1];
    load_lightmaps(4, scene->lightmaps);

    u16 timer = 0;
    u8 fire_system = MAX_PARTICLE_SYSTEMS;

    for (;;) {
        controller_input_update();
        if (g_controller_quit) {
            return;
        }
        if (g_key_pressed[INPUT_LEFT]) {
            g_camera.x -= 0.1f * cos(g_camera.theta);
            g_camera.y -= 0.1f * sin(g_camera.theta);
        }
        if (g_key_pressed[INPUT_RIGHT]) {
            g_camera.x += 0.1f * cos(g_camera.theta);
            g_camera.y += 0.1f * sin(g_camera.theta);
        }
        if (g_key_pressed[INPUT_UP]) {
            g_camera.x -= 0.1f * sin(g_camera.theta);
            g_camera.y += 0.1f * cos(g_camera.theta);
        }
        if (g_key_pressed[INPUT_DOWN]) {
            g_camera.x += 0.1f * sin(g_camera.theta);
            g_camera.y -= 0.1f * cos(g_camera.theta);
        }
        if (g_key_pressed[INPUT_PAGEUP]) {
            g_camera.z += 0.1f;
        }
        if (g_key_pressed[INPUT_PAGEDOWN]) {
            g_camera.z -= 0.1f;
        }
        if (g_key_pressed[INPUT_CONFIRM]) {
            g_camera.phi += 0.1f;
        }
        if (g_key_pressed[INPUT_CANCEL]) {
            g_camera.phi -= 0.1f;
        }
        if (g_key_pressed[INPUT_MENU]) { // S
            g_camera.theta -= 0.1f;
        }
        if (g_key_pressed[INPUT_PLAYCARDS]) { // A
            g_camera.theta += 0.1f;
        }

        ++timer;
        if (timer == 120) {
            // create_particle_system(PARTICLE_GREEN, 0.f, 0.f, 0.2f);
            timer = 0;
        }
        if (timer == 40) {
            create_particle_system(PARTICLE_FIRE, -0.5f, 0.f, 0.2f);
        }
        if (timer == 60) {
            create_particle_system(PARTICLE_BLUE_SWIRL, 0.5f, -1.f, 0.2f);
        }

        g_system_vars[SYSVAR_FRAMES]++;

        calc_camera_inverse_transform();
        update_particles();
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        start_render_cycle();
        render_static_objects(scene);
        render_particles();
        combine_and_post_process(POST_PROCESS_BLOOM);
        render_buffers_to_screen();

        SDL_GL_SwapWindow(window);
        SDL_Delay(30);
    }

    clean_lightmaps();
    clean_postprocessing();
    clean_gl();
}
