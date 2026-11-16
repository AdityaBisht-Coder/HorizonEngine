
#include "horizon/Scene/GameObject.hpp"

namespace horizon
{
    GameObject::GameObject(const std::string& objectName)
        : name(objectName)
    {
    }

    const std::string& GameObject::getName() const
    {
        return name;
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
