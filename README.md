# Horizon Engine

**Horizon Engine** is an experimental, lightweight 3D game engine built with **C++20**, **OpenGL 3.3**, and **Dear ImGui**. It is being developed as a modular foundation for creating games and interactive 3D applications.

<details>
<summary><strong>Project snapshot</strong></summary>

- **Current version:** 0.14.2
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

### Scene serialization — v0.14.2
- Initial scene-saving/serialization system
- Versioned, human-readable scene file format
- Saves object IDs and names
- Saves position, rotation, scale, and material colour
- Records mesh assignment/presence
- Validates saved numeric transform and colour values
- Scene loading and editor-integrated save/load workflow are not implemented yet

## Development history

Progress is documented version by version. Each milestone records the implemented work and its verification status.

### v0.14.2 — Scene Serialization
- Added the initial `SceneSerializer` save system.
- Writes a versioned scene format with object count and object records.
- Serializes IDs, names, transforms, material colour, and mesh information.
- Added serializer source to the CMake build.
- **Verification:** CMake configure and full build completed successfully on Windows with MSVC/NMake. Runtime save/load validation is still pending; this version implements saving only.

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

- **0.14.3:** Scene loading
- **0.14.x:** Editor-integrated scene save/load and stability improvements
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

Open an **x64 Native Tools Command Prompt for Visual Studio**, then run:

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

## Development status

Horizon Engine is an active experimental project. Version 0.14 focuses on scene data foundations: stable object IDs and serialization. APIs, editor workflows, and internal architecture may change as development continues.

This is not yet a production-ready game engine. The current desktop renderer uses OpenGL 3.3 and GLFW; mobile support is a future architectural goal, not an implemented export target.

## Licensing

The repository currently includes the **MIT License** in `LICENSE`.

A separate **Horizon Fair Use License (HFUL)** draft is also available for discussion and legal review. It is a proposal only and does not replace or modify the current MIT license.

## Repository

[GitHub: AdityaBisht-Coder/HorizonEngine](https://github.com/AdityaBisht-Coder/HorizonEngine)
