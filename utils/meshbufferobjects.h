#ifndef _mesh_buffer_objects_h_
#define _mesh_buffer_objects_h_

#include "mesh.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class mbos
{
public:
    static std::vector<float> vbo(const mesh& m);
    static std::vector<unsigned int> ibo(const mesh& m);

private:

    static std::vector<float> ComputeVertexNormals(const mesh& m);
    static std::vector<float> ComputeVertexTexCoords(const mesh& m);
};

#endif // _mesh_buffer_objects_h_
