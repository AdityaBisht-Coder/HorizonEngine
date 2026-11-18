
#include "horizon/Scene/SceneSerializer.hpp"

#include <fstream>
#include <iomanip>
#include <cmath>
#include <cstdint>
#include <limits>

namespace horizon
{
    bool SceneSerializer::save(
        const World& world,
        const std::string& filepath,
        std::string* errorMessage)
    {
        const auto fail = [errorMessage](const std::string& message)
        {
            if (errorMessage)
            {
                *errorMessage = message;
            }

            return false;
        };

        if (filepath.empty())
        {
            return fail("Scene filepath is empty.");
        }

        std::ofstream file(filepath);

        if (!file.is_open())
        {
            return fail("Could not open scene file for writing.");
        }

        file << std::setprecision(
            std::numeric_limits<float>::max_digits10
        );

        // ---------------------------------------------------------
        // FILE HEADER
        // ---------------------------------------------------------

        file << "HORIZON_SCENE 1\n";

        file << "OBJECT_COUNT "
             << world.getObjectCount()
             << "\n\n";

        // ---------------------------------------------------------
        // GAMEOBJECT DATA
        // ---------------------------------------------------------

        for (const auto& object : world.getObjects())
        {
            if (!object)
            {
                return fail("World contains an invalid GameObject.");
            }

            const auto& transform = object->getTransform();
            const auto& material = object->getMaterial();

            const Vec3& position = transform.position;
            const Vec3& rotation = transform.rotation;
            const Vec3& scale = transform.scale;
            const Vec3& color = material.color;

            // Reject invalid floating-point values.
            const float values[] =
            {
                position.x, position.y, position.z,
                rotation.x, rotation.y, rotation.z,
                scale.x, scale.y, scale.z,
                color.x, color.y, color.z
            };

            for (float value : values)
            {
                if (!std::isfinite(value))
                {
                    return fail(
                        "GameObject contains an invalid numeric value."
                    );
                }
            }

            // Object header.
            file << "OBJECT "
                 << object->getId() << " "
                 << std::quoted(object->getName())
                 << "\n";

            // Transform.
            file << "POSITION "
                 << position.x << " "
                 << position.y << " "
                 << position.z << "\n";

            file << "ROTATION "
                 << rotation.x << " "
                 << rotation.y << " "
                 << rotation.z << "\n";

            file << "SCALE "
                 << scale.x << " "
                 << scale.y << " "
                 << scale.z << "\n";

            // Material.
            file << "COLOR "
                 << color.x << " "
                 << color.y << " "
                 << color.z << "\n";

            // Mesh presence.
            // The actual mesh type will be expanded in the asset system.
            file << "MESH "
                 << (object->getMesh() ? "cube" : "none")
                 << "\n";

            file << "END_OBJECT\n\n";

            if (!file.good())
            {
                return fail("Error while writing GameObject data.");
            }
        }

        file << "END_SCENE\n";

        file.flush();

        if (!file.good())
        {
            return fail("Failed to finish writing the scene file.");
        }

        file.close();

        if (file.fail())
        {
            return fail("Failed to close the scene file.");
        }

        if (errorMessage)
        {
            errorMessage->clear();
        }

        return true;
    }
}
