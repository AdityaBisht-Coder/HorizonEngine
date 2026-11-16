
#pragma once

#include "horizon/Scene/Transform.hpp"

namespace horizon
{
    class Material
    {
    public:
        Material()
            : color(0.15f, 0.55f, 1.0f)
        {
        }

        explicit Material(const Vec3& materialColor)
            : color(materialColor)
        {
        }

        Vec3 color;
    };
}
