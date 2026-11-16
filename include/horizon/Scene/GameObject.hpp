
#pragma once

#include "horizon/Scene/Transform.hpp"
#include "horizon/Scene/Material.hpp"
#include "horizon/Scene/Mesh.hpp"

#include <string>
#include <memory>

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

        void setMesh(std::shared_ptr<Mesh> mesh);

        std::shared_ptr<Mesh> getMesh() const;

    private:
        std::string name;

        Transform transform;
        Material material;

        std::shared_ptr<Mesh> mesh;
    };
}
