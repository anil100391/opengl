#ifndef _vertexbuffer_h_
#define _vertexbuffer_h_

#include "renderer.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class VertexBuffer
{
public:

    VertexBuffer( const void* data, unsigned int size, int usage = GL_STATIC_DRAW);
    ~VertexBuffer();

    void Bind() const;
    void Unbind() const;
    void BufferData(const void* data, unsigned int size, int usage = GL_STATIC_DRAW) const;

private:

    unsigned int    _rendererID = 0;
};

#endif // _vertexbuffer_h_
