#ifndef _mesh_h_
#define _mesh_h_

#include <vector>
#include <string>

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

    const glm::mat4& GetTransform()
    {
        return _modelMat;
    }

    void SetTransform(const glm::mat4& modelmat)
    {
        _modelMat = modelmat;
    }

    void translate(const glm::vec3& dr)
    {
        _modelMat[3][0] += dr[0];
        _modelMat[3][1] += dr[1];
        _modelMat[3][2] += dr[2];
    }

private:
    glm::mat4                  _modelMat;
};

#endif // _mesh_h_
