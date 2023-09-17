#version 400 core
out vec4 FragColor;

in vec2 uv;
in vec4 rgba;

uniform sampler2D u_texture;

// Particles are unlit
// Note mesh particles should use the lit shader
void main()
{
    vec4 diff = texture(u_texture, vec2(uv.x, 1-uv.y)) * rgba;
    FragColor = diff;
}

