#pragma once

#include <string>
#include <iostream>
#include <stdexcept>
#include <GL/glew.h>

#include "object.hpp"

template <GLenum Type>
class Shader: public GlObject {
    public:
        Shader(): GlObject(glCreateShader(get_type())) {
            if (!get_handle())
                throw std::runtime_error("Could not create Buffer object");
        }

        Shader(const std::string &src): Shader() {
            set_source(src);
        }

        ~Shader() {
            glDeleteShader(get_handle());
        }

        void set_source(const std::string &src) const {
            const char *dat = src.c_str();
            glShaderSource(get_handle(), 1, &dat, NULL);
        }

        GLint compile() const {
            GLint rc;
            glCompileShader(get_handle());
            glGetShaderiv(get_handle(), GL_COMPILE_STATUS, &rc);
            return rc;
        }

        std::string get_log() const {
            std::string str(0x200, 0);
            glGetShaderInfoLog(get_handle(), str.size(), nullptr, (char *)str.data());
            return str;
        }

        void print_log() const {
            std::cout << get_log() << '\n';
        }

        static inline GLenum get_type() { return Type; }
};

class VertexShader: public Shader<GL_VERTEX_SHADER> {
    public:
        VertexShader() = default;
        VertexShader(const std::string &src): Shader(src) { }
};

class FragmentShader: public Shader<GL_FRAGMENT_SHADER> {
    public:
        FragmentShader() = default;
        FragmentShader(const std::string &src): Shader(src) { }
};
