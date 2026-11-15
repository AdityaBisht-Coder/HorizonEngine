
#pragma once

struct GLFWwindow;

namespace horizon
{
    class Input
    {
    public:
        Input();

        bool initialize(GLFWwindow* window);

        void update();

        bool isKeyPressed(int key) const;
        bool isMouseButtonPressed(int button) const;

        float mouseDeltaX() const;
        float mouseDeltaY() const;

        void setCursorCaptured(bool captured);
        bool isCursorCaptured() const;

    private:
        GLFWwindow* window;

        double previousMouseX;
        double previousMouseY;

        float deltaX;
        float deltaY;

        bool firstMouse;
        bool cursorCaptured;
        bool initialized;
    };
}
