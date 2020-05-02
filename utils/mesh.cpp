#include "mesh.h"
#include <cmath>
#include <cassert>
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
            m._trias.push_back( {nodes[0] - 1, nodes[1] - 1, nodes[2] - 1, nodes[3] - 1, nodes[4] - 1, nodes[5] - 1, nodes[6] - 1, nodes[7] - 1, nodes[8] - 1} );
        }
    }
    else if (has_normals)
    {
        formatTrias = "%d//%d %d//%d %d//%d";
        if ( 6 == sscanf(vdata.data(), formatTrias, &nodes[0], &nodes[1], &nodes[2],
                                                    &nodes[3], &nodes[4], &nodes[5]) )
        {
            m._trias.push_back( {nodes[0] - 1, 0, nodes[1] -1, nodes[2] - 1, 0, nodes[3] -1, nodes[4] - 1, 0, nodes[5] -1} );
        }
    }
    else if (has_texcoords)
    {
        formatTrias = "%d/%d %d/%d %d/%d";
        if ( 6 == sscanf(vdata.data(), formatTrias, &nodes[0], &nodes[1], &nodes[2],
                                                    &nodes[3], &nodes[4], &nodes[5]) )
        {
            m._trias.push_back( {nodes[0] - 1, nodes[1] -1, 0, nodes[2] - 1, nodes[3] -1, 0,nodes[4] - 1, nodes[5] -1, 0} );
        }
    }
    else
    {
        if ( 3 == sscanf(vdata.data(), formatTrias, &nodes[0], &nodes[1], &nodes[2]) )
        {
            m._trias.push_back( {nodes[0] - 1, 0, 0, nodes[1] - 1, 0, 0, nodes[2] - 1, 0, 0} );
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
            m._smoothShading = (strcmp(smooth, "1") == 0);
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
    : _name(filename)
{
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
                       << " trias: " << _trias.size() / 3 << "\n";
    // ComputeNormals();
    ComputeCog();
    ComputeBBox();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void mesh::ComputeNormals()
{
    if ( _vertices.empty() || _trias.empty() )
        return;

    _normals.clear();
    _normals.resize(_vertices.size(), 0.0f);

    auto evalTriaNorm = [this](unsigned int tria)
    {
        const mesh::triface &tri = _trias[3 * tria];
        float *v0 = &_vertices[3*tri[0]];
        float *v1 = &_vertices[3*tri[3]];
        float *v2 = &_vertices[3*tri[6]];
        float e0[3] = {v1[0] - v0[0], v1[1] - v0[1], v1[2] - v0[2] };
        float e1[3] = {v2[0] - v1[0], v2[1] - v1[1], v2[2] - v1[2] };
        float n[3] = { e0[1] * e1[2] - e1[1] * e0[2],
                       e1[0] * e0[2] - e0[0] * e1[2],
                       e0[0] * e1[1] - e1[0] * e0[1] };

        _normals[3*tri[0] + 0] += n[0];
        _normals[3*tri[0] + 1] += n[1];
        _normals[3*tri[0]+ 2] += n[2];

        _normals[3*tri[3] + 0] += n[0];
        _normals[3*tri[3] + 1] += n[1];
        _normals[3*tri[3] + 2] += n[2];

        _normals[3*tri[6] + 0] += n[0];
        _normals[3*tri[6] + 1] += n[1];
        _normals[3*tri[6] + 2] += n[2];
    };

    for ( int ii = 0; ii < _trias.size() / 3; ++ii )
        evalTriaNorm(ii);

    for ( int ii = 0; ii < _normals.size() / 3 ; ++ii )
    {
        float *nor = &_normals[3*ii];
        float len = std::sqrt(nor[0] * nor[0] + nor[1] * nor[1]  + nor[2] * nor[2]);
        if ( len > 0 )
        {
            nor[0] /= len;
            nor[1] /= len;
            nor[2] /= len;
        }
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void mesh::ComputeCog()
{
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
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void mesh::ComputeBBox()
{
    _bbox = box3();
    size_t numVertices = _vertices.size() / 3;
    for ( size_t ii = 0; ii < numVertices; ++ii )
    {
        const float* v = &_vertices[3*ii];
        _bbox.expand(v);
    }
}
