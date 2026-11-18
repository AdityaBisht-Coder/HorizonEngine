
#include "horizon/Editor/Editor.hpp"

#include "horizon/Scene/GameObject.hpp"
#include "horizon/Scene/Mesh.hpp"
#include "horizon/Scene/SceneSerializer.hpp"

#include "imgui.h"

#include <string>

namespace horizon
{
    // =============================================================
    // CREATE CUBE
    // =============================================================

    void Editor::createCube(World& world)
    {
        // Reuse an existing cube mesh whenever possible.
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

        // Create a cube mesh if none exists.
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
                    if (object && object->getName() == name)
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
        GameObject& cube = world.createObject(objectName);

        // Set its Transform.
        cube.getTransform().position =
            Vec3(0.0f, 0.0f, -2.0f);

        cube.getTransform().rotation =
            Vec3(0.0f, 0.0f, 0.0f);

        cube.getTransform().scale =
            Vec3(0.75f, 0.75f, 0.75f);

        // Set its Material.
        cube.getMaterial().color =
            Vec3(0.15f, 0.55f, 1.0f);

        // Assign the shared mesh.
        cube.setMesh(cubeMesh);

        // Select the newly created object.
        selectedObjectId = cube.getId();

        // Creating an object modifies the scene.
        sceneDirty = true;

        sceneStatusMessage =
            "Cube created. Scene has unsaved changes.";
    }

    // =============================================================
    // EDITOR RENDER
    // =============================================================

    void Editor::render(World& world)
    {
        // ---------------------------------------------------------
        // SCENE MANAGEMENT
        // ---------------------------------------------------------

        ImGui::SetNextWindowSize(
            ImVec2(360.0f, 220.0f),
            ImGuiCond_FirstUseEver
        );

        ImGui::Begin("Scene Management");

        ImGui::InputText(
            "Scene File",
            scenePath,
            sizeof(scenePath)
        );

        // Save Scene.
        if (ImGui::Button("Save Scene"))
        {
            std::string errorMessage;

            if (SceneSerializer::save(
                    world,
                    scenePath,
                    &errorMessage))
            {
                currentScenePath = scenePath;

                sceneDirty = false;

                sceneStatusMessage =
                    "Scene saved successfully.";
            }
            else
            {
                sceneStatusMessage =
                    "Save failed: " + errorMessage;
            }
        }

        ImGui::SameLine();

        // Load Scene.
        if (ImGui::Button("Load Scene"))
        {
            pendingSceneAction =
                PendingSceneAction::LoadScene;

            if (sceneDirty)
            {
                showUnsavedChangesPopup = true;
            }
            else
            {
                std::string errorMessage;

                if (SceneSerializer::load(
                        scenePath,
                        world,
                        &errorMessage))
                {
                    currentScenePath = scenePath;

                    sceneDirty = false;

                    selectedObjectId = 0;

                    sceneStatusMessage =
                        "Scene loaded successfully.";
                }
                else
                {
                    sceneStatusMessage =
                        "Load failed: " + errorMessage;
                }
            }
        }

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
                world.clear();

                selectedObjectId = 0;

                currentScenePath.clear();

                sceneDirty = false;

                sceneStatusMessage =
                    "New empty scene created.";
            }
        }

        ImGui::Separator();

        // Scene status.
        if (sceneDirty)
        {
            ImGui::TextColored(
                ImVec4(1.0f, 0.8f, 0.2f, 1.0f),
                "Status: Unsaved changes"
            );
        }
        else
        {
            ImGui::Text(
                "Status: Saved"
            );
        }

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

            // Save and Continue.
            if (ImGui::Button(
                    "Save & Continue",
                    ImVec2(140.0f, 0.0f)))
            {
std::string errorMessage;

// When loading another scene, preserve the requested
// load path and save the current scene separately.
std::string savePath = scenePath;

if (pendingSceneAction ==
    PendingSceneAction::LoadScene)
{
    savePath = currentScenePath.empty()
        ? "untitled.hscene"
        : currentScenePath;
}

if (SceneSerializer::save(
        world,
        savePath,
        &errorMessage))
{
    currentScenePath = savePath;

                    sceneDirty = false;

                    sceneStatusMessage =
                        "Scene saved successfully.";

                    // Continue with the pending action.
                    if (pendingSceneAction ==
                        PendingSceneAction::NewScene)
                    {
                        world.clear();

                        selectedObjectId = 0;

                        currentScenePath.clear();

                        sceneDirty = false;

                        sceneStatusMessage =
                            "New empty scene created.";
                    }
                    else if (pendingSceneAction ==
                        PendingSceneAction::LoadScene)
                    {
                        if (SceneSerializer::load(
                                scenePath,
                                world,
                                &errorMessage))
                        {
                            currentScenePath = scenePath;

                            sceneDirty = false;

                            selectedObjectId = 0;

                            sceneStatusMessage =
                                "Scene loaded successfully.";
                        }
                        else
                        {
                            sceneStatusMessage =
                                "Load failed: " +
                                errorMessage;
                        }
                    }

                    pendingSceneAction =
                        PendingSceneAction::None;

                    ImGui::CloseCurrentPopup();
                }
                else
                {
                    sceneStatusMessage =
                        "Save failed: " +
                        errorMessage;
                }
            }

            ImGui::SameLine();

            // Discard Changes.
            if (ImGui::Button(
                    "Discard",
                    ImVec2(100.0f, 0.0f)))
            {
                if (pendingSceneAction ==
                    PendingSceneAction::NewScene)
                {
                    world.clear();

                    selectedObjectId = 0;

                    currentScenePath.clear();

                    sceneDirty = false;

                    sceneStatusMessage =
                        "New empty scene created.";
                }
                else if (pendingSceneAction ==
                    PendingSceneAction::LoadScene)
                {
                    std::string errorMessage;

                    if (SceneSerializer::load(
                            scenePath,
                            world,
                            &errorMessage))
                    {
                        currentScenePath = scenePath;

                        sceneDirty = false;

                        selectedObjectId = 0;

                        sceneStatusMessage =
                            "Scene loaded successfully.";
                    }
                    else
                    {
                        sceneStatusMessage =
                            "Load failed: " +
                            errorMessage;
                    }
                }

                pendingSceneAction =
                    PendingSceneAction::None;

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

        // Display all GameObjects.
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

        // Find selected object by stable ID.
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

        // ---------------------------------------------------------
        // GAMEOBJECT INFORMATION
        // ---------------------------------------------------------

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
