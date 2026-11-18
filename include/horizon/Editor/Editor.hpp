#pragma once

#include "horizon/Scene/World.hpp"
#include "horizon/Scene/SceneStorage.hpp"

#include <memory>
#include <string>
#include <vector>
#include <filesystem>
#include <cstdint>

namespace horizon
{
    class Mesh;

    class Editor
    {
    public:
        Editor() = default;
        ~Editor() = default;

        Editor(const Editor&) = delete;
        Editor& operator=(const Editor&) = delete;

        void render(World& world);

    private:
        void createCube(World& world);

        void refreshSceneList();

        bool saveCurrentScene(World& world, bool saveAs);

        bool loadScene(
            const std::filesystem::path& path,
            World& world
        );

        void performPendingAction(World& world);

        enum class PendingSceneAction
        {
            None,
            NewScene,
            LoadScene
        };

        std::uint64_t selectedObjectId = 0;

        std::shared_ptr<Mesh> cubeMesh;

        SceneStorage sceneStorage;

        std::vector<std::filesystem::path> savedScenes;

        std::filesystem::path selectedScenePath;
        std::filesystem::path pendingScenePath;

        std::string currentScenePath;
        std::string sceneStatusMessage;

        char sceneName[128] = "Untitled";

        bool sceneStorageInitialized = false;
        bool sceneDirty = false;

        PendingSceneAction pendingSceneAction =
            PendingSceneAction::None;

        bool showUnsavedChangesPopup = false;
    };
}