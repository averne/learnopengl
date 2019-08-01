#include <cstdio>
#include <cmath>
#include <iostream>
#include <string>
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
#include "utils.hpp"

constexpr GLuint window_w = 800;
constexpr GLuint window_h = 800;

struct Vertex {
    union {
        GLfloat coords[3];
        struct {
            GLfloat x, y, z;
        };
    };
    union {
        GLfloat colors[3];
        struct {
            GLfloat r, g, b;
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

Vertex vertices[] = {
   { { -0.5f, -0.5f, +0.0f }, { +1.0f, +0.0f, +0.0f }, { +0.0f, +0.0f } , { +0.0f, +0.0f } },
   { { +0.5f, -0.5f, +0.0f }, { +0.0f, +1.0f, +0.0f }, { +1.0f, +0.0f } , { +2.0f, +0.0f } },
   { { +0.5f, +0.5f, +0.0f }, { +0.0f, +0.0f, +1.0f }, { +1.0f, +1.0f } , { +2.0f, +2.0f } },
   { { -0.5f, +0.5f, +0.0f }, { +1.0f, +1.0f, +0.0f }, { +0.0f, +1.0f } , { +0.0f, +2.0f } },
};

GLuint indices[] = {
    0, 1, 2,
    2, 3, 0,
};

std::string vertex_shr_src = R"text(
    #version 330 core

    layout (location = 0) in vec3 in_position;
    layout (location = 1) in vec3 in_color;
    layout (location = 2) in vec2 in_tex_coords_1;
    layout (location = 3) in vec2 in_tex_coords_2;

    out vec3 frag_color;
    out vec2 tex_coords_1;
    out vec2 tex_coords_2;

    uniform mat4 transform;

    void main() {
        frag_color = in_color;
        tex_coords_1 = vec2(in_tex_coords_1.x, 1.0f - in_tex_coords_1.y);
        tex_coords_2 = vec2(1.0f - in_tex_coords_2.x, 1.0f - in_tex_coords_2.y);
        gl_Position = transform * vec4(in_position, 1.0);
    }
)text";

std::string fragment_shr_src = R"text(
    #version 330 core

    in vec3 frag_color;
    in vec2 tex_coords_1;
    in vec2 tex_coords_2;

    out vec4 out_color;

    uniform sampler2D tex_1;
    uniform sampler2D tex_2;
    uniform float     factor;

    void main() {
        out_color = mix(texture(tex_1, tex_coords_1), texture(tex_2, tex_coords_2), factor) * vec4(frag_color, 1.0f);
    }
)text";

GLfloat g_mix_factor = 0.0f;

void window_sz_change_cb(GLFWwindow *window, int w, int h) {
    glViewport(0, 0, w, h);
}

void process_keys(GLFWwindow *window, int key, int scancode, int action, int modifiers) {
    switch (key) {
        case GLFW_KEY_ESCAPE:
        case GLFW_KEY_ENTER:
            if (action == GLFW_PRESS)
                glfwSetWindowShouldClose(window, true);
            break;
        case GLFW_KEY_UP:
            if ((action == GLFW_PRESS) && (g_mix_factor < 1.0f))
                g_mix_factor += 0.1f;
            break;
        case GLFW_KEY_DOWN:
            if ((action == GLFW_PRESS) && (g_mix_factor >= 0.1f))
                g_mix_factor -= 0.1f;
            break;
        default:
            break;
    }

}

int main(int argc, char **argv) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    GLFWwindow *window = glfwCreateWindow(window_w, window_h, "yeet", nullptr, nullptr);
    if (!window) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, process_keys);
    glfwSetFramebufferSizeCallback(window, window_sz_change_cb);

    if (glewInit() != GLEW_OK) {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    glViewport(0, 0, window_w, window_h);

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
    ElementBuffer ebo;

    vbo.set_data(vertices, sizeof(vertices));
    ebo.set_data(indices, sizeof(indices));
    vbo.set_attrib_ptr(0, SIZEOF_ARRAY(Vertex::coords), GL_FLOAT, sizeof(Vertex), (GLvoid *)offsetof(Vertex, coords));
    vbo.set_attrib_ptr(1, SIZEOF_ARRAY(Vertex::colors), GL_FLOAT, sizeof(Vertex), (GLvoid *)offsetof(Vertex, colors));
    vbo.set_attrib_ptr(2, SIZEOF_ARRAY(Vertex::tex_coords_1), GL_FLOAT, sizeof(Vertex), (GLvoid *)offsetof(Vertex, tex_coords_1));
    vbo.set_attrib_ptr(3, SIZEOF_ARRAY(Vertex::tex_coords_2), GL_FLOAT, sizeof(Vertex), (GLvoid *)offsetof(Vertex, tex_coords_2));

    int w1, h1, w2, h2;
    stbi_uc *data1 = stbi_load("data/191407_1308820425_orig.jpg", &w1, &h1, NULL, 0);
    stbi_uc *data2 = stbi_load("data/default_icon.jpg",           &w2, &h2, NULL, 0);
    if (!(data1 && data2)) {
        std::cout << "Failed to load textures" << std::endl;
        return 1;
    }

    Texture2d<1>::active(0);
    Texture2d tex1;
    tex1.set_data(data1, w1, h1);
    tex1.generate_mipmap();
    tex1.set_parameters({{GL_TEXTURE_WRAP_S, GL_REPEAT}, {GL_TEXTURE_WRAP_T, GL_REPEAT},
            {GL_TEXTURE_MIN_FILTER, GL_LINEAR}, {GL_TEXTURE_MAG_FILTER, GL_LINEAR}});

    Texture2d<1>::active(1);
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

    while(!glfwWindowShouldClose(window)){
        glClearColor(0.18f, 0.20f, 0.25f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        program.set_value("factor", g_mix_factor);

        glm::mat4 trans = glm::rotate(glm::mat4(1.0f), (GLfloat)glfwGetTime(), glm::vec3(1.0f, 1.0f, 0.0f));
        trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
        program.set_value("transform", trans);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        trans = glm::rotate(glm::mat4(1.0f), (GLfloat)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
        trans = glm::translate(trans, glm::vec3(-0.5f, 0.5f, 0.0f));
        trans = glm::scale(trans, glm::vec3(sin(glfwGetTime()) / 2.0f + 1.0f, cos(glfwGetTime()) / 2.0f + 1.0f, 1.0f));
        program.set_value("transform", trans);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}
