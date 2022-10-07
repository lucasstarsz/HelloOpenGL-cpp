#include <glad/glad.h>
#include "Shader.h"

#include "FileUtils.h"
#include "ShaderUtils.h"

namespace LearnOpenGL::Shader
{
    Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
    {
        const std::string vertexShaderSource = LearnOpenGL::Utilities::loadFile(vertexPath);
        const std::string fragmentShaderSource = LearnOpenGL::Utilities::loadFile(fragmentPath);

        const unsigned int vertexShader = Utilities::compileShader(vertexShaderSource, GL_VERTEX_SHADER);
        const unsigned int fragmentShader = Utilities::compileShader(fragmentShaderSource, GL_FRAGMENT_SHADER);

        // can't link shaders to program if they didn't all compile
        if (vertexShader == 0 || fragmentShader == 0)
        {
            id = 0;
            return;
        }

        id = Utilities::attachShaders({ vertexShader, fragmentShader });

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    unsigned int Shader::getId() const
    {
        return id;
    }

    void Shader::use() const
    {
        glUseProgram(id);
    }

    void Shader::setBool(const std::string& name, const bool value) const
    {
        glUniform1i(glGetUniformLocation(id, name.c_str()), static_cast<int>(value));
    }

    void Shader::setInt(const std::string& name, const int value) const
    {
        glUniform1i(glGetUniformLocation(id, name.c_str()), value);
    }

    void Shader::setFloat(const std::string& name, const float value) const
    {
        glUniform1f(glGetUniformLocation(id, name.c_str()), value);
    }
}
