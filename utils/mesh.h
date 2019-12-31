#ifndef _mesh_h_
#define _mesh_h_

#include <vector>
#include <string>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
struct mesh
{
    mesh(const char* filename);

    std::string                _name;
    std::vector<float>         _vertices;
    std::vector<float>         _normals;
    std::vector<unsigned int>  _trias;
    std::vector<unsigned int>  _quads;
};

#endif // _mesh_h_
