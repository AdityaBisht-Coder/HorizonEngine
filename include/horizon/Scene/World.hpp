
#pragma once

#include "horizon/Scene/GameObject.hpp"

#include <memory>
#include <string>
#include <vector>

namespace horizon
{
    class World
    {
    public:
        World() = default;
        ~World() = default;

        World(const World&) = delete;
        World& operator=(const World&) = delete;

        // Create and add an object to the world.
        GameObject& createObject(const std::string& name);

        // Remove an object by its name.
        bool removeObject(const std::string& name);

        // Remove every object.
        void clear();

        // Access all objects.
        const std::vector<std::unique_ptr<GameObject>>&
            getObjects() const;

        // Get the number of objects.
        std::size_t getObjectCount() const;

    private:
        std::vector<std::unique_ptr<GameObject>> objects;
    };
}
