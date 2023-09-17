#ifndef GUARD_VAO_H
#define GUARD_VAO_H

#include <GL/glew.h>
#include "graphics/graphics.h"
#include "types.h"

// Currently supports one buffer
static u32 vao;
static u32 vertex_buffer;
static u32 index_buffer;

void init_vao(void) {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vertex_buffer);
    glGenBuffers(1, &index_buffer);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
}

// write data from some array
void buffer_vertex_attribute_data(float *data, u32 stride, u32 gl_draw_type) {
    glBufferData(GL_ARRAY_BUFFER, stride, data, gl_draw_type);
}

#endif
