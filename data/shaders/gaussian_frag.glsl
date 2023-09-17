#version 330 core
out vec4 FragColor;
  
in vec2 uv;

uniform sampler2D u_texture;
uniform vec3 u_blur_params;

void main()
{
    vec4 result = texture(u_texture, uv) * 0.27;
    result += texture(u_texture, uv + u_blur_params.xy*vec2(1,0)) * 0.116;
    result += texture(u_texture, uv + u_blur_params.xy*vec2(0,1)) * 0.116;
    result += texture(u_texture, uv + u_blur_params.xy*vec2(-1,0)) * 0.116;
    result += texture(u_texture, uv + u_blur_params.xy*vec2(0,-1)) * 0.116;
    result += texture(u_texture, uv + u_blur_params.xy*vec2(1,1)) * 0.05;
    result += texture(u_texture, uv + u_blur_params.xy*vec2(-1,1)) * 0.05;
    result += texture(u_texture, uv + u_blur_params.xy*vec2(-1,-1)) * 0.05;
    result += texture(u_texture, uv + u_blur_params.xy*vec2(1,-1)) * 0.05;

    // FragColor = result;
    FragColor = result * u_blur_params.z;
}
