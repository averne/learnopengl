#pragma once

#include <string>
#include <iostream>
#include <algorithm>
#include <initializer_list>
#include <type_traits>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "shader.hpp"
#include "object.hpp"

class ShaderProgram: public GlObject {
    public:
        ShaderProgram(): GlObject(glCreateProgram()) {
            if (!get_handle()); // throw
        }

        ShaderProgram(std::initializer_list<GlObject> shaders): ShaderProgram() {
            set_shaders(shaders);
        }

        ~ShaderProgram() {
            glDeleteProgram(get_handle());
        }

        void set_shaders(std::initializer_list<GlObject> shaders) const {
            std::for_each(shaders.begin(), shaders.end(),
                [this](const GlObject &shader) { glAttachShader(get_handle(), shader.get_handle()); });
        }

        GLint link() const {
            GLint rc;
            glLinkProgram(get_handle());
            glGetProgramiv(get_handle(), GL_LINK_STATUS, &rc);
            if (rc); // delete shaders
            return rc;
        }

        void use() const {
            glUseProgram(get_handle());
        }

        template <typename T>
        void set_value(const std::string &name, const T &val) const {
            if constexpr (std::is_same_v<T, GLboolean> || std::is_same_v<T, GLint>)
                glUniform1i(glGetUniformLocation(get_handle(), name.c_str()), (int)val);
            else if constexpr (std::is_same_v<T, GLfloat>)
                glUniform1f(glGetUniformLocation(get_handle(), name.c_str()), val);
            else if constexpr (std::is_same_v<T, glm::vec2>)
                glUniform2fv(glGetUniformLocation(get_handle(), name.c_str()), 1, glm::value_ptr(val));
            else if constexpr (std::is_same_v<T, glm::vec3>)
                glUniform3fv(glGetUniformLocation(get_handle(), name.c_str()), 1, glm::value_ptr(val));
            else if constexpr (std::is_same_v<T, glm::vec4>)
                glUniform4fv(glGetUniformLocation(get_handle(), name.c_str()), 1, glm::value_ptr(val));
            else if constexpr (std::is_same_v<T, glm::mat2>)
                glUniformMatrix2fv(glGetUniformLocation(get_handle(), name.c_str()), 1, GL_FALSE, glm::value_ptr(val));
            else if constexpr (std::is_same_v<T, glm::mat3>)
                glUniformMatrix3fv(glGetUniformLocation(get_handle(), name.c_str()), 1, GL_FALSE, glm::value_ptr(val));
            else if constexpr (std::is_same_v<T, glm::mat4>)
                glUniformMatrix4fv(glGetUniformLocation(get_handle(), name.c_str()), 1, GL_FALSE, glm::value_ptr(val));
            else
                ; // throw
        }

        void set_value(const std::string &name, float val_1, float val_2) const {
            glUniform2f(glGetUniformLocation(get_handle(), name.c_str()), val_1, val_2);
        }

        void set_value(const std::string &name, float val_1, float val_2, float val_3) const {
            glUniform3f(glGetUniformLocation(get_handle(), name.c_str()), val_1, val_2, val_3);
        }

        void set_value(const std::string &name, float val_1, float val_2, float val_3, float val_4) const {
            glUniform4f(glGetUniformLocation(get_handle(), name.c_str()), val_1, val_2, val_3, val_4);
        }

        std::string get_log() const {
            std::string str(0x200, 0);
            glGetProgramInfoLog(get_handle(), str.size(), nullptr, (char *)str.data());
            return str;
        }

        void print_log() const {
            std::cout << get_log() << '\n';
        }
};
