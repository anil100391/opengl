STB_IMAGE_INCLUDE_DIR=vendor/stb_image
GLM_INCLUDE_DIR=vendor/glm

all:
	g++ vendor/stb_image/stb_image.cpp texture.cpp renderer.cpp vertexbufferlayout.cpp vertexarray.cpp indexbuffer.cpp vertexbuffer.cpp shader.cpp app.cpp -lGL -lglfw -lGLEW -I${STB_IMAGE_INCLUDE_DIR} -I${GLM_INCLUDE_DIR}

