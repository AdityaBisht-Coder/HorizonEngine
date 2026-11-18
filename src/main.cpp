
#include "horizon/Engine.hpp"

#include <iostream>

int main()
{
    std::cout << "================================\n";
    std::cout << "       HORIZON ENGINE\n";
    std::cout << "================================\n";

    horizon::Engine engine;

    if (!engine.initialize(
        1280,
        720,
        "Horizon Engine 0.14.5"
    ))
    {
        std::cerr << "Horizon Engine failed to initialize!\n";
        return 1;
    }

    engine.run();

    return 0;
}
