#include "horizon/Scene/World.hpp"

#include <algorithm>
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
        auto oldSize = objects.size();

        objects.erase(
            std::remove_if(
                objects.begin(),
                objects.end(),
                [&name](const std::unique_ptr<GameObject>& object)
                {
                    return object->getName() == name;
                }
            ),
            objects.end()
        );

        return objects.size() != oldSize;
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