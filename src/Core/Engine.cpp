
#include "horizon/Engine.hpp"

#include <glad/gl.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <chrono>
#include <iostream>

namespace horizon
{
    Engine::Engine()
        : running(false),
          initialized(false)
    {
    }

    Engine::~Engine()
    {
        shutdown();
    }

    bool Engine::initialize(
        int width,
        int height,
        const std::string& title)
    {
        if (initialized)
        {
            return true;
        }

        std::cout << "Initializing Horizon Engine...\n";

        // 1. Initialize the window.
        if (!window.create(width, height, title))
        {
            std::cerr << "Engine: Window initialization failed!\n";
            return false;
        }

        // 2. Initialize OpenGL functions.
        if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress))
        {
            std::cerr << "Engine: GLAD initialization failed!\n";
            return false;
        }

        std::cout << "OpenGL: "
                  << glGetString(GL_VERSION)
                  << '\n';

        // 3. Initialize the renderer.
        if (!renderer.create())
        {
            std::cerr << "Engine: Renderer initialization failed!\n";
            return false;
        }

        initialized = true;
        running = true;

        std::cout << "Horizon Engine initialized successfully!\n";

        return true;
    }

    void Engine::run()
    {
        if (!initialized)
        {
            std::cerr << "Engine must be initialized before running!\n";
            return;
        }

        std::cout << "Horizon Engine main loop started!\n";
        std::cout << "Press ESC to exit.\n";

        const auto startTime = std::chrono::steady_clock::now();

        while (running && !window.shouldClose())
        {
            // Process operating system events.
            window.pollEvents();

            // Temporary ESC input handling.
            if (glfwGetKey(
                window.nativeHandle(),
                GLFW_KEY_ESCAPE
            ) == GLFW_PRESS)
            {
                running = false;
            }

            const auto currentTime =
                std::chrono::steady_clock::now();

            const float elapsedTime =
                std::chrono::duration<float>(
                    currentTime - startTime
                ).count();

            // Render the current frame.
            renderer.render(
                elapsedTime,
                window.width(),
                window.height()
            );

            // Present the rendered frame.
            window.swapBuffers();
        }

        std::cout << "Horizon Engine main loop stopped.\n";

        shutdown();
    }

    void Engine::shutdown()
    {
        if (!initialized)
        {
            return;
        }

        std::cout << "Shutting down Horizon Engine...\n";

        running = false;

        // Release GPU resources while the OpenGL context exists.
        renderer.destroy();

        initialized = false;

        std::cout << "Horizon Engine shutdown complete!\n";
    }

    bool Engine::isRunning() const
    {
        return running;
    }
}
