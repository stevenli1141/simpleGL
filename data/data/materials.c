#include "GL/glew.h"
#include "graphics/materials.h"
#include "graphics/shaders.h"
#include "graphics/textures.h"

const struct material_data g_mat_data[NUM_MATERIALS] = {
    [MAT_EXAMPLEMMETALPAINT] = {
        .shader = LIT_SHADER_PROGRAM,
        .texture_diff = TEX_EXAMPLEMETALPAINT,
        .texture_norm = TEX_NORM_EXAMPLEMETALPAINT,
        .texture_spec = TEX_SPEC_EXAMPLEMETALPAINT
    },
    [MAT_EXAMPLEMMETALBRUSHED] = {
        .shader = LIT_SHADER_PROGRAM,
        .texture_diff = TEX_EXAMPLEMETALPAINTBRUSHED,
        .texture_norm = TEX_NORM_EXAMPLEMETALPAINTBRUSHED,
        .texture_spec = TEX_SPEC_EXAMPLEMETALPAINTBRUSHED },
    [MAT_EXAMPLEMMETALPAINTCHIP] = {
        .shader = LIT_SHADER_PROGRAM,
        .texture_diff = TEX_EXAMPLEMMETALPAINTCHIP,
        .texture_norm = TEX_NORM_EXAMPLEMMETALPAINTCHIP,
        .texture_spec = TEX_SPEC_EXAMPLEMMETALPAINTCHIP
    },
    [MAT_EXAMPLEMPOSTER] = {
        .shader = LIT_SHADER_PROGRAM,
        .texture_diff = TEX_EXAMPLEPOSTER
    },
    [MAT_EXAMPLEMWOODPARQUET] = {
        .shader = LIT_SHADER_PROGRAM,
        .texture_diff = TEX_EXAMPLEMWOODPARQUET,
        .texture_norm = TEX_SPEC_EXAMPLEMWOODPARQUET,
        .texture_spec = TEX_SPEC_EXAMPLEMWOODPARQUET
    },
    [MAT_EXAMPLEMLEATHERGREEN] = {
        .shader = LIT_SHADER_PROGRAM,
        .texture_diff = TEX_EXAMPLEMLEATHERGREEN,
        .texture_norm = TEX_NORM_EXAMPLEMLEATHERGREEN,
        .texture_spec = TEX_SPEC_EXAMPLEMLEATHERGREEN
    },
    [MAT_EXAMPLEMLIGHT] = {
        .shader = LIT_SHADER_PROGRAM,
        .texture_diff = TEX_EXAMPLELIGHT,
        .texture_emit = TEX_EMIT_EXAMPLELIGHT
    },
    [MAT_EXAMPLEMMONITORS] = {
        .shader = LIT_SHADER_PROGRAM,
        .texture_diff = TEX_EXAMPLEMONITORS,
        .texture_emit = TEX_EMIT_EXAMPLEMONITORS
    },
};
