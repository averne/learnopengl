#version 330 core

struct material_t {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
};

struct light_t {
    vec3 position;
    vec3 ambient, diffuse, specular;
};

in vec3 normal, frag_pos;
in vec2 tex_coords;

out vec4 out_color;

uniform vec3       u_view_pos;
uniform material_t u_material;
uniform light_t    u_light;

void main() {
    vec3 norm      = normalize(normal);
    vec3 light_dir = normalize(u_light.position - frag_pos);
    vec3 view_dir  = normalize(u_view_pos       - frag_pos);

    float diff = max(dot(norm, light_dir), 0.0f);
    float spec = pow(max(dot(view_dir, reflect(-light_dir, norm)), 0.0), u_material.shininess);

    vec3 ambient  =        u_light.ambient  * texture(u_material.diffuse,  tex_coords).rgb;
    vec3 diffuse  = diff * u_light.diffuse  * texture(u_material.diffuse,  tex_coords).rgb;
    vec3 specular = spec * u_light.specular * texture(u_material.specular, tex_coords).rgb;
    vec4 emission = texture(u_material.emission, tex_coords);

    out_color = vec4(mix(ambient + diffuse + specular, emission.rgb, emission.a), 1.0f);
}
