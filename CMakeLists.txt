cmake_minimum_required(VERSION 3.13)
project(opengl CXX)
set (CMAKE_CXX_STANDARD 17)

# for youcompleteme to work
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(freetype CONFIG REQUIRED)

include_directories ("${PROJECT_SOURCE_DIR}/vendor/imgui")
include_directories ("${PROJECT_SOURCE_DIR}/vendor/glm")
include_directories ("${PROJECT_SOURCE_DIR}/vendor/stb_image")
include_directories ("${PROJECT_SOURCE_DIR}/vendor/irrKlang/include")

link_directories ("${PROJECT_SOURCE_DIR}/vendor/irrKlang/lib")

file (GLOB tests_src "${PROJECT_SOURCE_DIR}/tests/*.cpp" )
file (GLOB chess_src "${PROJECT_SOURCE_DIR}/tests/chess/board.cpp" "${PROJECT_SOURCE_DIR}/tests/chess/zobristkeys.cpp" )
file (GLOB utils_src "${PROJECT_SOURCE_DIR}/utils/*.cpp" )
file (GLOB imgui_src "${PROJECT_SOURCE_DIR}/vendor/imgui/*.cpp" )
file (GLOB main_src "${PROJECT_SOURCE_DIR}/*.cpp" )

# undefine min/max macro from msvc minwindef.h
add_compile_options(-DNOMINMAX)

# for definition of M_PI for msvc
add_compile_options(-D_USE_MATH_DEFINES)

add_executable (app vendor/stb_image/stb_image.cpp ${imgui_src} ${utils_src} ${chess_src} ${tests_src} ${main_src})

if (MSVC)
    target_link_libraries (app PRIVATE glfw GLEW::GLEW opengl32 freetype irrKlang.lib)
else()
    target_link_libraries (app GL glfw GLEW IrrKlang)
endif()

