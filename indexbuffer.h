#ifndef _indexbuffer_h_
#define _indexbuffer_h_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class IndexBuffer
{
public:

    IndexBuffer( const unsigned int* data, unsigned int count );
    ~IndexBuffer();

    void Bind() const;
    void Unbind() const;

    unsigned int GetCount() const;

private:

    unsigned int    _rendererID = 0;
    unsigned int    _count      = 0;
};

#endif // _indexbuffer_h_
