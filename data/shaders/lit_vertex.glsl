#version 400 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec2 aLightmapTexCoord;

out vec4 cpos;
out vec3 cnorm;
out vec2 uv;
out vec2 lightmap_uv;

uniform mat4 u_camera_inverse_transform;

void main()
{
    cpos = u_camera_inverse_transform * vec4(aPos, 1.0);
    gl_Position = vec4(cpos.x, 1.8*cpos.y, -cpos.z-1, -cpos.z);
    cnorm = vec3(u_camera_inverse_transform * vec4(aNorm, 0.0));
    uv = aTexCoord;
    lightmap_uv = aLightmapTexCoord;
}
