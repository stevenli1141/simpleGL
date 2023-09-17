#version 400 core
layout (location = 0) in vec3 gpos;
layout (location = 1) in vec3 norm;
layout (location = 2) in vec2 texcoord;

out vec4 cpos;
out vec3 cnorm;
out vec2 uv;

uniform mat4 u_transform;
uniform mat4 u_camera_inverse_transform;

void main()
{
    cpos = u_camera_inverse_transform * u_transform * vec4(gpos, 1.0);
    gl_Position = vec4(cpos.x, 1.8*cpos.y, -cpos.z-1, -cpos.z);
    cnorm = vec3(u_camera_inverse_transform * u_transform * vec4(norm, 0.0));
    uv = texcoord;
}
