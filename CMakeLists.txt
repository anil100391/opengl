cmake_minimum_required (VERSION 3.1)
project (opengl)

set (CMAKE_CXX_STANDARD 17)

include_directories ("${PROJECT_SOURCE_DIR}/vendor/imgui")
include_directories ("${PROJECT_SOURCE_DIR}/vendor/glm")
include_directories ("${PROJECT_SOURCE_DIR}/vendor/stb_image")

file (GLOB tests_src "${PROJECT_SOURCE_DIR}/tests/*.cpp" )
file (GLOB utils_src "${PROJECT_SOURCE_DIR}/utils/*.cpp" )
file (GLOB imgui_src "${PROJECT_SOURCE_DIR}/vendor/imgui/*.cpp" )

add_executable (app vendor/stb_image/stb_image.cpp ${imgui_src} ${utils_src} ${tests_src} texture.cpp renderer.cpp vertexarray.cpp indexbuffer.cpp vertexbuffer.cpp shader.cpp app.cpp)

target_link_libraries (app GL glfw GLEW)
