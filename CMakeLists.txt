cmake_minimum_required(VERSION 3.15)
project(opengl CXX)
set (CMAKE_CXX_STANDARD 17)

include(cmake/StandardProjectSettings.cmake)

# Link this 'library' to set the c++ standard / compile-time options requested
add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_17)

if(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
  option(ENABLE_BUILD_WITH_TIME_TRACE "Enable -ftime-trace to generate time tracing .json files on clang" OFF)
  if (ENABLE_BUILD_WITH_TIME_TRACE)
    add_compile_definitions(project_options INTERFACE -ftime-trace)
  endif()
endif()

# Link this 'library' to use the warnings specified in CompilerWarnings.cmake
add_library(project_warnings INTERFACE)

# standard compiler warnings
include(cmake/CompilerWarnings.cmake)
set_project_warnings(project_warnings)

# sanitizer options if supported by compiler
include(cmake/Sanitizers.cmake)
enable_sanitizers(project_options)

# enable doxygen
include(cmake/Doxygen.cmake)
enable_doxygen()

# allow for static analysis options
include(cmake/StaticAnalyzers.cmake)

option(BUILD_SHARED_LIBS "Enable compilation of shared libraries" OFF)
option(ENABLE_TESTING "Enable Test Builds" ON)

# Very basic PCH example
option(ENABLE_PCH "Enable Precompiled Headers" OFF)
if (ENABLE_PCH)
  # This sets a global PCH parameter, each project will build its own PCH, which
  # is a good idea if any #define's change
  #
  # consider breaking this out per project as necessary
  target_precompile_headers(project_options INTERFACE <vector> <string> <map> <utility>)
endif()

# for youcompleteme to work
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories ("${PROJECT_SOURCE_DIR}/vendor/imgui")
include_directories ("${PROJECT_SOURCE_DIR}/vendor/glm")
include_directories ("${PROJECT_SOURCE_DIR}/vendor/stb_image")
file (GLOB tests_src "${PROJECT_SOURCE_DIR}/tests/*.cpp" )
file (GLOB utils_src "${PROJECT_SOURCE_DIR}/utils/*.cpp" )
file (GLOB imgui_src "${PROJECT_SOURCE_DIR}/vendor/imgui/*.cpp" )
file (GLOB main_src "${PROJECT_SOURCE_DIR}/*.cpp" )

add_executable (app vendor/stb_image/stb_image.cpp ${imgui_src} ${utils_src} ${tests_src} ${main_src})

target_link_libraries (app GL glfw GLEW)
