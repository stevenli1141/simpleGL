#version 400 core
layout (location = 0) in vec3 gpos;
layout (location = 1) in vec2 texcoord;
layout (location = 2) in vec4 color;

out vec4 cpos;
out vec2 uv;
out vec4 rgba;

uniform mat4 u_camera_inverse_transform;

void main()
{
    cpos = u_camera_inverse_transform * vec4(gpos, 1.0);
    gl_Position = vec4(cpos.x, 1.8*cpos.y, -cpos.z-1, -cpos.z);
    uv = texcoord;
    rgba = color;
}
