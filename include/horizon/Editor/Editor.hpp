
#pragma once

#include "horizon/Scene/World.hpp"

#include <memory>
#include <string>
#include <cstdint>

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

        // Stable selection using GameObject IDs.
        std::uint64_t selectedObjectId = 0;

        // Shared cube mesh used by editor-created objects.
        std::shared_ptr<Mesh> cubeMesh;
    };
}
