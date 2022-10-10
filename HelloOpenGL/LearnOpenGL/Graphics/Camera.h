#pragma once
#ifndef CAMERA_H
#define CAMERA_H
#include <glm/glm.hpp>

#include "../Math/Vector3.h"

namespace Vector3 = LearnOpenGL::Math::Vector3;

namespace LearnOpenGL::Graphics
{
    class Camera
    {
    public:
        inline static constexpr glm::vec3 DefaultPosition{ Vector3::Zero };
        inline static constexpr glm::vec3 DefaultFront{ Vector3::Forward };
        inline static constexpr glm::vec3 DefaultUp{ Vector3::Up };
        inline static constexpr glm::vec3 DefaultRight{ Vector3::Right };
        inline static constexpr float DefaultPitch{ 0.0f };
        inline static constexpr float DefaultYaw{ -90.0f };
        inline static constexpr float DefaultFov{ 45.0f };
        inline static constexpr float DefaultNearClip{ 0.001f };
        inline static constexpr float DefaultFarClip{ 1000.0f };
        inline static constexpr float MinFov{ 1.0f };
        inline static constexpr float MaxFov{ 179.0f };

        float pitch;
        float yaw;

        glm::vec3 cameraPos;
        glm::vec3 cameraFront;
        glm::vec3 cameraRight;
        glm::vec3 cameraUp;

        explicit Camera(const glm::vec3& pos = DefaultPosition, const glm::vec3& up = DefaultUp,
                        float pitch = DefaultPitch, float yaw = DefaultYaw);

        [[nodiscard]] float getFov() const;
        [[nodiscard]] float getNearClip() const;
        [[nodiscard]] float getFarClip() const;

        [[nodiscard]] glm::mat4 calculateView() const;
        [[nodiscard]] glm::mat4 calculateProjection() const;

        void setFov(float fov);
        void setClip(float nearClip, float farClip);

        void updateCameraVectors();

    private:
        float _fov;
        float _nearClip;
        float _farClip;

        glm::vec3 _worldUp;
    };
}
#endif
