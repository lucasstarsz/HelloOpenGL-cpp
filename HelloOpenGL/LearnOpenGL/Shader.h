#pragma once
#ifndef SHADER_H
#define SHADER_H

#include <string>

namespace LearnOpenGL::Shader
{
    class Shader
    {
    public:
        Shader(const std::string& vertexPath, const std::string& fragmentPath);

        unsigned int getId() const;

        void use() const;

        void setBool(const std::string& name, bool value) const;
        void setInt(const std::string& name, int value) const;
        void setFloat(const std::string& name, float value) const;

    private:
        unsigned int id;
    };
}

#endif // SHADER_H
