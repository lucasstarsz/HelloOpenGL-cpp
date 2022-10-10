#include "Camera.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace LearnOpenGL::Graphics
{
    Camera::Camera(const glm::vec3& pos, const glm::vec3& up, const float pitch, const float yaw)
        : pitch(pitch),
          yaw(yaw),
          cameraPos(pos),
          cameraFront(DefaultFront),
          cameraRight(DefaultRight),
          cameraUp(up),
          _fov(DefaultFov),
          _nearClip(DefaultNearClip),
          _farClip(DefaultFarClip),
          _worldUp(DefaultUp)
    {
        updateCameraVectors();
    }

    float Camera::getFov() const
    {
        return _fov;
    }

    float Camera::getNearClip() const
    {
        return _nearClip;
    }

    float Camera::getFarClip() const
    {
        return _farClip;
    }

    glm::mat4 Camera::calculateView() const
    {
        return lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    }

    glm::mat4 Camera::calculateProjection() const
    {
        // Get information about the viewport, to make sure the projection is calculated correctly.
        // https://www.khronos.org/registry/OpenGL-Refpages/es2.0/xhtml/glViewport.xml
        // viewportData[2] and [3] contain the width and height of the viewport.
        // Inspired by https://github.com/wmcnamara/crynn/

        float viewportData[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
        glGetFloatv(GL_VIEWPORT, viewportData);

        const float windowWidth = viewportData[2];
        const float windowHeight = viewportData[3];
        const float aspectRatio = windowWidth / windowHeight;

        if (windowWidth <= 0.0f || windowHeight <= 0.0f)
        {
            return glm::mat4(1.0f);
        }

        return glm::perspective(glm::radians(_fov), aspectRatio, _nearClip, _farClip);
    }

    void Camera::setFov(const float fov)
    {
        _fov = glm::clamp(fov, MinFov, MaxFov);
    }

    void Camera::setClip(const float nearClip, const float farClip)
    {
        _nearClip = nearClip;
        _farClip = farClip;
    }

    void Camera::updateCameraVectors()
    {
        glm::vec3 front;

        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

        cameraFront = front;
        cameraRight = normalize(cross(cameraFront, _worldUp));
        cameraUp = normalize(cross(cameraRight, cameraFront));
    }
}
