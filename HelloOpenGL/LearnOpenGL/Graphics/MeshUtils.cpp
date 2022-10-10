#include "MeshUtils.h"

namespace LearnOpenGL::Graphics
{
    std::pair<std::vector<float>, std::vector<unsigned int>> generateCube(const glm::vec3& position, const float scale,
                                                                          const bool textureCoords)
    {
        // vertices order:
        // top front left
        // top front right
        // top back left
        // top back right
        // bottom front left
        // bottom front right
        // bottom back left
        // bottom back right

        // indices triangle pairs:
        // top
        // front
        // bottom
        // back
        // left
        // right

        std::vector<float> vertices;

        if (textureCoords)
        {
            vertices =
            {
                position.x - scale, position.y - scale, position.z - scale, 0.0f, 0.0f,
                position.x + scale, position.y - scale, position.z - scale, 1.0f, 0.0f,
                position.x + scale, position.y + scale, position.z - scale, 1.0f, 1.0f,
                position.x - scale, position.y + scale, position.z - scale, 0.0f, 1.0f,

                position.x - scale, position.y - scale, position.z + scale, 0.0f, 0.0f,
                position.x + scale, position.y - scale, position.z + scale, 1.0f, 0.0f,
                position.x + scale, position.y + scale, position.z + scale, 1.0f, 1.0f,
                position.x - scale, position.y + scale, position.z + scale, 0.0f, 1.0f,

                position.x - scale, position.y + scale, position.z + scale, 1.0f, 0.0f,
                position.x - scale, position.y + scale, position.z - scale, 1.0f, 1.0f,
                position.x - scale, position.y - scale, position.z - scale, 0.0f, 1.0f,
                position.x - scale, position.y - scale, position.z + scale, 0.0f, 0.0f,

                position.x + scale, position.y + scale, position.z + scale, 1.0f, 0.0f,
                position.x + scale, position.y + scale, position.z - scale, 1.0f, 1.0f,
                position.x + scale, position.y - scale, position.z - scale, 0.0f, 1.0f,
                position.x + scale, position.y - scale, position.z + scale, 0.0f, 0.0f,

                position.x - scale, position.y - scale, position.z - scale, 0.0f, 1.0f,
                position.x + scale, position.y - scale, position.z - scale, 1.0f, 1.0f,
                position.x + scale, position.y - scale, position.z + scale, 1.0f, 0.0f,
                position.x - scale, position.y - scale, position.z + scale, 0.0f, 0.0f,

                position.x - scale, position.y + scale, position.z - scale, 0.0f, 1.0f,
                position.x + scale, position.y + scale, position.z - scale, 1.0f, 1.0f,
                position.x + scale, position.y + scale, position.z + scale, 1.0f, 0.0f,
                position.x - scale, position.y + scale, position.z + scale, 0.0f, 0.0f,
            };
        }
        else
        {
            vertices =
            {
                position.x - scale, position.y - scale, position.z - scale,
                position.x + scale, position.y - scale, position.z - scale,
                position.x + scale, position.y + scale, position.z - scale,
                position.x - scale, position.y + scale, position.z - scale,

                position.x - scale, position.y - scale, position.z + scale,
                position.x + scale, position.y - scale, position.z + scale,
                position.x + scale, position.y + scale, position.z + scale,
                position.x - scale, position.y + scale, position.z + scale,

                position.x - scale, position.y + scale, position.z + scale,
                position.x - scale, position.y + scale, position.z - scale,
                position.x - scale, position.y - scale, position.z - scale,
                position.x - scale, position.y - scale, position.z + scale,

                position.x + scale, position.y + scale, position.z + scale,
                position.x + scale, position.y + scale, position.z - scale,
                position.x + scale, position.y - scale, position.z - scale,
                position.x + scale, position.y - scale, position.z + scale,

                position.x - scale, position.y - scale, position.z - scale,
                position.x + scale, position.y - scale, position.z - scale,
                position.x + scale, position.y - scale, position.z + scale,
                position.x - scale, position.y - scale, position.z + scale,

                position.x - scale, position.y + scale, position.z - scale,
                position.x + scale, position.y + scale, position.z - scale,
                position.x + scale, position.y + scale, position.z + scale,
                position.x - scale, position.y + scale, position.z + scale,
            };
        }

        return {
            vertices,
            {
                0, 1, 2,
                2, 3, 0,

                4, 5, 6,
                6, 7, 4,

                8, 9, 10,
                10, 11, 8,

                12, 13, 14,
                14, 15, 12,

                16, 17, 18,
                18, 19, 16,

                20, 21, 22,
                22, 23, 20
            }
        };
    }

    std::pair<std::vector<float>, std::vector<unsigned int>> generateFloor(const glm::vec3& position, const float scale,
                                                                           const bool textureCoords)
    {
        std::vector<float> vertices;

        if (textureCoords)
        {
            vertices =
            {
                position.x - scale, position.y, position.z + scale, 0.0f, 0.0f,
                position.x + scale, position.y, position.z + scale, 1.0f, 0.0f,
                position.x + scale, position.y, position.z - scale, 1.0f, 1.0f,
                position.x - scale, position.y, position.z - scale, 0.0f, 1.0f
            };
        }
        else
        {
            vertices =
            {
                position.x - scale, position.y, position.z + scale,
                position.x + scale, position.y, position.z + scale,
                position.x + scale, position.y, position.z - scale,
                position.x - scale, position.y, position.z - scale,
            };
        }

        return {
            vertices,
            {
                0, 1, 2,
                2, 3, 0
            }
        };
    }

    std::pair<std::vector<float>, std::vector<unsigned int>> generateTexturedNormalCube(const float scale)
    {
        return {
            {
                -scale, -scale, -scale, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
                scale, -scale, -scale, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
                scale, scale, -scale, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
                -scale, scale, -scale, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,

                -scale, -scale, scale, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
                scale, -scale, scale, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
                scale, scale, scale, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
                -scale, scale, scale, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,

                -scale, scale, scale, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
                -scale, scale, -scale, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
                -scale, -scale, -scale, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
                -scale, -scale, scale, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,

                scale, scale, scale, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
                scale, scale, -scale, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
                scale, -scale, -scale, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
                scale, -scale, scale, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,

                -scale, -scale, -scale, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
                scale, -scale, -scale, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
                scale, -scale, scale, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
                -scale, -scale, scale, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,

                -scale, scale, -scale, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
                scale, scale, -scale, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
                scale, scale, scale, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
                -scale, scale, scale, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
            },
            {
                0, 1, 2,
                2, 3, 0,

                4, 5, 6,
                6, 7, 4,

                8, 9, 10,
                10, 11, 8,

                12, 13, 14,
                14, 15, 12,

                16, 17, 18,
                18, 19, 16,

                20, 21, 22,
                22, 23, 20
            }
        };
    }
}
