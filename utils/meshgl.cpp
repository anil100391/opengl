#include "meshgl.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
MeshGL::MeshGL( const std::vector<float> &vertices,
                const VertexBufferLayout &layout,
                const std::vector<unsigned int> &conn,
                int bufferUsage )
{
    PopulateBuffers( vertices, layout, conn, bufferUsage );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void MeshGL::PopulateBuffers( const std::vector<float> &vertices,
                              const VertexBufferLayout &layout,
                              const std::vector<unsigned int> &conn,
                              int bufferUsage )
{
    _vao = std::make_unique<VertexArray>();
    _vbo = std::make_unique<VertexBuffer>( vertices.data(), static_cast<unsigned int>(vertices.size() * sizeof( float )), bufferUsage );

    _vao->AddBuffer( *_vbo, layout );

    _ibo = std::make_unique<IndexBuffer>( conn.data(), static_cast<unsigned int>(conn.size()) );
}
