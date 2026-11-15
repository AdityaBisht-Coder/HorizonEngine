
# Horizon Engine

An experimental lightweight 3D game engine written in C++20 and OpenGL.

## About

Horizon Engine is an independent game engine project focused on
lightweight 3D rendering, modular architecture and performance.

The long-term goal is to provide a reusable foundation for
building games and interactive 3D applications.

## Current Features

- C++20 foundation
- GLFW window management
- OpenGL 3.3 Core
- GLAD 2 integration
- Vertex and fragment shaders
- 3D mesh rendering
- Depth testing
- Perspective projection
- Frame timing and FPS counter

## Technology

- C++20
- CMake
- GLFW 3.4
- OpenGL 3.3
- GLAD 2
- Microsoft Visual C++

## Current Status

Early development. APIs and architecture may change.

## Building on Windows

Requirements:

- CMake 3.20 or newer
- Git
- Visual Studio C++ Build Tools
- Windows 10 or newer

Open an x64 Developer Command Prompt:

```bat
cmake -S . -B build -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release

cmake --build build

build\Horizon.exe
```

## License

See LICENSE.
