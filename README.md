# Horizon Engine

**Horizon Engine** is an experimental, lightweight 3D game engine built with **C++20**, **OpenGL 3.3**, and **Dear ImGui**. It is being developed as a modular foundation for creating games and interactive 3D applications.

<details>
<summary><strong>Project snapshot</strong></summary>

- **Current version:** 0.13.0
- **Language:** C++20
- **Graphics API:** OpenGL 3.3 Core
- **Editor UI:** Dear ImGui
- **Build system:** CMake
- **Status:** Early development

</details>

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

Horizon Engine is an active experimental project. Version 0.13 introduces the first integrated Scene Editor. Engine APIs, editor workflows, and internal architecture may change as development continues.

This is not yet a production-ready game engine.

## Licensing

The repository currently includes the **MIT License** in `LICENSE`.

A separate **Horizon Fair Use License (HFUL)** draft is also available for discussion and legal review. It is a proposal only and does not replace or modify the current MIT license.

## Repository

[GitHub: AdityaBisht-Coder/HorizonEngine](https://github.com/AdityaBisht-Coder/HorizonEngine)
