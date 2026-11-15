
#include "horizon/Window.hpp"
#include "horizon/Shader.hpp"

#include <glad/gl.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <iostream>

int main()
{
    horizon::Window window;

    if (!window.create(1280, 720, "Horizon Engine - Shader Test"))
    {
        return 1;
    }

    // Load OpenGL functions after creating the context.
    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD!\n";
        return 1;
    }

    const std::string vertexShader = R"(
        #version 330 core

        void main()
        {
            gl_Position = vec4(0.0, 0.0, 0.0, 1.0);
        }
    )";

    const std::string fragmentShader = R"(
        #version 330 core

        uniform vec3 tint;
        out vec4 FragColor;

        void main()
        {
            FragColor = vec4(tint, 1.0);
        }
    )";

    horizon::Shader shader;

    if (!shader.create(vertexShader, fragmentShader))
    {
        std::cerr << "Shader test failed!\n";
        return 1;
    }

    shader.use();
    shader.setVec3("tint", 0.1f, 0.4f, 1.0f);

    std::cout << "Shader program ID: " << shader.id() << '\n';
    std::cout << "Horizon Shader Test passed!\n";

    while (!window.shouldClose())
    {
        window.pollEvents();

        glClearColor(0.03f, 0.03f, 0.08f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        window.swapBuffers();
    }

    return 0;
}
