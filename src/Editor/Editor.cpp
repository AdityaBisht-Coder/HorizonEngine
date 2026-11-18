
#include "horizon/Editor/Editor.hpp"

#include "horizon/Scene/GameObject.hpp"
#include "horizon/Scene/Mesh.hpp"
#include "horizon/Scene/SceneSerializer.hpp"

#include "imgui.h"

#include <string>
#include <filesystem>
#include <cstdio>

namespace horizon
{
    // =============================================================
    // SCENE BROWSER
    // =============================================================

    void Editor::refreshSceneList()
    {
        std::string errorMessage;

        if (!sceneStorage.ensureDirectories(&errorMessage))
        {
            sceneStatusMessage =
                "Storage error: " + errorMessage;
            return;
        }

        savedScenes =
            sceneStorage.listScenes(&errorMessage);

        if (!errorMessage.empty())
        {
            sceneStatusMessage =
                "Scene list error: " + errorMessage;
        }

        // Preserve selected scene if it still exists.
        bool selectionStillExists = false;

        for (const auto& path : savedScenes)
        {
            if (path == selectedScenePath)
            {
                selectionStillExists = true;
                break;
            }
        }

        if (!selectionStillExists)
        {
            selectedScenePath.clear();
        }

        // Select the currently loaded scene.
        if (!currentScenePath.empty())
        {
            const std::filesystem::path currentPath(
                currentScenePath
            );

            for (const auto& path : savedScenes)
            {
                if (path == currentPath)
                {
                    selectedScenePath = path;
                    break;
                }
            }
        }

        sceneStorageInitialized = true;
    }

    // =============================================================
    // SAVE SCENE
    // =============================================================

    bool Editor::saveCurrentScene(World& world, bool saveAs)
    {
        std::filesystem::path targetPath;

        // Save As, or save an untitled scene.
        if (saveAs || currentScenePath.empty())
        {
            std::string errorMessage;

            if (!sceneStorage.getScenePath(
                    sceneName,
                    targetPath,
                    &errorMessage))
            {
                sceneStatusMessage =
                    "Invalid scene name: " + errorMessage;

                return false;
            }

            // Prevent accidental overwriting of another scene.
            if (std::filesystem::exists(targetPath) &&
                targetPath !=
                    std::filesystem::path(currentScenePath))
            {
                sceneStatusMessage =
                    "A scene with this name already exists. "
                    "Choose another name.";

                return false;
            }
        }
        else
        {
            // Save to the currently loaded scene.
            targetPath = currentScenePath;
        }

        std::string errorMessage;

        if (!sceneStorage.ensureDirectories(&errorMessage))
        {
            sceneStatusMessage =
                "Storage error: " + errorMessage;

            return false;
        }

        if (!SceneSerializer::save(
                world,
                targetPath.string(),
                &errorMessage))
        {
            sceneStatusMessage =
                "Save failed: " + errorMessage;

            return false;
        }

        currentScenePath = targetPath.string();

        // Update the name field to match the saved filename.
        const std::string name =
            targetPath.stem().string();

        std::snprintf(
            sceneName,
            sizeof(sceneName),
            "%s",
            name.c_str()
        );

        sceneDirty = false;

        sceneStatusMessage =
            "Scene saved successfully.";

        refreshSceneList();

        return true;
    }

    // =============================================================
    // LOAD SCENE
    // =============================================================

    bool Editor::loadScene(
        const std::filesystem::path& path,
        World& world)
    {
        std::string errorMessage;

        if (!SceneSerializer::load(
                path.string(),
                world,
                &errorMessage))
        {
            sceneStatusMessage =
                "Load failed: " + errorMessage;

            return false;
        }

        currentScenePath = path.string();

        selectedScenePath = path;

        sceneDirty = false;

        selectedObjectId = 0;

        // Update the scene name field.
        std::string name =
            path.stem().string();

        if (name.size() >= sizeof(sceneName))
        {
            name.resize(sizeof(sceneName) - 1);
        }

        std::snprintf(
            sceneName,
            sizeof(sceneName),
            "%s",
            name.c_str()
        );

        sceneStatusMessage =
            "Scene loaded successfully.";

        refreshSceneList();

        return true;
    }

    // =============================================================
    // PENDING SCENE ACTION
    // =============================================================

    void Editor::performPendingAction(World& world)
    {
        if (pendingSceneAction ==
            PendingSceneAction::NewScene)
        {
            world.clear();

            selectedObjectId = 0;

            currentScenePath.clear();

            sceneDirty = false;

            std::snprintf(
                sceneName,
                sizeof(sceneName),
                "%s",
                "Untitled"
            );

            sceneStatusMessage =
                "New empty scene created.";
        }
        else if (pendingSceneAction ==
                 PendingSceneAction::LoadScene)
        {
            loadScene(
                pendingScenePath,
                world
            );
        }

        pendingSceneAction =
            PendingSceneAction::None;

        pendingScenePath.clear();
    }

    // =============================================================
    // CREATE CUBE
    // =============================================================

    void Editor::createCube(World& world)
    {
        // Reuse an existing cube mesh if possible.
        if (!cubeMesh)
        {
            for (const auto& object : world.getObjects())
            {
                if (object && object->getMesh())
                {
                    cubeMesh = object->getMesh();
                    break;
                }
            }
        }

        // Create a cube mesh if necessary.
        if (!cubeMesh)
        {
            cubeMesh = Mesh::createCube();
        }

        if (!cubeMesh)
        {
            sceneStatusMessage =
                "Failed to create cube mesh.";

            return;
        }

        // Generate a unique object name.
        std::string objectName = "Cube";

        unsigned int suffix = 1;

        const auto nameExists =
            [&world](const std::string& name)
            {
                for (const auto& object : world.getObjects())
                {
                    if (object &&
                        object->getName() == name)
                    {
                        return true;
                    }
                }

                return false;
            };

        while (nameExists(objectName))
        {
            objectName =
                "Cube_" + std::to_string(suffix);

            ++suffix;
        }

        // Create the GameObject.
        GameObject& cube =
            world.createObject(objectName);

        // Transform.
        cube.getTransform().position =
            Vec3(0.0f, 0.0f, -2.0f);

        cube.getTransform().rotation =
            Vec3(0.0f, 0.0f, 0.0f);

        cube.getTransform().scale =
            Vec3(0.75f, 0.75f, 0.75f);

        // Material.
        cube.getMaterial().color =
            Vec3(0.15f, 0.55f, 1.0f);

        // Mesh.
        cube.setMesh(cubeMesh);

        // Select the new object.
        selectedObjectId = cube.getId();

        sceneDirty = true;

        sceneStatusMessage =
            "Cube created. Scene has unsaved changes.";
    }

    // =============================================================
    // EDITOR RENDER
    // =============================================================

    void Editor::render(World& world)
    {
        // Initialize scene storage on first frame.
        if (!sceneStorageInitialized)
        {
            refreshSceneList();
        }

        // ---------------------------------------------------------
        // SCENE MANAGEMENT
        // ---------------------------------------------------------

        ImGui::SetNextWindowSize(
            ImVec2(380.0f, 430.0f),
            ImGuiCond_FirstUseEver
        );

        ImGui::Begin("Scene Management");

        ImGui::Text("Saved Scenes");

        ImGui::SameLine();

        if (ImGui::Button("Refresh"))
        {
            refreshSceneList();
        }

        // Scene list.
        ImGui::BeginChild(
            "SceneBrowser",
            ImVec2(0.0f, 150.0f),
            true
        );

        if (savedScenes.empty())
        {
            ImGui::TextDisabled(
                "No saved scenes yet."
            );
        }

        for (const auto& path : savedScenes)
        {
            const std::string filename =
                path.filename().string();

            const std::string label =
                filename + "##" + path.string();

            const bool selected =
                selectedScenePath == path;

            if (ImGui::Selectable(
                    label.c_str(),
                    selected))
            {
                selectedScenePath = path;
            }
        }

        ImGui::EndChild();

        // Load Selected.
        if (ImGui::Button("Load Selected"))
        {
            if (selectedScenePath.empty())
            {
                sceneStatusMessage =
                    "Select a scene first.";
            }
            else
            {
                pendingScenePath =
                    selectedScenePath;

                pendingSceneAction =
                    PendingSceneAction::LoadScene;

                if (sceneDirty)
                {
                    showUnsavedChangesPopup = true;
                }
                else
                {
                    performPendingAction(world);
                }
            }
        }

        ImGui::SameLine();

        // New Scene.
        if (ImGui::Button("New Scene"))
        {
            pendingSceneAction =
                PendingSceneAction::NewScene;

            if (sceneDirty)
            {
                showUnsavedChangesPopup = true;
            }
            else
            {
                performPendingAction(world);
            }
        }

        ImGui::Separator();

        // Scene name.
        ImGui::InputText(
            "Scene Name",
            sceneName,
            sizeof(sceneName)
        );

        // Save Scene.
        if (ImGui::Button("Save Scene"))
        {
            saveCurrentScene(world, false);
        }

        ImGui::SameLine();

        // Save As.
        if (ImGui::Button("Save As"))
        {
            saveCurrentScene(world, true);
        }

        ImGui::Separator();

        // Status.
        if (sceneDirty)
        {
            ImGui::TextColored(
                ImVec4(1.0f, 0.8f, 0.2f, 1.0f),
                "Status: Unsaved Changes"
            );
        }
        else
        {
            ImGui::Text(
                "Status: Saved"
            );
        }

        // Current scene.
        if (!currentScenePath.empty())
        {
            ImGui::TextWrapped(
                "Current Scene: %s",
                currentScenePath.c_str()
            );
        }
        else
        {
            ImGui::Text(
                "Current Scene: Untitled"
            );
        }

        // Status message.
        if (!sceneStatusMessage.empty())
        {
            ImGui::TextWrapped(
                "%s",
                sceneStatusMessage.c_str()
            );
        }

        ImGui::End();

        // ---------------------------------------------------------
        // UNSAVED CHANGES POPUP
        // ---------------------------------------------------------

        if (showUnsavedChangesPopup)
        {
            ImGui::OpenPopup("Unsaved Changes");

            showUnsavedChangesPopup = false;
        }

        if (ImGui::BeginPopupModal(
                "Unsaved Changes",
                nullptr,
                ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::TextWrapped(
                "This scene has unsaved changes."
            );

            ImGui::TextWrapped(
                "Would you like to save before continuing?"
            );

            ImGui::Separator();

            // Save & Continue.
            if (ImGui::Button(
                    "Save & Continue",
                    ImVec2(140.0f, 0.0f)))
            {
                if (saveCurrentScene(world, false))
                {
                    performPendingAction(world);

                    ImGui::CloseCurrentPopup();
                }
            }

            ImGui::SameLine();

            // Discard.
            if (ImGui::Button(
                    "Discard",
                    ImVec2(100.0f, 0.0f)))
            {
                performPendingAction(world);

                ImGui::CloseCurrentPopup();
            }

            ImGui::SameLine();

            // Cancel.
            if (ImGui::Button(
                    "Cancel",
                    ImVec2(100.0f, 0.0f)))
            {
                pendingSceneAction =
                    PendingSceneAction::None;

                pendingScenePath.clear();

                sceneStatusMessage =
                    "Scene operation cancelled.";

                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        // ---------------------------------------------------------
        // SCENE HIERARCHY
        // ---------------------------------------------------------

        ImGui::SetNextWindowSize(
            ImVec2(270.0f, 420.0f),
            ImGuiCond_FirstUseEver
        );

        ImGui::Begin("Scene Hierarchy");

        ImGui::Text(
            "Objects: %llu",
            static_cast<unsigned long long>(
                world.getObjectCount()
            )
        );

        // Create Cube.
        if (ImGui::Button("Create Cube"))
        {
            createCube(world);
        }

        ImGui::SameLine();

        // Delete Selected.
        if (selectedObjectId != 0 &&
            ImGui::Button("Delete Selected"))
        {
            if (world.removeObject(selectedObjectId))
            {
                selectedObjectId = 0;

                sceneDirty = true;

                sceneStatusMessage =
                    "Object deleted. Scene has unsaved changes.";
            }
        }

        ImGui::Separator();

        // Empty scene message.
        if (world.getObjectCount() == 0)
        {
            ImGui::TextDisabled(
                "The scene is empty."
            );
        }

        // GameObject list.
        for (const auto& object : world.getObjects())
        {
            if (!object)
            {
                continue;
            }

            const std::string label =
                object->getName() +
                "##" +
                std::to_string(object->getId());

            if (ImGui::Selectable(
                    label.c_str(),
                    selectedObjectId == object->getId()))
            {
                selectedObjectId = object->getId();
            }
        }

        ImGui::End();

        // ---------------------------------------------------------
        // INSPECTOR
        // ---------------------------------------------------------

        ImGui::SetNextWindowSize(
            ImVec2(320.0f, 440.0f),
            ImGuiCond_FirstUseEver
        );

        ImGui::Begin("Inspector");

        // Find selected object by ID.
        GameObject* selectedObject = nullptr;

        for (const auto& object : world.getObjects())
        {
            if (object &&
                object->getId() == selectedObjectId)
            {
                selectedObject = object.get();
                break;
            }
        }

        // No valid selection.
        if (selectedObject == nullptr)
        {
            selectedObjectId = 0;

            ImGui::TextDisabled(
                "Select an object in the hierarchy."
            );

            ImGui::End();

            return;
        }

        // GameObject information.
        ImGui::Text("GameObject");

        ImGui::Separator();

        ImGui::Text(
            "Name: %s",
            selectedObject->getName().c_str()
        );

        ImGui::Text(
            "ID: %llu",
            static_cast<unsigned long long>(
                selectedObject->getId()
            )
        );

        // ---------------------------------------------------------
        // TRANSFORM INSPECTOR
        // ---------------------------------------------------------

        if (ImGui::CollapsingHeader(
                "Transform",
                ImGuiTreeNodeFlags_DefaultOpen))
        {
            auto& transform =
                selectedObject->getTransform();

            bool transformChanged = false;

            transformChanged |= ImGui::DragFloat3(
                "Position",
                &transform.position.x,
                0.05f
            );

            transformChanged |= ImGui::DragFloat3(
                "Rotation",
                &transform.rotation.x,
                0.5f
            );

            transformChanged |= ImGui::DragFloat3(
                "Scale",
                &transform.scale.x,
                0.05f,
                0.01f,
                100.0f
            );

            if (transformChanged)
            {
                sceneDirty = true;

                sceneStatusMessage =
                    "Transform changed. Scene has unsaved changes.";
            }

            // Reset Transform.
            if (ImGui::Button("Reset Transform"))
            {
                transform.position =
                    Vec3(0.0f, 0.0f, 0.0f);

                transform.rotation =
                    Vec3(0.0f, 0.0f, 0.0f);

                transform.scale =
                    Vec3(1.0f, 1.0f, 1.0f);

                sceneDirty = true;

                sceneStatusMessage =
                    "Transform reset. Scene has unsaved changes.";
            }
        }

        // ---------------------------------------------------------
        // MATERIAL INSPECTOR
        // ---------------------------------------------------------

        if (ImGui::CollapsingHeader(
                "Material",
                ImGuiTreeNodeFlags_DefaultOpen))
        {
            auto& material =
                selectedObject->getMaterial();

            if (ImGui::ColorEdit3(
                    "Base Color",
                    &material.color.x))
            {
                sceneDirty = true;

                sceneStatusMessage =
                    "Material changed. Scene has unsaved changes.";
            }
        }

        // ---------------------------------------------------------
        // MESH INFORMATION
        // ---------------------------------------------------------

        ImGui::Separator();

        ImGui::Text(
            "Mesh: %s",
            selectedObject->getMesh()
                ? "Assigned"
                : "None"
        );

        ImGui::End();
    }
}
