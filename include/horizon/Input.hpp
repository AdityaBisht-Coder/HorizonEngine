
#pragma once

struct GLFWwindow;

namespace horizon
{
    class Input
    {
    public:
        Input();
        ~Input();

        bool initialize(GLFWwindow* window);

        void update();

        bool isKeyPressed(int key) const;
        bool isMouseButtonPressed(int button) const;

        float mouseDeltaX() const;
        float mouseDeltaY() const;

        // Returns accumulated scroll movement and resets it.
        float consumeScrollDeltaY();

        void setCursorCaptured(bool captured);
        bool isCursorCaptured() const;

    private:
        static void scrollCallback(
            GLFWwindow* window,
            double xOffset,
            double yOffset
        );

        GLFWwindow* window;

        double previousMouseX;
        double previousMouseY;

        float deltaX;
        float deltaY;
        float scrollDeltaY;

        bool firstMouse;
        bool cursorCaptured;
        bool initialized;
    };
}
