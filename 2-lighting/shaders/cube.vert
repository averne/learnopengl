#version 330 core

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_tex_coords;

out vec3 normal, frag_pos;
out vec2 tex_coords;

uniform mat4 u_view_proj, u_model;

void main() {
    normal = mat3(transpose(inverse(u_model))) * in_normal;
    frag_pos = vec3(u_model * vec4(in_position, 1.0f));
    tex_coords = in_tex_coords;
    gl_Position = u_view_proj * vec4(frag_pos, 1.0f);
}
