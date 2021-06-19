#include "vertexarray.h"
#include "vertexbuffer.h"
#include "vertexbufferlayout.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
VertexArray::VertexArray()
{
    glGenVertexArrays(1, &_rendererID);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
VertexArray::~VertexArray()
{
    glDeleteVertexArrays(1, &_rendererID);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void VertexArray::Bind() const
{
    glBindVertexArray(_rendererID);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void VertexArray::Unbind() const
{
    glBindVertexArray(0);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout &layout)
{
    Bind();
    vb.Bind();
    const auto& elements = layout.GetElements();
    unsigned int offset = 0;
    for ( unsigned int ii = 0; ii < elements.size(); ++ii )
    {
        const auto& element = elements[ii];
        glEnableVertexAttribArray(ii);
        glVertexAttribPointer(ii, element.count, element.type, element.normalized,
                layout.GetStride(), (const void*)(uint64_t)offset);
        offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
    }
}
