#pragma once

#include <algorithm>
#include <tuple>
#include <initializer_list>
#include <glad/glad.h>

#include "object.hpp"

template <GLenum Type, std::size_t N = 1>
class Texture: public GlObject {
    public:
        Texture() {
            glGenTextures(get_nb(), &this->handle);
            bind();
        }

        ~Texture() {
            glDeleteTextures(get_nb(), &this->handle);
        }

        static void active(GLuint idx) {
            glActiveTexture(GL_TEXTURE0 + idx);
        }

        static void generate_mipmap() {
            glGenerateMipmap(get_type());
        }

        template <typename ...Params>
        static void set_parameters(Params &&...params) {
            (glTexParameteri(get_type(), params.first, params.second), ...);
        }

        void bind() const {
            glBindTexture(get_type(), get_handle());
        }

        static void bind(GLuint handle) {
            glBindTexture(get_type(), handle);
        }

        static void unbind() {
            glBindTexture(get_type(), 0);
        }

        static inline std::size_t get_nb()   { return N; }
        static inline GLenum      get_type() { return Type; }
};

template <std::size_t N = 1>
class Texture1d: public Texture<GL_TEXTURE_1D, N> {
    public:
        void set_data(void *data, GLuint width, GLuint mipmap_lvl = 0,
                GLenum store_fmt = GL_RGB, GLenum load_fmt = GL_RGB, GLenum load_data_fmt = GL_UNSIGNED_BYTE, GLuint leg = 0) {
            glTexImage1D(this->get_type(), mipmap_lvl, store_fmt, width, leg, load_fmt, load_data_fmt, data);
        }
};

template <std::size_t N = 1>
class Texture2d: public Texture<GL_TEXTURE_2D, N> {
    public:
        void set_data(void *data, GLuint width, GLuint height, GLuint mipmap_lvl = 0,
                GLenum store_fmt = GL_RGB, GLenum load_fmt = GL_RGB, GLenum load_data_fmt = GL_UNSIGNED_BYTE, GLuint leg = 0) {
            glTexImage2D(this->get_type(), mipmap_lvl, store_fmt, width, height, leg, load_fmt, load_data_fmt, data);
        }
};

template <std::size_t N = 1>
class Texture3d: public Texture<GL_TEXTURE_3D, N> {
    public:
        void set_data(void *data, GLuint width, GLuint height, GLuint depth, GLuint mipmap_lvl = 0,
                GLenum store_fmt = GL_RGB, GLenum load_fmt = GL_RGB, GLenum load_data_fmt = GL_UNSIGNED_BYTE, GLuint leg = 0) {
            glTexImage3D(this->get_type(), mipmap_lvl, store_fmt, width, height, depth, leg, load_fmt, load_data_fmt, data);
        }
};
