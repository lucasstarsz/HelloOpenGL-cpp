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

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        std::cerr << "Failed to initialize GLAD\n";

        return -1;
    }

    // rendering setup

    const Shader shader("vertex.glsl", "fragment.glsl");

    const auto [vertices, indices] = LearnOpenGL::Graphics::generateCube(Vector3::Zero, 0.5f, true);

    unsigned int vao;
    unsigned int vbo;
    unsigned int ebo;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, static_cast<long long>(sizeof(float) * vertices.size()), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<long long>(sizeof(int) * indices.size()), indices.data(), GL_STATIC_DRAW);

    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    // texture
    constexpr int textureCoordsOffset = 3 * sizeof(float);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<const void*>(textureCoordsOffset));
    glEnableVertexAttribArray(1);

    stbi_set_flip_vertically_on_load(true);
    const Texture2D texture("Res/container.jpg");

    texture.setTextureWrap(GL_REPEAT, GL_REPEAT);
    texture.setTextureFilters(GL_LINEAR, GL_LINEAR);

    const auto [v2, i2] = LearnOpenGL::Graphics::generateFloor(Vector3::Down, 5.0f, true);

    unsigned int vao2;
    unsigned int vbo2;
    unsigned int ebo2;

    glGenVertexArrays(1, &vao2);
    glGenBuffers(1, &vbo2);
    glGenBuffers(1, &ebo2);

    glBindVertexArray(vao2);

    glBindBuffer(GL_ARRAY_BUFFER, vbo2);
    glBufferData(GL_ARRAY_BUFFER, static_cast<long long>(sizeof(float) * v2.size()), v2.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo2);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<long long>(sizeof(int) * i2.size()), i2.data(), GL_STATIC_DRAW);

    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    // texture
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<const void*>(textureCoordsOffset));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    Transform transform{};

    constexpr glm::mat4 identity(1.0f);
    constexpr glm::mat4 model(identity);

    // game loop except not in the slightest
    glViewport(0, 0, 800, 600);
    glEnable(GL_DEPTH_TEST);
    glfwSwapInterval(1);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);

    float accumulatedTime = 0.0f;
    float rotation = 0.01f;
    constexpr float scaleMod = -0.5f;

    shader.use();
    shader.setInt("tex", 0);
    shader.setMat4("transform", transform.get());
    shader.setMat4("model", model);

    while (!glfwWindowShouldClose(window))
    {
        const float deltaTime = timer.evaluateDeltaTime();

        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // fragment shader cycles all the way through color shifts completely every 9
        // one full shift (i.e. red -> green, green -> blue, blue -> red) takes 3
        // 3 full shifts => colors back to original state
        accumulatedTime = fmodf(accumulatedTime + deltaTime, 9.0f);

        transform.rotate(static_cast<float>(glfwGetTime()) * deltaTime, Vector3::Forward);

        const glm::mat4 view = camera.calculateView();
        const glm::mat4 projection = camera.calculateProjection();

        glBindVertexArray(vao);
        texture.use();
        shader.use();
        shader.setFloat("accumulatedTime", accumulatedTime);
        shader.setMat4("transform", transform.get());
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        glDrawElements(GL_TRIANGLES, static_cast<int>(indices.size()), GL_UNSIGNED_INT, nullptr);

        Transform t2{};
        t2.translate(glm::vec3(-1.0f, 0.5f, 0.0f));
        t2.scale(glm::vec3(scaleMod));
        t2.rotate(rotation, Vector3::Right + Vector3::Up);

        shader.setMat4("transform", t2.get());
        glDrawElements(GL_TRIANGLES, static_cast<int>(indices.size()), GL_UNSIGNED_INT, nullptr);

        Transform t3{};
        t3.translate(glm::vec3(1.0f, -0.5f, 0.0f));
        t3.scale(glm::vec3(scaleMod));
        t3.rotate(rotation, Vector3::Left + Vector3::Down);

        shader.setMat4("transform", t3.get());
        glDrawElements(GL_TRIANGLES, static_cast<int>(indices.size()), GL_UNSIGNED_INT, nullptr);

        glBindVertexArray(vao2);
        shader.setMat4("transform", glm::mat4(1.0f));
        glDrawElements(GL_TRIANGLES, static_cast<int>(i2.size()), GL_UNSIGNED_INT, nullptr);

        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();

        rotation = fmodf(rotation + 1, 360.0f);
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);

    // TODO determine better approach to deleting class-based content
    const GLuint textureToDelete = texture.getId();
    glDeleteTextures(1, &textureToDelete);
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
}

void scrollCallback(GLFWwindow*, double, const double yOffset)
{
    camera.setFov(camera.getFov() - static_cast<float>(yOffset));
}
