#include "horizon/Scene/GameObject.hpp"

#include <utility>

namespace horizon
{
    GameObject::GameObject(
        const std::string& objectName,
        std::uint64_t objectId
    )
        : name(objectName),
          id(objectId)
    {
    }

    const std::string& GameObject::getName() const
    {
        return name;
    }

    std::uint64_t GameObject::getId() const
    {
        return id;
    }

    Transform& GameObject::getTransform()
    {
        return transform;
    }

    const Transform& GameObject::getTransform() const
    {
        return transform;
    }

    Material& GameObject::getMaterial()
    {
        return material;
    }

    const Material& GameObject::getMaterial() const
    {
        return material;
    }

    void GameObject::setMesh(std::shared_ptr<Mesh> newMesh)
    {
        mesh = std::move(newMesh);
    }

    std::shared_ptr<Mesh> GameObject::getMesh() const
    {
        return mesh;
    }
}