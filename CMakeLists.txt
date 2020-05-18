cmake_minimum_required(VERSION 3.15)
project(opengl CXX)
set (CMAKE_CXX_STANDARD 17)

# Link this 'library' to use the warnings specified in CompilerWarnings.cmake
add_library(project_warnings INTERFACE)

# standard compiler warnings
include(cmake/CompilerWarnings.cmake)
set_project_warnings(project_warnings)

# sanitizer options if supported by compiler
include(cmake/Sanitizers.cmake)
enable_sanitizers(project_options)

# allow for static analysis options
include(cmake/StaticAnalyzers.cmake)

# for youcompleteme to work
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories ("${PROJECT_SOURCE_DIR}/vendor/imgui")
include_directories ("${PROJECT_SOURCE_DIR}/vendor/glm")
include_directories ("${PROJECT_SOURCE_DIR}/vendor/stb_image")

if (MSVC)
    include_directories("C:/dev/vcpkg/installed/x86-windows/include")
    link_directories("C:/dev/vcpkg/installed/x86-windows/lib")
endif()

file (GLOB tests_src "${PROJECT_SOURCE_DIR}/tests/*.cpp" )
file (GLOB utils_src "${PROJECT_SOURCE_DIR}/utils/*.cpp" )
file (GLOB imgui_src "${PROJECT_SOURCE_DIR}/vendor/imgui/*.cpp" )
file (GLOB main_src "${PROJECT_SOURCE_DIR}/*.cpp" )


add_executable (app vendor/stb_image/stb_image.cpp ${imgui_src} ${utils_src} ${tests_src} ${main_src})

if (MSVC)
    target_link_libraries (app glfw3dll.lib glew32.lib opengl32.lib)
else()
    target_link_libraries (app GL glfw GLEW)
endif()
