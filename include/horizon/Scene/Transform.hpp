
#pragma once

namespace horizon
{
    struct Vec3
    {
        float x;
        float y;
        float z;

        Vec3()
            : x(0.0f), y(0.0f), z(0.0f)
        {
        }

        Vec3(float xValue, float yValue, float zValue)
            : x(xValue), y(yValue), z(zValue)
        {
        }
    };

    class Transform
    {
    public:
        Transform();

        Vec3 position;
        Vec3 rotation;
        Vec3 scale;
    };
}
