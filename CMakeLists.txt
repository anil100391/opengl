cmake_minimum_required(VERSION 3.13)
project(opengl CXX)
set (CMAKE_CXX_STANDARD 17)

# for youcompleteme to work
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
if (MSVC)
find_package(freetype CONFIG REQUIRED)
else ()
    include_directories ("/usr/include/freetype2")
endif()

include_directories ("${PROJECT_SOURCE_DIR}/vendor/imgui")
include_directories ("${PROJECT_SOURCE_DIR}/vendor/glm")
include_directories ("${PROJECT_SOURCE_DIR}/vendor/stb_image")
include_directories ("${PROJECT_SOURCE_DIR}/vendor/irrKlang/include")
if (MSVC)
	include_directories ("${PROJECT_SOURCE_DIR}/vendor/assimp/include")
	link_directories ("${PROJECT_SOURCE_DIR}/vendor/assimp/lib")
endif()

link_directories ("${PROJECT_SOURCE_DIR}/vendor/irrKlang/lib")

file (GLOB tests_src "${PROJECT_SOURCE_DIR}/tests/*.cpp" )
file (GLOB chess_src "${PROJECT_SOURCE_DIR}/tests/chess/board.cpp" "${PROJECT_SOURCE_DIR}/tests/chess/zobristkeys.cpp" )
file (GLOB ludo_src "${PROJECT_SOURCE_DIR}/tests/ludo/*.cpp" )
file (GLOB utils_src "${PROJECT_SOURCE_DIR}/utils/*.cpp" )
file (GLOB imgui_src "${PROJECT_SOURCE_DIR}/vendor/imgui/*.cpp" )
file (GLOB main_src "${PROJECT_SOURCE_DIR}/*.cpp" )

# undefine min/max macro from msvc minwindef.h
add_compile_options(-DNOMINMAX)

# for definition of M_PI for msvc
add_compile_options(-D_USE_MATH_DEFINES)
add_compile_options(-D_CRT_SECURE_NO_WARNINGS)

add_executable (app vendor/stb_image/stb_image.cpp ${imgui_src} ${utils_src} ${chess_src} ${ludo_src} ${tests_src} ${main_src})

if (MSVC)
    target_link_libraries (app PRIVATE glfw GLEW::GLEW opengl32 freetype irrKlang.lib assimp-vc142-mt.lib)
else()
    target_link_libraries (app GL glfw GLEW freetype IrrKlang assimp)
endif()

# copy resources to bin directory
add_custom_command(TARGET app POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/res $<TARGET_FILE_DIR:app>/res)

add_custom_command(TARGET app POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/vendor/irrKlang/bin $<TARGET_FILE_DIR:app>)

add_custom_command(TARGET app POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/vendor/assimp/bin $<TARGET_FILE_DIR:app>)

