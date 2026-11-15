
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

// ========================================
// HORIZON ENGINE 0.4
// FIRST 3D RENDERER
// ========================================

// Window settings
const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

// Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// ========================================
// WINDOW RESIZE
// ========================================

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    (void)window;
    glViewport(0, 0, width, height);
}

// ========================================
// VERTEX SHADER
// ========================================

const char* vertexShaderSource = R"(
#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aColor;

out vec3 vertexColor;

uniform float uRotation;
uniform mat4 uProjection;

void main()
{
    float c = cos(uRotation);
    float s = sin(uRotation);

    mat3 rotation = mat3(
         c, 0.0, -s,
         0.0, 1.0, 0.0,
         s, 0.0, c
    );

    vec3 rotatedPosition = rotation * aPosition;

    // Move cube away from the camera
    rotatedPosition.z -= 4.0;

    gl_Position = uProjection * vec4(rotatedPosition, 1.0);

    vertexColor = aColor;
}
)";

// ========================================
// FRAGMENT SHADER
// ========================================

const char* fragmentShaderSource = R"(
#version 330 core

in vec3 vertexColor;

out vec4 FragColor;

void main()
{
    FragColor = vec4(vertexColor, 1.0);
}
)";

// ========================================
// SHADER COMPILATION
// ========================================

GLuint createShader(GLenum type, const char* source)
{
    GLuint shader = glCreateShader(type);

    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        char infoLog[512];

        glGetShaderInfoLog(shader, 512, nullptr, infoLog);

        std::cerr << "SHADER COMPILATION ERROR:\n"
                  << infoLog << "\n";
    }

    return shader;
}

// ========================================
// SHADER PROGRAM
// ========================================

GLuint createShaderProgram()
{
    GLuint vertexShader =
        createShader(GL_VERTEX_SHADER, vertexShaderSource);

    GLuint fragmentShader =
        createShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    GLuint program = glCreateProgram();

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (!success)
    {
        char infoLog[512];

        glGetProgramInfoLog(program, 512, nullptr, infoLog);

        std::cerr << "SHADER LINKING ERROR:\n"
                  << infoLog << "\n";
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

// ========================================
// PERSPECTIVE PROJECTION
// ========================================

void createProjection(float* matrix, float aspectRatio)
{
    const float fov = 1.04719755f; // 60 degrees
    const float nearPlane = 0.1f;
    const float farPlane = 100.0f;

    const float f = 1.0f / tanf(fov / 2.0f);

    for (int i = 0; i < 16; i++)
        matrix[i] = 0.0f;

    matrix[0] = f / aspectRatio;
    matrix[5] = f;
    matrix[10] = (farPlane + nearPlane) /
                 (nearPlane - farPlane);

    matrix[11] = -1.0f;

    matrix[14] = (2.0f * farPlane * nearPlane) /
                 (nearPlane - farPlane);
}

// ========================================
// MAIN ENGINE
// ========================================

int main()
{
    std::cout << "================================\n";
    std::cout << "       HORIZON ENGINE 0.4\n";
    std::cout << "================================\n";

    std::cout << "Initializing GLFW...\n";

    if (!glfwInit())
    {
        std::cerr << "GLFW initialization failed!\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,
                   GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        "Horizon Engine 0.4 - 3D Renderer",
        nullptr,
        nullptr
    );

    if (!window)
    {
        std::cerr << "Window creation failed!\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // ====================================
    // INITIALIZE GLAD
    // ====================================

    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress))
    {
        std::cerr << "GLAD initialization failed!\n";

        glfwDestroyWindow(window);
        glfwTerminate();

        return -1;
    }

    std::cout << "OpenGL initialized!\n";
    std::cout << "OpenGL version: "
              << glGetString(GL_VERSION) << "\n";

    glfwSwapInterval(1);

    glfwSetFramebufferSizeCallback(
        window,
        framebuffer_size_callback
    );

    int width, height;

    glfwGetFramebufferSize(window, &width, &height);

    glViewport(0, 0, width, height);

    // ====================================
    // DEPTH TESTING
    // ====================================

    glEnable(GL_DEPTH_TEST);

    // ====================================
    // CUBE GEOMETRY
    // 36 vertices, 6 faces
    // Each vertex: XYZ + RGB
    // ====================================

    float vertices[] = {

        // Front face - Red
        -0.5f,-0.5f, 0.5f, 1,0,0,
         0.5f,-0.5f, 0.5f, 1,0,0,
         0.5f, 0.5f, 0.5f, 1,0,0,

         0.5f, 0.5f, 0.5f, 1,0,0,
        -0.5f, 0.5f, 0.5f, 1,0,0,
        -0.5f,-0.5f, 0.5f, 1,0,0,

        // Back face - Green
        -0.5f,-0.5f,-0.5f, 0,1,0,
        -0.5f, 0.5f,-0.5f, 0,1,0,
         0.5f, 0.5f,-0.5f, 0,1,0,

         0.5f, 0.5f,-0.5f, 0,1,0,
         0.5f,-0.5f,-0.5f, 0,1,0,
        -0.5f,-0.5f,-0.5f, 0,1,0,

        // Left face - Blue
        -0.5f, 0.5f, 0.5f, 0,0,1,
        -0.5f, 0.5f,-0.5f, 0,0,1,
        -0.5f,-0.5f,-0.5f, 0,0,1,

        -0.5f,-0.5f,-0.5f, 0,0,1,
        -0.5f,-0.5f, 0.5f, 0,0,1,
        -0.5f, 0.5f, 0.5f, 0,0,1,

        // Right face - Yellow
         0.5f, 0.5f, 0.5f, 1,1,0,
         0.5f,-0.5f,-0.5f, 1,1,0,
         0.5f, 0.5f,-0.5f, 1,1,0,

         0.5f,-0.5f,-0.5f, 1,1,0,
         0.5f, 0.5f, 0.5f, 1,1,0,
         0.5f,-0.5f, 0.5f, 1,1,0,

        // Top face - Cyan
        -0.5f, 0.5f,-0.5f, 0,1,1,
        -0.5f, 0.5f, 0.5f, 0,1,1,
         0.5f, 0.5f, 0.5f, 0,1,1,

         0.5f, 0.5f, 0.5f, 0,1,1,
         0.5f, 0.5f,-0.5f, 0,1,1,
        -0.5f, 0.5f,-0.5f, 0,1,1,

        // Bottom face - Purple
        -0.5f,-0.5f,-0.5f, 1,0,1,
         0.5f,-0.5f, 0.5f, 1,0,1,
        -0.5f,-0.5f, 0.5f, 1,0,1,

         0.5f,-0.5f, 0.5f, 1,0,1,
        -0.5f,-0.5f,-0.5f, 1,0,1,
         0.5f,-0.5f,-0.5f, 1,0,1
    };

    // ====================================
    // CREATE GPU BUFFERS
    // ====================================

    GLuint VAO, VBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(vertices),
        vertices,
        GL_STATIC_DRAW
    );

    // Position attribute
    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE,
        6 * sizeof(float),
        (void*)0
    );

    glEnableVertexAttribArray(0);

    // Color attribute
    glVertexAttribPointer(
        1, 3, GL_FLOAT, GL_FALSE,
        6 * sizeof(float),
        (void*)(3 * sizeof(float))
    );

    glEnableVertexAttribArray(1);

    // ====================================
    // CREATE SHADERS
    // ====================================

    GLuint shaderProgram = createShaderProgram();

    GLint rotationLocation =
        glGetUniformLocation(shaderProgram, "uRotation");

    GLint projectionLocation =
        glGetUniformLocation(shaderProgram, "uProjection");

    // ====================================
    // PROJECTION MATRIX
    // ====================================

    float projection[16];

    float aspectRatio =
        static_cast<float>(width) /
        static_cast<float>(height);

    createProjection(projection, aspectRatio);

    std::cout << "3D cube geometry created!\n";
    std::cout << "Shader system initialized!\n";
    std::cout << "Starting renderer...\n";

    // ====================================
    // ENGINE LOOP
    // ====================================

    double fpsTimer = 0.0;
    int frameCount = 0;

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame =
            static_cast<float>(glfwGetTime());

        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, true);
        }

        // Update aspect ratio after resizing
        glfwGetFramebufferSize(window, &width, &height);

        if (width > 0 && height > 0)
        {
            glViewport(0, 0, width, height);

            aspectRatio =
                static_cast<float>(width) /
                static_cast<float>(height);

            createProjection(projection, aspectRatio);
        }

        // Clear screen and depth buffer
        glClearColor(0.035f, 0.045f, 0.085f, 1.0f);

        glClear(
            GL_COLOR_BUFFER_BIT |
            GL_DEPTH_BUFFER_BIT
        );

        // Render cube
        glUseProgram(shaderProgram);

        glUniform1f(
            rotationLocation,
            currentFrame * 0.8f
        );

        glUniformMatrix4fv(
            projectionLocation,
            1,
            GL_FALSE,
            projection
        );

        glBindVertexArray(VAO);

        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);

        // FPS calculation
        frameCount++;
        fpsTimer += deltaTime;

        if (fpsTimer >= 1.0)
        {
            double fps = frameCount / fpsTimer;

            double frameTime =
                fps > 0.0 ? 1000.0 / fps : 0.0;

            std::ostringstream title;

            title << "Horizon Engine 0.4 | FPS: "
                  << std::fixed
                  << std::setprecision(1)
                  << fps
                  << " | Frame: "
                  << frameTime << " ms";

            glfwSetWindowTitle(
                window,
                title.str().c_str()
            );

            frameCount = 0;
            fpsTimer = 0.0;
        }
    }

    // ====================================
    // CLEANUP
    // ====================================

    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();

    std::cout << "Horizon Engine shut down successfully!\n";

    return 0;
}
