#include "Mesh.h"

#include <iostream>
#include <string>
#include <glad/glad.h>

#include "Texture.h"

namespace LearnOpenGL::Model
{
    Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned> indices, std::vector<Texture> textures, Material material)
        : vertices(std::move(vertices)), indices(std::move(indices)), textures(std::move(textures)), material(material)
    {
        setupMesh();
    }

    void Mesh::draw(const Graphics::Shader& shader) const
    {
        // bind appropriate textures
        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;
        for (unsigned int i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i);

            std::string number;
            std::string name = textures[i].type;
            if (name == "diffuse")
            {
                number = std::to_string(diffuseNr++);
            }
            else if (name == "specular")
            {
                number = std::to_string(specularNr++);
            }

            shader.setInt(std::string("material.").append(name).append(number), i);
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }

        if (textures.empty())
        {
            shader.setVec3("material.diffuseColor", material.diffuseColor);
            shader.setVec3("material.specularColor", material.specularColor);
            shader.setVec3("material.emissionColor", material.emissionColor);
            shader.setFloat("material.shininess", material.shininess);
        }

        glBindVertexArray(_vao);
        glDrawElements(GL_TRIANGLES, static_cast<int>(indices.size()), GL_UNSIGNED_INT, nullptr);

        // unbind all
        glBindVertexArray(0);

        // unbind textures
        for (unsigned int i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }

    void Mesh::setupMesh()
    {
        glGenVertexArrays(1, &_vao);
        glGenBuffers(1, &_vbo);
        glGenBuffers(1, &_ebo);

        glBindVertexArray(_vao);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, static_cast<long long>(sizeof(Vertex) * vertices.size()), vertices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<long long>(sizeof(unsigned int) * indices.size()), indices.data(),
                     GL_STATIC_DRAW);

        // position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
        glEnableVertexAttribArray(0);

        // normal
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<const void*>(offsetof(Vertex, normal)));
        glEnableVertexAttribArray(1);

        // texture
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              reinterpret_cast<const void*>(offsetof(Vertex, textureCoordinates)));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
    }
}
