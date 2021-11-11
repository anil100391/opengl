#ifndef _mesh_h_
#define _mesh_h_

#include <vector>
#include <string>
#include <array>
#include "bbox.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define USE_ASSIMP 1

#ifdef USE_ASSIMP
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#endif


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
struct mesh
{
    template <int N>
    using polyface = std::array<int, 3 * N>;

    using triface = polyface<3>;

    mesh(const char* filename);

    std::string                _name;
    std::vector<float>         _vertices;
    std::vector<float>         _normals;
    std::vector<float>         _textureCoords;
    std::vector<triface>       _trias;

#ifdef USE_ASSIMP
    Assimp::Importer           _importer;
    const aiScene             *_assimpScene = nullptr;
    const aiMesh              *_assimpMesh  = nullptr;
#endif

    void initializeFromFile(const char* filename);

    [[nodiscard]] const glm::vec3& cog() const noexcept
    {
        return _cog;
    }

    [[nodiscard]] const box3& bbox() const noexcept
    {
        return _bbox;
    }

    [[nodiscard]] bool IsSmoothShaded() const noexcept
    {
        return _smoothShaded;
    }

    void SetSmoothShaded(bool smooth)
    {
        _smoothShaded = smooth;
    }

private:

    void ComputeBBox();
    void ComputeCog();

    bool        _smoothShaded = true;
    box3        _bbox;
    glm::vec3   _cog;
};

#endif // _mesh_h_
