all:
	g++ vendor/stb_image/stb_image.cpp texture.cpp renderer.cpp vertexbufferlayout.cpp vertexarray.cpp indexbuffer.cpp vertexbuffer.cpp shader.cpp app.cpp -lGL -lglfw -lGLEW

