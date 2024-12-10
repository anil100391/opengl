find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm CONFIG REQUIRED)

include_directories ("${PROJECT_SOURCE_DIR}/vendor/stb_image")

file (GLOB graphics_src "${PROJECT_SOURCE_DIR}/graphics/*.cpp" )

# undefine min/max macro from msvc minwindef.h
add_compile_options(-DNOMINMAX)

# for definition of M_PI for msvc
add_compile_options(-D_USE_MATH_DEFINES)
add_compile_options(-D_CRT_SECURE_NO_WARNINGS)

add_library (oglgraphics SHARED ${graphics_src})

if (MSVC)
    target_link_libraries (oglgraphics PRIVATE opengl32 GLEW::GLEW glm::glm-header-only)
else()
    target_link_libraries (oglgraphics GL GLEW glm::glm-header-only)
endif()
