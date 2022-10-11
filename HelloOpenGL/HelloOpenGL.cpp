#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb/stb_image.h>

#include "LearnOpenGL/Graphics/Camera.h"
#include "LearnOpenGL/Graphics/Shader.h"
#include "LearnOpenGL/Graphics/Texture2D.h"
#include "LearnOpenGL/Math/Transform.h"
#include "LearnOpenGL/Math/Vector3.h"
#include "LearnOpenGL/Model/Model.h"
#include "LearnOpenGL/Utilities/Timer.h"

typedef LearnOpenGL::Graphics::Shader Shader;
typedef LearnOpenGL::Graphics::Texture2D Texture2D;
typedef LearnOpenGL::Math::Transform Transform;
typedef LearnOpenGL::Graphics::Camera Camera;
typedef LearnOpenGL::Utilities::Timer Timer;
typedef LearnOpenGL::Model::Model Model;

namespace Vector3 = LearnOpenGL::Math::Vector3;

void frameBufferSizeCallback(GLFWwindow*, int width, int height);
void mouseCallback(GLFWwindow*, double xPos, double yPos);
void scrollCallback(GLFWwindow*, double, double yOffset);
void processInput(GLFWwindow* window);

constexpr float MaxCameraSpeed = 5.0f;
constexpr float MinCameraSpeed = 2.0f;

Camera camera{ Vector3::Forward * 3.0f };
Timer timer{ glfwGetTime };

float lastMouseX = 400.0f;
float lastMouseY = 300.0f;

bool firstMouse = true;

glm::vec3 lightPosition(1.2f, 1.0f, 2.0f);

float brightness = 0.5f;
bool enableEmissions = false;
bool enableFlashlight = false;
bool mFirstPressed = false;
bool fFirstPressed = false;

int main()
{
#if __cplusplus >= 202002L
    std::cerr << "We meet again, Rider...\n";
#endif

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", nullptr, nullptr);

    if (!window)
    {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();

        return -1;
    }
    std::cerr << "hi\n";

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, &frameBufferSizeCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }
    std::cerr << "glad to be here\n";

    glEnable(GL_DEPTH_TEST);

    // rendering setup
    std::cerr << "shader\n";

    const Shader shader("vertex.glsl", "phong.frag");
    constexpr glm::vec3 pointLightPositions[] = {
        glm::vec3(0.7f, 0.2f, 2.0f),
        glm::vec3(2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f, 2.0f, -12.0f),
        glm::vec3(0.0f, 0.0f, -3.0f)
    };

    std::cerr << "model\n";
    stbi_set_flip_vertically_on_load(true);

    Model::debugLogging = true;

    Model testModel{ "Res/backpack/backpack.obj" };
    Model testModel2{ "Res/textured_car/untitled.obj" };

    float vertices[] = {
        10.0f, -0.5f, 10.0f, 0.0f, 1.0f, 0.0f, 10.0f, 0.0f,
        -10.0f, -0.5f, 10.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        -10.0f, -0.5f, -10.0f, 0.0f, 1.0f, 0.0f, 0.0f, 10.0f,

        10.0f, -0.5f, 10.0f, 0.0f, 1.0f, 0.0f, 10.0f, 0.0f,
        -10.0f, -0.5f, -10.0f, 0.0f, 1.0f, 0.0f, 0.0f, 10.0f,
        10.0f, -0.5f, -10.0f, 0.0f, 1.0f, 0.0f, 10.0f, 10.0f
    };

    unsigned int indices[] = {
        0, 1, 2,
        3, 4, 5
    };

    // plane VAO
    unsigned int planeVao, planeVbo, planeEbo;
    glGenVertexArrays(1, &planeVao);
    glGenBuffers(1, &planeVbo);
    glGenBuffers(1, &planeEbo);
    glBindVertexArray(planeVao);
    glBindBuffer(GL_ARRAY_BUFFER, planeVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planeEbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<const void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<const void*>(6 * sizeof(float)));
    glBindVertexArray(0);

    unsigned int depthMapFbo;
    glGenFramebuffers(1, &depthMapFbo);

    constexpr unsigned int shadowWidth = 1024;
    constexpr unsigned int shadowHeight = 1024;

    unsigned int depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowWidth, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    Texture2D floorTexture{ "Res/wood.png", true, true };
    floorTexture.setTextureWrap(GL_REPEAT, GL_REPEAT);
    floorTexture.setTextureFilters(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    // game loop except not in the slightest
    shader.use();
    shader.setInt("material.diffuse", 0);
    shader.setInt("material.specular", 1);
    shader.setInt("material.emission", 2);

    while (!glfwWindowShouldClose(window))
    {
        timer.evaluateDeltaTime();
        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        const glm::mat4 view = camera.calculateView();
        const glm::mat4 projection = camera.calculateProjection();

        auto diffuseColor = glm::vec3(0.5f * brightness);
        auto ambientColor = glm::vec3(0.1f * brightness);
        auto specularColor = glm::vec3(0.5f * brightness);
        auto spotLightDiffuseColor = glm::vec3(1.25f, 1.0f, 1.0f);
        auto spotLightAmbientColor = glm::vec3(0.0f);
        auto spotLightSpecularColor = glm::vec3(1.0f);
        const float cutoff = glm::cos(glm::radians(12.5f));
        const float outerCutoff = glm::cos(glm::radians(17.5f));

        shader.use();
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        shader.setFloat("material.shininess", 32.0f);
        shader.setVec3("viewPosition", camera.cameraPos);

        for (int i = 0; i < 4; i++)
        {
            shader.setVec3("pointLights[" + std::to_string(i) + "].position", pointLightPositions[i]);
            shader.setVec3("pointLights[" + std::to_string(i) + "].ambient", ambientColor);
            shader.setVec3("pointLights[" + std::to_string(i) + "].diffuse", diffuseColor);
            shader.setVec3("pointLights[" + std::to_string(i) + "].specular", specularColor);
        }

        shader.setVec3("directionalLight.direction", Vector3::Down + Vector3::Forward);
        shader.setVec3("directionalLight.ambient", ambientColor);
        shader.setVec3("directionalLight.diffuse", diffuseColor);
        shader.setVec3("directionalLight.specular", specularColor);
        shader.setVec3("spotLight.position", Vector3::Up * 10.0f + Vector3::Back * 10.0f);
        shader.setVec3("spotLight.direction", Vector3::Down + Vector3::Forward);
        shader.setFloat("spotLight.cutoff", cutoff);
        shader.setFloat("spotLight.outerCutoff", outerCutoff);

        if (enableFlashlight)
        {
            shader.setVec3("spotLight.ambient", spotLightAmbientColor);
            shader.setVec3("spotLight.diffuse", spotLightDiffuseColor);
            shader.setVec3("spotLight.specular", spotLightSpecularColor);
        }
        else
        {
            shader.setVec3("spotLight.ambient", Vector3::Zero);
            shader.setVec3("spotLight.diffuse", Vector3::Zero);
            shader.setVec3("spotLight.specular", Vector3::Zero);
        }

        glBindVertexArray(planeVao);
        shader.use();
        floorTexture.use(GL_TEXTURE0);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        Transform modelTransform{};
        shader.setMat4("model", modelTransform.get());
        testModel.draw(shader);
        modelTransform.translate(Vector3::Forward * 5.0f);
        shader.setMat4("model", modelTransform.get());
        testModel2.draw(shader);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(shader.getId());

    glfwTerminate();

    return 0;
}

void frameBufferSizeCallback(GLFWwindow*, const int width, const int height)
{
    glViewport(0, 0, width, height);
}

void mouseCallback(GLFWwindow*, const double xPos, const double yPos)
{
    if (firstMouse)
    {
        lastMouseX = static_cast<float>(xPos);
        lastMouseY = static_cast<float>(yPos);
        firstMouse = false;
    }

    float xOffset = static_cast<float>(xPos) - lastMouseX;
    float yOffset = lastMouseY - static_cast<float>(yPos); // reversed, since y-coordinates range from bottom to top

    lastMouseX = static_cast<float>(xPos);
    lastMouseY = static_cast<float>(yPos);

    constexpr float sensitivity = 0.1f;
    xOffset *= sensitivity;
    yOffset *= sensitivity;

    camera.yaw += xOffset;
    camera.pitch = glm::clamp(camera.pitch + yOffset, -89.0f, 89.0f);
    camera.updateCameraVectors();
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    float cameraSpeed = MinCameraSpeed;

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        cameraSpeed = MaxCameraSpeed;
    }

    cameraSpeed *= timer.getDeltaTime();

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera.cameraPos += cameraSpeed * camera.cameraFront;
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera.cameraPos -= cameraSpeed * camera.cameraFront;
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera.cameraPos -= normalize(cross(camera.cameraFront, camera.cameraUp)) * cameraSpeed;
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.cameraPos += normalize(cross(camera.cameraFront, camera.cameraUp)) * cameraSpeed;
    }

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        camera.cameraPos -= cameraSpeed * Vector3::Up;
    }

    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        camera.cameraPos -= cameraSpeed * Vector3::Down;
    }

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        brightness = glm::clamp(brightness + 0.01f, 0.0f, 1.0f);
    }

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        brightness = glm::clamp(brightness - 0.01f, 0.0f, 1.0f);
    }

    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS && !mFirstPressed)
    {
        mFirstPressed = true;
        enableEmissions = !enableEmissions;
    }
    else if (glfwGetKey(window, GLFW_KEY_M) == GLFW_RELEASE && mFirstPressed)
    {
        mFirstPressed = false;
    }

    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && !fFirstPressed)
    {
        fFirstPressed = true;
        enableFlashlight = !enableFlashlight;
    }
    else if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE && fFirstPressed)
    {
        fFirstPressed = false;
    }
}

void scrollCallback(GLFWwindow*, double, const double yOffset)
{
    camera.setFov(camera.getFov() - static_cast<float>(yOffset));
}
