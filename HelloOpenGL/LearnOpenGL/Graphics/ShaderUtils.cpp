#include "ShaderUtils.h"

#include <iostream>
#include <string>
#include <unordered_map>
#include <glad/glad.h>

namespace LearnOpenGL::Graphics
{
    static const std::unordered_map<GLuint, std::string> ShaderNames = {
        { GL_VERTEX_SHADER, "Vertex" },
        { GL_FRAGMENT_SHADER, "Fragment" },
        { GL_GEOMETRY_SHADER, "Geometry" },
    };

    unsigned int compileShader(const std::string& shaderSource, const GLuint& shaderType)
    {
        const unsigned int shader = glCreateShader(shaderType);

        const char* shaderCString = shaderSource.c_str();
        glShaderSource(shader, 1, &shaderCString, nullptr);

        glCompileShader(shader);

        int shaderCompileSuccess;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &shaderCompileSuccess);

        if (!shaderCompileSuccess)
        {
            char infoLog[512];
            glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
            std::cerr << "ERROR: " << ShaderNames.at(shaderType) << " Graphics Compilation Failed\n" << infoLog << '\n';

            return 0;
        }

        std::cerr << "Successfully compiled " << ShaderNames.at(shaderType) << " shader.\n";

        return shader;
    }
    unsigned int attachShaders(const std::initializer_list<unsigned int>& shaders)
    {
        const unsigned int shaderProgram = glCreateProgram();

        for (const unsigned int shader : shaders)
        {
            glAttachShader(shaderProgram, shader);
        }

        glLinkProgram(shaderProgram);

        int shaderLinkSuccess;
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &shaderLinkSuccess);

        if (!shaderLinkSuccess)
        {
            char infoLog[512];
            glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
            std::cerr << "ERROR: Graphics Program Linking Failed\n" << infoLog << '\n';

            return 0;
        }

        std::cerr << "Successfully attached all shaders.\n";

        return shaderProgram;
    }
}
