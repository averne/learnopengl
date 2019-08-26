#pragma once

#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "shader.hpp"
#include "mesh.hpp"

class Model {
    public:
        Model() = default;
        Model(const std::string &path) {
            load(path);
        }

        void load(const std::string &path) {
            Assimp::Importer import;
            const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

            if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
                std::cout << "Could not load model:\n" << import.GetErrorString() << '\n';
                return;
            }

            this->meshes.clear();
            process_node(scene->mRootNode, scene);
        }

        void draw(ShaderProgram &shader) {
            for (auto &mesh: this->meshes)
                mesh.draw(shader);
        }

        inline const std::vector<Mesh> &get_meshes() const { return this->meshes; };

    private:
        void process_node(aiNode *node, const aiScene *scene) {
            this->meshes.reserve(node->mNumMeshes);
            for (std::size_t i = 0; i < node->mNumMeshes; ++i)
                this->meshes.push_back(process_mesh(scene->mMeshes[node->mMeshes[i]], scene));

            for (std::size_t i = 0; i < node->mNumChildren; ++i)
                process_node(node->mChildren[i], scene);
        }

        Mesh process_mesh(aiMesh *mesh, const aiScene *scene) {
            std::vector<Mesh::Vertex>  vertices;
            std::vector<GLuint>        indices;
            std::vector<Mesh::Texture> textures;

            vertices.reserve(mesh->mNumVertices);
            for (std::size_t i = 0; i < mesh->mNumVertices; ++i) {
                Mesh::Vertex vert;
                vert.position   = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
                vert.normal     = glm::vec3(mesh->mNormals[i].x,  mesh->mNormals[i].y,  mesh->mNormals[i].z);
                vert.tex_coords = (mesh->mTextureCoords[0]) ?
                    glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y) : glm::vec2(0.0f, 0.0f);
                vertices.push_back(vert);
            }

            for (std::size_t i = 0; i < mesh->mNumFaces; ++i) {
                aiFace face = mesh->mFaces[i];
                indices.reserve(face.mNumIndices);
                for (GLuint j = 0; j < face.mNumIndices; ++j)
                    indices.push_back(face.mIndices[j]);
            }


            if (mesh->mMaterialIndex >= 0) {
                aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
                std::vector<Mesh::Texture> diffuse_maps =
                    load_textures(material, aiTextureType_DIFFUSE, TextureType::Diffuse);
                textures.insert(textures.end(), diffuse_maps.begin(), diffuse_maps.end());

                std::vector<Mesh::Texture> specular_maps =
                    load_textures(material, aiTextureType_SPECULAR, TextureType::Specular);
                textures.insert(textures.end(), specular_maps.begin(), specular_maps.end());
            }

            return Mesh(vertices, indices, textures);
        }

        std::vector<Mesh::Texture> load_textures(aiMaterial *mat, aiTextureType ass_type, TextureType type) {
            std::vector<Mesh::Texture> textures;
            textures.reserve(mat->GetTextureCount(ass_type));
            for(GLuint i = 0; i < mat->GetTextureCount(ass_type); i++) {
                aiString str;
                mat->GetTexture(ass_type, i, &str);
                textures.push_back({
                    Texture2d{str.C_Str()},
                    type,
                    std::string{str.C_Str()}
                });
            }
            return textures;

        }

    protected:
        std::vector<Mesh> meshes;
};
