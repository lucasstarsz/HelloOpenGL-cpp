#pragma once
#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Vector3.h"

namespace LearnOpenGL::Math
{
    struct Transform
    {
        Transform();

        Transform(const Transform&) = default;
        Transform(Transform&&) = default;

        ~Transform() = default;

        Transform& operator=(const Transform&) = default;
        Transform& operator=(Transform&&) = default;

        [[nodiscard]] glm::mat4 get();

        void translate(float translationScalar);
        void translate(const glm::vec3& translation);

        void rotate(float rotationScalar, const glm::vec3& direction = Vector3::Up);
        void rotate(const glm::vec3& rotation, const glm::vec3& direction = Vector3::Up);

        void scale(float scaleScalar);
        void scale(const glm::vec3& scale);

        void forceCalculateTransform();

    private:
        glm::vec3 _translation;
        glm::vec3 _rotation;
        glm::vec3 _scale;
        glm::mat4 _transformCache;
        bool _isTransformDirty;

        static glm::vec3 normalizeRotation(glm::vec3& rotation);
    };
}

#endif
