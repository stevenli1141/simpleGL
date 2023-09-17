#version 400 core
out vec4 FragColor;

in vec4 cpos;
in vec2 uv;
in vec4 rgba;

uniform sampler2D u_texture_emit;
uniform float u_emissive_intensity;

// Particles are unlit
// Note mesh particles should use the lit shader
void main()
{
    vec4 sampled = texture(u_texture_emit, vec2(uv.x, 1-uv.y));
    float distance_correction = 1.0 + 0.1 * cpos.z * cpos.z;
    vec4 emissive = u_emissive_intensity * sampled * rgba * distance_correction;
    FragColor = emissive;
}

