#pragma once

#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <algorithm>
#include <initializer_list>

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

        GLint link() {
            GLint rc;
            glLinkProgram(get_handle());
            glGetProgramiv(get_handle(), GL_LINK_STATUS, &rc);
            if (rc); // delete shaders
            return rc;
        }

        void use() const {
            glUseProgram(get_handle());
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
