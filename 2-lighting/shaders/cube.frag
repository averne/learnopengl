#version 330 core

struct material_t {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
};

struct light_t {
    vec3 ambient, diffuse, specular;
};

struct pt_light_t {
    vec3 position;
    float constant, linear, quadratic;
    light_t light;
};

struct dir_light_t {
    vec3 direction;
    light_t light;
};

struct spotlight_t {
    vec3 direction, position;
    float inner_cutoff, outer_cutoff;
    light_t light;
};

in vec3 normal, frag_pos;
in vec2 tex_coords;

out vec4 out_color;

uniform vec3        u_view_pos;
uniform material_t  u_material;
uniform pt_light_t  u_pt_light[5];
uniform dir_light_t u_dir_light;
uniform spotlight_t u_spotlight;

vec3 calc_light(light_t light, float amb, float diff, float spec);
vec3 calc_dir_light(dir_light_t light, vec3 view_dir, vec3 normal);
vec3 calc_pt_light(  pt_light_t light, vec3 view_dir, vec3 normal, vec3 frag_pos);
vec3 calc_spotlight(spotlight_t light, vec3 view_dir, vec3 normal, vec3 frag_pos);

void main() {
    vec3 norm     = normalize(normal);
    vec3 view_dir = normalize(u_view_pos - frag_pos);

    out_color      = texture(u_material.emission, tex_coords);
    out_color.rgb += calc_dir_light(u_dir_light, view_dir, norm);
    for (int i = 0; i < u_pt_light.length(); ++i)
        out_color.rgb += calc_pt_light(u_pt_light[i], view_dir, norm, frag_pos);
    out_color.rgb += calc_spotlight(u_spotlight, view_dir, norm, frag_pos);
}

vec3 calc_light(light_t light, float amb, float diff, float spec) {
    vec3 amb_diff = (amb * light.ambient + diff * light.diffuse) * texture(u_material.diffuse, tex_coords).rgb;
    vec3 specular = spec * light.specular * texture(u_material.specular, tex_coords).rgb;
    return amb_diff + specular;
}

vec3 calc_dir_light(dir_light_t light, vec3 view_dir, vec3 normal) {
    vec3 light_dir = normalize(-light.direction);
    float diff = max(dot(normal, light_dir), 0.0);
    float spec = pow(max(dot(view_dir, reflect(-light_dir, normal)), 0.0), u_material.shininess);
    return calc_light(light.light, 1.0f, diff, spec);
}

vec3 calc_pt_light(pt_light_t light, vec3 view_dir, vec3 normal, vec3 frag_pos) {
    vec3 light_dir = normalize(light.position - frag_pos);
    float distance = length(light.position - frag_pos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    float diff = max(dot(normal, light_dir), 0.0);
    float spec = pow(max(dot(view_dir, reflect(-light_dir, normal)), 0.0), u_material.shininess);
    return calc_light(light.light, attenuation, attenuation * diff, attenuation * spec);
}

vec3 calc_spotlight(spotlight_t light, vec3 view_dir, vec3 normal, vec3 frag_pos) {
    vec3 light_dir = normalize(light.position - frag_pos);
    float theta = dot(light_dir, normalize(-light.direction));
    float intensity = clamp((theta - light.outer_cutoff) /
        (light.inner_cutoff - light.outer_cutoff), 0.0, 1.0);
    float diff  = max(dot(normal, light_dir), 0.0f);
    float spec  = pow(max(dot(view_dir, reflect(-light_dir, normal)), 0.0), u_material.shininess);
    return calc_light(light.light, 0.0f, intensity * diff, intensity * spec);
}
