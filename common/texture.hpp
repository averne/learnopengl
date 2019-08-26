#pragma once

#include <stdexcept>
#include <algorithm>
#include <tuple>
#include <initializer_list>
#include <glad/glad.h>
#include <stb_image.h>

#include "object.hpp"

enum class TextureType {
    Diffuse,
    Specular,
    Normal,
};

template <GLenum Type, std::size_t N = 1>
class Texture: public GlObject {
    public:
        Texture() {
            glGenTextures(get_nb(), &this->handle);
            if (!get_handle())
                throw std::runtime_error("Could not create texture");
            bind();
        }

        Texture(int idx) {
            if (idx >= 0) this->active(idx);
            glGenTextures(get_nb(), &this->handle);
            if (!get_handle())
                throw std::runtime_error("Could not create texture");
            bind();
        }

        ~Texture() {
            glDeleteTextures(get_nb(), &this->handle);
        }

        static void active(GLuint idx) {
            glActiveTexture(GL_TEXTURE0 + idx);
        }

        static void deactive(GLuint idx) {
            glActiveTexture(GL_TEXTURE0);
        }

        static void generate_mipmap() {
            glGenerateMipmap(get_type());
        }

        template <typename ...Params>
        static void set_parameters(Params &&...params) {
            (glTexParameteri(get_type(), params.first, params.second), ...);
        }

        static void set_default_parameters() {
            set_parameters(std::pair{GL_TEXTURE_WRAP_S, GL_REPEAT}, std::pair{GL_TEXTURE_WRAP_T, GL_REPEAT},
                std::pair{GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR}, std::pair{GL_TEXTURE_MAG_FILTER, GL_LINEAR});
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
        Texture1d() = default;
        Texture1d(int idx): Texture<GL_TEXTURE_1D, N>(idx) { }

        void set_data(void *data, GLuint width, GLuint mipmap_lvl = 0,
                GLenum store_fmt = GL_RGB, GLenum load_fmt = GL_RGB, GLenum load_data_fmt = GL_UNSIGNED_BYTE, GLuint leg = 0) {
            glTexImage1D(this->get_type(), mipmap_lvl, store_fmt, width, leg, load_fmt, load_data_fmt, data);
        }
};

template <std::size_t N = 1>
class Texture2d: public Texture<GL_TEXTURE_2D, N> {
    public:
        Texture2d() = default;
        Texture2d(int idx): Texture<GL_TEXTURE_2D, N>(idx) { }

        Texture2d(const std::string &path, GLint idx = -1,
                GLenum load_data_fmt = GL_UNSIGNED_BYTE, GLuint mipmap_lvl = 0): Texture2d(idx) {
            int w, h, nchan, fmt;
            stbi_set_flip_vertically_on_load(true);
            stbi_uc *data = stbi_load(path.c_str(), &w, &h, &nchan, 0);
            if (!data) {
                std::cout << stbi_failure_reason() << '\n';
                throw std::runtime_error("Could not load texture file");
            }

            if      (nchan == 3) fmt = GL_RGB;
            else if (nchan == 4) fmt = GL_RGBA;
            else                 fmt = GL_RED;
            this->set_data(data, w, h, fmt, fmt, load_data_fmt, mipmap_lvl);
            this->generate_mipmap();
            this->set_default_parameters();

            stbi_image_free(data);
        }

        void set_data(void *data, GLuint width, GLuint height, GLenum store_fmt = GL_RGB, GLenum load_fmt = GL_RGB,
                GLenum load_data_fmt = GL_UNSIGNED_BYTE, GLuint mipmap_lvl = 0, GLuint leg = 0) {
            glTexImage2D(this->get_type(), mipmap_lvl, store_fmt, width, height, leg, load_fmt, load_data_fmt, data);
        }
};

template <std::size_t N = 1>
class Texture3d: public Texture<GL_TEXTURE_3D, N> {
    public:
        Texture3d() = default;
        Texture3d(int idx): Texture<GL_TEXTURE_3D, N>(idx) { }

        void set_data(void *data, GLuint width, GLuint height, GLuint depth, GLuint mipmap_lvl = 0,
                GLenum store_fmt = GL_RGB, GLenum load_fmt = GL_RGB, GLenum load_data_fmt = GL_UNSIGNED_BYTE, GLuint leg = 0) {
            glTexImage3D(this->get_type(), mipmap_lvl, store_fmt, width, height, depth, leg, load_fmt, load_data_fmt, data);
        }
};
