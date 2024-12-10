find_package(GLEW REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)

file (GLOB utils_src "${PROJECT_SOURCE_DIR}/utils/*.cpp" )

# undefine min/max macro from msvc minwindef.h
add_compile_options(-DNOMINMAX)

# for definition of M_PI for msvc
add_compile_options(-D_USE_MATH_DEFINES)
add_compile_options(-D_CRT_SECURE_NO_WARNINGS)

include_directories (${PROJECT_SOURCE_DIR})

add_library (oglutils SHARED ${utils_src})

if (MSVC)
    target_link_libraries (oglutils PRIVATE GLEW::GLEW glm::glm-header-only assimp::assimp oglgraphics)
else()
    target_link_libraries (oglutils glm::glm-header-only assimp::assimp oglgraphics)
endif()
