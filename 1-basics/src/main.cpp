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
    { {-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f} },
    { { 0.5f, -0.5f, -0.5f}, {1.0f, 0.0f}, {1.0f, 0.0f} },
    { { 0.5f,  0.5f, -0.5f}, {1.0f, 1.0f}, {1.0f, 1.0f} },
    { { 0.5f,  0.5f, -0.5f}, {1.0f, 1.0f}, {1.0f, 1.0f} },
    { {-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f}, {0.0f, 1.0f} },
    { {-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f} },

    { {-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f} },
    { { 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f}, {1.0f, 0.0f} },
    { { 0.5f,  0.5f,  0.5f}, {1.0f, 1.0f}, {1.0f, 1.0f} },
    { { 0.5f,  0.5f,  0.5f}, {1.0f, 1.0f}, {1.0f, 1.0f} },
    { {-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f}, {0.0f, 1.0f} },
    { {-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f} },

    { {-0.5f,  0.5f,  0.5f}, {1.0f, 0.0f}, {1.0f, 0.0f} },
    { {-0.5f,  0.5f, -0.5f}, {1.0f, 1.0f}, {1.0f, 1.0f} },
    { {-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, {0.0f, 1.0f} },
    { {-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, {0.0f, 1.0f} },
    { {-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f} },
    { {-0.5f,  0.5f,  0.5f}, {1.0f, 0.0f}, {1.0f, 0.0f} },

    { { 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f}, {1.0f, 0.0f} },
    { { 0.5f,  0.5f, -0.5f}, {1.0f, 1.0f}, {1.0f, 1.0f} },
    { { 0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, {0.0f, 1.0f} },
    { { 0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, {0.0f, 1.0f} },
    { { 0.5f, -0.5f,  0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f} },
    { { 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f}, {1.0f, 0.0f} },

    { {-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, {0.0f, 1.0f} },
    { { 0.5f, -0.5f, -0.5f}, {1.0f, 1.0f}, {1.0f, 1.0f} },
    { { 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f}, {1.0f, 0.0f} },
    { { 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f}, {1.0f, 0.0f} },
    { {-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f} },
    { {-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, {0.0f, 1.0f} },

    { {-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f}, {0.0f, 1.0f} },
    { { 0.5f,  0.5f, -0.5f}, {1.0f, 1.0f}, {1.0f, 1.0f} },
    { { 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f}, {1.0f, 0.0f} },
    { { 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f}, {1.0f, 0.0f} },
    { {-0.5f,  0.5f,  0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f} },
    { {-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f}, {0.0f, 1.0f} },
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
Camera g_camera{{0.0f, 0.0f, 3.0f}, {0.0f, 0.0f, -1.0f}};
GLfloat g_mix_factor = 0.0f;

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
        if ((e.get_key() == GLFW_KEY_Q) && (g_mix_factor < 1.0f))
            g_mix_factor += 0.1f;
        else if ((e.get_key() == GLFW_KEY_W) && (g_mix_factor >= 0.1f))
            g_mix_factor -= 0.1f;
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

    ShaderProgram program{VertexShader{"shaders/cube.vert"}, FragmentShader{"shaders/cube.frag"}};

    VertexArray vao;
    VertexBuffer vbo;

    vbo.set_data(vertices, sizeof(vertices));
    vbo.set_layout({
        BufferElement::Float3,
        BufferElement::Float2,
        BufferElement::Float2,
    });

    Texture2d tex1{"data/191407_1308820425_orig.jpg", 0};
    Texture2d tex2{"data/default_icon.jpg",           1};

    program.use();

    program.set_value("tex_1", 0);
    program.set_value("tex_2", 1);

    while(!g_window->get_should_close()){
        glClearColor(0.18f, 0.20f, 0.25f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        program.set_value("factor",    g_mix_factor);
        program.set_value("view_proj", g_camera.get_view_proj());

        for (std::size_t i = 0; i < 10; ++i) {
            GLfloat rot = (i % 3 == 0) ? 20.0f * i + 1 : glfwGetTime();
            glm::mat4 model = glm::translate(glm::mat4(1.0f), cube_params[i].pos);
            model = glm::rotate(model, rot, cube_params[i].rot_axis);
            program.set_value("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        g_window->update();
    }

    glfwTerminate();
    return 0;
}
