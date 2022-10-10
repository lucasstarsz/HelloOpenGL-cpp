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

    const Shader lightShader("vertex.glsl", "lighting.frag");
    const Shader lightCubeShader("vertex.glsl", "lightSource.frag");

    const auto [vertices, indices] = LearnOpenGL::Graphics::generateCubeWithNormals(Vector3::Zero, 0.5f);

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

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
    glEnableVertexAttribArray(1);

    // light cube vao

    unsigned int lightCubeVao;
    glGenVertexArrays(1, &lightCubeVao);
    glBindVertexArray(lightCubeVao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // game loop except not in the slightest

    while (!glfwWindowShouldClose(window))
    {
        timer.evaluateDeltaTime();
        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        const glm::mat4 view = camera.calculateView();
        const glm::mat4 projection = camera.calculateProjection();
        glm::mat4 lightModel(1.0f);

        lightShader.use();
        lightShader.setMat4("model", lightModel);
        lightShader.setMat4("view", view);
        lightShader.setMat4("projection", projection);
        lightShader.setVec3("objectColor", glm::vec3(1.0f, 0.5f, 0.31f));
        lightShader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
        lightShader.setVec3("lightPosition", lightPosition);
        lightShader.setVec3("viewPosition", camera.cameraPos);

        glBindVertexArray(lightVao);
        glDrawElements(GL_TRIANGLES, static_cast<int>(indices.size()), GL_UNSIGNED_INT, nullptr);

        // light source

        lightCubeShader.use();

        lightModel = glm::mat4(1.0f);
        lightModel = translate(lightModel, lightPosition);
        lightModel = scale(lightModel, glm::vec3(0.5f));

        lightCubeShader.setMat4("model", lightModel);
        lightCubeShader.setMat4("view", view);
        lightCubeShader.setMat4("projection", projection);

        glBindVertexArray(lightCubeVao);
        glDrawElements(GL_TRIANGLES, static_cast<int>(indices.size()), GL_UNSIGNED_INT, nullptr);

        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);

    // TODO determine better approach to deleting class-based content
    // const GLuint textureToDelete = texture.getId();
    // glDeleteTextures(1, &textureToDelete);
    // glDeleteProgram(shader.getId());

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
}

void scrollCallback(GLFWwindow*, double, const double yOffset)
{
    camera.setFov(camera.getFov() - static_cast<float>(yOffset));
}
