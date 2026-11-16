#pragma once

#include "horizon/Window.hpp"
#include "horizon/Input.hpp"
#include "horizon/Camera.hpp"
#include "horizon/Scene/World.hpp"
#include "horizon/Renderer.hpp"
#include "horizon/Editor/Editor.hpp"

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
        // Declaration order ensures correct reverse destruction.
        Window window;
        Input input;
        Camera camera;
        World world;
        Renderer renderer;
        Editor editor;

        bool running;
        bool initialized;
        bool imguiInitialized;
        bool previousTabPressed;
    };
}
