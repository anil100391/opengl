#include "vertexbufferlayout.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
template<>
void VertexBufferLayout::Push<float>(unsigned int count)
{
    _elements.push_back({ GL_FLOAT, count, GL_FALSE });
    _stride += count * VertexBufferElement::GetSizeOfType(GL_FLOAT);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
template<>
void VertexBufferLayout::Push<unsigned int>(unsigned int count)
{
    _elements.push_back({ GL_UNSIGNED_INT, count, GL_FALSE });
    _stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
template<>
void VertexBufferLayout::Push<unsigned char>(unsigned int count)
{
    _elements.push_back({ GL_UNSIGNED_BYTE, count, GL_TRUE });
    _stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE);
}

