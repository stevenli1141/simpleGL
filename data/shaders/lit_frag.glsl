#version 400 core
out vec4 FragColor;

in vec4 cpos;
in vec3 cnorm;
in vec2 uv;
in vec2 lightmap_uv;

uniform sampler2D u_texture_diff;
uniform sampler2D u_texture_norm;
uniform sampler2D u_texture_spec;
// uniform sampler2D u_texture_ao;
uniform sampler2D u_texture_emit;
uniform sampler2D u_lightmap;
uniform float u_emissive_intensity;
uniform vec4 u_ambient_light;
uniform vec3 u_direction_vec;
uniform vec3 u_direction_light;
uniform vec3 u_point_locs[8];
uniform vec3 u_point_lights[8];
uniform vec3 u_spot_locs[8];
uniform vec3 u_spot_lights[8];
uniform vec3 u_spot_dirs[8];
uniform vec2 u_spot_bounds[8];

void main()
{
    vec4 albedo = texture(u_texture_diff, vec2(uv.x, 1-uv.y));
    if (albedo.a < 0.1) {
        discard;
    }
    // Get camera coordinates of norm and point light vectors as vec3 to avoid confusing operations
    vec3 c2f = vec3(cpos);
    // Rotate normal map by direction of vertex norm. Since it's a normalized vector calculations are a simpler
    // x, y, z = costheta*cosphi*x, sintheta*cosphi*y, sinphi*z
    // costheta = cnorm.x/len(cnorm.xy), sintheta = cnorm.y/len(cnorm.xy), sinphi = cnorm.z, cosphi = len(cnorm.xy)
    // x, y, z = cnorm.x*x, cnorm.y*y, cnorm.z*z
    vec3 snorm = normalize(cnorm + cnorm*vec3(texture(u_texture_norm, vec2(uv.x, 1-uv.y))));
    float specular_intensity = texture(u_texture_spec, vec2(uv.x, 1-uv.y)).x;
    vec4 lightmap_intensity = texture(u_lightmap, vec2(lightmap_uv.x, 1-lightmap_uv.y));

    vec4 diffuse_light = max(0, dot(vec3(-u_direction_vec), snorm)) * vec4(u_direction_light, 1.0);
    vec4 specular_light = vec4(0, 0, 0, 0);

    // Simple attenuation calculated by inverse square adjusted to be at most 1
    for (int i = 0; i < 8; ++i) {
        vec3 r = vec3(u_point_locs[i] - c2f);
        float r2 = dot(r, r);
        diffuse_light += vec4(u_point_lights[i], 1.0) / (1 + r2) * max(dot(normalize(u_point_locs[i] - c2f), snorm), 0);
        // blinn phong specular
        specular_light += vec4(u_point_lights[i], 1.0) / (1 + r2) * pow(max(dot(normalize(u_point_locs[i] - 2 * c2f), snorm), 0), 32);
    }

    for (int i = 0; i < 8; ++i) {
        vec3 r = vec3(u_spot_locs[i] - c2f);
        float r2 = dot(r, r);
        // Use clamped linear falloff
        float falloff_width = u_spot_bounds[i].x - u_spot_bounds[i].y;
        float spot_cosine_distance = dot(-u_spot_dirs[i], normalize(r));
        float spot_intensity = max(min(spot_cosine_distance / falloff_width - u_spot_bounds[i].x / falloff_width, 1), 0);
        // Reduce square distance factor by ~2x to account for spot concentration compared to point lights
        diffuse_light += spot_intensity * vec4(u_spot_lights[i], 1.0) / (1 + 0.25*r2)* max(dot(normalize(r), snorm), 0);
        specular_light += spot_intensity * vec4(u_spot_lights[i], 1.0) / (1 + 0.25*r2) * pow(max(dot(normalize(u_spot_locs[i] - 2 * c2f), snorm), 0), 32);
    }

    // Ensure alpha is 1 for light coefficients
    diffuse_light = vec4(diffuse_light.x, diffuse_light.y, diffuse_light.z, 1.0);
    specular_light = vec4(specular_light.x, specular_light.y, specular_light.z, 1.0);

    // Emissive just gets added on. If it is non-zero, its value will dominate everything else
    vec4 emissive = u_emissive_intensity * texture(u_texture_emit, vec2(uv.x, 1-uv.y));

    // Remove the albedo part if you want to test just lighting, keep only albedo to test without lighting
    FragColor = (u_ambient_light + diffuse_light*(1-lightmap_intensity.w) + lightmap_intensity)*albedo + 2*specular_light*specular_intensity + emissive;
}

