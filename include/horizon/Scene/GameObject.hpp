#pragma once

#include "horizon/Scene/Transform.hpp"
#include "horizon/Scene/Material.hpp"
#include "horizon/Scene/Mesh.hpp"

#include <string>
#include <memory>
#include <cstdint>

namespace horizon
{
    class GameObject
    {
    public:
        GameObject(
            const std::string& name,
            std::uint64_t id
        );

        const std::string& getName() const;

        std::uint64_t getId() const;

        Transform& getTransform();
        const Transform& getTransform() const;

        Material& getMaterial();
        const Material& getMaterial() const;

        void setMesh(std::shared_ptr<Mesh> mesh);

        std::shared_ptr<Mesh> getMesh() const;

    private:
        std::string name;
        std::uint64_t id;

        Transform transform;
        Material material;

        std::shared_ptr<Mesh> mesh;
    };
}