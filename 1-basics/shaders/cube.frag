#version 330 core

in vec2 tex_coords_1, tex_coords_2;

out vec4 out_color;

uniform sampler2D tex_1, tex_2;
uniform float     factor;

void main() {
    out_color = mix(texture(tex_1, tex_coords_1), texture(tex_2, tex_coords_2), factor);
}
