
#pragma once

#include "horizon/Window.hpp"
#include "horizon/Renderer.hpp"

#include <string>

namespace horizon
{
    class Engine
    {
    public:
        Engine();
        ~Engine();

        Engine(const Engine&) = delete;
        Engine& operator=(const Engine&) = delete;

        bool initialize(
            int width,
            int height,
            const std::string& title
        );

        void run();
        void shutdown();

        bool isRunning() const;

    private:
        // Declaration order matters:
        // Renderer is destroyed before Window.
        Window window;
        Renderer renderer;

        bool running;
        bool initialized;
    };
}
