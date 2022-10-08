﻿#pragma once
#ifndef SHADER_H
#define SHADER_H

#include <string>

namespace LearnOpenGL::Graphics
{
    class Shader
    {
    public:
        Shader(const std::string& vertexPath, const std::string& fragmentPath);

        [[nodiscard]] unsigned int getId() const;

        void use() const;

        void setBool(const std::string& name, bool value) const;
        void setInt(const std::string& name, int value) const;
        void setFloat(const std::string& name, float value) const;

    private:
        unsigned int _id;
    };
}

#endif // SHADER_H