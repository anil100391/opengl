#include "renderer.h"
#include "vertexbuffer.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
VertexBuffer::VertexBuffer( const void* data, unsigned int size, int usage )
{
    glGenBuffers(1, &_rendererID);
    Bind();
    BufferData(data, size, usage);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void VertexBuffer::BufferData(const void* data, unsigned int size, int usage) const
{
    glBufferData(GL_ARRAY_BUFFER, size, data, usage);
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
