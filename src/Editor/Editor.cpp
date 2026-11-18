
#include "horizon/Editor/Editor.hpp"

#include "horizon/Scene/GameObject.hpp"
#include "horizon/Scene/Mesh.hpp"

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

        // Select the newly created object by its permanent ID.
        selectedObjectId = cube.getId();
    }

    // =============================================================
    // EDITOR RENDER
    // =============================================================

    void Editor::render(World& world)
    {
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

        // Create Cube button.
        if (ImGui::Button("Create Cube"))
        {
            createCube(world);
        }

        ImGui::SameLine();

        // Delete Selected button.
        if (selectedObjectId != 0 &&
            ImGui::Button("Delete Selected"))
        {
            world.removeObject(selectedObjectId);

            selectedObjectId = 0;
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

            // ImGui uses the hidden ID to distinguish
            // objects even if their names are identical.
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

        // Find the selected object using its stable ID.
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

            ImGui::DragFloat3(
                "Position",
                &transform.position.x,
                0.05f
            );

            ImGui::DragFloat3(
                "Rotation",
                &transform.rotation.x,
                0.5f
            );

            ImGui::DragFloat3(
                "Scale",
                &transform.scale.x,
                0.05f,
                0.01f,
                100.0f
            );

            // Reset Transform.
            if (ImGui::Button("Reset Transform"))
            {
                transform.position =
                    Vec3(0.0f, 0.0f, 0.0f);

                transform.rotation =
                    Vec3(0.0f, 0.0f, 0.0f);

                transform.scale =
                    Vec3(1.0f, 1.0f, 1.0f);
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

            ImGui::ColorEdit3(
                "Base Color",
                &material.color.x
            );
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
