#pragma once
#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <unordered_map>
#include <glad/glad.h>
#include <glm/fwd.hpp>

namespace LearnOpenGL::Graphics
{
    class Shader
    {
    public:
        Shader(const std::string& vertexPath, const std::string& fragmentPath);
        Shader(const Shader& other);
        Shader(Shader&&) noexcept = default;

        Shader& operator=(const Shader& other);
        Shader& operator=(Shader&&) = default;

        ~Shader();

        [[nodiscard]] unsigned int getId() const;

        void use() const;

        void setBool(const std::string& name, bool value) const;
        void setInt(const std::string& name, int value) const;
        void setFloat(const std::string& name, float value) const;
        void setMat4(const std::string& name, const glm::mat4& value, GLboolean transposeMatrix = GL_FALSE) const;

    private:
        inline static std::pmr::unordered_map<unsigned int, unsigned int> _shaderReferences{ {} };

        unsigned int _shaderId;

        static void addReference(unsigned int shaderId);
        static void removeReference(unsigned int shaderId);
    };
}

#endif // SHADER_H
