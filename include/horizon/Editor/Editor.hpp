#pragma once

#include "horizon/Scene/World.hpp"

#include <memory>
#include <string>

namespace horizon
{
    class Mesh;

    class Editor
    {
    public:
        Editor() = default;
        ~Editor() = default;

        Editor(const Editor&) = delete;
        Editor& operator=(const Editor&) = delete;

        // Draws the editor UI and applies edits directly to the World.
        void render(World& world);

    private:
        void createCube(World& world);

        std::string selectedObjectName;
        std::shared_ptr<Mesh> cubeMesh;
    };
}
