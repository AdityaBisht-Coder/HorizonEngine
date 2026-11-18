# Horizon Engine

**Horizon Engine** is an experimental, lightweight 3D game engine built with **C++20**, **OpenGL 3.3**, and **Dear ImGui**. It is being developed as a modular foundation for creating games and interactive 3D applications.

<details>
<summary><strong>Project snapshot</strong></summary>

- **Current version:** 0.14.4
- **Language:** C++20
- **Graphics API:** OpenGL 3.3 Core
- **Editor UI:** Dear ImGui
- **Build system:** CMake
- **Status:** Early development

</details>

## Project principles

Horizon Engine is being designed around these priorities:

- **Performance for everyone:** Keep the engine efficient and consider entry-level hardware from the beginning.
- **Free and accessible:** No mandatory subscriptions or paid essential engine features.
- **Modular architecture:** Build systems that can evolve independently.
- **Scalable graphics:** Keep room for different hardware capabilities and quality settings.

## Features

### Rendering and engine foundation
- Modular C++20 engine architecture
- GLFW window and platform layer
- OpenGL 3.3 Core rendering
- GLAD 2 OpenGL loader
- Vertex and fragment shader support
- 3D mesh rendering with depth testing
- Perspective camera and projection
- Frame timing and FPS reporting

### Scene system
- World containing multiple GameObjects
- Stable numeric GameObject IDs
- Per-object transforms: position, rotation, and scale
- Per-object material colour
- Reusable cube mesh support

### Scene Editor — v0.13
- Dear ImGui-based editor interface
- Scene Hierarchy listing objects in the current World
- Select objects to inspect and edit them
- Inspector for Transform and Material properties
- Create Cube and Delete Selected actions
- Edit position, rotation, and scale
- Change an object's base material colour
- Reset an object's transform

### Scene serialization and management — v0.14
- Versioned, human-readable scene file format
- Saves and loads object IDs and names
- Saves and restores position, rotation, scale, and material colour
- Saves and restores cube mesh assignment; meshless objects remain meshless
- Validates IDs, object counts, file structure, and numeric values
- Loads into a temporary World and only replaces the existing World after successful validation
- Rejects malformed, unsupported, duplicate-ID, and missing-file cases with error messages
- Editor-integrated New Scene, Save Scene, and Load Scene operations
- Scene file path input and current-scene/status display
- Unsaved-change protection with Save & Continue, Discard, and Cancel options
- Tracks scene edits such as object creation/deletion, transform changes, and material colour changes

## Development history

Progress is documented version by version. Each milestone records the implemented work and its verification status.

### v0.14.4 — Editor Scene Management
- Integrated New Scene, Save Scene, and Load Scene into the editor.
- Added a scene file path input and current scene/status display.
- Added unsaved-change safeguards before New or Load operations, with Save & Continue, Discard, and Cancel.
- Tracks object creation/deletion, transform edits, and material colour changes as unsaved scene changes.
- Save/load operations use the existing scene serializer and safe loading behaviour.
- **Verification:** Full CMake build succeeded on Windows using MSVC/NMake. Manually tested creating, saving, loading, starting a new scene, and unsaved-change prompts. Saved scenes were successfully loaded again after shutting down and restarting Horizon.
- **Git:** Implementation committed locally; push pending.

### v0.14.3 — Scene Loading and Tests
- Implemented scene loading for the existing HORIZON_SCENE 1 format.
- Restores GameObject IDs, names, transforms, material colour, and cube mesh assignments.
- Preserves automatic ID allocation after loading.
- Uses a temporary World so failed loads do not destroy the current scene.
- Rejects invalid/duplicate IDs, malformed records, unsupported mesh types, invalid numeric values, and unexpected trailing data.
- Added SceneSerializerTest, a standalone automated test executable.
- **Verification:** Full CMake build succeeded on Windows using MSVC/NMake. All 23 serializer checks passed, including save/load restoration, mesh restoration, automatic IDs, corrupted-file rejection, and preservation of existing Worlds on failure.
- **Git:** Commit 8387e47 — Horizon Engine 0.14.3: Implement scene loading and tests.

### v0.14.2 — Scene Serialization
- Added the initial SceneSerializer save system.
- Writes a versioned scene format with object count and object records.
- Serializes IDs, names, transforms, material colour, and mesh information.
- Added serializer source to the CMake build.
- **Verification:** CMake configure and full build completed successfully on Windows with MSVC/NMake.

### v0.14.1 — Stable GameObject IDs
- Added stable numeric IDs to GameObjects.
- World assigns IDs to newly created objects and supports creating objects with specified IDs.
- World checks for duplicate/invalid IDs and maintains the next ID.
- Object removal now uses an ID.
- Editor selection and deletion are ID-based, and the Inspector displays the selected object's ID.
- **Verification:** Build completed successfully on Windows with MSVC/NMake.

### v0.13.0 — First Integrated Scene Editor
- Added the Dear ImGui scene editor.
- Scene Hierarchy and object selection.
- Inspector for transforms and material colour.
- Create Cube and Delete Selected controls.
- Reset transform controls.

### Earlier development
Earlier versions established the engine's window, input, camera, shader, mesh, material, world, and rendering foundations. See the repository commit history for implementation details.

## Roadmap

Planned milestones (subject to change):

- **0.14.5:** Scene workflow stability and additional validation tests
- **0.14.6:** Basic profiler (FPS, frame time, object count, draw calls)
- **0.15:** Asset management and model importing
- **0.16:** Expanded scene editor workflows
- **0.17:** Lighting and graphics settings
- **0.18:** Physics foundation
- **0.19:** Audio system
- **0.20:** First playable game project built with Horizon

## Technology

- C++20
- CMake 3.20+
- GLFW 3.4
- OpenGL 3.3
- GLAD 2
- Dear ImGui 1.91.9b
- Microsoft Visual C++ (MSVC)

## Building on Windows

### Requirements
- Windows 10 or newer
- CMake 3.20 or newer
- Git
- Visual Studio C++ Build Tools with the MSVC compiler
- An internet connection for CMake FetchContent dependencies on the first configure

### Build
Open an x64 Native Tools Command Prompt for Visual Studio, then run:

```bat
git clone https://github.com/AdityaBisht-Coder/HorizonEngine.git
cd HorizonEngine

cmake -S . -B build -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

### Run
```bat
build\\Horizon.exe
```

### Run the scene serializer tests
```bat
build\\SceneSerializerTest.exe
```

## Development status

Horizon Engine is an active experimental project. Version 0.14 focuses on scene data foundations and editor scene workflows: stable object IDs, serialization, loading, validation, and scene management. APIs, editor workflows, and internal architecture may change as development continues.

This is not yet a production-ready game engine. The current desktop renderer uses OpenGL 3.3 and GLFW; mobile support is a future architectural goal, not an implemented export target.

## Licensing

The repository currently includes the MIT License in LICENSE.

A separate Horizon Fair Use License (HFUL) draft is also available for discussion and legal review. It is a proposal only and does not replace or modify the current MIT license.

## Repository

[GitHub: AdityaBisht-Coder/HorizonEngine](https://github.com/AdityaBisht-Coder/HorizonEngine)
