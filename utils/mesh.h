#ifndef _mesh_h_
#define _mesh_h_

#include <vector>
#include <string>
#include "bbox.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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

    [[nodiscard]] const glm::vec3& cog() const noexcept
    {
        return _cog;
    }

    [[nodiscard]] const box3& bbox() const noexcept
    {
        return _bbox;
    }

private:

    void ComputeNormals();
    void ComputeBBox();
    void ComputeCog();

    box3        _bbox;
    glm::vec3   _cog;
};

#endif // _mesh_h_
