#version 400 core
out vec4 FragColor;

in vec2 uv;

uniform sampler2D u_texture;
uniform vec4 u_text_color;

void main()
{
    FragColor = texture(u_texture, uv) * u_text_color;
}
