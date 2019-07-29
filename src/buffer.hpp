#pragma once

#include <GLFW/glfw3.h>

#include "object.hpp"

template <GLenum Type, std::size_t N = 1>
class Buffer: public GlObject {
    public:
        Buffer() {
            glGenBuffers(get_nb(), &this->handle);
            if (!get_handle()); // throw
            bind();
        }

        void set_data(void *data, std::size_t size, GLenum draw_type = GL_STATIC_DRAW) {
            this->size = size;
            glBufferData(get_type(), size, data, draw_type);
        }

        void bind() const {
            glBindBuffer(get_type(), get_handle());
        }

        static void bind(GLuint handle) {
            glBindBuffer(get_type(), handle);
        }

        static void unbind() {
            glBindBuffer(get_type(), 0);
        }

        static inline std::size_t get_nb()         { return N; }
        static inline GLenum      get_type()       { return Type; }
        inline std::size_t        get_size() const { return this->size; }

    protected:
        std::size_t size;
};

template <std::size_t N = 1>
class VertexBuffer: public Buffer<GL_ARRAY_BUFFER, N> {
    public:
        void set_attrib_ptr(GLuint pos, GLuint size, GLenum type, GLuint stride = 0, GLvoid *off = nullptr, bool normalize = false) {
            glVertexAttribPointer(pos, size, type, normalize, stride, off);
            enable_attrib_arr(pos);
        }

        void enable_attrib_arr(GLuint pos) {
            glEnableVertexAttribArray(pos);
        }

        void disable_attrib_arr(GLuint pos) {
            glDisableVertexAttribArray(pos);
        }
};

template <std::size_t N = 1>
class ElementBuffer: public Buffer<GL_ELEMENT_ARRAY_BUFFER, N> { };
