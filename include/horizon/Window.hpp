
#pragma once

#include <string>

// Forward declaration: GLFW is used internally.
struct GLFWwindow;

namespace horizon
{
    class Window
    {
    public:
        Window();
        ~Window();

        // A Window cannot be copied.
        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        bool create(int width, int height, const std::string& title);

        void pollEvents() const;
        void swapBuffers() const;

        bool shouldClose() const;
        GLFWwindow* nativeHandle() const;

        int width() const;
        int height() const;

    private:
        GLFWwindow* handle;
        bool glfwInitialized;
    };
}
