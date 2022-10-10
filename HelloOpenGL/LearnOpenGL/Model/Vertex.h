#pragma once

#ifndef VERTEX_H
#define VERTEX_H

#include <glm/glm.hpp>

namespace LearnOpenGL::Model
{
    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 textureCoordinates;
    };
}

#endif
