
#include "horizon/Window.hpp"
#include "horizon/Renderer.hpp"

#include <glad/gl.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <chrono>
#include <iostream>

int main()
{
    std::cout << "================================\n";
    std::cout << "       HORIZON ENGINE\n";
    std::cout << "================================\n";

    // 1. Create the application window.
    horizon::Window window;

    if (!window.create(1280, 720, "Horizon Engine 0.5"))
    {
        std::cerr << "Window initialization failed!\n";
        return 1;
    }

    // 2. Initialize GLAD after the OpenGL context exists.
    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress))
    {
        std::cerr << "GLAD initialization failed!\n";
        return 1;
    }

    std::cout << "OpenGL: " << glGetString(GL_VERSION) << '\n';

    // 3. Initialize the Renderer.
    horizon::Renderer renderer;

    if (!renderer.create())
    {
        std::cerr << "Renderer initialization failed!\n";
        return 1;
    }

    std::cout << "Horizon Engine 0.5 is running!\n";
    std::cout << "Press ESC to exit.\n";

    const auto startTime = std::chrono::steady_clock::now();

    // 4. Main application loop.
    while (!window.shouldClose())
    {
        window.pollEvents();

        if (glfwGetKey(window.nativeHandle(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window.nativeHandle(), GLFW_TRUE);
        }

        const auto currentTime = std::chrono::steady_clock::now();

        const float elapsedTime =
            std::chrono::duration<float>(currentTime - startTime).count();

        renderer.render(
            elapsedTime,
            window.width(),
            window.height()
        );

        window.swapBuffers();
    }

    std::cout << "Horizon Engine shutting down...\n";

    // Renderer is destroyed before Window, while OpenGL is still available.
    return 0;
}
