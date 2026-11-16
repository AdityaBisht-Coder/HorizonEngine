
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
}
