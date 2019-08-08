#include <cstdio>
#include <cmath>
#include <iostream>
#include <string>
#include <algorithm>
#include <GL/glew.h>
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

std::string vertex_shr_src = R"text(
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
)text";

std::string fragment_shr_src = R"text(
    #version 330 core

    in vec2 tex_coords_1, tex_coords_2;

    out vec4 out_color;

    uniform sampler2D tex_1, tex_2;
    uniform float     factor;

    void main() {
        out_color = mix(texture(tex_1, tex_coords_1), texture(tex_2, tex_coords_2), factor);
    }
)text";

constexpr GLuint window_w = 800, window_h = 800;

Window *g_window;
Camera g_camera{{0.0f, 0.0f, 3.0f}, {0.0f, 0.0f, -1.0f}};
GLfloat g_mix_factor = 0.0f;

void keyboard_cb(GLFWwindow *win, int key, int scancode, int action, int modifiers) {
    switch (key) {
        case GLFW_KEY_ESCAPE:
        case GLFW_KEY_ENTER:
            if (action == GLFW_PRESS)
                g_window->set_should_close(true);
            break;
        case GLFW_KEY_Q:
            if ((action == GLFW_PRESS) && (g_mix_factor < 1.0f))
                g_mix_factor += 0.1f;
            break;
        case GLFW_KEY_W:
            if ((action == GLFW_PRESS) && (g_mix_factor >= 0.1f))
                g_mix_factor -= 0.1f;
            break;
        case GLFW_KEY_UP:
            g_camera.move(Camera::Movement::Forward);
            break;
        case GLFW_KEY_DOWN:
            g_camera.move(Camera::Movement::Backward);
            break;
        case GLFW_KEY_LEFT:
            g_camera.move(Camera::Movement::Left);
            break;
        case GLFW_KEY_RIGHT:
            g_camera.move(Camera::Movement::Right);
            break;
        default:
            break;
    }
}

int main(int argc, char **argv) {
    glfwInit();
    g_window = new Window(window_w, window_h, "yeet");
    g_window->set_size_cb([](GLFWwindow *, int w, int h) { g_window->set_viewport(w, h); g_camera.set_viewport_dims({w, h}); });
    g_window->set_keys_cb(keyboard_cb);
    g_window->set_cursor_cb([](GLFWwindow *window, double x, double y) { g_camera.rotate(x, y); });
    g_window->set_scroll_cb([](GLFWwindow *window, double x, double y) { g_camera.zoom(y); });

    if (glewInit() != GLEW_OK) {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    glfwSetInputMode(g_window->get_window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    g_window->set_viewport(window_w, window_h);
    g_camera.set_viewport_dims({window_w, window_h});
    glEnable(GL_DEPTH_TEST);

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

    ShaderProgram program = {vertex_shr, fragment_shr};
    if (!program.link()) {
        program.print_log();
        return 1;
    }

    VertexArray vao;
    VertexBuffer vbo;

    vbo.set_data(vertices, sizeof(vertices));
    vbo.set_layout({
        BufferElement::Float3,
        BufferElement::Float2,
        BufferElement::Float2,
    });

    int w1, h1, w2, h2;
    stbi_uc *data1 = stbi_load("data/191407_1308820425_orig.jpg", &w1, &h1, NULL, 0);
    stbi_uc *data2 = stbi_load("data/default_icon.jpg",           &w2, &h2, NULL, 0);
    if (!(data1 && data2)) {
        std::cout << "Failed to load textures" << std::endl;
        return 1;
    }

    Texture2d<>::active(0);
    Texture2d tex1;
    tex1.set_data(data1, w1, h1);
    tex1.generate_mipmap();
    tex1.set_parameters({{GL_TEXTURE_WRAP_S, GL_REPEAT}, {GL_TEXTURE_WRAP_T, GL_REPEAT},
            {GL_TEXTURE_MIN_FILTER, GL_LINEAR}, {GL_TEXTURE_MAG_FILTER, GL_LINEAR}});

    Texture2d<>::active(1);
    Texture2d tex2;
    tex2.set_data(data2, w2, h2);
    tex2.generate_mipmap();
    tex2.set_parameters({{GL_TEXTURE_WRAP_S, GL_REPEAT}, {GL_TEXTURE_WRAP_T, GL_REPEAT},
            {GL_TEXTURE_MIN_FILTER, GL_LINEAR}, {GL_TEXTURE_MAG_FILTER, GL_LINEAR}});

    stbi_image_free(data1);
    stbi_image_free(data2);

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
