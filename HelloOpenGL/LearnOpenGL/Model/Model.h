#pragma once

#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include "Material.h"
#include "Mesh.h"
#include "../Graphics/Shader.h"

namespace LearnOpenGL::Model
{
    class Model
    {
    public:
        explicit Model(const std::string& modelPath);
        void draw(const Graphics::Shader& shader) const;

    private:
        std::vector<Mesh> _meshes;
        std::vector<Texture> _texturesLoaded;
        std::string _modelDirectory;

        void loadModel(const std::string& path);
        void processNode(const aiNode* node, const aiScene* scene);
        Mesh processMesh(aiMesh* mesh, const aiScene* scene);
        static Material loadMaterial(const aiMaterial* aiMaterial);
        std::vector<Texture> loadMaterialTextures(const aiMaterial* mat, aiTextureType type, const std::string& typeName);
    };
}

#endif
