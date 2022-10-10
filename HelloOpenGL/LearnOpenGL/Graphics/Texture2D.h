#pragma once
#ifndef TEXTURE_2D_H
#define TEXTURE_2D_H
#include <string>
#include <unordered_map>
#include <glad/glad.h>

namespace LearnOpenGL::Graphics
{
    constexpr GLenum DefaultTexture = 0;

    class Texture2D
    {
    public:
        explicit Texture2D(const std::string& texturePath, bool useMipmaps = false);
        Texture2D(const Texture2D& other);
        Texture2D(Texture2D&&) noexcept = default;

        Texture2D& operator=(const Texture2D& other);
        Texture2D& operator=(Texture2D&&) = default;

        [[nodiscard]] unsigned int getId() const;

        void use(GLenum activeTexture = 0) const;

        void setTextureWrap(GLint wrapX, GLint wrapY, GLenum activeTexture = DefaultTexture) const;
        void setTextureBorder(const std::array<GLfloat, 4>& borderColor, GLenum activeTexture = DefaultTexture) const;
        void setTextureFilters(GLint minifyingFilter, GLint magnifyingFilter, GLenum activeTexture = DefaultTexture) const;

        void bind() const;

        ~Texture2D();

        static void stopUsing(GLenum activeTexture);
        static void unbind();

    private:
        inline static std::pmr::unordered_map<unsigned int, unsigned int> _textureReferences{ {} };

        unsigned int _textureId;

        static void addReference(unsigned int textureId);
        static void removeReference(unsigned int textureId);
    };
}

#endif
