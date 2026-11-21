
cmake_minimum_required(VERSION 3.20)

project(HorizonEngine VERSION 0.14.6 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
include(CTest)

# =====================================
# GLFW
# =====================================

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
)

FetchContent_MakeAvailable(glfw)

find_package(OpenGL REQUIRED)

# =====================================
# DEAR IMGUI
# =====================================

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.91.9b
)

FetchContent_GetProperties(imgui)

if(NOT imgui_POPULATED)
    FetchContent_Populate(imgui)
endif()

add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp

    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(imgui PUBLIC
    glfw
    OpenGL::GL
)

# =====================================
# HORIZON ENGINE
# =====================================

add_executable(Horizon
    src/main.cpp

    # Engine Core
    src/Core/Engine.cpp

    # Editor
    src/Editor/Editor.cpp

    # Platform
    src/Platform/Window.cpp

    # Input
    src/Input/Input.cpp

    # Camera
    src/Camera/Camera.cpp

    # Rendering
    src/Renderer/Shader.cpp
    src/Renderer/Renderer.cpp

    # OpenGL loader
    external/glad/src/gl.c

    # Scene System
    src/Scene/Transform.cpp
    src/Scene/GameObject.cpp
    src/Scene/World.cpp
    src/Scene/Mesh.cpp
    src/Scene/SceneSerializer.cpp
    src/Scene/SceneStorage.cpp
)

target_include_directories(Horizon PRIVATE
    include
    external/glad/include
)

target_link_libraries(Horizon PRIVATE
    glfw
    OpenGL::GL
    imgui
)

if(MSVC)
    target_compile_options(Horizon PRIVATE /W4)
endif()

# =====================================
# WINDOW TEST
# =====================================

add_executable(WindowTest
    tests/WindowTest.cpp
    src/Platform/Window.cpp
)

target_include_directories(WindowTest PRIVATE
    include
)

target_link_libraries(WindowTest PRIVATE
    glfw
    OpenGL::GL
)

# =====================================
# SHADER TEST
# =====================================

add_executable(ShaderTest
    tests/ShaderTest.cpp
    src/Platform/Window.cpp
    src/Renderer/Shader.cpp
    external/glad/src/gl.c
)

target_include_directories(ShaderTest PRIVATE
    include
    external/glad/include
)

target_link_libraries(ShaderTest PRIVATE
    glfw
    OpenGL::GL
)

# =====================================
# INPUT TEST
# =====================================

add_executable(InputTest
    tests/InputTest.cpp
    src/Platform/Window.cpp
    src/Input/Input.cpp
)

target_include_directories(InputTest PRIVATE
    include
)

target_link_libraries(InputTest PRIVATE
    glfw
    OpenGL::GL
)

# =====================================
# CAMERA TEST
# =====================================

add_executable(CameraTest
    tests/CameraTest.cpp
    src/Camera/Camera.cpp
)

target_include_directories(CameraTest PRIVATE
    include
)

# =====================================
# SCENE SERIALIZER TEST
# =====================================

add_executable(SceneSerializerTest
    tests/SceneSerializerTest.cpp

    src/Scene/Transform.cpp
    src/Scene/GameObject.cpp
    src/Scene/World.cpp
    src/Scene/Mesh.cpp
    src/Scene/SceneSerializer.cpp
)

target_include_directories(SceneSerializerTest PRIVATE
    include
)

# =====================================
# SCENE STORAGE TEST
# =====================================

add_executable(SceneStorageTest
    tests/SceneStorageTest.cpp
    src/Scene/SceneStorage.cpp
)

target_include_directories(SceneStorageTest PRIVATE
    include
)

# =====================================
# AUTOMATED TESTING - CTEST
# =====================================

if(BUILD_TESTING)

    add_test(
        NAME CameraTest
        COMMAND CameraTest
    )

    add_test(
        NAME SceneSerializerTest
        COMMAND SceneSerializerTest
    )

    add_test(
        NAME SceneStorageTest
        COMMAND SceneStorageTest
    )

endif()

# =====================================
# CONFIGURATION COMPLETE
# =====================================

message(STATUS "Horizon Engine configuration complete!")
