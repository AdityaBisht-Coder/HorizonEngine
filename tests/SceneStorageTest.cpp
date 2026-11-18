
#include "horizon/Scene/SceneStorage.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

int main()
{
    int passed = 0;
    int failed = 0;

    const auto check =
        [&passed, &failed](bool condition,
                           const std::string& description)
        {
            if (condition)
            {
                ++passed;

                std::cout
                    << "[PASS] "
                    << description
                    << "\n";
            }
            else
            {
                ++failed;

                std::cout
                    << "[FAIL] "
                    << description
                    << "\n";
            }
        };

    // Create a unique temporary test directory.
    const auto timestamp =
        std::chrono::steady_clock::now()
            .time_since_epoch()
            .count();

    const std::filesystem::path testRoot =
        std::filesystem::temp_directory_path()
        / ("HorizonSceneStorageTest_"
           + std::to_string(timestamp));

    horizon::SceneStorage storage(testRoot);

    std::string errorMessage;

    // ---------------------------------------------------------
    // DIRECTORY CREATION
    // ---------------------------------------------------------

    check(
        storage.ensureDirectories(&errorMessage),
        "Create Horizon storage directories"
    );

    check(
        std::filesystem::is_directory(testRoot),
        "Root directory exists"
    );

    check(
        std::filesystem::is_directory(
            testRoot / "Scenes"),
        "Scenes directory exists"
    );

    // ---------------------------------------------------------
    // SCENE PATH RESOLUTION
    // ---------------------------------------------------------

    std::filesystem::path scenePath;

    check(
        storage.getScenePath(
            "My First Scene",
            scenePath,
            &errorMessage),
        "Resolve scene name without extension"
    );

    check(
        scenePath.filename() ==
            "My First Scene.hscene",
        "Automatically append .hscene"
    );

    check(
        storage.getScenePath(
            "ExistingScene.hscene",
            scenePath,
            &errorMessage),
        "Accept scene name with extension"
    );

    check(
        scenePath.filename() ==
            "ExistingScene.hscene",
        "Preserve existing extension"
    );

    check(
        storage.getScenePath(
            "UpperCase.HSCENE",
            scenePath,
            &errorMessage),
        "Accept uppercase scene extension"
    );

    // ---------------------------------------------------------
    // INVALID NAMES
    // ---------------------------------------------------------

    check(
        !storage.getScenePath(
            "",
            scenePath,
            &errorMessage),
        "Reject empty scene name"
    );

    check(
        !storage.getScenePath(
            "../Outside",
            scenePath,
            &errorMessage),
        "Reject directory traversal"
    );

    check(
        !storage.getScenePath(
            "Folder/Scene",
            scenePath,
            &errorMessage),
        "Reject directory separators"
    );

    check(
        !storage.getScenePath(
            "Bad:Name",
            scenePath,
            &errorMessage),
        "Reject invalid filename characters"
    );

    check(
        !storage.getScenePath(
            "Scene.txt",
            scenePath,
            &errorMessage),
        "Reject incorrect extension"
    );

    check(
        !storage.getScenePath(
            "CON",
            scenePath,
            &errorMessage),
        "Reject reserved Windows filename"
    );

    // ---------------------------------------------------------
    // SCENE DISCOVERY
    // ---------------------------------------------------------

    const std::filesystem::path scenesDirectory =
        storage.getScenesDirectory();

    // Create some test files.
    std::ofstream(scenesDirectory / "Beta.hscene")
        << "test";

    std::ofstream(scenesDirectory / "Alpha.hscene")
        << "test";

    std::ofstream(scenesDirectory / "Gamma.HSCENE")
        << "test";

    std::ofstream(scenesDirectory / "Ignore.txt")
        << "test";

    std::filesystem::create_directory(
        scenesDirectory / "Folder.hscene"
    );

    const auto scenes =
        storage.listScenes(&errorMessage);

    check(
        scenes.size() == 3,
        "Discover only scene files"
    );

    check(
        scenes.size() >= 3 &&
        scenes[0].filename() == "Alpha.hscene" &&
        scenes[1].filename() == "Beta.hscene" &&
        scenes[2].filename() == "Gamma.HSCENE",
        "Sort scenes alphabetically"
    );

    // ---------------------------------------------------------
    // CLEANUP
    // ---------------------------------------------------------

    std::error_code cleanupError;

    std::filesystem::remove_all(
        testRoot,
        cleanupError
    );

    check(
        !cleanupError,
        "Clean up temporary test directory"
    );

    // ---------------------------------------------------------
    // RESULTS
    // ---------------------------------------------------------

    std::cout
        << "\n================================\n"
        << "SceneStorage Test Results\n"
        << "================================\n"
        << "Passed: " << passed << "\n"
        << "Failed: " << failed << "\n";

    if (failed == 0)
    {
        std::cout
            << "\nALL TESTS PASSED!\n";

        return 0;
    }

    std::cout
        << "\nSOME TESTS FAILED!\n";

    return 1;
}
