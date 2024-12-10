#ifndef _mesh_gl_h_
#define _mesh_gl_h_

#include <graphics/renderer.h>
#include <graphics/vertexarray.h>
#include <graphics/vertexbuffer.h>
#include <graphics/vertexbufferlayout.h>
#include <graphics/indexbuffer.h>

#include <vector>
#include <memory>

class MeshGL
{
public:

    MeshGL() = default;

    MeshGL( const std::vector<float> &vertices,
            const VertexBufferLayout &layout,
            const std::vector<unsigned int> &conn,
            int bufferUsage=GL_STATIC_DRAW);

    ~MeshGL() = default;

    VertexArray*  vao() const { return _vao.get(); }
    VertexBuffer* vbo() const { return _vbo.get(); }
    IndexBuffer*  ibo() const { return _ibo.get(); }

protected:

    void PopulateBuffers( const std::vector<float> &vertices,
                          const VertexBufferLayout &layout,
                          const std::vector<unsigned int> &conn,
                          int bufferUsage=GL_STATIC_DRAW);

    std::unique_ptr<VertexArray>    _vao;
    std::unique_ptr<VertexBuffer>   _vbo;
    std::unique_ptr<IndexBuffer>    _ibo;
};

#endif //_mesh_gl_h_
