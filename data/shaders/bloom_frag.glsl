#version 400 core
out vec4 FragColor;

in vec2 uv;

uniform sampler2D u_texture;
uniform sampler2D u_hdr_bloom;
uniform float u_exposure;

void main() {
    vec3 hdr = vec3(u_exposure * texture(u_texture, uv) + texture(u_hdr_bloom, uv));
    // vec3 result = pow(vec3(1.0, 1.0, 1.0) - exp(-hdr * u_exposure), vec3(1.0 / 2.2));
    FragColor = vec4(hdr, 1.0);
}
