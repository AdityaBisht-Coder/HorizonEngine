
#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace horizon
{
    class SceneStorage
    {
    public:
        // Uses Horizon's default user-data directory.
        SceneStorage();

        // Allows a custom directory, useful for testing.
        explicit SceneStorage(
            const std::filesystem::path& rootDirectory
        );

        // Root user-data directory.
        const std::filesystem::path& getRootDirectory() const noexcept;

        // Directory containing saved scenes.
        std::filesystem::path getScenesDirectory() const;

        // Creates required directories.
        bool ensureDirectories(
            std::string* errorMessage = nullptr
        ) const;

        // Finds all .hscene files.
        std::vector<std::filesystem::path> listScenes(
            std::string* errorMessage = nullptr
        ) const;

        // Converts a scene name into its full path.
        // Accepts a name with or without the .hscene extension.
        bool getScenePath(
            const std::string& sceneName,
            std::filesystem::path& outputPath,
            std::string* errorMessage = nullptr
        ) const;

    private:
        std::filesystem::path rootDirectory;
    };
}
