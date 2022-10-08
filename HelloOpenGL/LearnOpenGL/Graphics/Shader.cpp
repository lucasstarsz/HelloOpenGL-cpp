#include <glad/glad.h>
#include "Shader.h"

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
            _id = 0;
            return;
        }

        _id = attachShaders({ vertexShader, fragmentShader });

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    unsigned int Shader::getId() const
    {
        return _id;
    }

    void Shader::use() const
    {
        glUseProgram(_id);
    }

    void Shader::setBool(const std::string& name, const bool value) const
    {
        glUniform1i(glGetUniformLocation(_id, name.c_str()), static_cast<int>(value));
    }

    void Shader::setInt(const std::string& name, const int value) const
    {
        glUniform1i(glGetUniformLocation(_id, name.c_str()), value);
    }

    void Shader::setFloat(const std::string& name, const float value) const
    {
        glUniform1f(glGetUniformLocation(_id, name.c_str()), value);
    }
}
