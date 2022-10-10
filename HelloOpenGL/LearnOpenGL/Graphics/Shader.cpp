#include "Shader.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ShaderUtils.h"
#include "../Utilities/FileUtils.h"

namespace LearnOpenGL::Graphics
{
    Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
    {
        const std::string vertexShaderSource = Utilities::loadFile(vertexPath);
        const std::string fragmentShaderSource = Utilities::loadFile(fragmentPath);

        const unsigned int vertexShader = compileShader(vertexShaderSource, GL_VERTEX_SHADER);
        const unsigned int fragmentShader = compileShader(fragmentShaderSource, GL_FRAGMENT_SHADER);

        // can't link shaders to program if they didn't all compile
        if (vertexShader == 0 || fragmentShader == 0)
        {
            _shaderId = 0;
            return;
        }

        _shaderId = attachShaders({ vertexShader, fragmentShader });

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        addReference(_shaderId);
    }

    Shader::Shader(const Shader& other)
        : _shaderId(other._shaderId)
    {
        addReference(_shaderId);
    }

    Shader::~Shader()
    {
        removeReference(_shaderId);
    }

    unsigned int Shader::getId() const
    {
        return _shaderId;
    }

    void Shader::use() const
    {
        glUseProgram(_shaderId);
    }

    void Shader::setBool(const std::string& name, const bool value) const
    {
        glUniform1i(glGetUniformLocation(_shaderId, name.c_str()), static_cast<int>(value));
    }

    void Shader::setInt(const std::string& name, const int value) const
    {
        glUniform1i(glGetUniformLocation(_shaderId, name.c_str()), value);
    }

    void Shader::setFloat(const std::string& name, const float value) const
    {
        glUniform1f(glGetUniformLocation(_shaderId, name.c_str()), value);
    }

    void Shader::setMat4(const std::string& name, const glm::mat4& value, const GLboolean transposeMatrix) const
    {
        glUniformMatrix4fv(glGetUniformLocation(_shaderId, name.c_str()), 1, transposeMatrix, value_ptr(value));
    }

    void Shader::setVec3(const std::string& name, const glm::vec3& value) const
    {
        glUniform3fv(glGetUniformLocation(_shaderId, name.c_str()), 1, value_ptr(value));
    }

    void Shader::setMat3(const std::string& name, const glm::mat3& value, const GLboolean transposeMatrix) const
    {
        glUniformMatrix3fv(glGetUniformLocation(_shaderId, name.c_str()), 1, transposeMatrix, value_ptr(value));
    }

    void Shader::addReference(const unsigned int shaderId)
    {
        if (!shaderId)
        {
            return;
        }

        if (!_shaderReferences.contains(shaderId))
        {
            _shaderReferences.insert({ shaderId, 1 });
        }
        else
        {
            _shaderReferences[shaderId]++;
        }
    }

    void Shader::removeReference(const unsigned shaderId)
    {
        if (!_shaderReferences.contains(shaderId))
        {
            return;
        }

        _shaderReferences[shaderId]--;

        if (_shaderReferences[shaderId] == 0)
        {
            glDeleteProgram(shaderId);
            _shaderReferences.erase(shaderId);
        }
    }

    Shader& Shader::operator=(const Shader& other)
    {
        _shaderId = other._shaderId;
        addReference(_shaderId);

        return *this;
    }
}
