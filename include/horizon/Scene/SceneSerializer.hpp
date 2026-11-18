
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
    };
}
