
#include "horizon/Window.hpp"

#include <iostream>

int main()
{
    horizon::Window window;

    if (!window.create(1280, 720, "Horizon Engine - Window Test"))
    {
        std::cerr << "Window test failed!\n";
        return 1;
    }

    std::cout << "Window width: " << window.width() << '\n';
    std::cout << "Window height: " << window.height() << '\n';

    while (!window.shouldClose())
    {
        window.pollEvents();
        window.swapBuffers();
    }

    std::cout << "Window closed successfully!\n";

    return 0;
}
