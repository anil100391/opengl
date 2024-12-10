cmake_minimum_required(VERSION 3.20)
project(opengl CXX)
set(CMAKE_CXX_STANDARD 17)

# for intellisense to work
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# create .lib files on windows when creating dlls
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

add_subdirectory (graphics)
add_subdirectory (utils)
add_subdirectory (gui)
