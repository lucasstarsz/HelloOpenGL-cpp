#include <iostream>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
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
void mouseCallback(GLFWwindow* window, double xPos, double yPos);
void scrollCallback(GLFWwindow*, double, double yOffset);
void processInput(GLFWwindow* window);

constexpr float MaxCameraSpeed = 5.0f;
constexpr float MinCameraSpeed = 2.0f;

Camera camera{ Vector3::Forward * 3.0f };
Timer timer{ glfwGetTime };

float lastMouseX = 400.0f;
float lastMouseY = 300.0f;

bool firstMouse = true;
bool hoveredOverScene = false;

bool enableSpotLight = false;
glm::vec3 spotLightPosition{ Vector3::Up * 5.0f };
glm::vec3 spotLightDirection{ Vector3::Down };
float spotLightAngle = 0.0f;
float spotLightBrightness = 0.75f;

float environmentBrightness = 0.5f;
bool fFirstPressed = false;

void render(Shader& shader, unsigned int planeVao, Texture2D& floorTexture, Model& testModel, Model& testModel2);

constexpr glm::vec3 pointLightPositions[] = {
    glm::vec3(0.7f, 0.2f, 2.0f),
    glm::vec3(2.3f, -3.3f, -4.0f),
    glm::vec3(-4.0f, 2.0f, -12.0f),
    glm::vec3(0.0f, 0.0f, -3.0f)
};

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

    GLFWwindow* window = glfwCreateWindow(1280, 720, "LearnOpenGL", nullptr, nullptr);

    if (!window)
    {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();

        return -1;
    }
    std::cerr << "hi\n";

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }
    std::cerr << "glad to be here\n";

    glfwSetFramebufferSizeCallback(window, &frameBufferSizeCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSwapInterval(1);

    glEnable(GL_DEPTH_TEST);

    // imgui setup

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    // io.ConfigWindowsMoveFromTitleBarOnly = true;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
    // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows
    ImGui::StyleColorsDark();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    // rendering setup
    std::cerr << "shader\n";

    Shader shader{ "vertex.glsl", "phong.frag" };

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

    // FBO
    unsigned int fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    unsigned int fboTexture;
    glGenTextures(1, &fboTexture);
    glBindTexture(GL_TEXTURE_2D, fboTexture);

    int ww;
    int wh;
    glfwGetWindowSize(window, &ww, &wh);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, ww, wh, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTexture, 0);

    unsigned int framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // generate texture
    unsigned int textureColorBuffer;
    glGenTextures(1, &textureColorBuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, ww, wh, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorBuffer, 0);

    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, ww, wh);

    // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        // execute non-victory dance
        std::cerr << "Victory was not achieved.\n";
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    Texture2D floorTexture{ "Res/wood.png", true, true };
    floorTexture.setTextureWrap(GL_REPEAT, GL_REPEAT);
    floorTexture.setTextureFilters(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    // game loop except not in the slightest
    ImVec4 clearColor{ 0.1f, 0.1f, 0.1f, 1.0f };

    ImVec2 sceneWindowSize{ 1280.0f, 720.0f };

    while (!glfwWindowShouldClose(window))
    {
        timer.evaluateDeltaTime();

        glfwPollEvents();

        processInput(window);

        glClearColor(clearColor.x, clearColor.y, clearColor.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

        glClearColor(clearColor.x, clearColor.y, clearColor.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        int windowWidth;
        int windowHeight;
        glfwGetWindowSize(window, &windowWidth, &windowHeight);

        glViewport(0, 0, sceneWindowSize.x, sceneWindowSize.y);

        const glm::mat4 view = camera.calculateView();
        const glm::mat4 projection = camera.calculateProjection();

        auto diffuseColor = glm::vec3(0.5f * environmentBrightness);
        auto ambientColor = glm::vec3(0.1f * environmentBrightness);
        auto specularColor = glm::vec3(0.5f * environmentBrightness);
        auto spotLightDiffuseColor = glm::vec3(1.25f, 1.0f, 1.0f) * spotLightBrightness;
        auto spotLightAmbientColor = glm::vec3(0.0f) * spotLightBrightness;
        auto spotLightSpecularColor = glm::vec3(1.0f) * spotLightBrightness;
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
        shader.setVec3("spotLight.position", spotLightPosition);
        shader.setVec3("spotLight.direction", spotLightDirection + (Vector3::Forward * spotLightAngle));
        shader.setFloat("spotLight.cutoff", cutoff);
        shader.setFloat("spotLight.outerCutoff", outerCutoff);

        if (enableSpotLight)
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

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        ImGui::SetNextWindowPos(ImVec2{ 0.0f, 0.0f }, ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2{ static_cast<float>(windowWidth), static_cast<float>(windowHeight) }, ImGuiCond_Once);
        ImGui::Begin("how does this even begin to qualify as a game engine");
        {
            ImGui::BeginChild("Engine Content");

            ImGui::Begin("\"Inspector\"");
            {
                int mouseInputMode = glfwGetInputMode(window, GLFW_CURSOR);

                ImGui::Text("Mouse hovering on GUI: %s", mouseInputMode && io.WantCaptureMouse ? "True" : "False");
                ImGui::Text("Mouse Pointer: %s", mouseInputMode == GLFW_CURSOR_NORMAL ? "Normal" : "Disabled");

                ImGui::Text("Mouse Position: (%.2f, %.2f)", static_cast<double>(io.MousePos.x), static_cast<double>(io.MousePos.y));
                ImGui::Text("Camera Position: (%.1f, %.1f, %.1f)", camera.cameraPos.x, camera.cameraPos.y, camera.cameraPos.z);
                ImGui::Text("Camera Facing: (%.1f, %.1f, %.1f)", camera.cameraFront.x, camera.cameraFront.y, camera.cameraFront.z);

                ImGui::ColorEdit3("Background Color", reinterpret_cast<float*>(&clearColor));

                if (ImGui::Button("Reset Player Position"))
                {
                    camera.cameraPos = Vector3::Forward * 3.0f;
                    camera.cameraFront = Vector3::Back;
                    camera.cameraUp = Vector3::Up;
                }

                ImGui::Checkbox("Toggle Point Light (or press F)", &enableSpotLight);
                ImGui::SliderFloat("Environment Brightness", &environmentBrightness, 0.0f, 1.0f);
                ImGui::SliderFloat3("Spotlight Position", value_ptr(spotLightPosition), -10.0f, 10.0f);
                ImGui::SliderFloat("Spotlight Angle", &spotLightAngle, -1.0f, 1.0f);
                ImGui::SameLine();
                ImGui::InputFloat("(Edit angle)", &spotLightAngle);
                ImGui::SliderFloat("Spotlight Brightness", &spotLightBrightness, 0.0f, 1.0f);

                ImGui::Text("Render Time: %.3f ms/frame (%.1f FPS)", static_cast<double>(1000.0f / ImGui::GetIO().Framerate),
                            static_cast<double>(ImGui::GetIO().Framerate));
            }
            ImGui::End();

            ImGui::Begin("Scene View");
            {
                ImGui::BeginChild("Yeet");
                // Get the current cursor position (where your window is)
                ImVec2 currentCursorPosition = ImGui::GetWindowSize();
                ImGui::Image(reinterpret_cast<void*>(textureColorBuffer), currentCursorPosition, ImVec2(0, 1), ImVec2(1, 0));
                hoveredOverScene = (ImGui::IsItemHovered() || glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED);

                ImGui::GetForegroundDrawList()->AddRect({ 0, 0 }, ImGui::GetWindowSize(), ImColor{ 255, 0, 0 });
                ImGui::EndChild();
            }
            ImGui::End();

            ImGui::EndChild();
        }
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
        //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        glfwSwapBuffers(window);
    }

    glDeleteProgram(shader.getId());
    glDeleteFramebuffers(1, &fbo);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

void frameBufferSizeCallback(GLFWwindow*, const int width, const int height)
{
    glViewport(0, 0, width, height);
}

bool firstPressed = false;
bool mouseEnabled = true;

void mouseCallback(GLFWwindow* window, const double xPos, const double yPos)
{
    if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL)
    {
        return;
    }

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
    if (hoveredOverScene && !firstPressed && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        mouseEnabled = !mouseEnabled;
        firstPressed = true;

        glfwSetInputMode(window, GLFW_CURSOR, mouseEnabled ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);

        if (!mouseEnabled)
        {
            firstMouse = true;
        }
    }
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
    {
        firstPressed = false;
    }

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        mouseEnabled = true;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL)
    {
        return;
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
        environmentBrightness = glm::clamp(environmentBrightness + 0.01f, 0.0f, 1.0f);
    }

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        environmentBrightness = glm::clamp(environmentBrightness - 0.01f, 0.0f, 1.0f);
    }

    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && !fFirstPressed)
    {
        fFirstPressed = true;
        enableSpotLight = !enableSpotLight;
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
