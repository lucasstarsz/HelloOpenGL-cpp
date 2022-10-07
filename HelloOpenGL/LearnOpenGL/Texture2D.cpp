﻿#include "Texture2D.h"

#include <array>
#include <iostream>
#include <ostream>
#include <stb/stb_image.h>

namespace LearnOpenGL
{
    Texture2D::Texture2D(const std::string& texturePath, const bool useMipmaps)
    {
        glGenTextures(1, &textureId);

        if (!textureId)
        {
            textureId = 0;
            std::cerr << "Failed to generate texture id for texture at " << texturePath << "\n";

            return;
        }

        bind();

        int imageWidth;
        int imageHeight;
        int numberChannels;
        unsigned char* imageData = stbi_load(texturePath.c_str(), &imageWidth, &imageHeight, &numberChannels, 0);

        if (imageData)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);

            if (useMipmaps)
            {
                glGenerateMipmap(GL_TEXTURE_2D);
            }
        }
        else
        {
            std::cerr << "Failed to load texture at " << texturePath << "\n";
        }

        stbi_image_free(imageData);
        unbind();
    }

    unsigned int Texture2D::getId() const
    {
        return textureId;
    }

    void Texture2D::use(const GLenum activeTexture) const
    {
        glActiveTexture(activeTexture);
        bind();
    }

    void Texture2D::setTextureWrap(const GLint wrapX, const GLint wrapY, const GLenum activeTexture) const
    {
        use(activeTexture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapX);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapY);

        unbind();
    }

    void Texture2D::setTextureBorder(const std::array<GLfloat, 4>& borderColor, const GLenum activeTexture) const
    {
        use(activeTexture);

        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor.data());

        unbind();
    }

    void Texture2D::setTextureFilters(const GLint minifyingFilter, const GLint magnifyingFilter, const GLenum activeTexture) const
    {
        use(activeTexture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minifyingFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magnifyingFilter);

        unbind();
    }

    void Texture2D::bind() const
    {
        glBindTexture(GL_TEXTURE_2D, textureId);
    }

    void Texture2D::unbind()
    {
        glBindTexture(GL_TEXTURE_2D, DefaultTexture);
    }
}