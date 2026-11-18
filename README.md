# Horizon Engine

**Horizon Engine** is an experimental, lightweight 3D game engine built with **C++20**, **OpenGL 3.3**, and **Dear ImGui**. It is being developed as a modular foundation for creating games and interactive 3D applications.

<details>
<summary><strong>Project snapshot</strong></summary>

- **Current version:** 0.14.5
- **Language:** C++20
- **Graphics API:** OpenGL 3.3 Core
- **Editor UI:** Dear ImGui
- **Build system:** CMake
- **Status:** Early development

</details>

## Project principles

- **Performance for everyone:** Keep the engine efficient and consider entry-level hardware.
- **Free and accessible:** No mandatory subscriptions or paid essential engine features.
- **Modular architecture:** Build systems that can evolve independently.
- **Scalable graphics:** Leave room for different hardware capabilities and quality settings.

## Features

### Engine foundation
- Modular C++20 engine architecture
- GLFW window and platform layer
- OpenGL 3.3 Core rendering
- GLAD 2 OpenGL loader
- Vertex and fragment shaders
- 3D mesh rendering with depth testing
- Perspective camera and projection
- Frame timing and FPS reporting
- Keyboard and mouse input

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

### Scene Serialization — v0.14
- Versioned, human-readable scene file format
- Saves and loads object IDs and names
- Saves and restores position, rotation, scale, and material colour
- Saves and restores cube mesh assignments
- Validates IDs, object counts, file structure, and numeric values
- Loads into a temporary World before replacing the current World
- Rejects malformed, unsupported, duplicate-ID, and missing-file cases
- Automated SceneSerializer tests

### Scene Browser & Storage — v0.14.5
- Automatically creates Horizon's user data directories
- Stores scenes in the user's Local AppData directory
- Automatically discovers saved `.hscene` files
- Scene Browser list inside the Editor
- Refresh saved scene list
- Select and load saved scenes
- Create new scenes
- Save scenes using scene names
- Save As support
- Prevents accidental overwriting of existing scenes
- Unsaved-change protection with Save & Continue, Discard, and Cancel
- Tracks scene edits including object creation, deletion, transforms, and material changes

Default scene storage location:

`%LOCALAPPDATA%\HorizonEngine\Scenes\`

## Development history

### v0.14.5 — Scene Browser & Automatic Storage
- Added SceneStorage for automatic user data directory management.
- Automatically creates the Horizon Engine storage and Scenes directories.
- Added saved scene discovery and listing.
- Integrated Scene Browser into the Editor.
- Added scene selection, loading, refreshing, New Scene, Save Scene, and Save As workflows.
- Scene names are validated to prevent invalid filenames and path traversal.
- Added SceneStorageTest.
- **Verification:** Full CMake build succeeded using MSVC/NMake on Windows. SceneStorageTest passed all 17 checks. Manually verified scene creation, saving, loading, editor workflow, and persistence after restarting the engine.

### v0.14.4 — Editor Scene Management
- Integrated New Scene, Save Scene, and Load Scene into the editor.
- Added scene path input and current scene/status display.
- Added unsaved-change safeguards before New or Load operations.
- Tracks object creation/deletion, transform edits, and material changes.
- **Verification:** Build succeeded. Manually tested scene saving, loading, restarting, and unsaved-change prompts.

### v0.14.3 — Scene Loading and Tests
- Implemented scene loading for the HORIZON_SCENE 1 format.
- Restores GameObject IDs, names, transforms, material colour, and cube mesh assignments.
- Preserves automatic ID allocation after loading.
- Uses a temporary World so failed loads do not destroy the current scene.
- Rejects invalid IDs, malformed records, unsupported mesh types, invalid numeric values, and unexpected trailing data.
- Added SceneSerializerTest.
- **Verification:** All 23 serializer checks passed.

### v0.14.2 — Scene Serialization
- Added the initial SceneSerializer save system.
- Writes a versioned scene format with object count and object records.
- Serializes IDs, names, transforms, material colour, and mesh information.

### v0.14.1 — Stable GameObject IDs
- Added stable numeric IDs to GameObjects.
- World assigns IDs to newly created objects and supports creating objects with specified IDs.
- World checks for duplicate/invalid IDs and maintains the next ID.
- Object removal now uses an ID.
- Editor selection and deletion are ID-based.

### v0.13.0 — First Integrated Scene Editor
- Added the Dear ImGui scene editor.
- Scene Hierarchy and object selection.
- Inspector for transforms and material colour.
- Create Cube and Delete Selected controls.
- Reset transform controls.

### Earlier development
Earlier versions established the engine's window, input, camera, shader, mesh, material, world, and rendering foundations.

## Roadmap

Planned milestones (subject to change):

- **0.14.6:** Basic profiler — FPS, frame time, object count, and draw calls
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
- Visual Studio C++ Build Tools with MSVC
- Internet connection for CMake FetchContent dependencies on first configure

### Build

Open an x64 Native Tools Command Prompt for Visual Studio:

```bat
git clone https://github.com/AdityaBisht-Coder/HorizonEngine.git
cd HorizonEngine

cmake -S . -B build -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

### Run

```bat
build\Horizon.exe
```

### Run tests

```bat
build\SceneSerializerTest.exe
build\SceneStorageTest.exe
```

## Development status

Horizon Engine is an active experimental project. Version 0.14 focuses on scene serialization, validation, storage, and editor scene workflows.

This is **not yet a production-ready game engine**. APIs, editor workflows, and internal architecture may change as development continues. The current renderer uses OpenGL 3.3 and GLFW. Mobile support is a future architectural goal, not an implemented export target.

## Licensing

The repository currently includes the MIT License in LICENSE.

A separate Horizon Fair Use License (HFUL) draft is available for discussion and legal review. It is a proposal only and does not replace or modify the current MIT license.

## Repository

[GitHub: AdityaBisht-Coder/HorizonEngine](https://github.com/AdityaBisht-Coder/HorizonEngine)
