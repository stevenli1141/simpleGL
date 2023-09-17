// This is a test module for experimenting with OpenGL

#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <math.h>
#include <stdio.h>
#include "constants/scenes.h"
#include "graphics/font.h"
#include "graphics/graphics.h"
#include "graphics/lightmaps.h"
#include "graphics/linalg.h"
#include "graphics/postprocess.h"
#include "graphics/shaders.h"
#include "graphics/textures.h"
#include "camera.h"
#include "config.h"
#include "controller.h"
#include "log.h"
#include "scene.h"
#include "types.h"


static s16 counter = 0;

#define NUM_AVAILABLE_SCENES 2
static const u16 available_scenes[NUM_AVAILABLE_SCENES] = { TEST_SCENE_1, TEST_SCENE_2 };

// Simple test with panning and pitching camera
void test_gl_draw_scene(const u8 start_index) {
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
    init_textures(); LOG_INFO("Initialized textures\n")
    init_postprocessing(); LOG_INFO("Initialized post processing resources\n")
    init_text(); LOG_INFO("Initialized text renderer\n")

    g_camera.z = 1.f;

    u16 cur_scene = start_index;

    // TODO create a load map function
    const struct scene_data *scene = &g_scene_data[available_scenes[cur_scene]];
    load_lightmaps(4, scene->lightmaps);

    for (;;) {
        controller_input_update();
        if (g_controller_quit) {
            return;
        }
        if (g_key_pressed[INPUT_SPECIAL]) {
            cur_scene = (cur_scene + 1) % NUM_AVAILABLE_SCENES;
            LOG_INFO("Changing maps to %d\n", available_scenes[cur_scene])
            scene = &g_scene_data[available_scenes[cur_scene]];
            LOG_INFO("Loaded map %d\n", available_scenes[cur_scene])
            load_lightmaps(4, scene->lightmaps);
            LOG_INFO("Loaded map %d lightmaps\n", available_scenes[cur_scene])
            continue;
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

        calc_camera_inverse_transform();

        // render entire map
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        start_render_cycle();
        render_static_objects(scene);
        combine_and_post_process(POST_PROCESS_BLOOM);
        render_buffers_to_screen();

        // render_2d_rect(-0.9f, 0.9f, 0.0, -0.1f, 0.6f, 0.f, 0.f, 1.f, 1.f, TEXTURE_INTERFACE_DIALOG);
        render_text(g_scene_names[available_scenes[cur_scene]], MAX_SCENE_NAME_LEN, -0.8f, 0.8f, 0.f, FONT_LUCIDAS, 24, TEXT_COLOR_WHITE);

        SDL_GL_SwapWindow(window);
        SDL_Delay(30);
    }

    clean_lightmaps();
    clean_postprocessing();
    clean_gl();
}
