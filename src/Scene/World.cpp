#include "horizon/Scene/World.hpp"

#include <algorithm>
#include <limits>
#include <stdexcept>
#include <utility>

namespace horizon
{
    GameObject& World::createObject(const std::string& name)
    {
        if (nextObjectId == 0 ||
            nextObjectId == std::numeric_limits<std::uint64_t>::max())
        {
            throw std::overflow_error(
                "World: GameObject ID limit reached."
            );
        }

        return createObject(name, nextObjectId);
    }

    GameObject& World::createObject(
        const std::string& name,
        std::uint64_t id
    )
    {
        if (id == 0 ||
            id == std::numeric_limits<std::uint64_t>::max())
        {
            throw std::invalid_argument(
                "World: Invalid GameObject ID."
            );
        }

        for (const auto& object : objects)
        {
            if (object && object->getId() == id)
            {
                throw std::invalid_argument(
                    "World: Duplicate GameObject ID."
                );
            }
        }

        auto object = std::make_unique<GameObject>(name, id);

        GameObject& reference = *object;

        objects.push_back(std::move(object));

        nextObjectId = std::max(nextObjectId, id + 1);

        return reference;
    }

    bool World::removeObject(const std::string& name)
    {
        for (auto iterator = objects.begin();
             iterator != objects.end();
             ++iterator)
        {
            if (*iterator && (*iterator)->getName() == name)
            {
                objects.erase(iterator);
                return true;
            }
        }

        return false;
    }

    bool World::removeObject(std::uint64_t id)
    {
        for (auto iterator = objects.begin();
             iterator != objects.end();
             ++iterator)
        {
            if (*iterator && (*iterator)->getId() == id)
            {
                objects.erase(iterator);
                return true;
            }
        }

        return false;
    }

    void World::clear()
    {
        objects.clear();

        nextObjectId = 1;
    }

    const std::vector<std::unique_ptr<GameObject>>&
        World::getObjects() const
    {
        return objects;
    }

    std::size_t World::getObjectCount() const
    {
        return objects.size();
    }
}