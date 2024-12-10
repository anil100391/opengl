find_package(GLEW REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

file (GLOB event_src "${PROJECT_SOURCE_DIR}/gui/events/*.cpp")
file (GLOB gui_src "${PROJECT_SOURCE_DIR}/gui/*.cpp")

# undefine min/max macro from msvc minwindef.h
add_compile_options(-DNOMINMAX)

# for definition of M_PI for msvc
add_compile_options(-D_USE_MATH_DEFINES)
add_compile_options(-D_CRT_SECURE_NO_WARNINGS)

include_directories("${PROJECT_SOURCE_DIR}/gui")
add_library (oglapp SHARED ${event_src} ${gui_src})

if (MSVC)
    target_link_libraries (oglapp PRIVATE GLEW::GLEW glm::glm-header-only glfw oglgraphics oglutils)
else()
    target_link_libraries (oglapp glm::glm-header-only glfw oglgraphics oglutils)
endif()
