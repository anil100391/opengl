#include "renderer.h"
#include "indexbuffer.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
IndexBuffer::IndexBuffer( const unsigned int* data, unsigned int count )
    : _count(count)
{
    glGenBuffers(1, &_rendererID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _rendererID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
IndexBuffer::~IndexBuffer()
{
    glDeleteBuffers(1, &_rendererID);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void IndexBuffer::Bind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _rendererID);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void IndexBuffer::Unbind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
unsigned int IndexBuffer::GetCount() const
{
    return _count;
}

