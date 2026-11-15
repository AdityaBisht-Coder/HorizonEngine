
cmake_minimum_required(VERSION 3.20)

project(HorizonEngine VERSION 0.4.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

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

add_executable(Horizon
    src/main.cpp
    external/glad/src/gl.c
)

target_include_directories(Horizon PRIVATE
    external/glad/include
)

target_link_libraries(Horizon PRIVATE
    glfw
    OpenGL::GL
)

if(MSVC)
    target_compile_options(Horizon PRIVATE /W4)
endif()

message(STATUS "Horizon Engine 0.4 configuration complete!")
