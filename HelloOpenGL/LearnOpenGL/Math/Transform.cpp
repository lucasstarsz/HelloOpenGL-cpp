#include "Transform.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Vector3.h"

namespace LearnOpenGL::Math
{
    Transform::Transform()
        : _translation(glm::vec3(0.0f)),
          _rotation(glm::vec3(0.0f)),
          _scale(glm::vec3(1.0f)),
          _transformCache(glm::mat4(1.0f)),
          _isTransformDirty(false)
    {
    }

    glm::mat4 Transform::get()
    {
        if (_isTransformDirty)
        {
            glm::mat4 result(1.0f);

            result = glm::translate(result, _translation);
            result = glm::scale(result, _scale);
            result = glm::rotate(result, glm::radians(_rotation.x), Vector3::Right);
            result = glm::rotate(result, glm::radians(_rotation.y), Vector3::Up);
            result = glm::rotate(result, glm::radians(_rotation.z), Vector3::Forward);

            _transformCache = result;
        }

        return _transformCache;
    }

    void Transform::translate(const float translationScalar)
    {
        if (translationScalar == 0.0f)
        {
            return;
        }

        _translation += translationScalar;
        _isTransformDirty = true;
    }

    void Transform::translate(const glm::vec3& translation)
    {
        if (translation == Vector3::Zero)
        {
            return;
        }

        _translation += translation;
        _isTransformDirty = true;
    }

    void Transform::rotate(const float rotationScalar, const glm::vec3& direction)
    {
        if (rotationScalar == 0.0f)
        {
            return;
        }

        _rotation += direction * rotationScalar;
        _rotation = normalizeRotation(_rotation);

        _isTransformDirty = true;
    }

    void Transform::rotate(const glm::vec3& rotation, const glm::vec3& direction)
    {
        if (rotation == Vector3::Zero)
        {
            return;
        }

        _rotation += direction * rotation;
        _rotation = normalizeRotation(_rotation);

        _isTransformDirty = true;
    }

    void Transform::scale(const float scaleScalar)
    {
        if (scaleScalar == 0.0f)
        {
            return;
        }

        _scale += scaleScalar;
        _isTransformDirty = true;
    }

    void Transform::scale(const glm::vec3& scale)
    {
        if (scale == Vector3::Zero)
        {
            return;
        }

        _scale += scale;
        _isTransformDirty = true;
    }

    void Transform::forceCalculateTransform()
    {
        if (_isTransformDirty)
        {
            glm::mat4 result(1.0f);

            result = glm::translate(result, _translation);
            result = glm::scale(result, _scale);
            result = glm::rotate(result, glm::radians(_rotation.x), Vector3::Right);
            result = glm::rotate(result, glm::radians(_rotation.y), Vector3::Up);
            result = glm::rotate(result, glm::radians(_rotation.z), Vector3::Forward);

            _transformCache = result;
        }
    }

    glm::vec3 Transform::normalizeRotation(glm::vec3& rotation)
    {
        rotation.x = fmodf(rotation.x, 360.0f);
        rotation.y = fmodf(rotation.y, 360.0f);
        rotation.z = fmodf(rotation.z, 360.0f);

        return rotation;
    }
}
