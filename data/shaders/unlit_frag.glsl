#version 400 core
out vec4 FragColor;

in vec2 uv;

uniform sampler2D u_texture;

void main()
{
    vec4 sampled = texture(u_texture, vec2(uv.x, 1-uv.y));

    FragColor = sampled;
}
