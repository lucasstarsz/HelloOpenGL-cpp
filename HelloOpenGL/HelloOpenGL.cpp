#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>

namespace LearnOpenGL
{
    static const std::unordered_map<GLuint, std::string> ShaderNames = {
        { GL_VERTEX_SHADER, "Vertex" },
        { GL_FRAGMENT_SHADER, "Fragment" },
        { GL_GEOMETRY_SHADER, "Geometry" },
    };

    unsigned int compileShader(const std::string& shaderSource, const GLuint shaderType)
    {
        const unsigned int shader = glCreateShader(shaderType);

        const char* shaderCString = shaderSource.c_str();
        glShaderSource(shader, 1, &shaderCString, nullptr);

        glCompileShader(shader);

        int shaderCompileSuccess;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &shaderCompileSuccess);

        if (!shaderCompileSuccess)
        {
            char infoLog[512];
            glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
            std::cerr << "ERROR: " << ShaderNames.at(shaderType) << " Shader Compilation Failed\n" << infoLog << '\n';

            return 0;
        }

        std::cerr << "Successfully compiled " << ShaderNames.at(shaderType) << " shader.\n";

        return shader;
    }

    unsigned int attachShaders(const std::initializer_list<unsigned int> shaders)
    {
        const unsigned int shaderProgram = glCreateProgram();

        for (const unsigned int shader : shaders)
        {
            glAttachShader(shaderProgram, shader);
        }

        glLinkProgram(shaderProgram);

        int shaderLinkSuccess;
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &shaderLinkSuccess);

        if (!shaderLinkSuccess)
        {
            char infoLog[512];
            glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
            std::cerr << "ERROR: Shader Program Linking Failed\n" << infoLog << '\n';

            return 0;
        }

        std::cerr << "Successfully attached all shaders.\n";

        return shaderProgram;
    }

    unsigned int setupShaders(const std::string& vertex, const std::string& fragment)
    {
        const unsigned int vertexShader = compileShader(vertex, GL_VERTEX_SHADER);
        const unsigned int fragmentShader = compileShader(fragment, GL_FRAGMENT_SHADER);

        // can't link shaders to program if they didn't all compile
        if (vertexShader == 0 || fragmentShader == 0)
        {
            return 0;
        }

        const unsigned int shaderProgram = attachShaders({ vertexShader, fragmentShader });

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        return shaderProgram;
    }

    std::string loadFile(const std::string& fileLocation)
    {
        const std::ifstream file(fileLocation);

        if (!file.is_open())
        {
            std::cerr << "Unable to find file at " << fileLocation << " to open.\n";
            return "";
        }

        std::cerr << "Found file at " << fileLocation << " to open.\n";

        std::stringstream fileContents;
        fileContents << file.rdbuf();

        return fileContents.str();
    }
}

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

int main()
{
#if __cplusplus >= 202002L
    std::cout << "We meet again, Rider...\n";
#endif

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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

    int attributeCount;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &attributeCount);

    std::cerr << attributeCount << " possible attributes.\n";

    const std::string vertexShaderSource = LearnOpenGL::loadFile("vertex.glsl");
    const std::string fragmentShaderSource = LearnOpenGL::loadFile("fragment.glsl");
    const unsigned int shaderProgram = LearnOpenGL::setupShaders(vertexShaderSource, fragmentShaderSource);

    constexpr float vertices[] = {
        -0.5f, 0.25f, 0.0f, // 0
        0.0f, 0.25f, 0.0f, // 1
        0.5f, 0.25f, 0.0f, // 2
        -0.5f, -0.25f, 0.0f, // 3
        0.0f, -0.25f, 0.0f, // 4
        0.5f, -0.25f, 0.0f, // 5
    };

    constexpr unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 4, // second triangle
        4, 3, 1, // third triangle
        5, 4, 2, // fourth triangle
    };

    unsigned int ebo;
    glGenBuffers(1, &ebo);

    unsigned int vao;
    glGenVertexArrays(1, &vao);

    unsigned int vbo;
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // game loop except not in the slightest
    glViewport(0, 0, 800, 600);

    while (!glfwWindowShouldClose(window))
    {
        constexpr float pi = 3.141592653589793238464643383f;
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        const auto time = static_cast<float>(glfwGetTime());
        const float redValue = sin(2 * time) / 2.0f + 0.5f;
        const float greenValue = sin(2 * time - (pi / 2.25f)) / 2.0f + 0.5f;
        const float blueValue = sin(2 * time - (2 * pi / 2.25f)) / 2.0f + 0.5f;

        const int uniformColorLocation = glGetUniformLocation(shaderProgram, "uniformColor");

        glUseProgram(shaderProgram);
        glUniform4f(uniformColorLocation, redValue, greenValue, blueValue, 1.0f);

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, nullptr);

        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteProgram(shaderProgram);

    glfwTerminate();

    return 0;
}
