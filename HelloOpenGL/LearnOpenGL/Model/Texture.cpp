#include "Texture.h"

#include <iostream>
#include <glad/glad.h>
#include <stb/stb_image.h>

namespace LearnOpenGL::Model
{
    unsigned Texture::loadFromFile(const char* texturePath, const std::string& directory)
    {
        const auto filename = std::string(directory + '/' + texturePath);

        unsigned int textureId;
        glGenTextures(1, &textureId);

        if (!textureId)
        {
            textureId = 0;
            std::cerr << "Failed to generate texture id for texture at " << filename << "\n";

            return 0;
        }

        int imageWidth;
        int imageHeight;
        int numberChannels;
        unsigned char* imageData = stbi_load(filename.c_str(), &imageWidth, &imageHeight, &numberChannels, 0);

        if (imageData)
        {
            GLint format;
            switch (numberChannels)
            {
            case 3:
                format = GL_RGB;
                break;
            case 4:
                format = GL_RGBA;
                break;
            default:
                format = GL_RED;
                break;
            }

            glBindTexture(GL_TEXTURE_2D, textureId);

            glTexImage2D(GL_TEXTURE_2D, 0, format, imageWidth, imageHeight, 0, format, GL_UNSIGNED_BYTE, imageData);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }
        else
        {
            std::cerr << "Failed to load texture at " << filename << "\n";
            return 0;
        }

        stbi_image_free(imageData);
        glBindTexture(GL_TEXTURE_2D, 0);

        return textureId;
    }
}
