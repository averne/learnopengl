#pragma once

#include <GLFW/glfw3.h>

class GlObject {
    public:
        GlObject() = default;
        GlObject(GLuint handle): handle(handle) { }

        virtual ~GlObject() = default;

        inline GLuint get_handle() const { return this->handle; }

    protected:
        GLuint handle = 0;
};
