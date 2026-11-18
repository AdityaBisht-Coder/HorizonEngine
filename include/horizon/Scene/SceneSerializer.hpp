
#pragma once

#include "horizon/Scene/World.hpp"

#include <string>

namespace horizon
{
    class SceneSerializer
    {
    public:
        // Save a World to a Horizon scene file.
        static bool save(
            const World& world,
            const std::string& filepath,
            std::string* errorMessage = nullptr
        );

        // Load a scene file into a World.
        // The existing World remains unchanged if loading fails.
        static bool load(
            const std::string& filepath,
            World& world,
            std::string* errorMessage = nullptr
        );
    };
}
