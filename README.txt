# Horizon Engine 0.13 Editor Patch

## Included replacements
- `include/horizon/Engine.hpp`
- `src/Core/Engine.cpp`
- `include/horizon/Editor/Editor.hpp` (new)
- `src/Editor/Editor.cpp` (new)
- `src/Scene/World.cpp`
- `CMakeLists.txt`

## Install
1. Back up your current project.
2. Extract this archive into `C:\ProjectHorizon`, preserving the folder structure.
3. Allow the included files to replace the existing files.
4. From a Developer Command Prompt:

   ```bat
   cd /d C:\ProjectHorizon
   cmake -S . -B build -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release
   cmake --build build
   build\Horizon.exe
   ```

## Controls
- Cursor starts released for editor interaction.
- `TAB`: capture/release cursor.
- `WASD`, mouse movement and scroll: camera controls while cursor is captured.
- `ESC`: exit.

## Editor features in this milestone
- Scene Hierarchy window
- Create Cube
- Delete Selected
- Select scene objects
- Transform position, rotation and scale editing
- Reset Transform
- Material base-colour editing

This patch is source-only. It has not been compiled against the local checkout,
so run the build and share the first compiler error (if any) before proceeding.
