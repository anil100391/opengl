#ifndef _vertexarray_h_
#define _vertexarray_h_

#include "vertexbuffer.h"
#include "vertexbufferlayout.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class VertexArray
{
public:

    VertexArray();
    ~VertexArray();

    void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);

    void Bind() const;
    void Unbind() const;
private:

    unsigned int _rendererID = 0;
};

#endif // _vertexarray_h_
