find_package(GLEW CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(Freetype CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

# undefine min/max macro from msvc minwindef.h
add_compile_options(-DNOMINMAX)

# for definition of M_PI for msvc
add_compile_options(-D_USE_MATH_DEFINES)
add_compile_options(-D_CRT_SECURE_NO_WARNINGS)

include_directories (${PROJECT_SOURCE_DIR})
include_directories (${PROJECT_SOURCE_DIR}/gui)

add_executable (text "testapp.cpp")
add_executable (plot "testplot.cpp")
add_executable (fractal "testfractal.cpp")

if (MSVC)
    target_link_libraries (text PRIVATE GLEW::GLEW glm::glm-header-only assimp::assimp Freetype::Freetype imgui::imgui oglgraphics oglutils oglapp)
    target_link_libraries (plot PRIVATE GLEW::GLEW glm::glm-header-only assimp::assimp Freetype::Freetype imgui::imgui oglgraphics oglutils oglapp)
    target_link_libraries (fractal PRIVATE GLEW::GLEW glm::glm-header-only assimp::assimp Freetype::Freetype imgui::imgui oglgraphics oglutils oglapp)
else()
    target_link_libraries (oglutils glm::glm-header-only assimp::assimp oglgraphics)
endif()

add_custom_command(TARGET text POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                   $<TARGET_FILE_DIR:text>/../graphics/oglgraphics.dll
                   $<TARGET_FILE_DIR:text>/../utils/oglutils.dll
                   $<TARGET_FILE_DIR:text>/../gui/oglapp.dll
                   $<TARGET_FILE_DIR:text>)

add_custom_command(TARGET text POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_SOURCE_DIR}/res
                   $<TARGET_FILE_DIR:text>/res)
