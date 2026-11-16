
#include "horizon/Input.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace horizon
{
    Input::Input()
        : window(nullptr),
          previousMouseX(0.0),
          previousMouseY(0.0),
          deltaX(0.0f),
          deltaY(0.0f),
          scrollDeltaY(0.0f),
          firstMouse(true),
          cursorCaptured(false),
          initialized(false)
    {
    }

    Input::~Input()
    {
        if (window != nullptr)
        {
            if (glfwGetWindowUserPointer(window) == this)
            {
                glfwSetScrollCallback(window, nullptr);
                glfwSetWindowUserPointer(window, nullptr);
            }
        }
    }

    bool Input::initialize(GLFWwindow* windowHandle)
    {
        if (windowHandle == nullptr)
        {
            return false;
        }

        window = windowHandle;

        glfwGetCursorPos(
            window,
            &previousMouseX,
            &previousMouseY
        );

        firstMouse = true;
        deltaX = 0.0f;
        deltaY = 0.0f;
        scrollDeltaY = 0.0f;

        glfwSetWindowUserPointer(window, this);
        glfwSetScrollCallback(window, scrollCallback);

        initialized = true;

        return true;
    }

    void Input::update()
    {
        if (!initialized || window == nullptr)
        {
            return;
        }

        double currentMouseX = 0.0;
        double currentMouseY = 0.0;

        glfwGetCursorPos(
            window,
            &currentMouseX,
            &currentMouseY
        );

        if (firstMouse)
        {
            previousMouseX = currentMouseX;
            previousMouseY = currentMouseY;
            firstMouse = false;
        }

        deltaX = static_cast<float>(
            currentMouseX - previousMouseX
        );

        deltaY = static_cast<float>(
            previousMouseY - currentMouseY
        );

        previousMouseX = currentMouseX;
        previousMouseY = currentMouseY;
    }

    void Input::scrollCallback(
        GLFWwindow* window,
        double xOffset,
        double yOffset
    )
    {
        (void)xOffset;

        Input* input = static_cast<Input*>(
            glfwGetWindowUserPointer(window)
        );

        if (input != nullptr)
        {
            input->scrollDeltaY += static_cast<float>(yOffset);
        }
    }

    float Input::consumeScrollDeltaY()
    {
        float result = scrollDeltaY;
        scrollDeltaY = 0.0f;

        return result;
    }

    bool Input::isKeyPressed(int key) const
    {
        if (!initialized || window == nullptr)
        {
            return false;
        }

        return glfwGetKey(window, key) == GLFW_PRESS;
    }

    bool Input::isMouseButtonPressed(int button) const
    {
        if (!initialized || window == nullptr)
        {
            return false;
        }

        return glfwGetMouseButton(window, button) == GLFW_PRESS;
    }

    float Input::mouseDeltaX() const
    {
        return deltaX;
    }

    float Input::mouseDeltaY() const
    {
        return deltaY;
    }

    void Input::setCursorCaptured(bool captured)
    {
        if (!initialized || window == nullptr)
        {
            return;
        }

        cursorCaptured = captured;

        glfwSetInputMode(
            window,
            GLFW_CURSOR,
            captured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL
        );

        glfwGetCursorPos(
            window,
            &previousMouseX,
            &previousMouseY
        );

        firstMouse = true;
        deltaX = 0.0f;
        deltaY = 0.0f;
    }

    bool Input::isCursorCaptured() const
    {
        return cursorCaptured;
    }
}
