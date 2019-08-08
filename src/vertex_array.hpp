#pragma once

#include <GL/glew.h>

#include "object.hpp"

template <std::size_t N = 1>
class VertexArray: public GlObject {
    public:
        VertexArray() {
            glGenVertexArrays(get_nb(), &this->handle);
            bind();
        }

        ~VertexArray() {
            glDeleteVertexArrays(get_nb(), &this->handle);
        }

        void bind() const {
            glBindVertexArray(get_handle());
        }

        static void bind(GLuint handle) {
            glBindVertexArray(handle);
        }

        static void unbind() {
            glBindVertexArray(0);
        }

        static inline std::size_t get_nb() { return N; }
};
