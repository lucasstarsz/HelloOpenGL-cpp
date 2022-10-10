#include "Mesh.h"

#include <string>
#include <glad/glad.h>

#include "Texture.h"

namespace LearnOpenGL::Model
{
    Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned> indices, std::vector<Texture> textures)
        : vertices(std::move(vertices)), indices(std::move(indices)), textures(std::move(textures))
    {
        setupMesh();
    }

    void Mesh::draw(const Graphics::Shader& shader) const
    {
        unsigned int diffuseNumber = 1;
        unsigned int specularNumber = 1;

        for (int i = 0; i < static_cast<int>(textures.size()); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i);

            std::string number;
            std::string name = textures[i].type;

            if (name == "diffuse")
            {
                number = std::to_string(diffuseNumber);
                diffuseNumber++;
            }
            else if (name == "specular")
            {
                number = std::to_string(specularNumber);
                specularNumber++;
            }

            shader.setInt(std::string("material.").append(name), i);
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }

        glActiveTexture(GL_TEXTURE0);

        glBindVertexArray(_vao);
        glDrawElements(GL_TRIANGLES, static_cast<int>(indices.size()), GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
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
