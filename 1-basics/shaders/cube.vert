#version 330 core

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec2 in_tex_coords_1;
layout (location = 2) in vec2 in_tex_coords_2;

out vec2 tex_coords_1, tex_coords_2;

uniform mat4 view_proj, model;

void main() {
    tex_coords_1 = vec2(in_tex_coords_1.x, 1.0f - in_tex_coords_1.y);
    tex_coords_2 = vec2(in_tex_coords_2.x, 1.0f - in_tex_coords_2.y);
    gl_Position  = view_proj * model * vec4(in_position, 1.0);
}
