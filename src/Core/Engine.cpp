#include "horizon/Engine.hpp"

#include <glad/gl.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cfloat>
#include <cstdio>
#include <iostream>
#include <memory>

namespace horizon
{
    Engine::Engine()
        : running(false),
          initialized(false),
          imguiInitialized(false),
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

        std::cout << "Initializing Horizon Engine 0.14.5...\n";

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

        // 3. Initialize the input system.
        if (!input.initialize(window.nativeHandle()))
        {
            std::cerr << "Engine: Input initialization failed!\n";
            return false;
        }

        // 4. Initialize Dear ImGui.
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();
        io.IniFilename = nullptr;

        ImGui::StyleColorsDark();

        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowRounding = 0.0f;
        style.FrameRounding = 5.0f;
        style.GrabRounding = 5.0f;

        // Give the editor a restrained Horizon-blue accent.
        style.Colors[ImGuiCol_CheckMark] =
            ImVec4(0.15f, 0.55f, 1.0f, 1.0f);
        style.Colors[ImGuiCol_SliderGrab] =
            ImVec4(0.15f, 0.55f, 1.0f, 1.0f);
        style.Colors[ImGuiCol_Button] =
            ImVec4(0.08f, 0.20f, 0.36f, 1.0f);
        style.Colors[ImGuiCol_ButtonHovered] =
            ImVec4(0.10f, 0.34f, 0.60f, 1.0f);
        style.Colors[ImGuiCol_ButtonActive] =
            ImVec4(0.12f, 0.43f, 0.75f, 1.0f);
        style.Colors[ImGuiCol_Header] =
            ImVec4(0.08f, 0.20f, 0.36f, 1.0f);
        style.Colors[ImGuiCol_HeaderHovered] =
            ImVec4(0.10f, 0.34f, 0.60f, 1.0f);
        style.Colors[ImGuiCol_HeaderActive] =
            ImVec4(0.12f, 0.43f, 0.75f, 1.0f);

        if (!ImGui_ImplGlfw_InitForOpenGL(
                window.nativeHandle(),
                true))
        {
            std::cerr << "Engine: ImGui GLFW initialization failed!\n";
            ImGui::DestroyContext();
            return false;
        }

        if (!ImGui_ImplOpenGL3_Init("#version 330"))
        {
            std::cerr << "Engine: ImGui OpenGL initialization failed!\n";
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();
            return false;
        }

        imguiInitialized = true;

        // Used by every early exit after ImGui has been fully initialized.
        const auto cleanupImGui = [this]()
        {
            if (imguiInitialized)
            {
                ImGui_ImplOpenGL3_Shutdown();
                ImGui_ImplGlfw_Shutdown();
                ImGui::DestroyContext();
                imguiInitialized = false;
            }
        };

        std::cout << "Horizon Boot UI initialized!\n";

        // Draw one splash-screen stage.
        // Each stage remains visible briefly so messages can be read.
        const auto showSplashStage =
            [this](float progress, const char* message, float duration)
        {
            using Clock = std::chrono::steady_clock;

            const auto start = Clock::now();

            while (std::chrono::duration<float>(
                       Clock::now() - start).count() < duration)
            {
                if (window.shouldClose())
                {
                    return false;
                }

                window.pollEvents();

                glViewport(0, 0, window.width(), window.height());

                glClearColor(
                    0.025f,
                    0.035f,
                    0.075f,
                    1.0f
                );

                glClear(GL_COLOR_BUFFER_BIT);

                ImGui_ImplOpenGL3_NewFrame();
                ImGui_ImplGlfw_NewFrame();
                ImGui::NewFrame();

                ImGuiIO& frameIO = ImGui::GetIO();

                const float screenWidth = frameIO.DisplaySize.x;
                const float screenHeight = frameIO.DisplaySize.y;

                ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
                ImGui::SetNextWindowSize(frameIO.DisplaySize);

                ImGui::Begin(
                    "HorizonBootScreen",
                    nullptr,
                    ImGuiWindowFlags_NoDecoration |
                    ImGuiWindowFlags_NoMove |
                    ImGuiWindowFlags_NoResize |
                    ImGuiWindowFlags_NoSavedSettings |
                    ImGuiWindowFlags_NoBackground
                );

                ImDrawList* draw = ImGui::GetWindowDrawList();

                const ImVec2 center(
                    screenWidth * 0.5f,
                    screenHeight * 0.38f
                );

                const float animationTime =
                    static_cast<float>(ImGui::GetTime());

                const float pulse =
                    0.75f + 0.25f *
                    std::sin(animationTime * 2.0f);

                // Background.
                draw->AddRectFilled(
                    ImVec2(0.0f, 0.0f),
                    ImVec2(screenWidth, screenHeight),
                    IM_COL32(6, 10, 23, 255)
                );

                // Animated Horizon logo.
                const char* logo = "HORIZON";
                const char* subtitle = "ENGINE";

                ImFont* font = ImGui::GetFont();

                const ImVec2 logoSize =
                    font->CalcTextSizeA(
                        44.0f,
                        FLT_MAX,
                        0.0f,
                        logo
                    );

                draw->AddText(
                    font,
                    44.0f,
                    ImVec2(
                        center.x - logoSize.x * 0.5f,
                        center.y - 45.0f
                    ),
                    IM_COL32(
                        235,
                        244,
                        255,
                        static_cast<int>(255.0f * pulse)
                    ),
                    logo
                );

                const ImVec2 subtitleSize =
                    font->CalcTextSizeA(
                        20.0f,
                        FLT_MAX,
                        0.0f,
                        subtitle
                    );

                draw->AddText(
                    font,
                    20.0f,
                    ImVec2(
                        center.x - subtitleSize.x * 0.5f,
                        center.y + 12.0f
                    ),
                    IM_COL32(45, 155, 255, 255),
                    subtitle
                );

                // Animated blue accent line.
                const float lineWidth =
                    110.0f + 30.0f *
                    std::sin(animationTime * 2.5f);

                draw->AddLine(
                    ImVec2(center.x - lineWidth, center.y + 49.0f),
                    ImVec2(center.x + lineWidth, center.y + 49.0f),
                    IM_COL32(35, 145, 255, 230),
                    2.0f
                );

                // Loading section.
                const float barWidth =
                    std::min(480.0f, screenWidth * 0.72f);

                const float barHeight = 8.0f;

                const ImVec2 barStart(
                    center.x - barWidth * 0.5f,
                    screenHeight * 0.70f
                );

                const ImVec2 barEnd(
                    center.x + barWidth * 0.5f,
                    barStart.y + barHeight
                );

                draw->AddText(
                    ImVec2(barStart.x, barStart.y - 43.0f),
                    IM_COL32(210, 225, 245, 255),
                    "INITIALIZING ENGINE SYSTEMS"
                );

                draw->AddRectFilled(
                    barStart,
                    barEnd,
                    IM_COL32(30, 43, 65, 255),
                    5.0f
                );

                draw->AddRectFilled(
                    barStart,
                    ImVec2(
                        barStart.x + barWidth *
                            std::clamp(progress, 0.0f, 1.0f),
                        barEnd.y
                    ),
                    IM_COL32(35, 145, 255, 255),
                    5.0f
                );

                // Current initialization message.
                draw->AddText(
                    ImVec2(barStart.x, barStart.y + 22.0f),
                    IM_COL32(145, 170, 205, 255),
                    message
                );

                // Percentage.
                char percentageText[32];

                std::snprintf(
                    percentageText,
                    sizeof(percentageText),
                    "%.0f%%",
                    progress * 100.0f
                );

                const ImVec2 percentageSize =
                    ImGui::CalcTextSize(percentageText);

                draw->AddText(
                    ImVec2(
                        barEnd.x - percentageSize.x,
                        barStart.y + 22.0f
                    ),
                    IM_COL32(80, 175, 255, 255),
                    percentageText
                );

                // Footer.
                const char* footer =
                    "Horizon Engine 0.14.5  |  Windows";

                const ImVec2 footerSize =
                    ImGui::CalcTextSize(footer);

                draw->AddText(
                    ImVec2(
                        center.x - footerSize.x * 0.5f,
                        screenHeight - 35.0f
                    ),
                    IM_COL32(90, 110, 140, 255),
                    footer
                );

                ImGui::End();

                ImGui::Render();

                ImGui_ImplOpenGL3_RenderDrawData(
                    ImGui::GetDrawData()
                );

                window.swapBuffers();
            }

            return true;
        };

        // 5. Show the first boot stage.
        if (!showSplashStage(
                0.10f,
                "OpenGL context established...",
                0.30f))
        {
            cleanupImGui();
            return false;
        }

        // 6. Input system is ready.
        std::cout << "Input System initialized!\n";

        if (!showSplashStage(
                0.25f,
                "Initializing input system...",
                0.30f))
        {
            cleanupImGui();
            return false;
        }

        // 7. Create objects in the World.
        world.clear();

        GameObject& centerCube =
            world.createObject("CenterCube");

        centerCube.getTransform().position =
            Vec3(0.0f, 0.0f, 0.0f);

        centerCube.getTransform().scale =
            Vec3(1.0f, 1.0f, 1.0f);

        centerCube.getMaterial().color =
            Vec3(0.15f, 0.55f, 1.0f);

        GameObject& leftCube =
            world.createObject("LeftCube");

        leftCube.getTransform().position =
            Vec3(-1.8f, 0.0f, 0.0f);

        leftCube.getTransform().scale =
            Vec3(0.8f, 0.8f, 0.8f);

        leftCube.getTransform().rotation =
            Vec3(0.0f, 25.0f, 0.0f);

        leftCube.getMaterial().color =
            Vec3(1.0f, 0.08f, 0.08f);

        GameObject& rightCube =
            world.createObject("RightCube");

        rightCube.getTransform().position =
            Vec3(1.8f, 0.0f, 0.0f);

        rightCube.getTransform().scale =
            Vec3(1.2f, 1.2f, 1.2f);

        rightCube.getTransform().rotation =
            Vec3(0.0f, -25.0f, 0.0f);

        rightCube.getMaterial().color =
            Vec3(0.08f, 1.0f, 0.15f);

        std::cout
            << "World initialized with "
            << world.getObjectCount()
            << " GameObjects.\n";

        if (!showSplashStage(
                0.43f,
                "Creating world and GameObjects...",
                0.30f))
        {
            cleanupImGui();
            world.clear();
            return false;
        }

        // 8. Create one reusable cube Mesh.
        std::shared_ptr<Mesh> cubeMesh =
            Mesh::createCube();

        if (!cubeMesh)
        {
            std::cerr << "Engine: Cube Mesh creation failed!\n";

            cleanupImGui();
            world.clear();
            return false;
        }

        centerCube.setMesh(cubeMesh);
        leftCube.setMesh(cubeMesh);
        rightCube.setMesh(cubeMesh);

        std::cout << "Mesh System initialized!\n";
        std::cout << "Material System initialized!\n";

        if (!showSplashStage(
                0.65f,
                "Loading meshes and materials...",
                0.30f))
        {
            cleanupImGui();
            world.clear();
            return false;
        }

        // 9. Initialize the renderer.
        if (!renderer.create())
        {
            std::cerr << "Engine: Renderer initialization failed!\n";

            cleanupImGui();
            world.clear();
            return false;
        }

        if (!showSplashStage(
                0.88f,
                "Initializing Horizon Renderer...",
                0.35f))
        {
            renderer.destroy();
            cleanupImGui();
            world.clear();
            return false;
        }

        // 10. Engine initialization complete.
        if (!showSplashStage(
                1.0f,
                "HORIZON ENGINE READY!",
                0.85f))
        {
            renderer.destroy();
            cleanupImGui();
            world.clear();
            return false;
        }

        // Keep ImGui alive. It now transitions from the boot screen
        // into the interactive Horizon Editor.
        input.setCursorCaptured(false);

        initialized = true;
        running = true;

        std::cout << "\nHorizon Engine 0.14.5 initialized successfully!\n";
        std::cout << "Horizon Editor initialized!\n";

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
        std::cout << "     HORIZON ENGINE 0.14.5\n";
        std::cout << "================================\n";
        std::cout << "WASD       - Move camera (when captured)\n";
        std::cout << "Mouse      - Look around (when captured)\n";
        std::cout << "Scroll     - Adjust FOV (when captured)\n";
        std::cout << "TAB        - Capture/release mouse\n";
        std::cout << "ESC        - Exit\n";
        std::cout << "================================\n\n";

        using Clock = std::chrono::steady_clock;

        auto previousFrameTime = Clock::now();

        while (running && !window.shouldClose())
        {
            const auto currentFrameTime = Clock::now();

            float deltaTime =
                std::chrono::duration<float>(
                    currentFrameTime - previousFrameTime
                ).count();

            previousFrameTime = currentFrameTime;
            deltaTime = std::min(deltaTime, 0.1f);

            window.pollEvents();
            input.update();

            if (input.isKeyPressed(GLFW_KEY_ESCAPE))
            {
                running = false;
                break;
            }

            const bool tabPressed =
                input.isKeyPressed(GLFW_KEY_TAB);

            if (tabPressed && !previousTabPressed)
            {
                input.setCursorCaptured(
                    !input.isCursorCaptured()
                );
            }

            previousTabPressed = tabPressed;

            // Camera controls are active only while the cursor is captured.
            if (input.isCursorCaptured())
            {
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

                camera.processMouseMovement(
                    input.mouseDeltaX(),
                    input.mouseDeltaY()
                );

                const float scroll =
                    input.consumeScrollDeltaY();

                if (scroll != 0.0f)
                {
                    camera.processMouseScroll(scroll);
                }
            }
            else
            {
                // Discard editor scroll input so it is not applied to the
                // camera after the user captures the cursor again.
                input.consumeScrollDeltaY();
            }

            // Start the ImGui frame and draw the editor.
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            editor.render(world);

            ImGui::Render();

            // Draw the 3D scene first, then composite the editor UI over it.
            renderer.render(
                camera,
                world,
                window.width(),
                window.height()
            );

            ImGui_ImplOpenGL3_RenderDrawData(
                ImGui::GetDrawData()
            );

            window.swapBuffers();
        }

        std::cout << "\nHorizon Engine main loop stopped.\n";

        shutdown();
    }

    void Engine::shutdown()
    {
        if (!initialized && !imguiInitialized)
        {
            return;
        }

        std::cout << "Shutting down Horizon Engine...\n";

        running = false;

        if (initialized)
        {
            renderer.destroy();
            world.clear();
        }

        if (imguiInitialized)
        {
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();
            imguiInitialized = false;
        }

        initialized = false;

        std::cout << "Horizon Engine shutdown complete!\n";
    }

    bool Engine::isRunning() const
    {
        return running;
    }
}
