#include "renderer.h"
#include <iostream>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void GLClearError()
{
    while ( glGetError() != GL_NO_ERROR )
    {
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
bool GLLogCall(const char* function, const char* file, int line)
{
    while(GLenum error = glGetError())
    {
        std::cout << "[OpenGL Error] (" << error << "):" << function << " " << file << ": " << line << "\n";
        return false;
    }

    return true;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void Renderer::Clear() const
{
    glClear(GL_COLOR_BUFFER_BIT);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void Renderer::Draw(const VertexArray& va, const IndexBuffer &ib, const Shader &shader) const
{
    va.Bind();
    ib.Bind();
    shader.Bind();
    glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr);
}
