#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "LearnOpenGL/Shader.h"
#include <stb/stb_image.h>

#include "LearnOpenGL/Texture2D.h"

void frameBufferSizeCallback(GLFWwindow*, const int width, const int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

typedef LearnOpenGL::Shader::Shader Shader;
typedef LearnOpenGL::Texture2D Texture2D;

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

    constexpr float vertices[] =
    {
        // positions         // colors           // texture coords
        0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
        -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f // top left 
    };

    constexpr unsigned int indices[] =
    {
        0, 1, 3, // first triangle
        1, 2, 3, // second triangle
    };

    unsigned int vao;
    unsigned int vbo;
    unsigned int ebo;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    // color
    constexpr int colorSize = 3 * sizeof(float);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<const void*>(colorSize));
    glEnableVertexAttribArray(1);

    // texture
    constexpr int textureCoordsSize = 6 * sizeof(float);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<const void*>(textureCoordsSize));
    glEnableVertexAttribArray(2);

    stbi_set_flip_vertically_on_load(true);
    const LearnOpenGL::Texture2D texture("Res/container.jpg");

    texture.setTextureWrap(GL_REPEAT, GL_REPEAT);
    texture.setTextureFilters(GL_LINEAR, GL_LINEAR);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // game loop except not in the slightest
    glViewport(0, 0, 800, 600);

    auto previousTime = static_cast<float>(glfwGetTime());
    float accumulatedTime{};

    shader.use();
    shader.setInt("tex", 0);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        const auto currentTime = static_cast<float>(glfwGetTime());

        // fragment shader cycles all the way through color shifts completely every 9
        // one full shift (i.e. red -> green, green -> blue, blue -> red) takes 3
        // 3 full shifts => colors back to original state
        accumulatedTime = fmodf(accumulatedTime + currentTime - previousTime, 9.0f);

        texture.use();
        shader.use();
        shader.setFloat("accumulatedTime", accumulatedTime);

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();

        previousTime = currentTime;
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
