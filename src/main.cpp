#define GLEW_STATIC
#include <cstdio>
#include <cmath>
#include <iostream>
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "shader.hpp"
#include "shader_program.hpp"
#include "vertex_array.hpp"
#include "buffer.hpp"

constexpr GLuint window_w = 800;
constexpr GLuint window_h = 600;

GLfloat vertices[] = {
   -0.5f, -0.5f, +0.0f, +1.0f, +0.0f, +0.0f,
   +0.5f, -0.5f, +0.0f, +0.0f, +1.0f, +0.0f,
   +0.0f, +0.5f, +0.0f, +0.0f, +0.0f, +1.0f,
};

std::string vertex_shr_src = R"text(
    #version 330 core

    layout (location = 0) in vec3 in_position;
    layout (location = 1) in vec3 in_color;

    out vec3 frag_color;

    void main() {
        frag_color = in_color;
        gl_Position = vec4(in_position, 1.0);
    }
)text";

std::string fragment_shr_src = R"text(
    #version 330 core

    uniform float t;

    in vec3 frag_color;

    out vec4 out_color;

    void main() {
        out_color = vec4(frag_color, 1.0f);
    }
)text";

int main(int argc, char **argv) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow *window = glfwCreateWindow(window_w, window_h, "yeet", nullptr, nullptr);
    if (!window) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

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

    vao.bind();
    vbo.bind();
    vbo.set_data(vertices, sizeof(vertices));
    vbo.set_attrib_ptr(0, 3, GL_FLOAT, 6 * sizeof(GLfloat));
    vbo.set_attrib_ptr(1, 3, GL_FLOAT, 6 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
    vbo.enable_attrib_arr(0);
    vbo.enable_attrib_arr(1);
    vao.unbind();
    vbo.unbind();

    while(!glfwWindowShouldClose(window)){
        glClearColor(0.18f, 0.20f, 0.25f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        program.use();
        vao.bind();
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}
