
#include "horizon/Window.hpp"

#include <GLFW/glfw3.h>
#include <iostream>

namespace horizon
{
    Window::Window()
        : handle(nullptr),
          glfwInitialized(false)
    {
    }

    Window::~Window()
    {
        if (handle != nullptr)
        {
            glfwDestroyWindow(handle);
            handle = nullptr;
        }

        if (glfwInitialized)
        {
            glfwTerminate();
            glfwInitialized = false;
        }
    }

    bool Window::create(int width, int height, const std::string& title)
    {
        if (handle != nullptr)
        {
            std::cerr << "Window already exists!\n";
            return false;
        }

        if (glfwInit() != GLFW_TRUE)
        {
            std::cerr << "Failed to initialize GLFW!\n";
            return false;
        }

        glfwInitialized = true;

        // Request an OpenGL 3.3 Core Profile context.
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        handle = glfwCreateWindow(
            width,
            height,
            title.c_str(),
            nullptr,
            nullptr
        );

        if (handle == nullptr)
        {
            std::cerr << "Failed to create Horizon window!\n";
            glfwTerminate();
            glfwInitialized = false;
            return false;
        }

        // Make this window's OpenGL context current.
        glfwMakeContextCurrent(handle);

        // Enable VSync.
        glfwSwapInterval(1);

        std::cout << "Horizon Window created successfully!\n";

        return true;
    }

    void Window::pollEvents() const
    {
        glfwPollEvents();
    }

    void Window::swapBuffers() const
    {
        if (handle != nullptr)
        {
            glfwSwapBuffers(handle);
        }
    }

    bool Window::shouldClose() const
    {
        if (handle == nullptr)
        {
            return true;
        }

        return glfwWindowShouldClose(handle);
    }

    GLFWwindow* Window::nativeHandle() const
    {
        return handle;
    }

    int Window::width() const
    {
        if (handle == nullptr)
        {
            return 0;
        }

        int windowWidth = 0;
        glfwGetWindowSize(handle, &windowWidth, nullptr);

        return windowWidth;
    }

    int Window::height() const
    {
        if (handle == nullptr)
        {
            return 0;
        }

        int windowHeight = 0;
        glfwGetWindowSize(handle, nullptr, &windowHeight);

        return windowHeight;
    }
}
