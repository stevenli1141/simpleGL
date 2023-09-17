#version 400 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aTexCoord;

out vec2 uv;

uniform mat4 u_camera_inverse_transform;

void main()
{
    vec4 gpos = u_camera_inverse_transform * vec4(aPos, 1.0);
    gl_Position = vec4(gpos.x, 1.77*gpos.y, -gpos.z-1, -gpos.z);
    uv = aTexCoord;
}
