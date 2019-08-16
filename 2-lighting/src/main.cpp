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
        GLfloat tex_coords[2];
        struct {
            GLfloat s, t;
        };
    };
};

constexpr Vertex vertices[] = {
    { {-0.5f, -0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {0.0f, 0.0f} },
    { { 0.5f, -0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {1.0f, 0.0f} },
    { { 0.5f,  0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {1.0f, 1.0f} },
    { { 0.5f,  0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {1.0f, 1.0f} },
    { {-0.5f,  0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {0.0f, 1.0f} },
    { {-0.5f, -0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {0.0f, 0.0f} },

    { {-0.5f, -0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {0.0f, 0.0f} },
    { { 0.5f, -0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {1.0f, 0.0f} },
    { { 0.5f,  0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {1.0f, 1.0f} },
    { { 0.5f,  0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {1.0f, 1.0f} },
    { {-0.5f,  0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {0.0f, 1.0f} },
    { {-0.5f, -0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {0.0f, 0.0f} },

    { {-0.5f,  0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 0.0f} },
    { {-0.5f,  0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 1.0f} },
    { {-0.5f, -0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 1.0f} },
    { {-0.5f, -0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 1.0f} },
    { {-0.5f, -0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 0.0f} },
    { {-0.5f,  0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 0.0f} },

    { { 0.5f,  0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f}, {1.0f, 0.0f} },
    { { 0.5f,  0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f}, {1.0f, 1.0f} },
    { { 0.5f, -0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f}, {0.0f, 1.0f} },
    { { 0.5f, -0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f}, {0.0f, 1.0f} },
    { { 0.5f, -0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f}, {0.0f, 0.0f} },
    { { 0.5f,  0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f}, {1.0f, 0.0f} },

    { {-0.5f, -0.5f, -0.5f}, { 0.0f, -1.0f,  0.0f}, {0.0f, 1.0f} },
    { { 0.5f, -0.5f, -0.5f}, { 0.0f, -1.0f,  0.0f}, {1.0f, 1.0f} },
    { { 0.5f, -0.5f,  0.5f}, { 0.0f, -1.0f,  0.0f}, {1.0f, 0.0f} },
    { { 0.5f, -0.5f,  0.5f}, { 0.0f, -1.0f,  0.0f}, {1.0f, 0.0f} },
    { {-0.5f, -0.5f,  0.5f}, { 0.0f, -1.0f,  0.0f}, {0.0f, 0.0f} },
    { {-0.5f, -0.5f, -0.5f}, { 0.0f, -1.0f,  0.0f}, {0.0f, 1.0f} },

    { {-0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f}, {0.0f, 1.0f} },
    { { 0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f}, {1.0f, 1.0f} },
    { { 0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f}, {1.0f, 0.0f} },
    { { 0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f}, {1.0f, 0.0f} },
    { {-0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f}, {0.0f, 0.0f} },
    { {-0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f}, {0.0f, 1.0f} },
};

struct CubeParam {
    glm::vec3 pos, rot_axis;
};

constexpr CubeParam cube_params[] = {
    { glm::vec3( 0.0f,  0.0f,   0.0f), glm::vec3(-0.85f, -0.13f, -0.44f) },
    { glm::vec3( 2.0f,  5.0f, -15.0f), glm::vec3(-0.13f, -0.97f, -0.93f) },
    { glm::vec3(-1.5f, -2.2f, - 2.5f), glm::vec3( 0.70f,  0.30f,  0.06f) },
    { glm::vec3(-3.8f, -2.0f, -12.3f), glm::vec3(-0.87f,  0.37f,  0.51f) },
    { glm::vec3( 2.4f, -0.4f, - 3.5f), glm::vec3( 0.46f,  0.99f,  0.27f) },
    { glm::vec3(-1.7f,  3.0f, - 7.5f), glm::vec3(-0.85f,  0.87f, -0.37f) },
    { glm::vec3( 1.3f, -2.0f, - 2.5f), glm::vec3(-0.62f,  0.38f,  0.84f) },
    { glm::vec3( 1.5f,  2.0f, - 2.5f), glm::vec3(-0.29f,  0.51f,  0.76f) },
    { glm::vec3( 1.5f,  0.2f, - 1.5f), glm::vec3(-0.95f,  0.77f,  0.55f) },
    { glm::vec3(-1.3f,  1.0f, - 1.5f), glm::vec3( 0.93f, -0.13f,  0.85f) },
};

constexpr GLuint window_w = 800, window_h = 800;

Window *g_window;
Camera g_camera{{0.0f, 0.0f, 5.0f}, {0.0f, 0.0f, -1.0f}};

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

    VertexShader vert_sh{"shaders/cube.vert"};
    ShaderProgram program{vert_sh, FragmentShader{"shaders/cube.frag"}};
    ShaderProgram light_program{vert_sh, FragmentShader{"shaders/light.frag"}};

    VertexArray vao;
    VertexBuffer vbo;

    vbo.set_data(vertices, sizeof(vertices));
    vbo.set_layout({
        BufferElement::Float3,
        BufferElement::Float3,
        BufferElement::Float2,
    });

    VertexArray light_vao;
    vbo.set_layout({
        BufferElement::Float3,
        BufferElement::Float3,
        BufferElement::Float2,
    });

    Texture2d diff_tex_1{"data/marble_01_diff_1k.png"};
    Texture2d spec_tex_1{"data/marble_01_spec_1k.png"};
    Texture2d diff_tex_2{"data/green_metal_rust_diff_1k.png", 0};
    Texture2d spec_tex_2{"data/green_metal_rust_spec_1k.png", 1};
    Texture2d emission_tex{"data/lava-emission.png",          2};

    input_man.register_callback<KeyPressedEvent>([&diff_tex_1, &spec_tex_1, &diff_tex_2, &spec_tex_2](KeyPressedEvent &e) {
        if (e.get_key() != GLFW_KEY_Q) return;
        static bool tex_to_use;
        tex_to_use ^= 1;
        Texture2d<>::active(0);
        (tex_to_use ? diff_tex_1 : diff_tex_2).bind();
        Texture2d<>::active(1);
        (tex_to_use ? spec_tex_1 : spec_tex_2).bind();
    });

    glm::vec3 light_col = glm::vec3(1.0f);

    program.use();
    program.set_value("u_material.diffuse",   0);
    program.set_value("u_material.specular",  1);
    program.set_value("u_material.emission",  2);
    program.set_value("u_material.shininess", 32.0f);
    program.set_value("u_light.ambient",  0.4f * light_col);
    program.set_value("u_light.diffuse",  0.7f * light_col);
    program.set_value("u_light.specular",        light_col);

    light_program.use();
    light_program.set_value("u_light_col", light_col);

    while(!g_window->get_should_close()) {
        glClearColor(0.18f, 0.20f, 0.25f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::vec3 light_pos = glm::vec3(2.0f * glm::cos(glfwGetTime()),
            glm::cos(0.3f * glfwGetTime()), 2.0f * glm::sin(glfwGetTime()));;

        vao.bind();
        program.use();
        program.set_value("u_view_proj", g_camera.get_view_proj());
        program.set_value("u_view_pos",  g_camera.get_pos());
        program.set_value("u_light.position", light_pos);

        for (std::size_t i = 0; i < 10; ++i) {
            GLfloat rot = (i % 3 == 0) ? 20.0f * i + 1 : glfwGetTime();
            glm::mat4 model = glm::translate(glm::mat4(1.0f), cube_params[i].pos);
            model = glm::rotate(model, rot, cube_params[i].rot_axis);
            program.set_value("u_model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        light_vao.bind();
        light_program.use();
        light_program.set_value("u_view_proj", g_camera.get_view_proj());
        light_program.set_value("u_model",
            glm::scale(glm::translate(glm::mat4(1.0f), light_pos), glm::vec3(0.3f)));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        g_window->update();
    }

    glfwTerminate();
    return 0;
}
