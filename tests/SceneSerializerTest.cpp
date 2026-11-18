
#include "horizon/Scene/SceneSerializer.hpp"
#include "horizon/Scene/Mesh.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>

namespace
{
    int failures = 0;

    void check(bool condition, const std::string& description)
    {
        if (condition)
        {
            std::cout << "[PASS] " << description << "\n";
        }
        else
        {
            std::cerr << "[FAIL] " << description << "\n";
            ++failures;
        }
    }

    bool nearlyEqual(float a, float b)
    {
        return (a - b) < 0.0001f &&
               (b - a) < 0.0001f;
    }
}

int main()
{
    using namespace horizon;

    const std::string scenePath =
        "HorizonSerializerTest.scene";

    const std::string corruptPath =
        "HorizonSerializerCorrupt.scene";

    std::cout << "====================================\n";
    std::cout << " HORIZON SCENE SERIALIZER TEST\n";
    std::cout << "====================================\n\n";

    // ---------------------------------------------------------
    // CREATE ORIGINAL WORLD
    // ---------------------------------------------------------

    World original;

    GameObject& cube =
        original.createObject("Test Cube", 7);

    cube.getTransform().position =
        Vec3(1.25f, -2.5f, 3.75f);

    cube.getTransform().rotation =
        Vec3(10.0f, 20.0f, 30.0f);

    cube.getTransform().scale =
        Vec3(2.0f, 3.0f, 4.0f);

    cube.getMaterial().color =
        Vec3(0.2f, 0.4f, 0.8f);

    cube.setMesh(Mesh::createCube());

    GameObject& emptyObject =
        original.createObject("Empty Object", 12);

    emptyObject.getTransform().position =
        Vec3(-5.0f, 6.0f, 7.0f);

    emptyObject.setMesh(nullptr);

    check(
        original.getObjectCount() == 2,
        "Original World contains two objects"
    );

    // ---------------------------------------------------------
    // SAVE
    // ---------------------------------------------------------

    std::string error;

    const bool saveResult =
        SceneSerializer::save(
            original,
            scenePath,
            &error
        );

    check(saveResult, "Scene saves successfully");

    if (!saveResult)
    {
        std::cerr << "Save error: " << error << "\n";
    }

    // ---------------------------------------------------------
    // LOAD INTO A DIFFERENT WORLD
    // ---------------------------------------------------------

    World loaded;

    loaded.createObject("Old Object");

    const bool loadResult =
        SceneSerializer::load(
            scenePath,
            loaded,
            &error
        );

    check(loadResult, "Scene loads successfully");

    if (!loadResult)
    {
        std::cerr << "Load error: " << error << "\n";
    }

    check(
        loaded.getObjectCount() == 2,
        "Loaded World contains correct object count"
    );

    // ---------------------------------------------------------
    // VERIFY FIRST OBJECT
    // ---------------------------------------------------------

    if (loaded.getObjectCount() >= 1)
    {
        const GameObject& loadedCube =
            *loaded.getObjects()[0];

        check(
            loadedCube.getId() == 7,
            "Cube ID is preserved"
        );

        check(
            loadedCube.getName() == "Test Cube",
            "Cube name is preserved"
        );

        const Transform& transform =
            loadedCube.getTransform();

        check(
            nearlyEqual(transform.position.x, 1.25f) &&
            nearlyEqual(transform.position.y, -2.5f) &&
            nearlyEqual(transform.position.z, 3.75f),
            "Cube position is preserved"
        );

        check(
            nearlyEqual(transform.rotation.x, 10.0f) &&
            nearlyEqual(transform.rotation.y, 20.0f) &&
            nearlyEqual(transform.rotation.z, 30.0f),
            "Cube rotation is preserved"
        );

        check(
            nearlyEqual(transform.scale.x, 2.0f) &&
            nearlyEqual(transform.scale.y, 3.0f) &&
            nearlyEqual(transform.scale.z, 4.0f),
            "Cube scale is preserved"
        );

        const Vec3& color =
            loadedCube.getMaterial().color;

        check(
            nearlyEqual(color.x, 0.2f) &&
            nearlyEqual(color.y, 0.4f) &&
            nearlyEqual(color.z, 0.8f),
            "Cube material color is preserved"
        );

        check(
            loadedCube.getMesh() != nullptr,
            "Cube mesh is restored"
        );

        if (loadedCube.getMesh())
        {
            check(
                loadedCube.getMesh()->getVertexCount() == 36,
                "Restored cube has 36 vertices"
            );
        }
    }

    // ---------------------------------------------------------
    // VERIFY SECOND OBJECT
    // ---------------------------------------------------------

    if (loaded.getObjectCount() >= 2)
    {
        const GameObject& loadedEmpty =
            *loaded.getObjects()[1];

        check(
            loadedEmpty.getId() == 12,
            "Second object ID is preserved"
        );

        check(
            loadedEmpty.getName() == "Empty Object",
            "Second object name is preserved"
        );

        check(
            loadedEmpty.getMesh() == nullptr,
            "Object without mesh remains meshless"
        );
    }

    // ---------------------------------------------------------
    // VERIFY NEXT AUTOMATIC ID
    // ---------------------------------------------------------

    GameObject& nextObject =
        loaded.createObject("Next Object");

    check(
        nextObject.getId() == 13,
        "Automatic ID continues after loaded IDs"
    );

    // ---------------------------------------------------------
    // CORRUPT FILE TEST
    // ---------------------------------------------------------

    {
        std::ofstream corruptFile(corruptPath);

        corruptFile
            << "HORIZON_SCENE 1\n"
            << "OBJECT_COUNT 2\n\n"

            << "OBJECT 5 \"First\"\n"
            << "POSITION 0 0 0\n"
            << "ROTATION 0 0 0\n"
            << "SCALE 1 1 1\n"
            << "COLOR 1 1 1\n"
            << "MESH none\n"
            << "END_OBJECT\n\n"

            << "OBJECT 5 \"Duplicate\"\n"
            << "POSITION 0 0 0\n"
            << "ROTATION 0 0 0\n"
            << "SCALE 1 1 1\n"
            << "COLOR 1 1 1\n"
            << "MESH none\n"
            << "END_OBJECT\n\n"

            << "END_SCENE\n";
    }

    // Create a World that must survive failed loading.
    World protectedWorld;

    GameObject& protectedObject =
        protectedWorld.createObject("Protected Object", 50);

    protectedObject.getTransform().position =
        Vec3(100.0f, 200.0f, 300.0f);

    const bool corruptLoad =
        SceneSerializer::load(
            corruptPath,
            protectedWorld,
            &error
        );

    check(
        !corruptLoad,
        "Corrupted scene is rejected"
    );

    check(
        protectedWorld.getObjectCount() == 1,
        "Failed loading does not destroy existing World"
    );

    if (protectedWorld.getObjectCount() == 1)
    {
        check(
            protectedWorld.getObjects()[0]->getId() == 50,
            "Existing object's ID remains unchanged"
        );

        check(
            protectedWorld.getObjects()[0]->getName()
                == "Protected Object",
            "Existing object's name remains unchanged"
        );
    }

    check(
        !error.empty(),
        "Loading failure provides an error message"
    );

    // ---------------------------------------------------------
    // MISSING FILE TEST
    // ---------------------------------------------------------

    World missingFileWorld;

    missingFileWorld.createObject("Keep Me");

    const bool missingFileResult =
        SceneSerializer::load(
            "ThisSceneDoesNotExist.scene",
            missingFileWorld,
            &error
        );

    check(
        !missingFileResult,
        "Missing scene file is rejected"
    );

    check(
        missingFileWorld.getObjectCount() == 1,
        "Missing file leaves existing World untouched"
    );

    // ---------------------------------------------------------
    // CLEANUP
    // ---------------------------------------------------------

    std::remove(scenePath.c_str());
    std::remove(corruptPath.c_str());

    // ---------------------------------------------------------
    // RESULTS
    // ---------------------------------------------------------

    std::cout << "\n====================================\n";

    if (failures == 0)
    {
        std::cout << " ALL TESTS PASSED!\n";
    }
    else
    {
        std::cout
            << " TESTS FAILED: "
            << failures
            << "\n";
    }

    std::cout << "====================================\n";

    return failures == 0 ? 0 : 1;
}
