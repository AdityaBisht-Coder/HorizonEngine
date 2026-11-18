#pragma once

#include "horizon/Scene/GameObject.hpp"

#include <memory>
#include <string>
#include <vector>
#include <cstdint>

namespace horizon
{
    class World
    {
    public:
        World() = default;
        ~World() = default;

        World(const World&) = delete;
        World& operator=(const World&) = delete;

        GameObject& createObject(const std::string& name);

        // Create an object with a specific ID.
        // This will be useful when loading saved scenes.
        GameObject& createObject(
            const std::string& name,
            std::uint64_t id
        );

        bool removeObject(const std::string& name);

        bool removeObject(std::uint64_t id);

        void clear();

        const std::vector<std::unique_ptr<GameObject>>&
            getObjects() const;

        std::size_t getObjectCount() const;

    private:
        std::vector<std::unique_ptr<GameObject>> objects;

        std::uint64_t nextObjectId = 1;
    };
}