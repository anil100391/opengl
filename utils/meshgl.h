#ifndef _mesh_gl_h_
#define _mesh_gl_h_

#include "../renderer.h"
#include "../vertexarray.h"
#include "../vertexbuffer.h"
#include "../vertexbufferlayout.h"
#include "../indexbuffer.h"

#include <vector>
#include <memory>

class MeshGL
{
public:

    MeshGL() = default;

    MeshGL( const std::vector<float> &vertices,
            const VertexBufferLayout &layout,
            const std::vector<unsigned int> &conn );

    ~MeshGL() = default;

    VertexArray*  vao() const { return _vao.get(); }
    VertexBuffer* vbo() const { return _vbo.get(); }
    IndexBuffer*  ibo() const { return _ibo.get(); }

protected:

    void PopulateBuffers( const std::vector<float> &vertices,
                          const VertexBufferLayout &layout,
                          const std::vector<unsigned int> &conn );

    std::unique_ptr<VertexArray>    _vao;
    std::unique_ptr<VertexBuffer>   _vbo;
    std::unique_ptr<IndexBuffer>    _ibo;
};

#endif //_mesh_gl_h_
