
#include "horizon/Engine.hpp"

#include <glad/gl.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <algorithm>
#include <chrono>
#include <iostream>

namespace horizon
{
    Engine::Engine()
        : running(false),
          initialized(false),
          previousTabPressed(false)
    {
    }

    Engine::~Engine()
    {
        shutdown();
    }

    bool Engine::initialize(
        int width,
        int height,
        const std::string& title)
    {
        if (initialized)
        {
            return true;
        }

        std::cout << "Initializing Horizon Engine 0.10...\n";

        // 1. Create the window and OpenGL context.
        if (!window.create(width, height, title))
        {
            std::cerr << "Engine: Window initialization failed!\n";
            return false;
        }

        // 2. Initialize OpenGL functions.
        if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress))
        {
            std::cerr << "Engine: GLAD initialization failed!\n";
            return false;
        }

        std::cout << "OpenGL: "
                  << glGetString(GL_VERSION)
                  << '\n';

        // 3. Initialize input.
        if (!input.initialize(window.nativeHandle()))
        {
            std::cerr << "Engine: Input initialization failed!\n";
            return false;
        }

        // Start with the mouse captured.
        input.setCursorCaptured(true);

        // 4. Create objects in the World.

        // CENTER CUBE
        GameObject& centerCube =
            world.createObject("CenterCube");

        centerCube.getTransform().position =
            Vec3(0.0f, 0.0f, 0.0f);

        centerCube.getTransform().scale =
            Vec3(1.0f, 1.0f, 1.0f);

        centerCube.getMaterial().color =
            Vec3(0.15f, 0.55f, 1.0f);

        // LEFT CUBE
        GameObject& leftCube =
            world.createObject("LeftCube");

        leftCube.getTransform().position =
            Vec3(-1.8f, 0.0f, 0.0f);

        leftCube.getTransform().scale =
            Vec3(0.8f, 0.8f, 0.8f);

        leftCube.getTransform().rotation =
            Vec3(0.0f, 25.0f, 0.0f);

        leftCube.getMaterial().color =
            Vec3(1.0f, 0.15f, 0.15f);

        // RIGHT CUBE
        GameObject& rightCube =
            world.createObject("RightCube");

        rightCube.getTransform().position =
            Vec3(1.8f, 0.0f, 0.0f);

        rightCube.getTransform().scale =
            Vec3(1.2f, 1.2f, 1.2f);

        rightCube.getTransform().rotation =
            Vec3(0.0f, -25.0f, 0.0f);

        rightCube.getMaterial().color =
            Vec3(0.15f, 1.0f, 0.25f);

        std::cout
            << "World initialized with "
            << world.getObjectCount()
            << " GameObjects.\n";

        std::cout << "Material System initialized!\n";

        // 5. Initialize renderer.
        if (!renderer.create())
        {
            std::cerr << "Engine: Renderer initialization failed!\n";
            return false;
        }

        initialized = true;
        running = true;

        std::cout << "\nHorizon Engine 0.10 initialized successfully!\n";

        return true;
    }

    void Engine::run()
    {
        if (!initialized)
        {
            std::cerr << "Engine must be initialized before running!\n";
            return;
        }

        std::cout << "\n================================\n";
        std::cout << "     HORIZON ENGINE 0.10\n";
        std::cout << "================================\n";
        std::cout << "WASD       - Move camera\n";
        std::cout << "Mouse      - Look around\n";
        std::cout << "Scroll     - Adjust FOV\n";
        std::cout << "TAB        - Capture/release mouse\n";
        std::cout << "ESC        - Exit\n";
        std::cout << "================================\n\n";

        using Clock = std::chrono::steady_clock;

        auto previousFrameTime = Clock::now();

        while (running && !window.shouldClose())
        {
            // Calculate frame delta time.
            const auto currentFrameTime = Clock::now();

            float deltaTime =
                std::chrono::duration<float>(
                    currentFrameTime - previousFrameTime
                ).count();

            previousFrameTime = currentFrameTime;

            // Avoid large movement after a pause or window drag.
            deltaTime = std::min(deltaTime, 0.1f);

            // 1. Process window events.
            window.pollEvents();

            // 2. Update input state.
            input.update();

            // 3. Handle exit.
            if (input.isKeyPressed(GLFW_KEY_ESCAPE))
            {
                running = false;
                break;
            }

            // 4. Toggle mouse capture with TAB.
            const bool tabPressed =
                input.isKeyPressed(GLFW_KEY_TAB);

            if (tabPressed && !previousTabPressed)
            {
                input.setCursorCaptured(
                    !input.isCursorCaptured()
                );
            }

            previousTabPressed = tabPressed;

            // 5. Camera movement.
            if (input.isKeyPressed(GLFW_KEY_W))
            {
                camera.processKeyboard(
                    CameraDirection::Forward,
                    deltaTime
                );
            }

            if (input.isKeyPressed(GLFW_KEY_S))
            {
                camera.processKeyboard(
                    CameraDirection::Backward,
                    deltaTime
                );
            }

            if (input.isKeyPressed(GLFW_KEY_A))
            {
                camera.processKeyboard(
                    CameraDirection::Left,
                    deltaTime
                );
            }

            if (input.isKeyPressed(GLFW_KEY_D))
            {
                camera.processKeyboard(
                    CameraDirection::Right,
                    deltaTime
                );
            }

            // 6. Mouse look.
            if (input.isCursorCaptured())
            {
                camera.processMouseMovement(
                    input.mouseDeltaX(),
                    input.mouseDeltaY()
                );
            }

            // 7. Camera zoom.
            const float scroll = input.consumeScrollDeltaY();

            if (scroll != 0.0f)
            {
                camera.processMouseScroll(scroll);
            }

            // 8. Render all objects in the World.
            renderer.render(
                camera,
                world,
                window.width(),
                window.height()
            );

            // 9. Present the frame.
            window.swapBuffers();
        }

        std::cout << "\nHorizon Engine main loop stopped.\n";

        shutdown();
    }

    void Engine::shutdown()
    {
        if (!initialized)
        {
            return;
        }

        std::cout << "Shutting down Horizon Engine...\n";

        running = false;

        // Release OpenGL resources before destroying the context.
        renderer.destroy();

        // Remove all objects from the World.
        world.clear();

        initialized = false;

        std::cout << "Horizon Engine shutdown complete!\n";
    }

    bool Engine::isRunning() const
    {
        return running;
    }
}
