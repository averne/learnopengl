#pragma once

#include <string>
#include <tuple>
#include <glad/glad.h>
#include <glm/glm.hpp>

#include "vertex_array.hpp"
#include "buffer.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "utils.hpp"

class Mesh {
    public:
        struct Vertex {
            glm::vec3 position;
            glm::vec3 normal;
            glm::vec2 tex_coords;
        };

        struct Texture {
            Texture2d<> tex;
            TextureType type;
            std::string path;
        };

        Mesh(std::vector<Vertex> &vertices, std::vector<GLuint> &indices, std::vector<Texture> &textures):
                vertices(vertices), indices(indices), textures(textures) {
            bind_all(this->vao, this->vbo, this->ebo);
            this->vbo.set_data(this->vertices.data(), this->vertices.size() * sizeof(Vertex));
            this->vbo.set_layout({BufferElement::Float3, BufferElement::Float3, BufferElement::Float2});
            this->ebo.set_data(this->indices.data(), this->indices.size() * sizeof(GLuint));
        }

        void draw(ShaderProgram &program) {
            std::size_t i = 0, diff_cnt = 0, spec_cnt = 0;
            for (auto &[texture, type, path]: this->textures) {
                texture.active(i++);
                std::string id = "";
                if      (type == TextureType::Diffuse)
                    id = "tex_diff_" + std::to_string(diff_cnt++);
                else if (type == TextureType::Specular)
                    id = "tex_spec_" + std::to_string(spec_cnt++);
                program.set_value(("material." + id).c_str(), i);
                texture.bind();
            }
            this->vao.bind();
            glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
        }

    protected:
        VertexArray<>   vao;
        VertexBuffer<>  vbo;
        ElementBuffer<> ebo;

        std::vector<Vertex>  vertices;
        std::vector<GLuint>  indices;
        std::vector<Texture> textures;
};
