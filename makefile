STB_IMAGE_INCLUDE_DIR=vendor/stb_image
GLM_INCLUDE_DIR=vendor/glm
IMGUI_INCLUDE_DIR=vendor/imgui

all:
	g++ vendor/stb_image/stb_image.cpp vendor/imgui/*.cpp tests/*.cpp texture.cpp renderer.cpp vertexarray.cpp indexbuffer.cpp vertexbuffer.cpp shader.cpp app.cpp -lGL -lglfw -lGLEW -I${STB_IMAGE_INCLUDE_DIR} -I${GLM_INCLUDE_DIR} -I${IMGUI_INCLUDE_DIR} -std=c++17
