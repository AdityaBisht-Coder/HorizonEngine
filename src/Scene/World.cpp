#include "horizon/Scene/World.hpp"

#include <utility>

namespace horizon
{
    GameObject& World::createObject(const std::string& name)
    {
        auto object = std::make_unique<GameObject>(name);
        GameObject& reference = *object;

        objects.push_back(std::move(object));

        return reference;
    }

    bool World::removeObject(const std::string& name)
    {
        // Remove only the first matching object. This keeps the operation
        // predictable even if a project contains duplicate object names.
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

    void World::clear()
    {
        objects.clear();
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
