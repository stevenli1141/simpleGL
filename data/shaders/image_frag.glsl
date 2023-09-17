#version 400 core
out vec4 FragColor;

in vec2 uv;

uniform sampler2D u_texture;
uniform vec4 u_light;
uniform vec4 u_fade_color;
uniform float u_fade_rate;

void main()
{
    vec4 sampled = texture(u_texture, vec2(uv.x, 1-uv.y));
    if (sampled.a < 0.1) {
        discard;
    }
    vec4 color = sampled * u_light;
    FragColor = (1.0 - u_fade_rate) * color + u_fade_rate * u_fade_color * vec4(1.0, 1.0, 1.0, color.w);
}
