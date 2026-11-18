
#include "horizon/Scene/SceneSerializer.hpp"

#include "horizon/Scene/Mesh.hpp"

#include <fstream>
#include <iomanip>
#include <cmath>
#include <cstdint>
#include <limits>
#include <memory>
#include <utility>

namespace horizon
{
    // =============================================================
    // SAVE SCENE
    // =============================================================

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

        file << "HORIZON_SCENE 1\n";

        file << "OBJECT_COUNT "
             << world.getObjectCount()
             << "\n\n";

        for (const auto& object : world.getObjects())
        {
            if (!object)
            {
                return fail(
                    "World contains an invalid GameObject."
                );
            }

            const auto& transform = object->getTransform();
            const auto& material = object->getMaterial();

            const Vec3& position = transform.position;
            const Vec3& rotation = transform.rotation;
            const Vec3& scale = transform.scale;
            const Vec3& color = material.color;

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

            file << "OBJECT "
                 << object->getId() << " "
                 << std::quoted(object->getName())
                 << "\n";

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

            file << "COLOR "
                 << color.x << " "
                 << color.y << " "
                 << color.z << "\n";

            file << "MESH "
                 << (object->getMesh() ? "cube" : "none")
                 << "\n";

            file << "END_OBJECT\n\n";

            if (!file.good())
            {
                return fail(
                    "Error while writing GameObject data."
                );
            }
        }

        file << "END_SCENE\n";

        file.flush();

        if (!file.good())
        {
            return fail(
                "Failed to finish writing the scene file."
            );
        }

        file.close();

        if (file.fail())
        {
            return fail(
                "Failed to close the scene file."
            );
        }

        if (errorMessage)
        {
            errorMessage->clear();
        }

        return true;
    }

    // =============================================================
    // LOAD SCENE
    // =============================================================

    bool SceneSerializer::load(
        const std::string& filepath,
        World& world,
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

        std::ifstream file(filepath);

        if (!file.is_open())
        {
            return fail("Could not open scene file for reading.");
        }

        // ---------------------------------------------------------
        // FILE HEADER
        // ---------------------------------------------------------

        std::string header;
        int version = 0;

        if (!(file >> header >> version))
        {
            return fail("Scene file header is missing or invalid.");
        }

        if (header != "HORIZON_SCENE")
        {
            return fail("Invalid Horizon scene file.");
        }

        if (version != 1)
        {
            return fail(
                "Unsupported scene file version."
            );
        }

        // ---------------------------------------------------------
        // OBJECT COUNT
        // ---------------------------------------------------------

        std::string token;
        std::uint64_t objectCount = 0;

        if (!(file >> token >> objectCount))
        {
            return fail("Scene object count is missing.");
        }

        if (token != "OBJECT_COUNT")
        {
            return fail("Expected OBJECT_COUNT.");
        }

        // Safety limit against malformed files requesting
        // unreasonable amounts of memory.
        constexpr std::uint64_t MAX_OBJECTS = 1000000;

        if (objectCount > MAX_OBJECTS)
        {
            return fail(
                "Scene contains too many GameObjects."
            );
        }

        // ---------------------------------------------------------
        // TEMPORARY WORLD
        // ---------------------------------------------------------

        World loadedWorld;

        std::shared_ptr<Mesh> cubeMesh;

        // ---------------------------------------------------------
        // LOAD GAMEOBJECTS
        // ---------------------------------------------------------

        for (std::uint64_t i = 0; i < objectCount; ++i)
        {
            std::uint64_t objectId = 0;
            std::string objectName;

            if (!(file >> token >> objectId
                      >> std::quoted(objectName)))
            {
                return fail(
                    "Failed to read GameObject header."
                );
            }

            if (token != "OBJECT")
            {
                return fail(
                    "Expected OBJECT in scene file."
                );
            }

            if (objectId == 0 ||
                objectId == std::numeric_limits<std::uint64_t>::max())
            {
                return fail(
                    "Scene contains an invalid GameObject ID."
                );
            }

            // -----------------------------------------------------
            // TRANSFORM AND MATERIAL VALUES
            // -----------------------------------------------------

            Vec3 position;
            Vec3 rotation;
            Vec3 scale;
            Vec3 color;

            if (!(file >> token
                      >> position.x
                      >> position.y
                      >> position.z))
            {
                return fail(
                    "Failed to read GameObject POSITION."
                );
            }

            if (token != "POSITION")
            {
                return fail("Expected POSITION.");
            }

            if (!(file >> token
                      >> rotation.x
                      >> rotation.y
                      >> rotation.z))
            {
                return fail(
                    "Failed to read GameObject ROTATION."
                );
            }

            if (token != "ROTATION")
            {
                return fail("Expected ROTATION.");
            }

            if (!(file >> token
                      >> scale.x
                      >> scale.y
                      >> scale.z))
            {
                return fail(
                    "Failed to read GameObject SCALE."
                );
            }

            if (token != "SCALE")
            {
                return fail("Expected SCALE.");
            }

            if (!(file >> token
                      >> color.x
                      >> color.y
                      >> color.z))
            {
                return fail(
                    "Failed to read GameObject COLOR."
                );
            }

            if (token != "COLOR")
            {
                return fail("Expected COLOR.");
            }

            // -----------------------------------------------------
            // VALIDATE FLOAT VALUES
            // -----------------------------------------------------

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
                        "Scene contains invalid numeric values."
                    );
                }
            }

            // -----------------------------------------------------
            // MESH
            // -----------------------------------------------------

            std::string meshType;

            if (!(file >> token >> meshType))
            {
                return fail(
                    "Failed to read GameObject MESH."
                );
            }

            if (token != "MESH")
            {
                return fail("Expected MESH.");
            }

            if (meshType != "cube" &&
                meshType != "none")
            {
                return fail(
                    "Unsupported mesh type: " + meshType
                );
            }

            // -----------------------------------------------------
            // OBJECT END
            // -----------------------------------------------------

            if (!(file >> token))
            {
                return fail(
                    "GameObject END_OBJECT is missing."
                );
            }

            if (token != "END_OBJECT")
            {
                return fail(
                    "Expected END_OBJECT."
                );
            }

            // -----------------------------------------------------
            // CREATE OBJECT IN TEMPORARY WORLD
            // -----------------------------------------------------

            try
            {
                GameObject& object =
                    loadedWorld.createObject(
                        objectName,
                        objectId
                    );

                object.getTransform().position = position;
                object.getTransform().rotation = rotation;
                object.getTransform().scale = scale;

                object.getMaterial().color = color;

                if (meshType == "cube")
                {
                    if (!cubeMesh)
                    {
                        cubeMesh = Mesh::createCube();
                    }

                    if (!cubeMesh)
                    {
                        return fail(
                            "Failed to create cube mesh."
                        );
                    }

                    object.setMesh(cubeMesh);
                }
            }
            catch (const std::exception& exception)
            {
                return fail(
                    std::string("Failed to create GameObject: ") +
                    exception.what()
                );
            }
            catch (...)
            {
                return fail(
                    "Unknown error while creating GameObject."
                );
            }
        }

        // ---------------------------------------------------------
        // END SCENE
        // ---------------------------------------------------------

        if (!(file >> token))
        {
            return fail("END_SCENE is missing.");
        }

        if (token != "END_SCENE")
        {
            return fail("Expected END_SCENE.");
        }

        // Reject unexpected trailing content.
        std::string extraToken;

        if (file >> extraToken)
        {
            return fail(
                "Unexpected data after END_SCENE."
            );
        }

        if (file.bad())
        {
            return fail(
                "Error occurred while reading scene file."
            );
        }

        // ---------------------------------------------------------
        // COMMIT
        // ---------------------------------------------------------

        // The existing world is untouched until this point.
        world.swap(loadedWorld);

        if (errorMessage)
        {
            errorMessage->clear();
        }

        return true;
    }
}
