#pragma once

#ifndef MESH_H
#define MESH_H

#include <vector>

#include "Texture.h"
#include "Vertex.h"
#include "../Graphics/Shader.h"

using LearnOpenGL::Model::Texture;

namespace LearnOpenGL::Model
{
    class Mesh
    {
    public:
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;

        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
        void draw(const Graphics::Shader& shader) const;

    private:
        unsigned int _vao{};
        unsigned int _vbo{};
        unsigned int _ebo{};

        void setupMesh();
    };
}

#endif
