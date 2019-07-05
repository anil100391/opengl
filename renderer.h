#ifndef _renderer_h_
#define _renderer_h_

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#ifdef WIN
    #define ASSERT(x) if (!(x)) __debugBreak();
    #define GLCall(x) GLClearError();\
        x;\
        ASSERT(GLLogCall(#x, __FILE__, __LINE__))
#endif

void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);

#endif // _renderer_h_
