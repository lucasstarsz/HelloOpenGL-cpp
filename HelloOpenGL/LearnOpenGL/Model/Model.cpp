﻿#include "Model.h"

#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/gtc/type_ptr.hpp>

namespace LearnOpenGL::Model
{
    Model::Model(const std::string& modelPath)
    {
        loadModel(modelPath);
    }

    void Model::draw(const Graphics::Shader& shader) const
    {
        for (const auto& mesh : _meshes)
        {
            mesh.draw(shader);
        }
    }

    void Model::loadModel(const std::string& path)
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(
            path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            std::cerr << "Assimp Error: " << importer.GetErrorString() << '\n';
            return;
        }

        _modelDirectory = path.substr(0, path.find_last_of('/'));
        processNode(scene->mRootNode, scene);
    }

    void Model::processNode(const aiNode* node, const aiScene* scene)
    {
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            _meshes.push_back(processMesh(mesh, scene));
        }

        // then do the same for each of its children
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }
    }

    Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;

        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex{};

            glm::vec3 v3;

            v3.x = mesh->mVertices[i].x;
            v3.y = mesh->mVertices[i].y;
            v3.z = mesh->mVertices[i].z;

            vertex.position = v3;

            if (mesh->HasNormals())
            {
                v3.x = mesh->mNormals[i].x;
                v3.y = mesh->mNormals[i].y;
                v3.z = mesh->mNormals[i].z;

                vertex.normal = v3;
            }

            if (mesh->mTextureCoords[0])
            {
                glm::vec2 v2;

                v2.x = mesh->mTextureCoords[0][i].x;
                v2.y = mesh->mTextureCoords[0][i].y;

                vertex.textureCoordinates = v2;
            }
            else
            {
                vertex.textureCoordinates = glm::vec2(0.0f);
            }

            vertices.push_back(vertex);
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
            {
                indices.push_back(face.mIndices[j]);
            }
        }

        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

        return { vertices, indices, textures };
    }

    std::vector<Texture> Model::loadMaterialTextures(const aiMaterial* mat, const aiTextureType type, const std::string& typeName)
    {
        std::vector<Texture> textures;

        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);

            bool textureAlreadyLoaded = false;

            for (auto& texture : _texturesLoaded)
            {
                if (std::strcmp(texture.path.data(), str.C_Str()) == 0)
                {
                    textures.push_back(texture);
                    textureAlreadyLoaded = true;
                    break;
                }
            }

            if (!textureAlreadyLoaded)
            {
                Texture texture;

                texture.id = Texture::loadFromFile(str.C_Str(), _modelDirectory);
                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);

                _texturesLoaded.push_back(texture);
            }
        }

        return textures;
    }
}
