#version 400 core
out vec4 FragColor;

in vec4 cpos;
in vec3 cnorm;
in vec2 uv;

uniform sampler2D u_texture_diff;
uniform sampler2D u_texture_norm;
uniform sampler2D u_texture_spec;
uniform sampler2D u_texture_emit;
uniform float u_emissive_intensity;
uniform vec4 u_ambient_light;
uniform vec3 u_direction_vec;
uniform vec3 u_direction_light;
uniform vec4 u_rgba_multiplier;

void main()
{
    vec4 albedo = texture(u_texture_diff, vec2(uv.x, 1-uv.y)) * u_rgba_multiplier;

    // brightness correction for dynamic objects
    albedo = vec4(2 * albedo.xyz, albedo.w);

    vec3 c2f = vec3(cpos);
    vec3 snorm = normalize(cnorm + cnorm*vec3(texture(u_texture_norm, vec2(uv.x, 1-uv.y))));
    float specular_intensity = texture(u_texture_spec, vec2(uv.x, 1-uv.y)).x;

    vec4 diffuse_light = max(0, dot(vec3(-u_direction_vec), snorm)) * vec4(u_direction_light, 1.0);

    // Ensure alpha is 1 for light coefficients
    diffuse_light = vec4(diffuse_light.xyz, 1.0);
    vec4 specular_light = vec4(pow(max(dot(normalize(u_direction_vec - c2f), snorm), 0), 32)) * specular_intensity;

    // Emissive just gets added on. If it is non-zero, its value will dominate everything else
    vec4 emissive = u_emissive_intensity * texture(u_texture_emit, vec2(uv.x, 1-uv.y));

    // Multiply by 2 since dynamic objects tend to be darker
    FragColor = (u_ambient_light + diffuse_light)*albedo + specular_light*specular_intensity;
    // FragColor = (u_ambient_light + diffuse_light)*albedo + specular_light*specular_intensity + emissive;
}

