#pragma once

#ifndef SHADER_UTILS_H
#define SHADER_UTILS_H

#include <string>
#include <glad/glad.h>

namespace LearnOpenGL::Graphics
{
    unsigned int compileShader(const std::string& shaderSource, const GLuint& shaderType);
    unsigned int attachShaders(const std::initializer_list<unsigned int>& shaders);
}

#endif // SHADER_UTILS_H
