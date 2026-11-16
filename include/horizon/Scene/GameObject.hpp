
#pragma once

#include "horizon/Scene/Transform.hpp"
#include "horizon/Scene/Material.hpp"

#include <string>

namespace horizon
{
    class GameObject
    {
    public:
        explicit GameObject(const std::string& name);

        const std::string& getName() const;

        Transform& getTransform();
        const Transform& getTransform() const;

        Material& getMaterial();
        const Material& getMaterial() const;

    private:
        std::string name;

        Transform transform;
        Material material;
    };
}
