#pragma once

#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>

namespace LearnOpenGL::Model
{
    class Texture
    {
    public:
        unsigned int id;
        std::string type;
        std::string path;
        static unsigned int loadFromFile(const char* texturePath, const std::string& directory);
    };
}

#endif
