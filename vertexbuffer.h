#ifndef _vertexbuffer_h_
#define _vertexbuffer_h_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class VertexBuffer
{
public:

    VertexBuffer( const void* data, unsigned int size );
    ~VertexBuffer();

    void Bind() const;
    void Unbind() const;

private:

    unsigned int    _rendererID = 0;
};

#endif // _vertexbuffer_h_
