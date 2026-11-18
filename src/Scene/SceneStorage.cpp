
#include "horizon/Scene/SceneStorage.hpp"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <system_error>

namespace horizon
{
    namespace
    {
        std::string getEnvironmentVariable(
            const char* name)
        {
#ifdef _MSC_VER
            char* value = nullptr;
            size_t length = 0;

            if (_dupenv_s(&value, &length, name) != 0 ||
                value == nullptr)
            {
                return {};
            }

            std::string result(value);

            std::free(value);

            return result;
#else
            const char* value = std::getenv(name);

            if (value == nullptr)
            {
                return {};
            }

            return value;
#endif
        }

        std::filesystem::path resolveDefaultRoot()
        {
            const std::string localAppData =
                getEnvironmentVariable("LOCALAPPDATA");

            if (!localAppData.empty())
            {
                return std::filesystem::path(localAppData)
                    / "HorizonEngine";
            }

            const std::string userProfile =
                getEnvironmentVariable("USERPROFILE");

            if (!userProfile.empty())
            {
                return std::filesystem::path(userProfile)
                    / "AppData"
                    / "Local"
                    / "HorizonEngine";
            }

            const std::string home =
                getEnvironmentVariable("HOME");

            if (!home.empty())
            {
                return std::filesystem::path(home)
                    / ".local"
                    / "share"
                    / "HorizonEngine";
            }

            return {};
        }

        std::string toLower(std::string value)
        {
            std::transform(
                value.begin(),
                value.end(),
                value.begin(),
                [](unsigned char character)
                {
                    return static_cast<char>(
                        std::tolower(character)
                    );
                }
            );

            return value;
        }

        bool isValidSceneName(
            const std::string& sceneName,
            std::string* errorMessage)
        {
            const auto fail =
                [errorMessage](const std::string& message)
                {
                    if (errorMessage)
                    {
                        *errorMessage = message;
                    }

                    return false;
                };

            if (sceneName.empty())
            {
                return fail("Scene name cannot be empty.");
            }

            if (sceneName == "." ||
                sceneName == "..")
            {
                return fail("Invalid scene name.");
            }

            // Prevent directory traversal and path injection.
            for (unsigned char character : sceneName)
            {
                if (character < 32 ||
                    character == '<' ||
                    character == '>' ||
                    character == ':' ||
                    character == '"' ||
                    character == '/' ||
                    character == '\\' ||
                    character == '|' ||
                    character == '?' ||
                    character == '*')
                {
                    return fail(
                        "Scene name contains invalid characters."
                    );
                }
            }

            if (sceneName.back() == '.' ||
                sceneName.back() == ' ')
            {
                return fail(
                    "Scene name cannot end with a dot or space."
                );
            }

            const std::filesystem::path namePath(sceneName);

            const std::string extension =
                toLower(namePath.extension().string());

            if (!extension.empty() &&
                extension != ".hscene")
            {
                return fail(
                    "Scene files must use the .hscene extension."
                );
            }

            std::string baseName =
                namePath.stem().string();

            if (extension.empty())
            {
                baseName = sceneName;
            }

            baseName = toLower(baseName);

            // Windows reserved device names.
            if (baseName == "con" ||
                baseName == "prn" ||
                baseName == "aux" ||
                baseName == "nul")
            {
                return fail(
                    "This name is reserved by Windows."
                );
            }

            for (int i = 1; i <= 9; ++i)
            {
                if (baseName == "com" + std::to_string(i) ||
                    baseName == "lpt" + std::to_string(i))
                {
                    return fail(
                        "This name is reserved by Windows."
                    );
                }
            }

            if (baseName.empty())
            {
                return fail(
                    "Scene name cannot be empty."
                );
            }

            return true;
        }
    }

    // =============================================================
    // CONSTRUCTORS
    // =============================================================

    SceneStorage::SceneStorage()
        : rootDirectory(resolveDefaultRoot())
    {
    }

    SceneStorage::SceneStorage(
        const std::filesystem::path& rootDirectory)
        : rootDirectory(rootDirectory)
    {
    }

    // =============================================================
    // DIRECTORY ACCESS
    // =============================================================

    const std::filesystem::path&
    SceneStorage::getRootDirectory() const noexcept
    {
        return rootDirectory;
    }

    std::filesystem::path
    SceneStorage::getScenesDirectory() const
    {
        return rootDirectory / "Scenes";
    }

    // =============================================================
    // CREATE DIRECTORIES
    // =============================================================

    bool SceneStorage::ensureDirectories(
        std::string* errorMessage) const
    {
        const auto fail =
            [errorMessage](const std::string& message)
            {
                if (errorMessage)
                {
                    *errorMessage = message;
                }

                return false;
            };

        if (rootDirectory.empty())
        {
            return fail(
                "Could not determine Horizon user-data directory."
            );
        }

        std::error_code error;

        std::filesystem::create_directories(
            rootDirectory,
            error
        );

        if (error)
        {
            return fail(
                "Could not create Horizon directory: " +
                error.message()
            );
        }

        if (!std::filesystem::is_directory(
                rootDirectory,
                error) ||
            error)
        {
            return fail(
                "Horizon user-data path is not a directory."
            );
        }

        const std::filesystem::path scenesDirectory =
            getScenesDirectory();

        std::filesystem::create_directories(
            scenesDirectory,
            error
        );

        if (error)
        {
            return fail(
                "Could not create Scenes directory: " +
                error.message()
            );
        }

        if (!std::filesystem::is_directory(
                scenesDirectory,
                error) ||
            error)
        {
            return fail(
                "Scenes path is not a directory."
            );
        }

        if (errorMessage)
        {
            errorMessage->clear();
        }

        return true;
    }

    // =============================================================
    // SCENE DISCOVERY
    // =============================================================

    std::vector<std::filesystem::path>
    SceneStorage::listScenes(
        std::string* errorMessage) const
    {
        std::vector<std::filesystem::path> scenes;

        const auto fail =
            [errorMessage](const std::string& message)
            {
                if (errorMessage)
                {
                    *errorMessage = message;
                }
            };

        if (!ensureDirectories(errorMessage))
        {
            return scenes;
        }

        std::error_code error;

        std::filesystem::directory_iterator iterator(
            getScenesDirectory(),
            error
        );

        if (error)
        {
            fail(
                "Could not read Scenes directory: " +
                error.message()
            );

            return scenes;
        }

        const std::filesystem::directory_iterator end;

        while (iterator != end)
        {
            const auto& entry = *iterator;

            if (entry.is_regular_file(error))
            {
                if (error)
                {
                    fail(
                        "Could not inspect scene file: " +
                        error.message()
                    );

                    return {};
                }

                const std::string extension =
                    toLower(
                        entry.path().extension().string()
                    );

                if (extension == ".hscene")
                {
                    scenes.push_back(entry.path());
                }
            }

            if (error)
            {
                fail(
                    "Error while scanning Scenes directory: " +
                    error.message()
                );

                return {};
            }

            iterator.increment(error);

            if (error)
            {
                fail(
                    "Error while scanning Scenes directory: " +
                    error.message()
                );

                return {};
            }
        }

        // Sort scenes alphabetically.
        std::sort(
            scenes.begin(),
            scenes.end(),
            [](const std::filesystem::path& first,
               const std::filesystem::path& second)
            {
                return toLower(
                    first.filename().string()
                ) < toLower(
                    second.filename().string()
                );
            }
        );

        if (errorMessage)
        {
            errorMessage->clear();
        }

        return scenes;
    }

    // =============================================================
    // SCENE PATH RESOLUTION
    // =============================================================

    bool SceneStorage::getScenePath(
        const std::string& sceneName,
        std::filesystem::path& outputPath,
        std::string* errorMessage) const
    {
        outputPath.clear();

        if (!isValidSceneName(
                sceneName,
                errorMessage))
        {
            return false;
        }

        std::filesystem::path filename(sceneName);

        if (filename.extension().empty())
        {
            filename += ".hscene";
        }

        outputPath =
            getScenesDirectory() / filename;

        if (errorMessage)
        {
            errorMessage->clear();
        }

        return true;
    }
}
