#include <cstdio>
#include <cmath>
#include <iostream>
#include <string>
#include <algorithm>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>

#include "shader.hpp"
#include "shader_program.hpp"
#include "vertex_array.hpp"
#include "buffer.hpp"
#include "texture.hpp"
#include "window.hpp"
#include "camera.hpp"
#include "input.hpp"
#include "utils.hpp"

struct Vertex {
    union {
        GLfloat coords[3];
        struct {
            GLfloat x, y, z;
        };
    };
    union {
        GLfloat normal[3];
        struct {
            GLfloat normal_x, normal_y, normal_z;
        };
    };
    union {
        GLfloat tex_coords_1[2];
        struct {
            GLfloat s_1, t_1;
        };
    };
    union {
        GLfloat tex_coords_2[2];
        struct {
            GLfloat s_2, t_2;
        };
    };
};

constexpr Vertex vertices[] = {
    { {-0.5f, -0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f} },
    { { 0.5f, -0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {1.0f, 0.0f}, {1.0f, 0.0f} },
    { { 0.5f,  0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f} },
    { { 0.5f,  0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f} },
    { {-0.5f,  0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {0.0f, 1.0f}, {0.0f, 1.0f} },
    { {-0.5f, -0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f} },

    { {-0.5f, -0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f} },
    { { 0.5f, -0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {1.0f, 0.0f}, {1.0f, 0.0f} },
    { { 0.5f,  0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f} },
    { { 0.5f,  0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f} },
    { {-0.5f,  0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {0.0f, 1.0f}, {0.0f, 1.0f} },
    { {-0.5f, -0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f} },

    { {-0.5f,  0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}, {1.0f, 0.0f} },
    { {-0.5f,  0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f} },
    { {-0.5f, -0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}, {0.0f, 1.0f} },
    { {-0.5f, -0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}, {0.0f, 1.0f} },
    { {-0.5f, -0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f} },
    { {-0.5f,  0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}, {1.0f, 0.0f} },

    { { 0.5f,  0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}, {1.0f, 0.0f} },
    { { 0.5f,  0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f} },
    { { 0.5f, -0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}, {0.0f, 1.0f} },
    { { 0.5f, -0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}, {0.0f, 1.0f} },
    { { 0.5f, -0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f} },
    { { 0.5f,  0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}, {1.0f, 0.0f} },

    { {-0.5f, -0.5f, -0.5f}, { 0.0f, -1.0f,  0.0f}, {0.0f, 1.0f}, {0.0f, 1.0f} },
    { { 0.5f, -0.5f, -0.5f}, { 0.0f, -1.0f,  0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f} },
    { { 0.5f, -0.5f,  0.5f}, { 0.0f, -1.0f,  0.0f}, {1.0f, 0.0f}, {1.0f, 0.0f} },
    { { 0.5f, -0.5f,  0.5f}, { 0.0f, -1.0f,  0.0f}, {1.0f, 0.0f}, {1.0f, 0.0f} },
    { {-0.5f, -0.5f,  0.5f}, { 0.0f, -1.0f,  0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f} },
    { {-0.5f, -0.5f, -0.5f}, { 0.0f, -1.0f,  0.0f}, {0.0f, 1.0f}, {0.0f, 1.0f} },

    { {-0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f}, {0.0f, 1.0f}, {0.0f, 1.0f} },
    { { 0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f} },
    { { 0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f}, {1.0f, 0.0f}, {1.0f, 0.0f} },
    { { 0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f}, {1.0f, 0.0f}, {1.0f, 0.0f} },
    { {-0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f} },
    { {-0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f}, {0.0f, 1.0f}, {0.0f, 1.0f} },
};

// struct CubeParam {
//     glm::vec3 pos, rot_axis;
// };

// constexpr CubeParam cube_params[] = {
//     { glm::vec3( 0.0f,  0.0f,   0.0f), glm::vec3(-0.85f, -0.13f, -0.44f) },
//     { glm::vec3( 2.0f,  5.0f, -15.0f), glm::vec3(-0.13f, -0.97f, -0.93f) },
//     { glm::vec3(-1.5f, -2.2f, - 2.5f), glm::vec3( 0.70f,  0.30f,  0.06f) },
//     { glm::vec3(-3.8f, -2.0f, -12.3f), glm::vec3(-0.87f,  0.37f,  0.51f) },
//     { glm::vec3( 2.4f, -0.4f, - 3.5f), glm::vec3( 0.46f,  0.99f,  0.27f) },
//     { glm::vec3(-1.7f,  3.0f, - 7.5f), glm::vec3(-0.85f,  0.87f, -0.37f) },
//     { glm::vec3( 1.3f, -2.0f, - 2.5f), glm::vec3(-0.62f,  0.38f,  0.84f) },
//     { glm::vec3( 1.5f,  2.0f, - 2.5f), glm::vec3(-0.29f,  0.51f,  0.76f) },
//     { glm::vec3( 1.5f,  0.2f, - 1.5f), glm::vec3(-0.95f,  0.77f,  0.55f) },
//     { glm::vec3(-1.3f,  1.0f, - 1.5f), glm::vec3( 0.93f, -0.13f,  0.85f) },
// };

std::string vertex_shr_src = R"text(
    #version 330 core

    layout (location = 0) in vec3 in_position;
    layout (location = 1) in vec3 in_normal;
    // layout (location = 2) in vec2 in_tex_coords_1;
    // layout (location = 3) in vec2 in_tex_coords_2;

    out vec3 normal, frag_pos;
    // out vec2 tex_coords_1, tex_coords_2;

    uniform mat4 u_view_proj, u_model;

    void main() {
        // tex_coords_1 = vec2(in_tex_coords_1.x, 1.0f - in_tex_coords_1.y);
        // tex_coords_2 = vec2(in_tex_coords_2.x, 1.0f - in_tex_coords_2.y);
        frag_pos = vec3(u_model * vec4(in_position, 1.0f));
        normal = in_normal;
        gl_Position = u_view_proj * vec4(frag_pos, 1.0f);
    }
)text";

std::string fragment_shr_src = R"text(
    #version 330 core

    struct material_t {
        vec3 ambient, diffuse, specular;
        float shininess;
    };

    struct light_t {
        vec3 position;
        vec3 ambient, diffuse, specular;
    };

    in vec3 frag_pos, normal;
    // in vec2 tex_coords_1, tex_coords_2;

    out vec4 out_color;

    // uniform sampler2D u_tex_1, u_tex_2;
    // uniform float     u_factor;
    uniform vec3       u_view_pos;
    uniform material_t u_material;
    uniform light_t    u_light;

    void main() {
        // tex_coords_1 = vec2(in_tex_coords_1.x, 1.0f - in_tex_coords_1.y);
        // tex_coords_2 = vec2(in_tex_coords_2.x, 1.0f - in_tex_coords_2.y);
        // out_color    = mix(texture(u_tex_1, tex_coords_1), texture(u_tex_2, tex_coords_2), u_factor);
        vec3 norm      = normalize(normal);
        vec3 light_dir = normalize(u_light.position - frag_pos);
        vec3 view_dir  = normalize(u_view_pos  - frag_pos);

        float diff = max(dot(norm, light_dir), 0.0f);
        float spec = pow(max(dot(view_dir, reflect(-light_dir, norm)), 0.0), u_material.shininess);

        vec3 ambient  =        u_light.ambient  * u_material.ambient;
        vec3 diffuse  = diff * u_light.diffuse  * u_material.diffuse;
        vec3 specular = spec * u_light.specular * u_material.specular;

        out_color = vec4(ambient + diffuse + specular, 1.0f);
    }
)text";

std::string light_fragment_shr_src = R"text(
    #version 330 core

    out vec4 out_color;

    uniform vec3 u_light_col;

    void main() {
        out_color = vec4(u_light_col, 1.0f);
    }
)text";

constexpr GLuint window_w = 800, window_h = 800;

Window *g_window;
Camera g_camera{{0.0f, 0.0f, 5.0f}, {0.0f, 0.0f, -1.0f}};
// GLfloat g_mix_factor = 0.0f;

int main(int argc, char **argv) {
    glfwInit();
    g_window = new Window(window_w, window_h, "yeet");
    g_window->set_vsync(true);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    glfwSetInputMode(g_window->get_window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    g_window->set_viewport(window_w, window_h);
    g_camera.set_viewport_dims({window_w, window_h});
    glEnable(GL_DEPTH_TEST);

    InputManager input_man{g_window};
    input_man.register_callback<KeyPressedEvent>([](KeyPressedEvent &e) {
        if (e.get_key() == GLFW_KEY_UP)
            g_camera.move(Camera::Movement::Forward);
        else if (e.get_key() == GLFW_KEY_DOWN)
            g_camera.move(Camera::Movement::Backward);
        else if (e.get_key() == GLFW_KEY_LEFT)
            g_camera.move(Camera::Movement::Left);
        else if (e.get_key() == GLFW_KEY_RIGHT)
            g_camera.move(Camera::Movement::Right);
    });
    // input_man.register_callback<KeyPressedEvent>([](KeyPressedEvent &e) {
    //     if ((e.get_key() == GLFW_KEY_Q) && (g_mix_factor < 1.0f))
    //         g_mix_factor += 0.1f;
    //     else if ((e.get_key() == GLFW_KEY_W) && (g_mix_factor >= 0.1f))
    //         g_mix_factor -= 0.1f;
    // });
    input_man.register_callback<KeyPressedEvent>([](KeyPressedEvent &e) {
        if ((e.get_key() == GLFW_KEY_ESCAPE) || (e.get_key() == GLFW_KEY_ENTER))
            g_window->set_should_close(true);
    });
    input_man.register_callback<MouseMovedEvent>([](MouseMovedEvent &e) {
        g_camera.rotate(e.get_x(), e.get_y());
    });
    input_man.register_callback<MouseScrolledEvent>([](MouseScrolledEvent &e) {
        g_camera.zoom(e.get_y());
    });
    input_man.register_callback<WindowResizedEvent>([](WindowResizedEvent &e) {
        g_window->set_viewport(e.get_w(), e.get_h());
        g_camera.set_viewport_dims(e.get_dims());
    });

    VertexShader vertex_shr = {vertex_shr_src};
    if (!vertex_shr.compile()) {
        vertex_shr.print_log();
        return 1;
    }

    FragmentShader fragment_shr = {fragment_shr_src};
    if (!fragment_shr.compile()) {
        fragment_shr.print_log();
        return 1;
    }

    FragmentShader light_fragment_shr = {light_fragment_shr_src};
    if (!light_fragment_shr.compile()) {
        light_fragment_shr.print_log();
        return 1;
    }

    ShaderProgram program = {vertex_shr, fragment_shr};
    if (!program.link()) {
        program.print_log();
        return 1;
    }

    ShaderProgram light_program = {vertex_shr, light_fragment_shr};
    if (!light_program.link()) {
        light_program.print_log();
        return 1;
    }

    VertexArray vao;
    VertexBuffer vbo;

    vbo.set_data(vertices, sizeof(vertices));
    vbo.set_layout({
        BufferElement::Float3,
        BufferElement::Float3,
        BufferElement::Float2,
        BufferElement::Float2,
    });

    VertexArray light_vao;
    vbo.set_layout({
        BufferElement::Float3,
        BufferElement::Float3,
        BufferElement::Float2,
        BufferElement::Float2,
    });

    // int w1, h1, w2, h2;
    // stbi_uc *data1 = stbi_load("data/191407_1308820425_orig.jpg", &w1, &h1, NULL, 0);
    // stbi_uc *data2 = stbi_load("data/default_icon.jpg",           &w2, &h2, NULL, 0);
    // if (!(data1 && data2)) {
    //     std::cout << "Failed to load textures" << std::endl;
    //     return 1;
    // }

    // Texture2d<>::active(0);
    // Texture2d tex1;
    // tex1.set_data(data1, w1, h1);
    // tex1.generate_mipmap();
    // tex1.set_parameters(std::pair{GL_TEXTURE_WRAP_S, GL_REPEAT}, std::pair{GL_TEXTURE_WRAP_T, GL_REPEAT},
    //         std::pair{GL_TEXTURE_MIN_FILTER, GL_LINEAR}, std::pair{GL_TEXTURE_MAG_FILTER, GL_LINEAR});

    // Texture2d<>::active(1);
    // Texture2d tex2;
    // tex2.set_data(data2, w2, h2);
    // tex2.generate_mipmap();
    // tex2.set_parameters(std::pair{GL_TEXTURE_WRAP_S, GL_REPEAT}, std::pair{GL_TEXTURE_WRAP_T, GL_REPEAT},
    //         std::pair{GL_TEXTURE_MIN_FILTER, GL_LINEAR}, std::pair{GL_TEXTURE_MAG_FILTER, GL_LINEAR});

    // stbi_image_free(data1);
    // stbi_image_free(data2);

    program.use();
    // program.set_value("u_tex_2", 1);
    // program.set_value("u_tex_1", 0);
    program.set_value("u_model", glm::mat4(1.0f));
    program.set_value("u_material.ambient",   1.0f, 0.5f, 0.31f);
    program.set_value("u_material.diffuse",   1.0f, 0.5f, 0.31f);
    program.set_value("u_material.specular",  0.5f, 0.5f, 0.5f);
    program.set_value("u_material.shininess", 32.0f);

    light_program.use();

    while(!g_window->get_should_close()) {
        glClearColor(0.18f, 0.20f, 0.25f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::vec3 light_pos = glm::vec3(2.0f * glm::cos(glfwGetTime()),
            glm::cos(0.3f * glfwGetTime()), 2.0f * glm::sin(glfwGetTime()));
        glm::vec3 light_col = glm::vec3(glm::sin(7.0f * glfwGetTime()),
            glm::sin(3.0f * glfwGetTime()), glm::sin(5.0f * glfwGetTime()));

        vao.bind();
        program.use();
        // program.set_value("u_factor", g_mix_factor);
        program.set_value("u_view_proj", g_camera.get_view_proj());
        program.set_value("u_view_pos",  g_camera.get_pos());
        program.set_value("u_light.position", light_pos);
        program.set_value("u_light.ambient",  0.2f * light_col);
        program.set_value("u_light.diffuse",  0.5f * light_col);
        program.set_value("u_light.specular",        light_col);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        light_vao.bind();
        light_program.use();
        light_program.set_value("u_view_proj", g_camera.get_view_proj());
        light_program.set_value("u_model",
            glm::scale(glm::translate(glm::mat4(1.0f), light_pos), glm::vec3(0.3f)));
        light_program.set_value("u_light_col", light_col);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        g_window->update();
    }

    glfwTerminate();
    return 0;
}
