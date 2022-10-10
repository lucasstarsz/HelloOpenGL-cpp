#pragma once
#ifndef VECTOR3_H
#define VECTOR3_H

#include <glm/glm.hpp>

namespace LearnOpenGL::Math::Vector3
{
    static constexpr glm::vec3 Zero = glm::vec3();
    static constexpr glm::vec3 Unit = glm::vec3(1.0f);

    static constexpr glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
    static constexpr glm::vec3 Down = glm::vec3(0.0f, -1.0f, 0.0f);
    static constexpr glm::vec3 Right = glm::vec3(1.0f, 0.0f, 0.0f);
    static constexpr glm::vec3 Left = glm::vec3(-1.0f, 0.0f, 0.0f);
    static constexpr glm::vec3 Forward = glm::vec3(0.0f, 0.0f, 1.0f);
    static constexpr glm::vec3 Back = glm::vec3(0.0f, 0.0f, -1.0f);
}

#endif
