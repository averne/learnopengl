#pragma once

#include <string>
#include <iostream>
#include <algorithm>
#include <initializer_list>
#include <type_traits>
#include <utility>
#include <stdexcept>
#include <map>
#include <glad/glad.h>
#include <glm/glm.hpp>

#include "shader.hpp"
#include "object.hpp"

class ShaderProgram: public GlObject {
    public:
        ShaderProgram(): GlObject(glCreateProgram()) {
            if (!get_handle())
                throw std::runtime_error("Could not create Buffer object");
        }

        template <typename ...Shaders>
        ShaderProgram(Shaders &&...shaders): ShaderProgram() {
            set_shaders(std::forward<Shaders>(shaders)...);
            if (!link()) {
                print_log();
                throw std::runtime_error("Could not link shader program");
            }
        }

        ~ShaderProgram() {
            glDeleteProgram(get_handle());
        }

        template <typename ...Shaders>
        void set_shaders(Shaders &&...shaders) const {
            (glAttachShader(get_handle(), shaders.get_handle()), ...);
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

        static void unuse() {
            glUseProgram(0);
        }

        inline void bind() const { use(); }
        inline static void unbind() { unuse(); }

        inline GLint get_uniform_loc(const std::string &name) {
            auto it = this->uniform_loc_cache.find(name);
            if (it != this->uniform_loc_cache.end())
                return it->second;
            GLint loc = glGetUniformLocation(get_handle(), name.c_str());
            if (loc == -1)
                std::cout << "Could not find uniform " << name << '\n';
            this->uniform_loc_cache[name] = loc;
            return loc;

        }

        template <typename T>
        void set_value(GLint loc, const T &val) const {
            if constexpr (std::is_same_v<T, GLboolean> || std::is_same_v<T, GLint>)
                glUniform1i(loc, (int)val);
            else if constexpr (std::is_same_v<T, GLfloat>)
                glUniform1f(loc, val);
            else if constexpr (std::is_same_v<T, glm::vec2>)
                glUniform2fv(loc, 1, glm::value_ptr(val));
            else if constexpr (std::is_same_v<T, glm::vec3>)
                glUniform3fv(loc, 1, glm::value_ptr(val));
            else if constexpr (std::is_same_v<T, glm::vec4>)
                glUniform4fv(loc, 1, glm::value_ptr(val));
            else if constexpr (std::is_same_v<T, glm::mat2>)
                glUniformMatrix2fv(loc, 1, GL_FALSE, glm::value_ptr(val));
            else if constexpr (std::is_same_v<T, glm::mat3>)
                glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(val));
            else if constexpr (std::is_same_v<T, glm::mat4>)
                glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(val));
            else
                throw std::invalid_argument("Invalid argument for ShaderProgram::set_value");
        }

        void set_value(GLint loc, float val_1, float val_2) const {
            glUniform2f(loc, val_1, val_2);
        }

        void set_value(GLint loc, float val_1, float val_2, float val_3) const {
            glUniform3f(loc, val_1, val_2, val_3);
        }

        void set_value(GLint loc, float val_1, float val_2, float val_3, float val_4) const {
            glUniform4f(loc, val_1, val_2, val_3, val_4);
        }

        template <typename ...Args>
        void set_value(const std::string &name, Args &&...args) {
            set_value(get_uniform_loc(name), std::forward<Args>(args)...);
        }

        std::string get_log() const {
            std::string str(0x200, 0);
            glGetProgramInfoLog(get_handle(), str.size(), nullptr, (char *)str.data());
            return str;
        }

        void print_log() const {
            std::cout << get_log() << '\n';
        }

    private:
        struct Comp {
            bool operator()(const std::string &s1, const std::string &s2) const {
                return strcmp(s1.c_str(), s2.c_str()) < 0;
            }
        };
        std::map<std::string, GLint, Comp> uniform_loc_cache;
};
