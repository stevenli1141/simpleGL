#version 400 core
out vec4 EmitColor;

in vec4 cpos;
in vec3 cnorm;
in vec2 uv;

uniform sampler2D u_texture_emit;
uniform float u_emissive_intensity;

void main()
{
    // Emissive just gets added on. If it is non-zero, its value will dominate everything else
    vec4 sampled = texture(u_texture_emit, vec2(uv.x, 1-uv.y));
    float distance_correction = 1.0 + 0.5 * cpos.z * cpos.z;
    vec4 emissive = u_emissive_intensity * sampled * distance_correction;

    // Filter only high emissive values
    float brightness = dot(emissive, vec4(0.2126, 0.7152, 0.0722, 0));
    float emit_threshold = brightness > 1 ? 1 : 0;
    EmitColor = emit_threshold * 0.1 * emissive * sampled.w;
}
