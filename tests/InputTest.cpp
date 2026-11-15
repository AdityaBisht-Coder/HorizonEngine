
#include "horizon/Window.hpp"
#include "horizon/Input.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <iostream>

int main()
{
    horizon::Window window;

    if (!window.create(1280, 720, "Horizon Engine - Input Test"))
    {
        return 1;
    }

    horizon::Input input;

    if (!input.initialize(window.nativeHandle()))
    {
        std::cerr << "Input initialization failed!\n";
        return 1;
    }

    std::cout << "Horizon Input System initialized!\n";
    std::cout << "Press W, A, S, D to test keyboard input.\n";
    std::cout << "Press C to capture or release the mouse.\n";
    std::cout << "Move the mouse to test mouse tracking.\n";
    std::cout << "Press ESC to exit.\n";

    bool previousW = false;
    bool previousA = false;
    bool previousS = false;
    bool previousD = false;
    bool previousC = false;

    while (!window.shouldClose())
    {
        window.pollEvents();
        input.update();

        const bool w = input.isKeyPressed(GLFW_KEY_W);
        const bool a = input.isKeyPressed(GLFW_KEY_A);
        const bool s = input.isKeyPressed(GLFW_KEY_S);
        const bool d = input.isKeyPressed(GLFW_KEY_D);
        const bool c = input.isKeyPressed(GLFW_KEY_C);

        if (w && !previousW)
            std::cout << "W pressed\n";

        if (a && !previousA)
            std::cout << "A pressed\n";

        if (s && !previousS)
            std::cout << "S pressed\n";

        if (d && !previousD)
            std::cout << "D pressed\n";

        if (c && !previousC)
        {
            input.setCursorCaptured(!input.isCursorCaptured());

            std::cout << "Mouse capture: "
                      << (input.isCursorCaptured() ? "ON" : "OFF")
                      << '\n';
        }

        if (input.isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
{
    // Left mouse button is pressed.
}

if (input.mouseDeltaX() != 0.0f ||
    input.mouseDeltaY() != 0.0f)
{
    std::cout << "Mouse movement: X = "
              << input.mouseDeltaX()
              << ", Y = "
              << input.mouseDeltaY()
              << '\n';
}

        if (glfwGetKey(window.nativeHandle(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window.nativeHandle(), GLFW_TRUE);
        }

        previousW = w;
        previousA = a;
        previousS = s;
        previousD = d;
        previousC = c;

        window.swapBuffers();
    }

    std::cout << "Horizon Input Test closed successfully!\n";

    return 0;
}
