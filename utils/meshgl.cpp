#include "meshgl.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
MeshGL::MeshGL( const std::vector<float> &vertices,
                const VertexBufferLayout &layout,
                const std::vector<unsigned int> &conn )
{
    PopulateBuffers( vertices, layout, conn );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void MeshGL::PopulateBuffers( const std::vector<float> &vertices,
                              const VertexBufferLayout &layout,
                              const std::vector<unsigned int> &conn )
{
    _vao = std::make_unique<VertexArray>();
    _vbo = std::make_unique<VertexBuffer>( vertices.data(), vertices.size() * sizeof( float ) );

    _vao->AddBuffer( *_vbo, layout );

    _ibo = std::make_unique<IndexBuffer>( conn.data(), conn.size() );
}
