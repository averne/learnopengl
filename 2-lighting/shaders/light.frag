#version 330 core

out vec4 out_color;

uniform vec3 u_light_col;

void main() {
    out_color = vec4(u_light_col, 1.0f);
}
