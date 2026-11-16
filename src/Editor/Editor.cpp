#include "horizon/Editor/Editor.hpp"

#include "horizon/Scene/GameObject.hpp"
#include "horizon/Scene/Mesh.hpp"

#include "imgui.h"

#include <string>

namespace horizon
{
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

        if (!cubeMesh)
        {
            cubeMesh = Mesh::createCube();
        }

        if (!cubeMesh)
        {
            return;
        }

        // Generate a name that is unique in the current World.
        std::string objectName = "Cube";
        unsigned int suffix = 1;

        const auto nameExists = [&world](const std::string& name)
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
            objectName = "Cube_" + std::to_string(suffix);
            ++suffix;
        }

        GameObject& cube = world.createObject(objectName);

        cube.getTransform().position = Vec3(0.0f, 0.0f, -2.0f);
        cube.getTransform().rotation = Vec3(0.0f, 0.0f, 0.0f);
        cube.getTransform().scale = Vec3(0.75f, 0.75f, 0.75f);

        cube.getMaterial().color = Vec3(0.15f, 0.55f, 1.0f);
        cube.setMesh(cubeMesh);

        selectedObjectName = objectName;
    }

    void Editor::render(World& world)
    {
        // ---------------------------------------------------------
        // Scene Hierarchy
        // ---------------------------------------------------------

        ImGui::SetNextWindowSize(
            ImVec2(270.0f, 420.0f),
            ImGuiCond_FirstUseEver
        );

        ImGui::Begin("Scene Hierarchy");

        ImGui::Text(
            "Objects: %llu",
            static_cast<unsigned long long>(world.getObjectCount())
        );

        if (ImGui::Button("Create Cube"))
        {
            createCube(world);
        }

        ImGui::SameLine();

        if (!selectedObjectName.empty() &&
            ImGui::Button("Delete Selected"))
        {
            world.removeObject(selectedObjectName);
            selectedObjectName.clear();
        }

        ImGui::Separator();

        if (world.getObjectCount() == 0)
        {
            ImGui::TextDisabled("The scene is empty.");
        }

        for (const auto& object : world.getObjects())
        {
            if (!object)
            {
                continue;
            }

            const std::string& name = object->getName();

            if (ImGui::Selectable(
                    name.c_str(),
                    selectedObjectName == name))
            {
                selectedObjectName = name;
            }
        }

        ImGui::End();

        // ---------------------------------------------------------
        // Inspector
        // ---------------------------------------------------------

        ImGui::SetNextWindowSize(
            ImVec2(320.0f, 440.0f),
            ImGuiCond_FirstUseEver
        );

        ImGui::Begin("Inspector");

        GameObject* selectedObject = nullptr;

        for (const auto& object : world.getObjects())
        {
            if (object && object->getName() == selectedObjectName)
            {
                selectedObject = object.get();
                break;
            }
        }

        if (selectedObject == nullptr)
        {
            ImGui::TextDisabled("Select an object in the hierarchy.");
            ImGui::End();
            return;
        }

        ImGui::Text("GameObject");
        ImGui::Separator();
        ImGui::Text("%s", selectedObject->getName().c_str());

        if (ImGui::CollapsingHeader(
                "Transform",
                ImGuiTreeNodeFlags_DefaultOpen))
        {
            auto& transform = selectedObject->getTransform();

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

            if (ImGui::Button("Reset Transform"))
            {
                transform.position = Vec3(0.0f, 0.0f, 0.0f);
                transform.rotation = Vec3(0.0f, 0.0f, 0.0f);
                transform.scale = Vec3(1.0f, 1.0f, 1.0f);
            }
        }

        if (ImGui::CollapsingHeader(
                "Material",
                ImGuiTreeNodeFlags_DefaultOpen))
        {
            auto& material = selectedObject->getMaterial();

            ImGui::ColorEdit3(
                "Base Color",
                &material.color.x
            );
        }

        ImGui::Separator();

        ImGui::Text(
            "Mesh: %s",
            selectedObject->getMesh() ? "Assigned" : "None"
        );

        ImGui::End();
    }
}
