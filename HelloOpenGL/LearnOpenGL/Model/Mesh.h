#pragma once

#ifndef MESH_H
#define MESH_H

#include <vector>

#include "Material.h"
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
        Material material;

        Mesh(std::vector<Vertex> vertices, std::vector<unsigned> indices, std::vector<Texture> textures, Material material);
        void draw(const Graphics::Shader& shader) const;

    private:
        unsigned int _vao{};
        unsigned int _vbo{};
        unsigned int _ebo{};

        void setupMesh();
    };
}

#endif
