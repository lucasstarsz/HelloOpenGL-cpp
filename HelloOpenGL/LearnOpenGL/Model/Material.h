#pragma once

#ifndef MATERIAL_H
#define MATERIAL_H

#include <glm/glm.hpp>

namespace LearnOpenGL::Model
{
    struct Material
    {
        glm::vec3 diffuseColor = glm::vec3(0.0f);
        glm::vec3 specularColor = glm::vec3(0.0f);
        float shininess = 0.5f;
    };
}

#endif
