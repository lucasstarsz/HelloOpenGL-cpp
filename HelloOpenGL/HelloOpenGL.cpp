#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "LearnOpenGL/Graphics/Camera.h"
#include "LearnOpenGL/Graphics/MeshUtils.h"
#include "LearnOpenGL/Graphics/Texture2D.h"
#include "LearnOpenGL/Graphics/Shader.h"
#include "LearnOpenGL/Math/Transform.h"
#include "LearnOpenGL/Math/Vector3.h"
#include "LearnOpenGL/Utilities/Timer.h"

typedef LearnOpenGL::Graphics::Shader Shader;
typedef LearnOpenGL::Graphics::Texture2D Texture2D;
typedef LearnOpenGL::Math::Transform Transform;
typedef LearnOpenGL::Graphics::Camera Camera;
typedef LearnOpenGL::Utilities::Timer Timer;

namespace Vector3 = LearnOpenGL::Math::Vector3;

void frameBufferSizeCallback(GLFWwindow*, int width, int height);
void mouseCallback(GLFWwindow*, double xPos, double yPos);
void scrollCallback(GLFWwindow*, double, double yOffset);
void processInput(GLFWwindow* window);

constexpr float MaxCameraSpeed = 4.0f;
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
    std::cout << "We meet again, Rider...\n";
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

    glEnable(GL_DEPTH_TEST);

    // rendering setup

    const Shader lightShader("vertex.glsl", "phong.frag");
    const Shader lightCubeShader("vertex.glsl", "lightSource.frag");

    const Texture2D diffuseMap("Res/container2.png", true);
    diffuseMap.setTextureWrap(GL_REPEAT, GL_REPEAT);
    diffuseMap.setTextureFilters(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    const Texture2D specularMap("Res/container2_specular.png", true);
    specularMap.setTextureWrap(GL_REPEAT, GL_REPEAT);
    specularMap.setTextureFilters(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    const Texture2D emissionMap("Res/matrix.jpg", true);
    emissionMap.setTextureWrap(GL_REPEAT, GL_REPEAT);
    emissionMap.setTextureFilters(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    constexpr glm::vec3 pointLightPositions[] = {
        glm::vec3(0.7f, 0.2f, 2.0f),
        glm::vec3(2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f, 2.0f, -12.0f),
        glm::vec3(0.0f, 0.0f, -3.0f)
    };

    constexpr glm::vec3 cubePositions[] = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(2.0f, 5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f, 3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f, 2.0f, -2.5f),
        glm::vec3(1.5f, 0.2f, -1.5f),
        glm::vec3(-1.3f, 1.0f, -1.5f)
    };

    const auto [vertices, indices] = LearnOpenGL::Graphics::generateTexturedNormalCube(0.5f);

    unsigned int lightVao;
    unsigned int vbo;
    unsigned int ebo;

    glGenVertexArrays(1, &lightVao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(lightVao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, static_cast<long long>(sizeof(float) * vertices.size()), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<long long>(sizeof(int) * indices.size()), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    constexpr int normalOffset = 3 * sizeof(float);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<const void*>(normalOffset));
    glEnableVertexAttribArray(1);
    constexpr int textureCoordsOffset = 6 * sizeof(float);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<const void*>(textureCoordsOffset));
    glEnableVertexAttribArray(2);

    // light cube vao

    unsigned int lightCubeVao;
    glGenVertexArrays(1, &lightCubeVao);
    glBindVertexArray(lightCubeVao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // game loop except not in the slightest
    lightShader.use();
    lightShader.setInt("material.diffuse", 0);
    lightShader.setInt("material.specular", 1);
    lightShader.setInt("material.emission", 2);

    while (!glfwWindowShouldClose(window))
    {
        timer.evaluateDeltaTime();
        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        const glm::mat4 view = camera.calculateView();
        const glm::mat4 projection = camera.calculateProjection();
        glm::mat4 lightModel(1.0f);

        auto diffuseColor = glm::vec3(0.5f * brightness);
        auto ambientColor = glm::vec3(0.1f * brightness);
        auto specularColor = glm::vec3(0.5f * brightness);
        auto spotLightDiffuseColor = glm::vec3(0.75f);
        auto spotLightAmbientColor = glm::vec3(0.0f);
        auto spotLightSpecularColor = glm::vec3(0.75f);
        constexpr float lightConstant = 1.0f;
        constexpr float lightLinear = 0.09f;
        constexpr float lightQuadratic = 0.032f;
        const float cutoff = glm::cos(glm::radians(12.5f));
        const float outerCutoff = glm::cos(glm::radians(17.5f));

        lightShader.use();
        lightShader.setMat4("model", lightModel);
        lightShader.setMat4("view", view);
        lightShader.setMat4("projection", projection);

        lightShader.setFloat("material.shininess", 64.0f);
        lightShader.setVec3("viewPosition", camera.cameraPos);

        for (int i = 0; i < 4; i++)
        {
            lightShader.setVec3("pointLights[" + std::to_string(i) + "].position", pointLightPositions[i]);
            lightShader.setVec3("pointLights[" + std::to_string(i) + "].ambient", ambientColor);
            lightShader.setVec3("pointLights[" + std::to_string(i) + "].diffuse", diffuseColor);
            lightShader.setVec3("pointLights[" + std::to_string(i) + "].specular", specularColor);
            lightShader.setFloat("pointLights[" + std::to_string(i) + "].constant", lightConstant);
            lightShader.setFloat("pointLights[" + std::to_string(i) + "].linear", lightLinear);
            lightShader.setFloat("pointLights[" + std::to_string(i) + "].quadratic", lightQuadratic);
        }

        lightShader.setVec3("directionalLight.direction", camera.cameraFront);
        lightShader.setVec3("directionalLight.ambient", ambientColor);
        lightShader.setVec3("directionalLight.diffuse", diffuseColor);
        lightShader.setVec3("directionalLight.specular", specularColor);

        lightShader.setVec3("spotLight.position", camera.cameraPos);
        lightShader.setVec3("spotLight.direction", camera.cameraFront);
        lightShader.setFloat("spotLight.constant", lightConstant);
        lightShader.setFloat("spotLight.linear", lightLinear);
        lightShader.setFloat("spotLight.quadratic", lightQuadratic);
        lightShader.setFloat("spotLight.cutoff", cutoff);
        lightShader.setFloat("spotLight.outerCutoff", outerCutoff);

        if (enableFlashlight)
        {
            lightShader.setVec3("spotLight.ambient", spotLightAmbientColor);
            lightShader.setVec3("spotLight.diffuse", spotLightDiffuseColor);
            lightShader.setVec3("spotLight.specular", spotLightSpecularColor);
        }
        else
        {
            lightShader.setVec3("spotLight.ambient", Vector3::Zero);
            lightShader.setVec3("spotLight.diffuse", Vector3::Zero);
            lightShader.setVec3("spotLight.specular", Vector3::Zero);
        }

        lightShader.setFloat("time", static_cast<float>(timer.getCurrentTime()));

        diffuseMap.use(GL_TEXTURE0);
        specularMap.use(GL_TEXTURE1);
        if (enableEmissions)
        {
            emissionMap.use(GL_TEXTURE2);
        }
        else
        {
            Texture2D::stopUsing(GL_TEXTURE2);
        }

        glBindVertexArray(lightVao);
        glDrawElements(GL_TRIANGLES, static_cast<int>(indices.size()), GL_UNSIGNED_INT, nullptr);

        for (unsigned int i = 0; i < 10; i++)
        {
            auto model = glm::mat4(1.0f);
            model = translate(model, cubePositions[i]);
            const float angle = 20.0f * static_cast<float>(i);
            model = rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            lightShader.setMat4("model", model);

            glDrawElements(GL_TRIANGLES, static_cast<int>(indices.size()), GL_UNSIGNED_INT, nullptr);
        }

        // light sources

        lightCubeShader.use();
        lightCubeShader.setMat4("view", view);
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setFloat("brightness", brightness);

        // we now draw as many light bulbs as we have point lights.
        glBindVertexArray(lightCubeVao);
        for (auto pointLightPosition : pointLightPositions)
        {
            lightModel = glm::mat4(1.0f);
            lightModel = translate(lightModel, pointLightPosition);
            lightModel = scale(lightModel, glm::vec3(0.2f));
            lightCubeShader.setMat4("model", lightModel);

            glDrawElements(GL_TRIANGLES, static_cast<int>(indices.size()), GL_UNSIGNED_INT, nullptr);
        }

        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);

    // TODO determine better approach to deleting class-based content
    const GLuint diffuseMapId = diffuseMap.getId();
    glDeleteTextures(1, &diffuseMapId);
    const GLuint specularMapId = specularMap.getId();
    glDeleteTextures(1, &specularMapId);
    const GLuint emissionMapId = emissionMap.getId();
    glDeleteTextures(1, &emissionMapId);
    glDeleteProgram(lightShader.getId());
    glDeleteProgram(lightCubeShader.getId());

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
