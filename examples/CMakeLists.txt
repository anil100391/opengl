find_package(GLEW CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(Freetype CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

file (GLOB main_src "${PROJECT_SOURCE_DIR}/examples/*.cpp" )

# undefine min/max macro from msvc minwindef.h
add_compile_options(-DNOMINMAX)

# for definition of M_PI for msvc
add_compile_options(-D_USE_MATH_DEFINES)
add_compile_options(-D_CRT_SECURE_NO_WARNINGS)

include_directories (${PROJECT_SOURCE_DIR})
include_directories (${PROJECT_SOURCE_DIR}/gui)

add_executable (main ${main_src})

if (MSVC)
    target_link_libraries (main PRIVATE GLEW::GLEW glm::glm-header-only assimp::assimp Freetype::Freetype imgui::imgui oglgraphics oglutils oglapp)
else()
    target_link_libraries (oglutils glm::glm-header-only assimp::assimp oglgraphics)
endif()

add_custom_command(TARGET main POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                   $<TARGET_FILE_DIR:main>/../graphics/oglgraphics.dll
                   $<TARGET_FILE_DIR:main>/../utils/oglutils.dll
                   $<TARGET_FILE_DIR:main>/../gui/oglapp.dll
                   $<TARGET_FILE_DIR:main>)

add_custom_command(TARGET main POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_SOURCE_DIR}/res
                   $<TARGET_FILE_DIR:main>/res)
