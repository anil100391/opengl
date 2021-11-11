#include "mesh.h"
#include <cmath>
#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
static void ParseVertex(std::string_view vdata, mesh &m)
{
    float x = 0, y = 0, z = 0;
    if ( 3 == sscanf(vdata.data(), "%f %f %f", &x, &y, &z) )
    {
        m._vertices.push_back(x);
        m._vertices.push_back(y);
        m._vertices.push_back(z);
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
static void ParseTextureCoordinates(std::string_view vdata, mesh &m)
{
    float u = 0, v = 0;
    if ( 2 == sscanf(vdata.data(), "%f %f", &u, &v) )
    {
        m._textureCoords.push_back(u);
        m._textureCoords.push_back(v);
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
static void ParseNormal(std::string_view vdata, mesh &m)
{
    float x = 0, y = 0, z = 0;
    if ( 3 == sscanf(vdata.data(), "%f %f %f", &x, &y, &z) )
    {
        float len = std::sqrt(x*x + y*y  +z*z);
        x /= len;
        y /= len;
        z /= len;
        m._normals.push_back(x);
        m._normals.push_back(y);
        m._normals.push_back(z);
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
static void ParseFace(std::string_view vdata, mesh &m)
{
    int nodes[9] = {0};

    bool has_normals   = !m._normals.empty();
    bool has_texcoords = !m._textureCoords.empty();

    const char *formatTrias = "%d %d %d";
    if (has_normals && has_texcoords)
    {
        formatTrias = "%d/%d/%d %d/%d/%d %d/%d/%d";
        if ( 9 == sscanf(vdata.data(), formatTrias, &nodes[0], &nodes[1], &nodes[2],
                                                    &nodes[3], &nodes[4], &nodes[5],
                                                    &nodes[6], &nodes[7], &nodes[8]) )
        {
            m._trias.push_back( { nodes[0] - 1, nodes[1] - 1, nodes[2] - 1,
                                  nodes[3] - 1, nodes[4] - 1, nodes[5] - 1,
                                  nodes[6] - 1, nodes[7] - 1, nodes[8] - 1 } );
        }
    }
    else if (has_normals)
    {
        formatTrias = "%d//%d %d//%d %d//%d";
        if ( 6 == sscanf(vdata.data(), formatTrias, &nodes[0], &nodes[2],
                                                    &nodes[3], &nodes[5],
                                                    &nodes[6], &nodes[8]) )
        {
            m._trias.push_back( { nodes[0] - 1, nodes[1] - 1, nodes[2] - 1,
                                  nodes[3] - 1, nodes[4] - 1, nodes[5] - 1,
                                  nodes[6] - 1, nodes[7] - 1, nodes[8] - 1 } );
        }
    }
    else if (has_texcoords)
    {
        formatTrias = "%d/%d %d/%d %d/%d";
        if ( 6 == sscanf(vdata.data(), formatTrias, &nodes[0], &nodes[1],
                                                    &nodes[3], &nodes[4],
                                                    &nodes[6], &nodes[7]) )
        {
            m._trias.push_back( { nodes[0] - 1, nodes[1] - 1, nodes[2] - 1,
                                  nodes[3] - 1, nodes[4] - 1, nodes[5] - 1,
                                  nodes[6] - 1, nodes[7] - 1, nodes[8] - 1 } );
        }
    }
    else
    {
        if ( 3 == sscanf(vdata.data(), formatTrias, &nodes[0], &nodes[3], &nodes[6]) )
        {
            m._trias.push_back( { nodes[0] - 1, nodes[1] - 1, nodes[2] - 1,
                                  nodes[3] - 1, nodes[4] - 1, nodes[5] - 1,
                                  nodes[6] - 1, nodes[7] - 1, nodes[8] - 1 } );
        }
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
static bool parseline(const std::string &line, mesh& m)
{
    if (line.size() < 2)
    {
        return false;
    }

    switch (line[0])
    {
    case 'v':
    {
        if (line[1] == ' ')
            ParseVertex(std::string_view(&line[2]), m);
        else if (line[1] == 'n')
            ParseNormal(std::string_view(&line[3]), m);
        else if(line[1] == 't' )
            ParseTextureCoordinates(std::string_view(&line[3]), m);
        break;
    }
    case '#':
        // ignore comments
        break;
    case 's':
    {
        // smooth shading
        char c;
        char smooth[8];
        if ( 2 == sscanf(line.data(), "%c %s", &c, smooth) )
        {
            m.SetSmoothShaded(strcmp(smooth, "1") == 0);
        }
        break;
    }
    case 'f':
        ParseFace(std::string_view(&line[2]), m);
        break;
    case 'o':
        m._name = line;
        m._name.erase(0, 2);
        std::cout << "Reading mesh data for object: " << m._name << "\n";
        break;
    }

    return true;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
mesh::mesh(const char* filename)
{
    initializeFromFile(filename);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void mesh::initializeFromFile(const char *filename)
{
    _name = filename;

#ifdef USE_ASSIMP

    unsigned int importFlags =  aiProcess_GenBoundingBoxes       |
                                aiProcess_CalcTangentSpace       |
                                aiProcess_Triangulate            |
                                aiProcess_JoinIdenticalVertices  |
                                aiProcess_SortByPType;

    _assimpScene = _importer.ReadFile(filename, importFlags);
    if (!_assimpScene || !_assimpScene->HasMeshes())
        return;

    _assimpMesh = _assimpScene ->mMeshes[0];

#else
    std::ifstream obj(filename);
    if ( !obj.is_open() )
    {
        std::cerr << "Failed to open file " << filename << "\n";
        return;
    }

    std::string line;
    while (std::getline(obj, line))
    {
        parseline(line, *this);
    }

    std::cout << _name << " stats\n"
                       << " vertices: " << _vertices.size() / 3 << "\n"
                       << " normals: " << _normals.size() / 3 << "\n"
                       << " tex coords: " << _textureCoords.size() / 2 << "\n"
                       << " trias: " << _trias.size() << "\n";
#endif
    ComputeCog();
    ComputeBBox();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void mesh::ComputeCog()
{
#ifdef USE_ASSIMP
    auto center = _assimpMesh->mAABB.mMin + _assimpMesh->mAABB.mMax;
    _cog = glm::vec3(center[0], center[1], center[2]);
#else
    float sum[3] = {0.0f, 0.0f, 0.0f};
    size_t numVertices = _vertices.size() / 3;
    for ( size_t ii = 0; ii < numVertices; ++ii )
    {
        const float* v = &_vertices[3*ii];
        sum[0] += v[0];
        sum[1] += v[1];
        sum[2] += v[2];
    }

    _cog = glm::vec3(sum[0], sum[1], sum[2]);
    _cog /= numVertices;
#endif
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void mesh::ComputeBBox()
{
    _bbox = box3();
#ifdef USE_ASSIMP
    auto min = _assimpMesh->mAABB.mMin;
    auto max = _assimpMesh->mAABB.mMax;
    _bbox.expand(&min[0]);
    _bbox.expand(&max[0]);
#else
    size_t numVertices = _vertices.size() / 3;
    for ( size_t ii = 0; ii < numVertices; ++ii )
    {
        const float* v = &_vertices[3*ii];
        _bbox.expand(v);
    }
#endif
}
