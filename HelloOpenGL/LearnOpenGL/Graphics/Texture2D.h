#pragma once
#ifndef TEXTURE_H
#define TEXTURE_H
#include <string>
#include <glad/glad.h>

namespace LearnOpenGL::Graphics
{
    constexpr GLenum DefaultTexture = 0;

    class Texture2D
    {
    public:
        explicit Texture2D(const std::string& texturePath, bool useMipmaps = false);

        [[nodiscard]] unsigned int getId() const;
        
        void use(GLenum activeTexture = 0) const;

        void setTextureWrap(GLint wrapX, GLint wrapY, GLenum activeTexture = DefaultTexture) const;
        void setTextureBorder(const std::array<GLfloat, 4>& borderColor, GLenum activeTexture = DefaultTexture) const;
        void setTextureFilters(GLint minifyingFilter, GLint magnifyingFilter, GLenum activeTexture = DefaultTexture) const;

        void bind() const;
        static void unbind();

    private:
        unsigned int _textureId;
    };
}

#endif
