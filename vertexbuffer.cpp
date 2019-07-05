#include "renderer.h"
#include "vertexbuffer.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
VertexBuffer::VertexBuffer( const void* data, unsigned int size )
{
    glGenBuffers(1, &_rendererID);
    glBindBuffer(GL_ARRAY_BUFFER, _rendererID);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
VertexBuffer::~VertexBuffer()
{
    glDeleteBuffers(1, &_rendererID);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void VertexBuffer::Bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, _rendererID);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void VertexBuffer::Unbind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
