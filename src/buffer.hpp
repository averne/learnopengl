#pragma once

#include <cstdint>
#include <stdexcept>
#include <initializer_list>
#include <glad/glad.h>

#include "object.hpp"
#include "utils.hpp"

struct BufferElement {
    enum Type: uint8_t {
        _1   = BIT(0), _2  = BIT(1), _3    = BIT(2), _4   = BIT(3),
        Bool = BIT(4), Int = BIT(5), Float = BIT(6), _Mat = BIT(7),
        _Nb = _1 | _2 | _3 | _4, _Type = Bool | Int | Float | _Mat,
        Int2   = Int   | _2, Int3   = Int   | _3, Int4   = Int   | _4,
        Float2 = Float | _2, Float3 = Float | _3, Float4 = Float | _4,
        Mat2  = _Mat   | _2, Mat3   = _Mat  | _3, Mat4   = _Mat  | _4,
    };

    GLenum gl_type;
    std::size_t nb, size;
    bool normalized;

    constexpr BufferElement(): gl_type(0), nb(0), size(0), normalized(0) { }
    constexpr BufferElement(Type type, bool normalized = false):
        gl_type(get_gl_type(type)), nb(get_nb(type)), size(get_size(type)), normalized(normalized) { }

    static constexpr std::size_t get_size(Type type) {
        return ((type & Type::Bool) ? 1 : 4) * get_nb(type);
    }

    static constexpr std::size_t get_nb(Type type) {
        if (type & Type::_Mat)
            return (type & Type::_Nb) * (type & Type::_Nb);
        return __builtin_ffs(type & Type::_Nb);
    }

    static constexpr GLenum get_gl_type(Type type) {
        if      (type & Type::Bool)                 return GL_BOOL;
        else if (type & Type::Int)                  return GL_INT;
        else if (type & (Type::Float | Type::_Mat)) return GL_FLOAT;
    }
};

struct BufferLayout {
    constexpr BufferLayout(std::initializer_list<BufferElement> &&elements):
        elements(elements), stride(get_stride(std::forward<std::initializer_list<BufferElement>>(elements))) { }

    static constexpr std::size_t get_stride(std::initializer_list<BufferElement> &&elements) {
        std::size_t stride = 0;
        for (auto &element: elements)
            stride += element.size;
        return stride;
    }

    std::initializer_list<BufferElement> elements;
    std::size_t stride;
};

template <GLenum Type, std::size_t N = 1>
class Buffer: public GlObject {
    public:
        Buffer() {
            glGenBuffers(get_nb(), &this->handle);
            if (!get_handle())
                throw std::runtime_error("Could not create Buffer object");
            bind();
        }

        ~Buffer() {
            glDeleteBuffers(get_nb(), &this->handle);
        }

        void set_data(const void *data, std::size_t size, GLenum draw_type = GL_STATIC_DRAW) {
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
        void set_layout(BufferLayout &&layout) {
            std::size_t i = 0, off = 0;
            for (auto &element: layout.elements) {
                set_attrib_ptr(i, element.nb, element.gl_type, layout.stride, (GLvoid *)off, element.normalized);
                off += element.size; ++i;
            }
        }

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
