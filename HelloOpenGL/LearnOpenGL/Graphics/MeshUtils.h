#pragma once
#ifndef MESH_UTILS_H
#define MESH_UTILS_H

#include <vector>
#include <glm/glm.hpp>

namespace LearnOpenGL::Graphics
{
    std::pair<std::vector<float>, std::vector<unsigned int>> generateCube(const glm::vec3& position = glm::vec3(),
                                                                          float scale = 1.0f, bool textureCoords = false);

    std::pair<std::vector<float>, std::vector<unsigned int>> generateFloor(const glm::vec3& position = glm::vec3(), float scale = 1.0f,
                                                                           bool textureCoords = false);

    std::pair<std::vector<float>, std::vector<unsigned int>> generateTexturedNormalCube(float scale);
}

#endif
